// Shared pimpl code (not an actual header)

namespace UrlLib
{
    UrlRequest::UrlRequest()
        : m_impl{std::make_unique<Impl>()}
    {
    }

    UrlRequest::~UrlRequest() = default;

    // Copy semantics
    UrlRequest::UrlRequest(const UrlRequest&) = default;
    UrlRequest& UrlRequest::operator=(const UrlRequest&) = default;

    // Move semantics
    UrlRequest::UrlRequest(UrlRequest&&) noexcept = default;
    UrlRequest& UrlRequest::operator=(UrlRequest&&) noexcept = default;

    void UrlRequest::Abort()
    {
        m_impl->Abort();
    }

    void UrlRequest::Open(UrlMethod method, const std::string& url)
    {
        m_impl->Open(method, url);
    }

    UrlResponseType UrlRequest::ResponseType() const
    {
        return m_impl->ResponseType();
    }

    void UrlRequest::ResponseType(UrlResponseType value)
    {
        m_impl->ResponseType(value);
    }

    void UrlRequest::SetRequestBody(std::string requestBody)
    {
        m_impl->SetRequestBody(requestBody);
    }

    void UrlRequest::SetRequestHeader(std::string key, std::string value)
    {
        m_impl->SetRequestHeader(key, value);
    }

    const std::unordered_map<std::string, std::string>& UrlRequest::GetAllResponseHeaders() const
    {
        return m_impl->GetAllResponseHeaders();
    }

    std::optional<std::string> UrlRequest::GetResponseHeader(const std::string& headerName) const
    {
        return m_impl->GetResponseHeader(headerName);
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
