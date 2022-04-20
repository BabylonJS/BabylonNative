// Shared pimpl code (not an actual header)

namespace UrlLib
{
    UrlRequest::UrlRequest()
        : m_impl{std::make_unique<Impl>()}
    {
    }

    UrlRequest::~UrlRequest() = default;

    // Copy semantics
    UrlRequest::UrlRequest(const UrlRequest&) noexcept = default;
    UrlRequest& UrlRequest::operator=(const UrlRequest&) noexcept = default;

    // Move semantics
    UrlRequest::UrlRequest(UrlRequest&&) noexcept = default;
    UrlRequest& UrlRequest::operator=(UrlRequest&&) noexcept = default;

    void UrlRequest::Abort()
    {
        m_impl->Abort();
    }

    void UrlRequest::Open(UrlMethod method, std::string url)
    {
        m_impl->Open(method, std::move(url));
    }

    UrlResponseType UrlRequest::ResponseType() const
    {
        return m_impl->ResponseType();
    }

    void UrlRequest::ResponseType(UrlResponseType value)
    {
        m_impl->ResponseType(value);
    }

    arcana::task<void, std::exception_ptr> UrlRequest::SendAsync()
    {
        return m_impl->SendAsync();
    }

    UrlStatusCode UrlRequest::StatusCode() const
    {
        return m_impl->StatusCode();
    }

    std::string_view UrlRequest::ResponseUrl() const
    {
        return m_impl->ResponseUrl();
    }

    std::string_view UrlRequest::ResponseString() const
    {
        return m_impl->ResponseString();
    }

    gsl::span<const std::byte> UrlRequest::ResponseBuffer() const
    {
        return m_impl->ResponseBuffer();
    }
}
