// Tests for UrlRequest's transport-level error reporting (ErrorString / ErrorSymbol /
// ErrorCode). All scenarios are offline-deterministic: a local file that exists, a local
// file that doesn't, a loopback port with no listener, and a hostname under the reserved
// `.invalid` TLD (RFC 6761 guarantees it never resolves).

#include <UrlLib/UrlLib.h>

#include <arcana/threading/cancellation.h>

#include <gtest/gtest.h>
#include <gsl/util>

#include <algorithm>
#include <cctype>
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <future>
#include <memory>
#include <regex>
#include <string>
#include <thread>
#include <vector>

#if defined(_WIN32)
#include <winsock2.h>
#include <ws2tcpip.h>
#include <process.h>
#else
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

// The Windows and Android backends do not populate transport-error detail yet (see the
// platform-support note in README.md). Skip the assertions that depend on it so the gap
// stays loudly visible in test output without failing the suite.
#if defined(_WIN32)
#define SKIP_WITHOUT_TRANSPORT_ERROR_DETAIL() \
    GTEST_SKIP() << "the Windows backend does not populate transport-error detail yet"
#elif defined(__ANDROID__)
#define SKIP_WITHOUT_TRANSPORT_ERROR_DETAIL() \
    GTEST_SKIP() << "the Android backend does not populate transport-error detail yet"
#else
#define SKIP_WITHOUT_TRANSPORT_ERROR_DETAIL() (void)0
#endif

namespace
{
#if defined(_WIN32)
    using NativeSocket = SOCKET;
    using SocketLength = int;
    constexpr NativeSocket InvalidSocket = INVALID_SOCKET;

    void CloseSocket(NativeSocket socket)
    {
        ::closesocket(socket);
    }

    // Wakes a thread blocked in accept()/recv() on this socket. Closing the socket alone does not
    // reliably interrupt a blocking call in another thread.
    void ShutdownSocket(NativeSocket socket)
    {
        ::shutdown(socket, SD_BOTH);
    }

    bool EnsureSocketsInitialized()
    {
        static const bool initialized = [] {
            WSADATA data{};
            return ::WSAStartup(MAKEWORD(2, 2), &data) == 0;
        }();
        return initialized;
    }

    int CurrentProcessId()
    {
        return ::_getpid();
    }
#else
    using NativeSocket = int;
    using SocketLength = socklen_t;
    constexpr NativeSocket InvalidSocket = -1;

    void CloseSocket(NativeSocket socket)
    {
        ::close(socket);
    }

    // Wakes a thread blocked in accept()/recv() on this socket. On Linux, close() in another thread
    // does NOT interrupt a blocking accept(); shutdown() does.
    void ShutdownSocket(NativeSocket socket)
    {
        ::shutdown(socket, SHUT_RDWR);
    }

    bool EnsureSocketsInitialized()
    {
        return true;
    }

    int CurrentProcessId()
    {
        return ::getpid();
    }
#endif
    // Block until SendAsync completes; false on timeout. On timeout the request is
    // aborted and the shared promise keeps the continuation's target alive regardless.
    // (UrlRequest::Abort() currently only interrupts the Windows backend's transport, so
    // the timeout is a CI backstop; the scenarios in this suite are offline-deterministic
    // and complete quickly by construction.)
    [[nodiscard]] bool SendAndWait(UrlLib::UrlRequest& request)
    {
        auto done = std::make_shared<std::promise<void>>();
        auto future = done->get_future();

        request.SendAsync().then(arcana::inline_scheduler, arcana::cancellation::none(),
            [done](const arcana::expected<void, std::exception_ptr>&) {
                done->set_value();
            });

        if (future.wait_for(std::chrono::seconds{30}) != std::future_status::ready)
        {
            request.Abort();
            return false;
        }

        return true;
    }

