#include "WebSocket_Base.h"
#include "WebSocket_Apple_ObjC.h"

namespace UrlLib
{
class API_AVAILABLE(ios(13.0)) WebSocket::Impl : public ImplBase
    {
    public:
        Impl(std::string url, std::function<void()> onOpen, std::function<void()> onClose, std::function<void(std::string)> onMessage, std::function<void()> onError)
        : ImplBase{url, onOpen, onClose, onMessage, onError}
        {
            void (^openCallback)() =  [this]()
            {
                m_onOpen();
            };
            void (^closeCallback)() =  [this]()
            {
                m_onClose();
            };
            void (^messageCallback)(NSString* messageStr) =  [this](NSString* messageStr)
            {
                std::string cppString( [messageStr UTF8String] );
                m_onMessage(cppString);
            };
            void (^errorCallback)() =  [this]()
            {
                m_onError();
            };

            webSocket = [[WebSocket_ObjC alloc] initWithCallbacks:@(url.data()) onOpen:openCallback onClose:closeCallback onMessage:messageCallback onError:errorCallback];
        }

        void Open()
        {
            [webSocket open];
        }
        
        void Send(std::string message)
        {
            [webSocket sendMessage:@(message.data())];
        }

        void Close()
        {
            [webSocket close];
        }
 
    private:
        WebSocket_ObjC *webSocket;
    };
}

#include "WebSocket_Shared.h"
