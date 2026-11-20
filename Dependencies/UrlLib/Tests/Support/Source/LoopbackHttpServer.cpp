#include <UrlLib/TestSupport/LoopbackHttpServer.h>

#include <algorithm>
#include <chrono>
#include <condition_variable>
#include <cstddef>
#include <cstdint>
#include <list>
#include <map>
#include <mutex>
#include <string>
#include <string_view>
#include <thread>
#include <utility>
#include <vector>

#if defined(_WIN32)
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#endif

namespace UrlLib::TestSupport
{
    class LoopbackHttpServer::Impl final
    {
    public:
        Impl()
        {
            if (!EnsureSocketsInitialized())
            {
                return;
            }

            const NativeSocket listener = ::socket(AF_INET, SOCK_STREAM, 0);
            if (listener == InvalidSocket)
            {
                return;
            }

            sockaddr_in address{};
            address.sin_family = AF_INET;
            address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
            address.sin_port = 0;
            SocketLength addressLength = static_cast<SocketLength>(sizeof(address));
            if (::bind(listener, reinterpret_cast<const sockaddr*>(&address), static_cast<SocketLength>(sizeof(address))) != 0 ||
                ::getsockname(listener, reinterpret_cast<sockaddr*>(&address), &addressLength) != 0 ||
                ::listen(listener, 64) != 0)
            {
                CloseSocket(listener);
                return;
            }

            m_listener = listener;
            m_port = ntohs(address.sin_port);
            m_acceptThread = std::thread{[this]() { AcceptLoop(); }};
        }

        Impl(const Impl&) = delete;
        Impl& operator=(const Impl&) = delete;

        ~Impl()
        {
            {
                const std::lock_guard<std::mutex> lock{m_mutex};
                m_stopping = true;
            }
            m_stopCondition.notify_all();

            if (m_listener != InvalidSocket)
            {
                // shutdown() as well as close(): on Linux, closing a socket from another thread
                // does not wake a thread blocked in accept() on it.
                ShutdownSocket(m_listener);
                CloseSocket(m_listener);
            }
            if (m_acceptThread.joinable())
            {
                m_acceptThread.join();
            }

            // No more connections are accepted now, so the list only shrinks. Wake any handler
            // blocked in recv() and wait for all of them.
            {
                const std::lock_guard<std::mutex> lock{m_mutex};
                for (const Connection& connection : m_connections)
                {
                    if (connection.socket != InvalidSocket)
                    {
                        ShutdownSocket(connection.socket);
                    }
                }
            }
            for (Connection& connection : m_connections)
            {
                if (connection.thread.joinable())
                {
                    connection.thread.join();
                }
            }
        }

        bool Valid() const
        {
            return m_listener != InvalidSocket;
        }

        uint16_t Port() const
        {
            return m_port;
        }

        static std::string ClosedPortOrigin()
        {
            if (!EnsureSocketsInitialized())
            {
                return {};
            }

            const NativeSocket reserved = ::socket(AF_INET, SOCK_STREAM, 0);
            if (reserved == InvalidSocket)
            {
                return {};
            }

            sockaddr_in address{};
            address.sin_family = AF_INET;
            address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
            address.sin_port = 0;
            SocketLength addressLength = static_cast<SocketLength>(sizeof(address));
            const bool bound =
                ::bind(reserved, reinterpret_cast<const sockaddr*>(&address), static_cast<SocketLength>(sizeof(address))) == 0 &&
                ::getsockname(reserved, reinterpret_cast<sockaddr*>(&address), &addressLength) == 0;
            CloseSocket(reserved);

            return bound ? "http://127.0.0.1:" + std::to_string(ntohs(address.sin_port)) : std::string{};
        }

    private:
#if defined(_WIN32)
        using NativeSocket = SOCKET;
        using SocketLength = int;
        using IoLength = int;
        static constexpr NativeSocket InvalidSocket = INVALID_SOCKET;

        static bool EnsureSocketsInitialized()
        {
            static const bool initialized = [] {
                WSADATA data{};
                return ::WSAStartup(MAKEWORD(2, 2), &data) == 0;
            }();
            return initialized;
        }

        static void CloseSocket(NativeSocket socket)
        {
            ::closesocket(socket);
        }

        static void ShutdownSocket(NativeSocket socket)
        {
            ::shutdown(socket, SD_BOTH);
        }

        static void ShutdownSend(NativeSocket socket)
        {
            ::shutdown(socket, SD_SEND);
        }

