namespace UrlLib
{
    WebSocket::WebSocket(const std::string& url, std::function<void()> onOpen, std::function<void(int, const std::string&)> onClose, std::function<void(const std::string&)> onMessage, std::function<void(const std::string&)> onError)
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
