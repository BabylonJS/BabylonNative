#include "WebSocket_Base.h"
#include <AndroidExtensions/Globals.h>
#include <AndroidExtensions/JavaWrappers.h>

using namespace android::global;
using namespace android::net;
using namespace java::lang;
using namespace java::io;
using namespace java::net;
using namespace java::websocket;

namespace UrlLib
{
    class WebSocket::Impl : public ImplBase
    {
    public:
        Impl(std::string url, std::function<void()> onopen, std::function<void()> onclose, std::function<void(std::string)> onmessage, std::function<void()> onerror)
            : ImplBase{url, onopen, onclose, onmessage, onerror}
            , m_webSocket{url, onopen, onclose, onmessage, onerror}
        {
        }

        void Open()
        {
            m_webSocket.Open();
        }

        void Send(std::string message)
        {
            m_webSocket.Send(message);
        }

        void Close()
        {
            m_webSocket.Close();
        }

    private:
        WebSocketClient m_webSocket;
    };
}

#include "WebSocket_Shared.h"