    // A loopback TCP port that deterministically refuses connections. On Linux the socket
    // is kept bound (reserving the port against reuse by any other process) but never
    // listen()ed on; SYNs to a bound-but-not-listening port are refused with RST, so the
    // setup is race-free. On Darwin that same SYN is silently dropped instead (the connect
    // attempt times out rather than being refused -- verified empirically), so there the
    // socket is closed after reserving the port number and connects get an immediate RST
    // from the now-unbound port; the reuse window between close and connect is
    // microseconds wide, which is acceptable for CI.
    class RefusingPort
    {
    public:
        static std::optional<RefusingPort> Acquire()
        {
            if (!EnsureSocketsInitialized())
            {
                return std::nullopt;
            }

            NativeSocket fd = ::socket(AF_INET, SOCK_STREAM, 0);
            if (fd == InvalidSocket)
            {
                return std::nullopt;
            }

            sockaddr_in address{};
            address.sin_family = AF_INET;
            address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
            address.sin_port = 0;
            SocketLength addressLength = static_cast<SocketLength>(sizeof(address));
            if (::bind(fd, reinterpret_cast<const sockaddr*>(&address), sizeof(address)) != 0 ||
                ::getsockname(fd, reinterpret_cast<sockaddr*>(&address), &addressLength) != 0)
            {
                CloseSocket(fd);
                return std::nullopt;
            }

#if defined(__APPLE__)
            CloseSocket(fd);
            fd = InvalidSocket;
#endif

            return RefusingPort{fd, ntohs(address.sin_port)};
        }

        RefusingPort(RefusingPort&& other) noexcept
            : m_fd{other.m_fd}
            , m_port{other.m_port}
        {
            other.m_fd = InvalidSocket;
        }

        RefusingPort(const RefusingPort&) = delete;
        RefusingPort& operator=(const RefusingPort&) = delete;
        RefusingPort& operator=(RefusingPort&&) = delete;

        ~RefusingPort()
        {
            if (m_fd != InvalidSocket)
            {
                CloseSocket(m_fd);
            }
        }

        std::string Url() const
        {
            return "http://127.0.0.1:" + std::to_string(m_port) + "/";
        }

    private:
        RefusingPort(NativeSocket fd, uint16_t port)
            : m_fd{fd}
            , m_port{port}
        {
        }

        NativeSocket m_fd;
        uint16_t m_port;
    };

    // A loopback TCP server that accepts connections but, by default, never responds, so a request
    // hangs until it is aborted. A background thread accept()s connections and holds them open
    // until teardown. Used to verify that UrlRequest::Abort() interrupts an in-flight request
    // rather than waiting for the transport's own timeout. Non-movable: the accept thread captures
    // `this`. An optional wire response also supports deterministic HTTP response tests.
    class HangingServer
    {
    public:
        explicit HangingServer(std::string response = {}, std::promise<std::string>* capturedRequest = nullptr,
            size_t requestBodyLength = 0, std::promise<void>* connectionAccepted = nullptr)
        {
            if (!EnsureSocketsInitialized())
            {
                return;
            }

            NativeSocket listener = ::socket(AF_INET, SOCK_STREAM, 0);
            if (listener == InvalidSocket)
            {
                return;
            }

            sockaddr_in address{};
            address.sin_family = AF_INET;
            address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
            address.sin_port = 0;
            SocketLength addressLength = static_cast<SocketLength>(sizeof(address));
            if (::bind(listener, reinterpret_cast<const sockaddr*>(&address), sizeof(address)) != 0 ||
                ::getsockname(listener, reinterpret_cast<sockaddr*>(&address), &addressLength) != 0 ||
                ::listen(listener, 8) != 0)
            {
                CloseSocket(listener);
                return;
            }

            m_listener = listener;
            m_port = ntohs(address.sin_port);
            m_acceptThread = std::thread{[this, response = std::move(response), capturedRequest,
                requestBodyLength, connectionAccepted]() mutable {
                for (;;)
                {
                    NativeSocket connection = ::accept(m_listener, nullptr, nullptr);
                    if (connection == InvalidSocket)
                    {
                        break; // listener closed during teardown
                    }
                    if (connectionAccepted)
                    {
                        connectionAccepted->set_value();
                        connectionAccepted = nullptr;
                    }
                    if (response.empty())
                    {
                        m_accepted.push_back(connection); // hold open, never respond
                    }
                    else
                    {
#if defined(__APPLE__)
                        const int noSignal = 1;
                        ::setsockopt(connection, SOL_SOCKET, SO_NOSIGPIPE, &noSignal, sizeof(noSignal));
#endif
#if defined(_WIN32)
                        const DWORD timeout = 5000;
#else
                        const timeval timeout{5, 0};
#endif
                        if (::setsockopt(connection, SOL_SOCKET, SO_RCVTIMEO,
                            reinterpret_cast<const char*>(&timeout), sizeof(timeout)) != 0)
                        {
                            ShutdownSocket(connection);
                            CloseSocket(connection);
                            continue;
                        }
                        char chunk[4096];
                        std::string request;
                        bool received = false;
                        for (;;)
                        {
                            const auto count = ::recv(connection, chunk, sizeof(chunk), 0);
                            if (count <= 0)
                            {
                                break;
                            }
                            received = true;
                            if (!capturedRequest)
                            {
                                break;
                            }
                            request.append(chunk, static_cast<size_t>(count));
                            const auto headerEnd = request.find("\r\n\r\n");
                            if (headerEnd != std::string::npos && request.size() - headerEnd - 4 >= requestBodyLength)
                            {
                                break;
                            }
                        }
                        if (capturedRequest)
                        {
                            capturedRequest->set_value(std::move(request));
                            capturedRequest = nullptr;
                        }
                        if (received)
                        {
                            size_t sent = 0;
                            while (sent < response.size())
                            {
                                const auto count = ::send(connection, response.data() + sent,
                                    static_cast<int>(response.size() - sent), 0);
                                if (count <= 0)
                                {
                                    break;
                                }
                                sent += static_cast<size_t>(count);
                            }
                        }
                        ShutdownSocket(connection);
                        CloseSocket(connection);
                    }
                }
            }};
        }

