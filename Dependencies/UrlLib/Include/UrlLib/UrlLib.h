#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <arcana/threading/task.h>
#include <unordered_map>
#include <vector>

namespace UrlLib
{
    enum class UrlStatusCode
    {
        None = 0,
        Ok = 200,
    };

    enum class UrlMethod
    {
        Get,
        Post
    };

    enum class UrlResponseType
    {
        String,
        Buffer,
    };

    // Result returned by a custom URL scheme resolver (see UrlRequest::RegisterSchemeResolver).
    // When `handled` is false the URL had no live entry (e.g. a revoked blob: URL) and the request
    // surfaces as a network error (status stays 0/None), mirroring the transport-failure contract.
    struct UrlSchemeResolverResult
    {
        bool handled{false};
        UrlStatusCode statusCode{UrlStatusCode::None};
        std::string statusText{};
        std::string contentType{};
        std::shared_ptr<const std::vector<std::byte>> body{};
    };

    // Resolves a URL of a registered non-transport scheme (e.g. "blob") to an in-memory response.
    using UrlSchemeResolver = std::function<UrlSchemeResolverResult(const std::string& url)>;

    class UrlRequest final
    {
    public:
        UrlRequest();
        ~UrlRequest();

        // Copy semantics
        UrlRequest(const UrlRequest&);
        UrlRequest& operator=(const UrlRequest&);

        // Move semantics
        UrlRequest(UrlRequest&&) noexcept;
        UrlRequest& operator=(UrlRequest&&) noexcept;

        void Abort();

        void Open(UrlMethod method, const std::string& url);

        // Registers a resolver for a non-transport URL scheme such as "blob". Registration is
        // process-global. When a UrlRequest is opened with a URL whose scheme has a registered
        // resolver, the platform transport is bypassed and the resolver supplies the response at
        // SendAsync() time, so every consumer (fetch, XMLHttpRequest, image / video src, texture
        // loaders, ...) resolves such URLs uniformly through UrlRequest instead of each carrying
        // its own branch.
        //
        // A resolver that reports the URL as not handled -- or that throws -- surfaces as a
        // transport-style failure (status stays None, with an error symbol recorded); an exception
        // never escapes SendAsync().
        //
        // Throws std::invalid_argument if `scheme` is empty or `resolver` is null. Removal is the
        // explicit UnregisterSchemeResolver call rather than registering an empty std::function, so
        // a moved-from or default-constructed resolver cannot silently unregister a scheme.
        static void RegisterSchemeResolver(std::string scheme, UrlSchemeResolver resolver);

        // Removes the resolver registered for `scheme`, if any. Unknown schemes are ignored.
        // In-flight requests already diverted to that resolver are unaffected: each request holds
        // its own reference to the resolver from the time it was opened.
        static void UnregisterSchemeResolver(std::string scheme);

        UrlResponseType ResponseType() const;

        void ResponseType(UrlResponseType value);

        arcana::task<void, std::exception_ptr> SendAsync();

        void SetRequestBody(std::string requestBody);

        void SetRequestHeader(std::string name, std::string value);

        std::optional<std::string> GetResponseHeader(const std::string& headerName) const;

        const std::unordered_map<std::string, std::string>& GetAllResponseHeaders() const;
        
        UrlStatusCode StatusCode() const;

        std::string_view StatusText() const;

        // Transport-level error reporting. ErrorString() and ErrorSymbol() are empty (and
        // ErrorCode() is 0) when the request did not fail at the transport layer; an HTTP
        // error status like 404 is NOT a transport failure. ErrorString() is normalized for
        // log-pipeline filtering:
        //   "<domain>:<symbol>(<code>): <detail>"
        // where <domain> and <symbol> are stable ASCII tokens (e.g. "curl",
        // "CURLE_COULDNT_CONNECT") and <detail> is the platform's human-readable message.
        //
        // To detect a transport failure, test whether ErrorString() (or ErrorSymbol()) is
        // non-empty -- NOT whether ErrorCode() != 0. A genuine failure can carry a zero code
        // (e.g. a missing app:/// resource reports "AppResourceNotFound" with code 0).
        std::string_view ErrorString() const;

        // The stable, platform-specific symbolic name of the failure, e.g.
        // "CURLE_COULDNT_RESOLVE_HOST" or "NSURLErrorTimedOut". Empty when the request did not
        // fail at the transport layer.
        std::string_view ErrorSymbol() const;

        // The raw numeric platform error code (CURLcode, NSError code, etc.). May be 0 even for
        // a genuine failure (e.g. a missing app:/// resource); use ErrorString()/ErrorSymbol()
        // emptiness as the failure signal rather than this value.
        int32_t ErrorCode() const;

        std::string_view ResponseUrl() const;

        std::string_view ResponseString() const;

        gsl::span<const std::byte> ResponseBuffer() const;

    private:
        class Impl;
        class ImplBase;

        std::shared_ptr<Impl> m_impl{};
    };

    class WebSocket final
    {
    public:
        WebSocket(const std::string& url, std::function<void()> onOpen, std::function<void(int, const std::string&)> onClose, std::function<void(const std::string&)> onMessage, std::function<void(const std::string&)> onError);
        ~WebSocket();

        // Copy semantics
        WebSocket(const WebSocket&);
        WebSocket& operator=(const WebSocket&);

        // Move semantics
        WebSocket(WebSocket&&) noexcept;
        WebSocket& operator=(WebSocket&&) noexcept;

        std::string GetURL();

        void Open();
        void Close();
        void Send(std::string message);

    private:
        class Impl;
        class ImplBase;

        std::shared_ptr<Impl> m_impl{};
    };
}
