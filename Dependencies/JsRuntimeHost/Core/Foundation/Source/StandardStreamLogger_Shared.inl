// Shared stdout/stderr tee + drain implementation.
//
// Platform TUs define these in the enclosing anonymous namespace, then include:
//   struct ChannelPlatformState { ... };
//   int OsDuplicate(int fd);
//   int OsDuplicateTo(int source, int target);
//   int OsClose(int fd);
//   int64_t OsRead(int fd, void* data, size_t size);
//   int64_t OsWrite(int fd, const void* data, size_t size);
//   int OsCreatePipe(int fds[2]);
//   bool OsOccupyTarget(int target);
//   void OsWritePlatform(bool isError, const std::string& line);
//   bool OsOnStartChannel(ChannelPlatformState& state, int target, bool isError);
//   bool OsOnRedirected(ChannelPlatformState& state, int target);
//   bool OsOnRestore(ChannelPlatformState& state, int target);

#include <array>
#include <cerrno>
#include <cstdint>
#include <cstdio>
#include <chrono>
#include <future>
#include <iostream>
#include <string>
#include <system_error>
#include <thread>
#include <utility>
#include <vector>

namespace
{
    enum class Stream
    {
        Output,
        Error,
    };

    struct Channel
    {
        int Target{-1};
        int Original{-1};
        ChannelPlatformState Platform{};
        std::future<void> Completion{};
        std::thread Reader{};
    };

    // Drain futures retained after Stop() times out and detaches the reader.
    // Start() reaps these before installing a new redirection.
    std::vector<std::future<void>> g_outstandingDrains{};
    Channel g_stdout{};
    Channel g_stderr{};

    bool WriteAll(int fd, const char* data, size_t size)
    {
        while (size != 0)
        {
            const auto written = OsWrite(fd, data, size);
            if (written > 0)
            {
                data += written;
                size -= static_cast<size_t>(written);
                continue;
            }
            if (written < 0 && errno == EINTR)
            {
                continue;
            }
            return false;
        }
        return true;
    }

    void EmitLine(Stream stream, std::string line)
    {
        if (!line.empty() && line.back() == '\r')
        {
            line.pop_back();
        }
        OsWritePlatform(stream == Stream::Error, line);
    }

    void Drain(int readFd, int originalFd, Stream stream)
    {
        // Cap mirrored lines below typical platform limits (~4 KiB for
        // OutputDebugStringA / logcat / os_log). Leave headroom under 4096.
        constexpr size_t MAX_PLATFORM_LINE_SIZE{3800};
        std::array<char, 1024> buffer{};
        std::string pending{};

        for (;;)
        {
            const auto count = OsRead(readFd, buffer.data(), buffer.size());
            if (count == 0)
            {
                break;
            }
            if (count < 0)
            {
                if (errno == EINTR)
                {
                    continue;
                }
                break;
            }

            const size_t size = static_cast<size_t>(count);
            if (originalFd >= 0)
            {
                (void)WriteAll(originalFd, buffer.data(), size);
            }

            pending.append(buffer.data(), size);

            // Consume complete lines via a start index so we only memmove once
            // per read batch instead of on every newline.
            size_t start = 0;
            for (;;)
            {
                const size_t newline = pending.find('\n', start);
                if (newline != std::string::npos)
                {
                    EmitLine(stream, pending.substr(start, newline - start));
                    start = newline + 1;
                }
                else if (pending.size() - start >= MAX_PLATFORM_LINE_SIZE)
                {
                    EmitLine(stream, pending.substr(start, MAX_PLATFORM_LINE_SIZE));
                    start += MAX_PLATFORM_LINE_SIZE;
                }
                else
                {
                    break;
                }
            }
            if (start != 0)
            {
                pending.erase(0, start);
            }
        }

        if (!pending.empty())
        {
            EmitLine(stream, std::move(pending));
        }
        (void)OsClose(readFd);
        if (originalFd >= 0)
        {
            (void)OsClose(originalFd);
        }
    }

    void RollbackRedirect(Channel& channel, int pipeReadFd, int readerOriginal)
    {
        if (channel.Original >= 0)
        {
            (void)OsDuplicateTo(channel.Original, channel.Target);
            (void)OsClose(channel.Original);
        }
        else
        {
            (void)OsClose(channel.Target);
        }
        (void)OsOnRestore(channel.Platform, channel.Target);
        if (pipeReadFd >= 0)
        {
            (void)OsClose(pipeReadFd);
        }
        if (readerOriginal >= 0)
        {
            (void)OsClose(readerOriginal);
        }
        channel = {};
    }

