#pragma once

#include <memory>
#include <string_view>
#include <arcana/threading/task.h>

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

        void Open(UrlMethod method, std::string url);

        UrlResponseType ResponseType() const;

        void ResponseType(UrlResponseType value);

        arcana::task<void, std::exception_ptr> SendAsync();

        UrlStatusCode StatusCode() const;

        std::string_view ResponseUrl() const;

        std::string_view ResponseString() const;

        std::string ResponseHeader(const std::string& headerName) const;

        std::string ResponseHeaders() const;

        gsl::span<const std::byte> ResponseBuffer() const;

    private:
        class Impl;
        std::shared_ptr<Impl> m_impl{};
    };
}
