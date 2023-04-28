#pragma once

#include "UrlLib/UrlLib.h"

namespace UrlLib
{
    class WebSocket::ImplBase
    {
    public:
    ImplBase(std::string url, std::function<void()> onOpen, std::function<void()> onClose, std::function<void(std::string)> onMessage, std::function<void()> onError)
    {
        m_url = url;
        m_onOpen = onOpen;
        m_onClose = onClose;
        m_onMessage = onMessage;
        m_onError = onError;
    }

    protected:
        std::string m_url;
        std::function<void()> m_onOpen;
        std::function<void()> m_onClose;
        std::function<void(std::string)> m_onMessage;
        std::function<void()> m_onError;
    };
}