        HangingServer(const HangingServer&) = delete;
        HangingServer& operator=(const HangingServer&) = delete;
        HangingServer(HangingServer&&) = delete;
        HangingServer& operator=(HangingServer&&) = delete;

        ~HangingServer()
        {
            if (m_listener != InvalidSocket)
            {
                // shutdown() (not just close()) so a thread blocked in accept() is woken: on Linux
                // close() in another thread does not interrupt the blocking accept().
                ShutdownSocket(m_listener);
                CloseSocket(m_listener);
            }
            if (m_acceptThread.joinable())
            {
                m_acceptThread.join();
            }
            for (NativeSocket connection : m_accepted)
            {
                CloseSocket(connection);
            }
        }

        bool Valid() const
        {
            return m_listener != InvalidSocket;
        }

        std::string Url() const
        {
            return "http://127.0.0.1:" + std::to_string(m_port) + "/";
        }

        uint16_t Port() const
        {
            return m_port;
        }

    private:
        NativeSocket m_listener{InvalidSocket};
        uint16_t m_port{0};
        std::vector<NativeSocket> m_accepted{};
        std::thread m_acceptThread{};
    };

    // RAII temp file with a per-process-unique name, so parallel test runs sharing a temp
    // directory don't collide and no artifacts outlive the test. Pass nullptr contents
    // for a path that is guaranteed not to exist.
    class TempFile
    {
    public:
        TempFile(const char* tag, const char* contents)
            : m_path{std::filesystem::temp_directory_path() /
                  ("urllib_error_reporting_" + std::string{tag} + "_" + std::to_string(CurrentProcessId()) + ".tmp")}
        {
            std::error_code ignored{};
            std::filesystem::remove(m_path, ignored);
            if (contents != nullptr)
            {
                std::ofstream stream{m_path, std::ios::trunc};
                stream << contents;
            }
        }

        ~TempFile()
        {
            std::error_code ignored{};
            std::filesystem::remove(m_path, ignored);
        }

        std::string Url() const
        {
            // POSIX generic paths already start with '/' ("file:///tmp/x"); Windows drive
            // paths do not ("file:///C:/x" needs the third slash added explicitly).
            const std::string generic = m_path.generic_string();
            return (!generic.empty() && generic.front() == '/') ? "file://" + generic : "file:///" + generic;
        }

    private:
        std::filesystem::path m_path;
    };
}

namespace
{
    class HttpPost : public testing::Test
    {
    protected:
        void SetUp() override
        {
#if !defined(_WIN32) && !defined(__APPLE__)
            GTEST_SKIP() << "the Linux backend does not implement POST yet";
#endif
        }

