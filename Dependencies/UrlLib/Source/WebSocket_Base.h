#pragma once

#include "UrlLib/UrlLib.h"

namespace UrlLib
{
    class WebSocket::ImplBase
    {
    public:
    ImplBase(const std::string& url, std::function<void()> onOpen, std::function<void(int, const std::string&)> onClose, std::function<void(const std::string&)> onMessage, std::function<void(const std::string&)> onError)
        : m_url(url)
    {
        m_onOpen = onOpen;
        m_onClose = onClose;
        m_onMessage = onMessage;
        m_onError = onError;
    }

    protected:
        const std::string m_url;
        std::function<void()> m_onOpen;
        std::function<void(int, const std::string&)> m_onClose;
        std::function<void(const std::string&)> m_onMessage;
        std::function<void(const std::string&)> m_onError;
    };
}