        static void ConfigureConnection(NativeSocket socket)
        {
            // Give up on a peer that connects and never sends a request.
            const DWORD timeoutMilliseconds = ReceiveTimeoutSeconds * 1000;
            ::setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<const char*>(&timeoutMilliseconds), static_cast<int>(sizeof(timeoutMilliseconds)));
        }

        static constexpr int SendFlags = 0;
#else
        using NativeSocket = int;
        using SocketLength = socklen_t;
        using IoLength = size_t;
        static constexpr NativeSocket InvalidSocket = -1;

        static bool EnsureSocketsInitialized()
        {
            return true;
        }

        static void CloseSocket(NativeSocket socket)
        {
            ::close(socket);
        }

        static void ShutdownSocket(NativeSocket socket)
        {
            ::shutdown(socket, SHUT_RDWR);
        }

        static void ShutdownSend(NativeSocket socket)
        {
            ::shutdown(socket, SHUT_WR);
        }

        static void ConfigureConnection(NativeSocket socket)
        {
            // Give up on a peer that connects and never sends a request.
            timeval timeout{};
            timeout.tv_sec = ReceiveTimeoutSeconds;
            ::setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, static_cast<SocketLength>(sizeof(timeout)));
#if defined(SO_NOSIGPIPE)
            // A client that aborts leaves the server writing to a closed socket; that must not
            // raise SIGPIPE in the test process. (Linux and Android use MSG_NOSIGNAL instead.)
            const int enable = 1;
            ::setsockopt(socket, SOL_SOCKET, SO_NOSIGPIPE, &enable, static_cast<SocketLength>(sizeof(enable)));
#endif
        }

#if defined(MSG_NOSIGNAL)
        static constexpr int SendFlags = MSG_NOSIGNAL;
#else
        static constexpr int SendFlags = 0;