        void CheckPost(std::optional<std::string> headerName, const std::string& body,
            const std::string& contentType = "application/json")
        {
            std::promise<std::string> capturedRequest;
            auto wire = capturedRequest.get_future();
            HangingServer server{
                "HTTP/1.1 200 OK\r\nContent-Length: 2\r\nConnection: close\r\n\r\nok",
                &capturedRequest, body.size()};
            ASSERT_TRUE(server.Valid());
            UrlLib::UrlRequest request;
            request.Open(UrlLib::UrlMethod::Post, server.Url());
            if (headerName)
            {
                request.SetRequestHeader(*headerName, contentType);
            }
            request.SetRequestHeader("x-urllib-test", "post");
            request.SetRequestBody(body);
            ASSERT_TRUE(SendAndWait(request));
            ASSERT_EQ(request.StatusCode(), UrlLib::UrlStatusCode::Ok) << request.ErrorString();
            EXPECT_EQ(request.ResponseString(), "ok");
            EXPECT_TRUE(request.ErrorString().empty());
            ASSERT_EQ(wire.wait_for(std::chrono::seconds{5}), std::future_status::ready);
            const auto sent = wire.get();
            EXPECT_EQ(sent.find("POST / HTTP/1.1\r\n"), 0u);
            const auto headerEnd = sent.find("\r\n\r\n");
            ASSERT_NE(headerEnd, std::string::npos);
            EXPECT_EQ(sent.substr(headerEnd + 4), body);
            std::string headers = sent.substr(0, headerEnd + 2);
            std::transform(headers.begin(), headers.end(), headers.begin(),
                [](unsigned char value) { return static_cast<char>(std::tolower(value)); });
            EXPECT_NE(headers.find("\r\nx-urllib-test: post\r\n"), std::string::npos);
            if (headerName)
            {
                EXPECT_NE(headers.find("\r\ncontent-type: " + contentType + "\r\n"), std::string::npos);
            }
#if defined(_WIN32)
            else
            {
                EXPECT_EQ(headers.find("\r\ncontent-type:"), std::string::npos);
            }
#endif
        }
    };
}

TEST_F(HttpPost, JsonContentTypeIsCaseInsensitive)
{
    for (const auto* name : {"Content-Type", "content-type", "CoNtEnT-TyPe"})
    {
        SCOPED_TRACE(name);
        CheckPost(name, "{\"message\":\"caf\xC3\xA9\"}\n");
    }
}

TEST_F(HttpPost, WithoutContentType)
{
    CheckPost(std::nullopt, "plain text\r\n");
    CheckPost(std::nullopt, "");
}

TEST_F(HttpPost, PreservesContentTypeParameters)
{
    CheckPost("content-type", "{\"ready\":true}", "application/json; charset=utf-8");
}

TEST_F(HttpPost, PreservesRawBodyBytes)
{
    for (const auto& body : {std::string{"a\0b", 3}, std::string{"\x80\xff\r\n", 4},
        std::string{"\0\x80\xff\r\n", 5}})
    {
        CheckPost("content-type", body, "application/octet-stream");
    }
}

#if defined(_WIN32)
namespace
{
    class WindowsHttp : public testing::Test
    {
    protected:
        void CheckResponse(const std::string& wire, int status, std::string_view body,
            std::optional<std::string> contentType)
        {
            HangingServer server{wire};
            ASSERT_TRUE(server.Valid());
            for (auto type : {UrlLib::UrlResponseType::String, UrlLib::UrlResponseType::Buffer})
            {
                UrlLib::UrlRequest request;
                request.Open(UrlLib::UrlMethod::Get, server.Url());
                request.ResponseType(type);
                auto settled = std::make_shared<std::promise<std::exception_ptr>>();
                auto future = settled->get_future();
                request.SendAsync().then(arcana::inline_scheduler, arcana::cancellation::none(),
                    [settled](const arcana::expected<void, std::exception_ptr>& result) {
                        settled->set_value(result.has_error() ? result.error() : nullptr);
                    });
                ASSERT_EQ(future.wait_for(std::chrono::seconds{30}), std::future_status::ready);
                const auto error = future.get();
                ASSERT_EQ(error, nullptr) << "HTTP task failed";
                EXPECT_EQ(static_cast<int>(request.StatusCode()), status);
                EXPECT_EQ(request.GetResponseHeader("content-type"), contentType);
                EXPECT_TRUE(request.ErrorString().empty());
                if (type == UrlLib::UrlResponseType::String)
                {
                    EXPECT_EQ(request.ResponseString(), body);
                }
                else
                {
                    const auto bytes = request.ResponseBuffer();
                    ASSERT_EQ(bytes.size(), body.size());
                    if (!bytes.empty())
                    {
                        EXPECT_EQ(std::string_view(reinterpret_cast<const char*>(bytes.data()), bytes.size()), body);
                    }
                }
            }
        }
    };
}

