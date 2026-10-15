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
        // Divert URLs whose scheme has a registered resolver (e.g. blob:) away from the platform
        // transport; the resolver supplies the response in SendAsync().
        if (m_impl->BeginSchemeResolution(url))
        {
            return;
        }

        m_impl->Open(method, url);
    }

    void UrlRequest::RegisterSchemeResolver(std::string scheme, UrlSchemeResolver resolver)
    {
        Impl::RegisterSchemeResolver(std::move(scheme), std::move(resolver));
    }

    void UrlRequest::UnregisterSchemeResolver(std::string scheme)
    {
        Impl::UnregisterSchemeResolver(std::move(scheme));
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
        // Registered-scheme requests (e.g. blob:) are served synchronously from the resolver; the
        // resolution is deferred to here (rather than Open) so a blob: URL revoked between open()
        // and send() is honored.
        if (m_impl->IsSchemeResolution())
        {
            m_impl->ResolveScheme();
            return arcana::task_from_result<std::exception_ptr>();
        }

        return m_impl->SendAsync();
    }

    UrlStatusCode UrlRequest::StatusCode() const
    {
        return m_impl->StatusCode();
    }

    std::string_view UrlRequest::StatusText() const
    {
        return m_impl->StatusText();
    }

    std::string_view UrlRequest::ErrorString() const
    {
        return m_impl->ErrorString();
    }

    std::string_view UrlRequest::ErrorSymbol() const
    {
        return m_impl->ErrorSymbol();
    }

    int32_t UrlRequest::ErrorCode() const
    {
        return m_impl->ErrorCode();
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
        if (m_impl->IsSchemeResolution())
        {
            return m_impl->ResolvedResponseBuffer();
        }

        return m_impl->ResponseBuffer();
    }
}
