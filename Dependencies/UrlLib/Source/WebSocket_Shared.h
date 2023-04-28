namespace UrlLib
{
    WebSocket::WebSocket(std::string url, std::function<void()> onOpen, std::function<void()> onClose, std::function<void(std::string)> onMessage, std::function<void()> onError)
    : m_impl{std::make_unique<WebSocket::Impl>(url, onOpen, onClose, onMessage, onError)}
    {
    }

    WebSocket::~WebSocket() = default;

    // Copy semantics
    WebSocket::WebSocket(const WebSocket&) = default;
    WebSocket& WebSocket::operator=(const WebSocket&) = default;

    // Move semantics
    WebSocket::WebSocket(WebSocket&&) noexcept = default;
    WebSocket& WebSocket::operator=(WebSocket&&) noexcept = default;

    void WebSocket::Close()
    {
        m_impl->Close();
    }

    void WebSocket::Send(std::string message)
    {
        m_impl->Send(message);
    }

    void WebSocket::Open()
    {
        m_impl->Open();
    }
}
