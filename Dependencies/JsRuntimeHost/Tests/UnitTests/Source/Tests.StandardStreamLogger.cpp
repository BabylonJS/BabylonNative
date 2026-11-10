#include <Babylon/StandardStreamLogger.h>
#include <gtest/gtest.h>

#include <array>
#include <cstdio>
#include <string>

#if defined(_WIN32)
#include <Windows.h>
#include <io.h>
#else
#include <unistd.h>
#endif

namespace
{
#if defined(_WIN32)
    int DuplicateFileDescriptor(int fd)
    {
        return ::_dup(fd);
    }

    int DuplicateFileDescriptorTo(int source, int target)
    {
        return ::_dup2(source, target);
    }

    int CloseFileDescriptor(int fd)
    {
        return ::_close(fd);
    }

    int FileDescriptor(FILE* file)
    {
        return ::_fileno(file);
    }
#else
    int DuplicateFileDescriptor(int fd)
    {
        return ::dup(fd);
    }

    int DuplicateFileDescriptorTo(int source, int target)
    {
        return ::dup2(source, target) < 0 ? -1 : 0;
    }

    int CloseFileDescriptor(int fd)
    {
        return ::close(fd);
    }

    int FileDescriptor(FILE* file)
    {
        return ::fileno(file);
    }
#endif

    class StdoutCapture
    {
    public:
        StdoutCapture()
        {
            std::fflush(stdout);
            m_original = DuplicateFileDescriptor(1);
#if defined(_WIN32)
            m_originalStdHandle = ::GetStdHandle(STD_OUTPUT_HANDLE);
            const intptr_t originalFdHandle = ::_get_osfhandle(1);
            m_originalStdHandleUsesTarget =
                originalFdHandle != -1 &&
                m_originalStdHandle != nullptr &&
                m_originalStdHandle != INVALID_HANDLE_VALUE &&
                m_originalStdHandle == reinterpret_cast<HANDLE>(originalFdHandle);
#endif
            m_file = std::tmpfile();
            if (m_original < 0 || m_file == nullptr || DuplicateFileDescriptorTo(FileDescriptor(m_file), 1) != 0)
            {
                Restore();
                return;
            }
#if defined(_WIN32)
            const intptr_t handle = ::_get_osfhandle(1);
            if (handle == -1 || !::SetStdHandle(STD_OUTPUT_HANDLE, reinterpret_cast<HANDLE>(handle)))
            {
                Restore();
                return;
            }
#endif
            m_valid = true;
        }

        ~StdoutCapture()
        {
            Restore();
        }

        bool Valid() const
        {
            return m_valid;
        }

        std::string ReadAndRestore()
        {
            std::fflush(stdout);
            std::rewind(m_file);

            std::string result{};
            std::array<char, 256> buffer{};
            for (;;)
            {
                const size_t size = std::fread(buffer.data(), 1, buffer.size(), m_file);
                result.append(buffer.data(), size);
                if (size != buffer.size())
                {
                    break;
                }
            }

            Restore();
            return result;
        }

    private:
        void Restore()
        {
            if (m_original >= 0)
            {
                std::fflush(stdout);
                const bool restored = DuplicateFileDescriptorTo(m_original, 1) == 0;
                (void)CloseFileDescriptor(m_original);
                m_original = -1;
#if defined(_WIN32)
                if (restored)
                {
                    HANDLE handle = m_originalStdHandle;
                    if (m_originalStdHandleUsesTarget)
                    {
                        const intptr_t restoredFdHandle = ::_get_osfhandle(1);
                        handle = restoredFdHandle == -1
                            ? INVALID_HANDLE_VALUE
                            : reinterpret_cast<HANDLE>(restoredFdHandle);
                    }
                    (void)::SetStdHandle(STD_OUTPUT_HANDLE, handle);
                }
#endif
            }
            if (m_file != nullptr)
            {
                std::fclose(m_file);
                m_file = nullptr;
            }
            m_valid = false;
        }

        FILE* m_file{};
        int m_original{-1};
        bool m_valid{};
#if defined(_WIN32)
        HANDLE m_originalStdHandle{INVALID_HANDLE_VALUE};
        bool m_originalStdHandleUsesTarget{};
#endif
    };
}

TEST(StandardStreamLogger, Lifecycle)
{
    if (Babylon::StandardStreamLogger::IsStarted())
    {
        GTEST_SKIP() << "The platform host already owns standard-stream forwarding.";
    }

    StdoutCapture capture{};
    if (!capture.Valid())
    {
        GTEST_SKIP() << "The platform does not expose a writable temporary-file location.";
    }

    const bool started = Babylon::StandardStreamLogger::Start();
    const bool isStarted = Babylon::StandardStreamLogger::IsStarted();
    const bool secondStart = Babylon::StandardStreamLogger::Start();

    // Only exercise stdout here. iOS CI captures simctl launch stderr as the
    // process exit code (`2> /tmp/exitCode`), so writing to stderr would corrupt
    // that handshake even when the tests themselves succeed.
    std::fputs("StandardStreamLogger stdout test", stdout);
    std::fflush(stdout);

    const bool stopped = Babylon::StandardStreamLogger::Stop();
    const std::string captured = capture.ReadAndRestore();

    EXPECT_TRUE(started);
    EXPECT_TRUE(isStarted);
    EXPECT_TRUE(secondStart);
    EXPECT_TRUE(stopped);
    EXPECT_FALSE(Babylon::StandardStreamLogger::IsStarted());
    EXPECT_TRUE(Babylon::StandardStreamLogger::Stop());
    EXPECT_EQ(captured, "StandardStreamLogger stdout test");
}