TEST_F(WindowsHttp, MissingContentTypePreservesBody)
{
    CheckResponse("HTTP/1.1 200 OK\r\nContent-Length: 5\r\nConnection: close\r\n\r\nhello",
        200, "hello", std::nullopt);
}

TEST_F(WindowsHttp, PostRejectsInvalidContentTypeAndCanReopen)
{
    const std::string body = "reopened without Content-Type";
    std::promise<std::string> capturedRequest;
    auto wire = capturedRequest.get_future();
    HangingServer server{"HTTP/1.1 200 OK\r\nContent-Length: 0\r\nConnection: close\r\n\r\n",
        &capturedRequest, body.size()};
    ASSERT_TRUE(server.Valid());
    UrlLib::UrlRequest request;
    request.Open(UrlLib::UrlMethod::Post, server.Url());
    request.SetRequestHeader("content-type", "not a media type");
    request.SetRequestBody("unchanged");
    ASSERT_TRUE(SendAndWait(request));
    EXPECT_EQ(request.StatusCode(), UrlLib::UrlStatusCode::None);
    EXPECT_FALSE(request.ErrorString().empty());
    EXPECT_TRUE(request.ResponseString().empty());
    EXPECT_TRUE(request.GetAllResponseHeaders().empty());

    request.Open(UrlLib::UrlMethod::Post, server.Url());
    request.SetRequestBody(body);
    ASSERT_TRUE(SendAndWait(request));
    ASSERT_EQ(request.StatusCode(), UrlLib::UrlStatusCode::Ok) << request.ErrorString();
    EXPECT_TRUE(request.ErrorString().empty());
    ASSERT_EQ(wire.wait_for(std::chrono::seconds{5}), std::future_status::ready);
    const auto sent = wire.get();
    const auto headerEnd = sent.find("\r\n\r\n");
    ASSERT_NE(headerEnd, std::string::npos);
    EXPECT_EQ(sent.substr(headerEnd + 4), body);
    std::string headers = sent.substr(0, headerEnd);
    std::transform(headers.begin(), headers.end(), headers.begin(),
        [](unsigned char value) { return static_cast<char>(std::tolower(value)); });
    EXPECT_EQ(headers.find("\r\ncontent-type:"), std::string::npos);
}

TEST_F(WindowsHttp, MissingContentTypeWithEmptyBody)
{
    CheckResponse("HTTP/1.1 200 OK\r\nContent-Length: 0\r\nConnection: close\r\n\r\n",
        200, "", std::nullopt);
}

TEST_F(WindowsHttp, NoContentRetainsExistingSuccessNormalization)
{
    CheckResponse("HTTP/1.1 204 No Content\r\nConnection: close\r\n\r\n",
        200, "", std::nullopt);
}

TEST_F(WindowsHttp, ContentTypePreserved)
{
    CheckResponse("HTTP/1.1 200 OK\r\nContent-Type: text/plain; charset=utf-8\r\nContent-Length: 5\r\nConnection: close\r\n\r\nhello",
        200, "hello", std::string{"text/plain; charset=utf-8"});
}

TEST_F(WindowsHttp, NonSuccessRetainsExistingStatusAndEmptyBody)
{
    CheckResponse("HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\nContent-Length: 7\r\nConnection: close\r\n\r\nmissing",
        404, "", std::nullopt);
    CheckResponse("HTTP/1.1 500 Internal Server Error\r\nContent-Length: 0\r\nConnection: close\r\n\r\n",
        500, "", std::nullopt);
    CheckResponse("HTTP/1.1 304 Not Modified\r\nConnection: close\r\n\r\n",
        304, "", std::nullopt);
}

TEST_F(WindowsHttp, ConnectionFailureIsNotSuccess)
{
    const auto port = RefusingPort::Acquire();
    ASSERT_TRUE(port);
    UrlLib::UrlRequest request;
    request.Open(UrlLib::UrlMethod::Get, port->Url());
    auto settled = std::make_shared<std::promise<bool>>();
    auto future = settled->get_future();
    request.SendAsync().then(arcana::inline_scheduler, arcana::cancellation::none(),
        [settled](const arcana::expected<void, std::exception_ptr>& result) {
            settled->set_value(result.has_error());
        });
    ASSERT_EQ(future.wait_for(std::chrono::seconds{30}), std::future_status::ready);
    EXPECT_TRUE(future.get() || !request.ErrorString().empty());
    EXPECT_EQ(request.StatusCode(), UrlLib::UrlStatusCode::None);
    EXPECT_TRUE(request.ResponseString().empty());
    EXPECT_TRUE(request.GetAllResponseHeaders().empty());
}
#endif

