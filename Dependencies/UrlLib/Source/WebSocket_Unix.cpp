#include "WebSocket_Base.h"

namespace UrlLib
{
    class WebSocket::Impl : public ImplBase
    {
    public:
        Impl(const std::string& url, std::function<void()> onOpen, std::function<void(int, const std::string&)> onClose, std::function<void(const std::string&)> onMessage, std::function<void(const std::string&)> onError)
            : ImplBase{url, onOpen, onClose, onMessage, onError}
        {
        }

        ~Impl()
        {
        }
        
        void Send(std::string /*message*/)
        {
            throw std::runtime_error{"Web socket not implemented for Unix"};
        }

        void Open()
        {
            throw std::runtime_error{"Web socket not implemented for Unix"};
        }

        void Close()
        {
            throw std::runtime_error{"Web socket not implemented for Unix"};
        }
    };
}

#include "WebSocket_Shared.h"
