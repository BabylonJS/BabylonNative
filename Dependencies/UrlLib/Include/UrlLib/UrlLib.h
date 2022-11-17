#pragma once

#include <memory>
#include <optional>
#include <string>
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

        void Open(UrlMethod method, const std::string& url);

        UrlResponseType ResponseType() const;

        void ResponseType(UrlResponseType value);

        arcana::task<void, std::exception_ptr> SendAsync();

        UrlStatusCode StatusCode() const;

        std::string_view ResponseUrl() const;

        std::string_view ResponseString() const;

        gsl::span<const std::byte> ResponseBuffer() const;

        std::optional<std::string> GetResponseHeader(const std::string& headerName) const;

    private:
        class Impl;
        class ImplBase;

        std::shared_ptr<Impl> m_impl{};
    };
}