TEST(LoopbackServer, IdleResponseConnectionDoesNotBlockTeardown)
{
    std::promise<void> accepted;
    auto acceptedFuture = accepted.get_future();
    auto server = std::make_unique<HangingServer>(
        "HTTP/1.1 200 OK\r\nContent-Length: 0\r\nConnection: close\r\n\r\n", nullptr, 0, &accepted);
    ASSERT_TRUE(server->Valid());
    const NativeSocket connection = ::socket(AF_INET, SOCK_STREAM, 0);
    ASSERT_NE(connection, InvalidSocket);
    auto closeConnection = gsl::finally([connection] { CloseSocket(connection); });
    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    address.sin_port = htons(server->Port());
    ASSERT_EQ(::connect(connection, reinterpret_cast<const sockaddr*>(&address), sizeof(address)), 0);
    ASSERT_EQ(acceptedFuture.wait_for(std::chrono::seconds{5}), std::future_status::ready);

    std::promise<void> destroyed;
    auto destroyedFuture = destroyed.get_future();
    std::thread teardown{[server = std::move(server), &destroyed]() mutable {
        server.reset();
        destroyed.set_value();
    }};
    const auto completion = destroyedFuture.wait_for(std::chrono::seconds{10});
    ShutdownSocket(connection);
    teardown.join();
    EXPECT_EQ(completion, std::future_status::ready);
}

TEST(UrlRequestErrorReporting, SuccessfulLocalFileReportsNoError)
{
    const TempFile file{"ok", "hello urllib"};

    UrlLib::UrlRequest request{};
    request.Open(UrlLib::UrlMethod::Get, file.Url());
    request.ResponseType(UrlLib::UrlResponseType::String);
    ASSERT_TRUE(SendAndWait(request));

    EXPECT_EQ(request.StatusCode(), UrlLib::UrlStatusCode::Ok);
    EXPECT_EQ(request.ResponseString(), "hello urllib");
    EXPECT_TRUE(request.ErrorString().empty()) << request.ErrorString();
    EXPECT_TRUE(request.ErrorSymbol().empty()) << request.ErrorSymbol();
    EXPECT_EQ(request.ErrorCode(), 0);
}

TEST(UrlRequestErrorReporting, SwitchingBetweenDataAndPlatformTransportClearsResponse)
{
    const TempFile file{"scheme_reuse", "file body"};
    UrlLib::UrlRequest request;
    request.ResponseType(UrlLib::UrlResponseType::Buffer);
    request.Open(UrlLib::UrlMethod::Get, "data:application/octet-stream,previous");
    ASSERT_TRUE(SendAndWait(request));
    ASSERT_EQ(request.ResponseBuffer().size(), 8u);
    request.Open(UrlLib::UrlMethod::Get, file.Url());
    ASSERT_TRUE(SendAndWait(request));
    EXPECT_EQ(request.StatusCode(), UrlLib::UrlStatusCode::Ok);
    EXPECT_FALSE(request.GetResponseHeader("content-type"));
    const auto body = request.ResponseBuffer();
    ASSERT_EQ(body.size(), 9u);
    EXPECT_EQ(std::string_view(reinterpret_cast<const char*>(body.data()), body.size()), "file body");
    request.Open(UrlLib::UrlMethod::Get, "data:,");
    ASSERT_TRUE(SendAndWait(request));
    EXPECT_TRUE(request.ResponseBuffer().empty());
    EXPECT_EQ(request.GetResponseHeader("content-type"), std::optional<std::string>{"text/plain;charset=US-ASCII"});
}