#endif
#endif

        static constexpr int ReceiveTimeoutSeconds = 10;
        static constexpr size_t MaxRequestSize = 1024 * 1024;

        struct Connection
        {
            NativeSocket socket{InvalidSocket};
            std::thread thread{};
        };

        struct Request
        {
            std::string method{};
            std::string path{};
            std::string query{};
            std::map<std::string, std::string> parameters{};
            // Header names are lower-cased.
            std::map<std::string, std::string> headers{};
            std::string body{};
        };

        void AcceptLoop()
        {
            for (;;)
            {
                const NativeSocket accepted = ::accept(m_listener, nullptr, nullptr);

                const std::lock_guard<std::mutex> lock{m_mutex};
                if (m_stopping)
                {
                    if (accepted != InvalidSocket)
                    {
                        CloseSocket(accepted);
                    }
                    return;
                }
                if (accepted == InvalidSocket)
                {
                    continue;
                }

                ConfigureConnection(accepted);
                m_connections.emplace_back();
                Connection* connection = &m_connections.back();
                connection->socket = accepted;
                connection->thread = std::thread{[this, connection, accepted]() {
                    Serve(accepted);

                    // Mark the socket closed under the lock before closing it, so the destructor
                    // never calls shutdown() on a descriptor the process may have reused.
                    const std::lock_guard<std::mutex> closeLock{m_mutex};
                    connection->socket = InvalidSocket;
                    ShutdownSend(accepted);
                    CloseSocket(accepted);
                }};
            }
        }

        void Serve(NativeSocket socket)
        {
            Request request{};
            if (!ReadRequest(socket, request))
            {
                return;
            }

            Send(socket, Respond(request));
        }

        static bool ReadRequest(NativeSocket socket, Request& request)
        {
            std::string data{};
            size_t headerEnd = std::string::npos;
            while (headerEnd == std::string::npos)
            {
                if (data.size() > MaxRequestSize || !Receive(socket, data))
                {
                    return false;
                }
                headerEnd = data.find("\r\n\r\n");
            }

            const std::string_view head{data.data(), headerEnd};
            const size_t requestLineEnd = head.find("\r\n");
            const std::string_view requestLine = head.substr(0, requestLineEnd);
            const size_t methodEnd = requestLine.find(' ');
            const size_t targetEnd = requestLine.rfind(' ');
            if (methodEnd == std::string_view::npos || targetEnd == methodEnd)
            {
                return false;
            }

            request.method = std::string{requestLine.substr(0, methodEnd)};
            const std::string_view target = requestLine.substr(methodEnd + 1, targetEnd - methodEnd - 1);
            const size_t queryStart = target.find('?');
            request.path = std::string{target.substr(0, queryStart)};
            if (queryStart != std::string_view::npos)
            {
                request.query = std::string{target.substr(queryStart + 1)};
                ParseQuery(request.query, request.parameters);
            }

            size_t lineStart = requestLineEnd == std::string_view::npos ? head.size() : requestLineEnd + 2;
            while (lineStart < head.size())
            {
                size_t lineEnd = head.find("\r\n", lineStart);
                if (lineEnd == std::string_view::npos)
                {
                    lineEnd = head.size();
                }
                const std::string_view line = head.substr(lineStart, lineEnd - lineStart);
                const size_t colon = line.find(':');
                if (colon != std::string_view::npos)
                {
                    std::string name{line.substr(0, colon)};
                    std::transform(name.begin(), name.end(), name.begin(), [](char c) {
                        return (c >= 'A' && c <= 'Z') ? static_cast<char>(c - 'A' + 'a') : c;
                    });
                    size_t valueStart = colon + 1;
                    while (valueStart < line.size() && line[valueStart] == ' ')
                    {
                        ++valueStart;
                    }
                    request.headers[name] = std::string{line.substr(valueStart)};
                }
                lineStart = lineEnd + 2;
            }

            size_t contentLength = 0;
            const auto contentLengthHeader = request.headers.find("content-length");
            if (contentLengthHeader != request.headers.end())
            {
                contentLength = ParseNumber(contentLengthHeader->second, 0);
            }
            if (contentLength > MaxRequestSize)
            {
                return false;
            }

            const size_t bodyStart = headerEnd + 4;
            while (data.size() - bodyStart < contentLength)
            {
                if (!Receive(socket, data))
                {
                    return false;
                }
            }
            request.body = data.substr(bodyStart, contentLength);
            return true;
        }

        static bool Receive(NativeSocket socket, std::string& data)
        {
            char buffer[4096];
            const auto received = ::recv(socket, buffer, static_cast<IoLength>(sizeof(buffer)), 0);
            if (received <= 0)
            {
                return false;
            }
            data.append(buffer, static_cast<size_t>(received));
            return true;
        }

        static void Send(NativeSocket socket, const std::string& data)
        {
            size_t sent = 0;
            while (sent < data.size())
            {
                const auto result = ::send(socket, data.data() + sent, static_cast<IoLength>(data.size() - sent), SendFlags);
                if (result <= 0)
                {
                    return;
                }
                sent += static_cast<size_t>(result);
            }
        }

        static size_t ParseNumber(const std::string& text, size_t fallback)
        {
            if (text.empty())
            {
                return fallback;
            }

            size_t value = 0;
            for (const char c : text)
            {
                if (c < '0' || c > '9')
                {
                    return fallback;
                }
                value = value * 10 + static_cast<size_t>(c - '0');
            }
            return value;
        }

        static int HexValue(char c)
        {
            if (c >= '0' && c <= '9')
            {
                return c - '0';
            }
            if (c >= 'a' && c <= 'f')
            {
                return c - 'a' + 10;
            }
            if (c >= 'A' && c <= 'F')
            {
                return c - 'A' + 10;
            }
            return -1;
        }

        // Decodes application/x-www-form-urlencoded text: "%XX" escapes and "+" for a space.
        static std::string Decode(std::string_view text)
        {
            std::string decoded{};
            decoded.reserve(text.size());
            for (size_t i = 0; i < text.size(); ++i)
            {
                if (text[i] == '+')
                {
                    decoded.push_back(' ');
                }
                else if (text[i] == '%' && i + 2 < text.size() && HexValue(text[i + 1]) >= 0 && HexValue(text[i + 2]) >= 0)
                {
                    decoded.push_back(static_cast<char>(HexValue(text[i + 1]) * 16 + HexValue(text[i + 2])));
                    i += 2;
                }
                else
                {
                    decoded.push_back(text[i]);
                }
            }
            return decoded;
        }

        static void ParseQuery(std::string_view query, std::map<std::string, std::string>& parameters)
        {
            size_t start = 0;
            while (start <= query.size())
            {
                size_t end = query.find('&', start);
                if (end == std::string_view::npos)
                {
                    end = query.size();
                }
                const std::string_view pair = query.substr(start, end - start);
                if (!pair.empty())
                {
                    const size_t equals = pair.find('=');
                    std::string name = Decode(pair.substr(0, equals));
                    // The first occurrence wins, like wptserve's request.GET.first().
                    if (parameters.find(name) == parameters.end())
                    {
                        parameters[std::move(name)] = equals == std::string_view::npos ? std::string{} : Decode(pair.substr(equals + 1));
                    }
                }
                start = end + 1;
            }
        }

        static std::string Parameter(const Request& request, const std::string& name, const std::string& fallback)
        {
            const auto it = request.parameters.find(name);
            return it == request.parameters.end() ? fallback : it->second;
        }

        static std::string Header(const Request& request, const std::string& name, const std::string& fallback)
        {
            const auto it = request.headers.find(name);
            return it == request.headers.end() ? fallback : it->second;
        }

        // A response with a Content-Length, so the client knows the body size up front.
        static std::string Response(const std::string& statusLine, const std::vector<std::pair<std::string, std::string>>& headers, const std::string& body)
        {
            std::string response = "HTTP/1.1 " + statusLine + "\r\n";
            for (const auto& header : headers)
            {
                response += header.first + ": " + header.second + "\r\n";
            }
            response += "Content-Length: " + std::to_string(body.size()) + "\r\n\r\n";
            response += body;
            return response;
        }

        // Waits for `milliseconds`, or less when the server is being torn down.
        void Delay(size_t milliseconds)
        {
            std::unique_lock<std::mutex> lock{m_mutex};
            m_stopCondition.wait_for(lock, std::chrono::milliseconds{milliseconds}, [this]() { return m_stopping; });
        }

        std::string Respond(const Request& request)
        {
            const std::string& path = request.path;

            // The .asis resources are sent byte for byte. They carry no Content-Length, so the body
            // (empty here) ends where the connection closes.
            if (path == "/xhr/resources/headers.asis")
            {
                return "HTTP/1.1 200 YAYAYAYA\r\n"
                       "foo-TEST: 1\r\n"
                       "FOO-test: 2\r\n"
                       "__Custom: token\r\n"
                       "ALSO-here: Mr. PB\r\n"
                       "ewok: lego\r\n"
                       "\r\n";
            }
            if (path == "/xhr/resources/headers-basic.asis")
            {
                return "HTTP/1.1 280 HELLO\r\n"
                       "foo-test: 1\r\n"
                       "foo-test: 2\r\n"
                       "foo-test: 3\r\n"
                       "\r\n";
            }
            if (path == "/xhr/resources/headers-www-authenticate.asis")
            {
                return "HTTP/1.1 280 HELLO\r\n"
                       "www-authenticate: 1\r\n"
                       "www-authenticate: 2\r\n"
                       "www-authenticate: 3, 4\r\n"
                       "\r\n";
            }
            if (path == "/xhr/resources/header-content-length.asis")
            {
                return "HTTP/1.0 200 NANANA\r\n"
                       "CONTENT-LENGTH:  0\r\n"
                       "\r\n";
            }
            if (path == "/xhr/resources/headers-double-empty.asis")
            {
                return "HTTP/1.1 444 HI\r\n"
                       "double-trouble:\r\n"
                       "double-trouble:\r\n"
                       "\r\n";
            }

            if (path == "/xhr/resources/headers.py")
            {
                return Response("200 OK",
                    {
                        {"Content-Type", "text/plain"},
                        {"X-Custom-Header", "test"},
                        {"Set-Cookie", "test"},
                        {"Set-Cookie2", "test"},
                        {"X-Custom-Header-Empty", ""},
                        {"X-Custom-Header-Comma", "1"},
                        {"X-Custom-Header-Comma", "2"},
                    },
                    "TEST");
            }

            if (path == "/xhr/resources/status.py")
            {
                const std::string code = Parameter(request, "code", "200");
                const std::string text = Parameter(request, "text", "OMG");
                std::vector<std::pair<std::string, std::string>> headers{{"X-Request-Method", request.method}};
                const std::string type = Parameter(request, "type", "");
                if (!type.empty())
                {
                    headers.emplace_back("Content-Type", type);
                }
                return Response(code + " " + text, headers, Parameter(request, "content", ""));
            }

            if (path == "/xhr/resources/redirect.py")
            {
                if (request.parameters.find("followed") != request.parameters.end())
                {
                    return Response("200 OK", {{"Content-Type", "text/plain"}}, "MAGIC HAPPENED");
                }

                const std::string code = Parameter(request, "code", "302");
                std::string location = Parameter(request, "location", path + "?followed");
                if (location.rfind("redirect.py", 0) == 0)
                {
                    location += "&code=" + code;
                }
                Delay(ParseNumber(Parameter(request, "delay", "0"), 0));
                return Response(code + " WEBSRT MARKETING", {{"Location", location}}, "TEST");
            }

            if (path == "/xhr/resources/content.py")
            {
                const auto content = request.parameters.find("content");
                return Response("200 OK",
                    {
                        {"Content-Type", "text/plain"},
                        {"X-Request-Method", request.method},
                        {"X-Request-Query", request.query.empty() ? "NO" : request.query},
                        {"X-Request-Content-Length", Header(request, "content-length", "NO")},
                        {"X-Request-Content-Type", Header(request, "content-type", "NO")},
                    },
                    content == request.parameters.end() ? request.body : content->second);
            }

            if (path == "/xhr/resources/delay.py")
            {
                Delay(ParseNumber(Parameter(request, "ms", "500"), 500));
                return Response("200 OK", {{"Content-Type", "text/plain"}}, "TEST_DELAY");
            }

            if (path == "/xhr/resources/well-formed.xml")
            {
                return Response("200 OK", {{"Content-Type", "application/xml"}},
                    "<html xmlns=\"http://www.w3.org/1999/xhtml\">\n"
                    "  <p id=\"n&#49;\">1</p>\n"
                    "  <p xmlns=\"namespacesarejuststrings\" id=\"n2\">2</p>\n"
                    "</html>\n");
            }

            // WPT keeps the redirect count in a server-side stash keyed by a token. Carrying it in
            // the URL instead keeps this server stateless: every hop redirects to `location` with
            // the count incremented, and the hop that reaches max_count answers with the count.
            if (path == "/fetch/api/resources/redirect.py")
            {
                const size_t count = ParseNumber(Parameter(request, "count", "0"), 0);
                const auto maxCount = request.parameters.find("max_count");
                if (maxCount != request.parameters.end() && count >= ParseNumber(maxCount->second, 0))
                {
                    return Response("200 OK", {{"Content-Type", "text/plain"}}, std::to_string(count));
                }

                const std::string status = Parameter(request, "redirect_status", "302");
                std::vector<std::pair<std::string, std::string>> headers{{"Content-Type", "text/plain"}, {"Cache-Control", "no-cache"}};
                const auto location = request.parameters.find("location");
                if (location != request.parameters.end())
                {
                    std::string url = location->second;
                    if (maxCount != request.parameters.end())
                    {
                        url += url.find('?') == std::string::npos ? "?" : "&";
                        url += "redirect_status=" + status + "&max_count=" + maxCount->second + "&location=" + Encode(location->second) + "&count=" + std::to_string(count + 1);
                    }
                    headers.emplace_back("Location", url);
                }
                return Response(status + " Redirect", headers, "");
            }

            return Response("404 Not Found", {{"Content-Type", "text/plain"}}, "Not Found");
        }

        static std::string Encode(std::string_view text)
        {
            static constexpr char Hex[] = "0123456789ABCDEF";
            std::string encoded{};
            for (const char c : text)
            {
                const bool unreserved = (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '-' || c == '_' || c == '.' || c == '~';
                if (unreserved)
                {
                    encoded.push_back(c);
                }
                else
                {
                    const auto byte = static_cast<unsigned char>(c);
                    encoded.push_back('%');
                    encoded.push_back(Hex[byte >> 4]);
                    encoded.push_back(Hex[byte & 0x0F]);
                }
            }
            return encoded;
        }

        NativeSocket m_listener{InvalidSocket};
        uint16_t m_port{0};
        std::thread m_acceptThread{};

        std::mutex m_mutex{};
        std::condition_variable m_stopCondition{};
        bool m_stopping{false};
        // std::list so a Connection's address stays valid while later ones are added.
        std::list<Connection> m_connections{};
    };

    LoopbackHttpServer::LoopbackHttpServer()
        : m_impl{std::make_unique<Impl>()}
    {
    }

    LoopbackHttpServer::~LoopbackHttpServer() = default;

    bool LoopbackHttpServer::Valid() const
    {
        return m_impl->Valid();
    }

    uint16_t LoopbackHttpServer::Port() const
    {
        return m_impl->Port();
    }

    std::string LoopbackHttpServer::Origin() const
    {
        return "http://127.0.0.1:" + std::to_string(m_impl->Port());
    }

    std::string LoopbackHttpServer::ClosedPortOrigin()
    {
        return Impl::ClosedPortOrigin();
    }
}