    bool StartChannel(Channel& channel, int target, Stream stream)
    {
        channel.Target = target;
        if (!OsOnStartChannel(channel.Platform, target, stream == Stream::Error))
        {
            channel = {};
            return false;
        }

        errno = 0;
        channel.Original = OsDuplicate(target);
        if (channel.Original < 0 && errno != EBADF)
        {
            channel = {};
            return false;
        }
        if (channel.Original < 0 && !OsOccupyTarget(target))
        {
            channel = {};
            return false;
        }

        int pipeFds[2]{-1, -1};
        if (OsCreatePipe(pipeFds) != 0)
        {
            if (channel.Original >= 0)
            {
                (void)OsClose(channel.Original);
            }
            else
            {
                (void)OsClose(target);
            }
            channel = {};
            return false;
        }

        if (OsDuplicateTo(pipeFds[1], target) != 0)
        {
            (void)OsClose(pipeFds[0]);
            (void)OsClose(pipeFds[1]);
            if (channel.Original >= 0)
            {
                (void)OsClose(channel.Original);
            }
            else
            {
                (void)OsClose(target);
            }
            channel = {};
            return false;
        }
        (void)OsClose(pipeFds[1]);

        if (!OsOnRedirected(channel.Platform, target))
        {
            RollbackRedirect(channel, pipeFds[0], -1);
            return false;
        }

        int readerOriginal{-1};
        if (channel.Original >= 0)
        {
            readerOriginal = OsDuplicate(channel.Original);
            if (readerOriginal < 0)
            {
                RollbackRedirect(channel, pipeFds[0], -1);
                return false;
            }
        }

        std::promise<void> completed{};
        channel.Completion = completed.get_future();
        try
        {
            channel.Reader = std::thread{
                [readFd = pipeFds[0], originalFd = readerOriginal, stream, completed = std::move(completed)]() mutable {
                    Drain(readFd, originalFd, stream);
                    completed.set_value();
                }};
        }
        catch (const std::system_error&)
        {
            RollbackRedirect(channel, pipeFds[0], readerOriginal);
            return false;
        }
        return true;
    }

    bool StopChannel(Channel& channel)
    {
        bool restored{true};
        if (channel.Original >= 0)
        {
            restored = OsDuplicateTo(channel.Original, channel.Target) == 0;
            if (!restored)
            {
                (void)OsClose(channel.Target);
            }
        }
        else
        {
            restored = OsClose(channel.Target) == 0;
        }

        restored = OsOnRestore(channel.Platform, channel.Target) && restored;

        if (channel.Original >= 0)
        {
            (void)OsClose(channel.Original);
        }

        if (channel.Reader.joinable())
        {
            if (channel.Completion.wait_for(std::chrono::seconds{2}) == std::future_status::ready)
            {
                channel.Reader.join();
            }
            else
            {
                // Detach so Stop can return, but keep the future so Start()
                // can refuse a restart until this drain actually finishes.
                g_outstandingDrains.push_back(std::move(channel.Completion));
                channel.Reader.detach();
                restored = false;
            }
        }
        channel = {};
        return restored;
    }

    bool ReapOutstandingDrains(std::chrono::milliseconds timeout)
    {
        const auto deadline = std::chrono::steady_clock::now() + timeout;
        while (!g_outstandingDrains.empty())
        {
            auto& front = g_outstandingDrains.front();
            const auto remaining = deadline - std::chrono::steady_clock::now();
            if (remaining <= std::chrono::milliseconds::zero())
            {
                if (front.wait_for(std::chrono::milliseconds::zero()) != std::future_status::ready)
                {
                    return false;
                }
            }
            else if (front.wait_for(remaining) != std::future_status::ready)
            {
                return false;
            }

            g_outstandingDrains.erase(g_outstandingDrains.begin());
        }
        return true;
    }
}

namespace Babylon::StandardStreamLogger::Platform
{
    bool Start()
    {
        if (!ReapOutstandingDrains(std::chrono::seconds{2}))
        {
            return false;
        }

        std::cout.flush();
        std::cerr.flush();
        std::fflush(stdout);
        std::fflush(stderr);

        if (!StartChannel(g_stdout, 1, Stream::Output))
        {
            return false;
        }
        if (!StartChannel(g_stderr, 2, Stream::Error))
        {
            (void)StopChannel(g_stdout);
            return false;
        }
        return true;
    }

    bool Stop()
    {
        std::cout.flush();
        std::cerr.flush();
        std::fflush(stdout);
        std::fflush(stderr);
        bool stopped = StopChannel(g_stdout);
        stopped = StopChannel(g_stderr) && stopped;
        return stopped;
    }
}