TEST(UrlRequestErrorReporting, MissingLocalFileReportsError)
{
    SKIP_WITHOUT_TRANSPORT_ERROR_DETAIL();

    const TempFile missing{"missing", nullptr};

    UrlLib::UrlRequest request{};
    request.Open(UrlLib::UrlMethod::Get, missing.Url());
    ASSERT_TRUE(SendAndWait(request));

    EXPECT_EQ(request.StatusCode(), UrlLib::UrlStatusCode::None);
    EXPECT_FALSE(request.ErrorString().empty());
#if defined(__APPLE__)
    EXPECT_EQ(request.ErrorSymbol(), "NSURLErrorFileDoesNotExist") << request.ErrorString();
    EXPECT_EQ(request.ErrorCode(), -1100) << request.ErrorString();
#else
    EXPECT_EQ(request.ErrorSymbol(), "CURLE_FILE_COULDNT_READ_FILE") << request.ErrorString();
    EXPECT_EQ(request.ErrorCode(), 37) << request.ErrorString();
#endif
}

TEST(UrlRequestErrorReporting, ConnectionRefusedReportsError)
{
    SKIP_WITHOUT_TRANSPORT_ERROR_DETAIL();

    const auto port = RefusingPort::Acquire();
    ASSERT_TRUE(port.has_value());

    UrlLib::UrlRequest request{};
    request.Open(UrlLib::UrlMethod::Get, port->Url());
    ASSERT_TRUE(SendAndWait(request));

    EXPECT_EQ(request.StatusCode(), UrlLib::UrlStatusCode::None);
    EXPECT_FALSE(request.ErrorString().empty());
#if defined(__APPLE__)
    EXPECT_EQ(request.ErrorSymbol(), "NSURLErrorCannotConnectToHost") << request.ErrorString();
    EXPECT_EQ(request.ErrorCode(), -1004) << request.ErrorString();
#else
    EXPECT_EQ(request.ErrorSymbol(), "CURLE_COULDNT_CONNECT") << request.ErrorString();
    EXPECT_EQ(request.ErrorCode(), 7) << request.ErrorString();
    // libcurl's CURLOPT_ERRORBUFFER detail names the host it could not reach.
    EXPECT_NE(request.ErrorString().find("127.0.0.1"), std::string_view::npos) << request.ErrorString();
#endif
}

TEST(UrlRequestErrorReporting, DnsResolutionFailureReportsError)
{
    SKIP_WITHOUT_TRANSPORT_ERROR_DETAIL();

    UrlLib::UrlRequest request{};
    request.Open(UrlLib::UrlMethod::Get, "http://urllib-error-reporting-test.invalid/");
    ASSERT_TRUE(SendAndWait(request));

    EXPECT_EQ(request.StatusCode(), UrlLib::UrlStatusCode::None);
    EXPECT_FALSE(request.ErrorString().empty());
#if defined(__APPLE__)
    // Depending on resolver/network state this surfaces as CannotFindHost (-1003),
    // DNSLookupFailed (-1006), or NotConnectedToInternet (-1009); all are NSURL errors.
    EXPECT_EQ(request.ErrorString().substr(0, 6), "nsurl:") << request.ErrorString();
    EXPECT_NE(request.ErrorCode(), 0) << request.ErrorString();
#else
    // Normally CURLE_COULDNT_RESOLVE_HOST(6). Under a proxy this can surface as
    // CURLE_COULDNT_RESOLVE_PROXY(5), and a DNS-hijacking resolver that hands back an address
    // for an .invalid name can turn this into a later connect failure. Require a curl transport
    // error with a non-zero code rather than pinning the exact symbol; the exact symbols are
    // asserted by the ConnectionRefused and MissingLocalFile cases.
    EXPECT_EQ(request.ErrorString().substr(0, 5), "curl:") << request.ErrorString();
    EXPECT_NE(request.ErrorCode(), 0) << request.ErrorString();
#endif
}

TEST(UrlRequestErrorReporting, ErrorStringMatchesGreppableGrammar)
{
    SKIP_WITHOUT_TRANSPORT_ERROR_DETAIL();

    const auto port = RefusingPort::Acquire();
    ASSERT_TRUE(port.has_value());

    UrlLib::UrlRequest request{};
    request.Open(UrlLib::UrlMethod::Get, port->Url());
    ASSERT_TRUE(SendAndWait(request));

    // "<domain>:<symbol>(<code>): <detail>" with domain/symbol as stable ASCII tokens.
    const std::regex grammar{R"(^[A-Za-z0-9_.\-]+:[A-Za-z0-9_.\-]+\(-?[0-9]+\): .+)"};
    const std::string errorString{request.ErrorString()};
    EXPECT_TRUE(std::regex_search(errorString, grammar)) << errorString;
}

