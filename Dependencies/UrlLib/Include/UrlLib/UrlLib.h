#pragma once

#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <arcana/threading/task.h>
#include <unordered_map>

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

        UrlResponseType ResponseType() const;

        void ResponseType(UrlResponseType value);

        arcana::task<void, std::exception_ptr> SendAsync();

        void SetRequestBody(std::string requestBody);

        void SetRequestHeader(std::string name, std::string value);

        std::optional<std::string> GetResponseHeader(const std::string& headerName) const;

        const std::unordered_map<std::string, std::string>& GetAllResponseHeaders() const;
        
        UrlStatusCode StatusCode() const;

        std::string_view ResponseUrl() const;

        std::string_view ResponseString() const;

        gsl::span<const std::byte> ResponseBuffer() const;

    private:
        class Impl;
        class ImplBase;

        std::shared_ptr<Impl> m_impl{};
    };
}