TEST(UrlRequestErrorReporting, ReopenClearsPriorError)
{
    SKIP_WITHOUT_TRANSPORT_ERROR_DETAIL();

    const auto port = RefusingPort::Acquire();
    ASSERT_TRUE(port.has_value());

    UrlLib::UrlRequest request{};
    request.Open(UrlLib::UrlMethod::Get, port->Url());
    ASSERT_TRUE(SendAndWait(request));
    ASSERT_FALSE(request.ErrorString().empty());

    const TempFile file{"reuse", "reused"};
    request.Open(UrlLib::UrlMethod::Get, file.Url());
    EXPECT_TRUE(request.ErrorString().empty()) << request.ErrorString();
    EXPECT_EQ(request.ErrorCode(), 0);

    request.ResponseType(UrlLib::UrlResponseType::String);
    ASSERT_TRUE(SendAndWait(request));
    EXPECT_EQ(request.StatusCode(), UrlLib::UrlStatusCode::Ok);
    EXPECT_EQ(request.ResponseString(), "reused");
    EXPECT_TRUE(request.ErrorString().empty()) << request.ErrorString();
}

TEST(UrlRequestErrorReporting, AbortInterruptsInFlightRequest)
{
    // The Windows backend already observes Abort() (its WinRT continuations are guarded by
    // m_cancellationSource), but it does not populate the transport-error accessors, so the
    // symbol assertions below are gated to the backends that do.
    SKIP_WITHOUT_TRANSPORT_ERROR_DETAIL();

    HangingServer server{};
    ASSERT_TRUE(server.Valid());

    UrlLib::UrlRequest request{};
    request.Open(UrlLib::UrlMethod::Get, server.Url());

    auto done = std::make_shared<std::promise<void>>();
    auto future = done->get_future();
    request.SendAsync().then(arcana::inline_scheduler, arcana::cancellation::none(),
        [done](const arcana::expected<void, std::exception_ptr>&) {
            done->set_value();
        });

    // Let the request connect to the hanging server and start waiting for a response that never
    // comes, then abort. Without Abort() being observed on this backend the request would block
    // until the transport's own timeout.
    std::this_thread::sleep_for(std::chrono::milliseconds{250});
    request.Abort();

    ASSERT_EQ(future.wait_for(std::chrono::seconds{15}), std::future_status::ready)
        << "Abort did not interrupt the in-flight request";

    EXPECT_EQ(request.StatusCode(), UrlLib::UrlStatusCode::None);
    EXPECT_FALSE(request.ErrorString().empty());
#if defined(__APPLE__)
    EXPECT_EQ(request.ErrorSymbol(), "NSURLErrorCancelled") << request.ErrorString();
    EXPECT_EQ(request.ErrorCode(), -999) << request.ErrorString();
#else
    // The guarantee under test is that Abort() interrupts the request promptly (the bounded wait
    // above) and records a curl transport error. Assert the "curl:" prefix rather than pinning the
    // exact CURLcode, which can vary with libcurl internals/timing.
    EXPECT_EQ(request.ErrorString().substr(0, 5), "curl:") << request.ErrorString();
#endif
}

#if defined(__APPLE__)
TEST(UrlRequestErrorReporting, MissingAppResourceReportsError)
{
    UrlLib::UrlRequest request{};
    request.Open(UrlLib::UrlMethod::Get, "app:///urllib_error_reporting_missing.js");
    ASSERT_TRUE(SendAndWait(request));

    EXPECT_EQ(request.StatusCode(), UrlLib::UrlStatusCode::None);
    EXPECT_EQ(request.ErrorSymbol(), "AppResourceNotFound") << request.ErrorString();
    // A genuine failure that intentionally reports code 0, so consumers must key off
    // ErrorString()/ErrorSymbol() emptiness -- not ErrorCode() -- to detect failures.
    EXPECT_EQ(request.ErrorCode(), 0) << request.ErrorString();
    EXPECT_FALSE(request.ErrorString().empty());
    const std::string errorString{request.ErrorString()};
    EXPECT_NE(errorString.find("urllib:AppResourceNotFound(0): "), std::string::npos) << errorString;
    EXPECT_NE(errorString.find("app:///urllib_error_reporting_missing.js"), std::string::npos) << errorString;
}
#endif
