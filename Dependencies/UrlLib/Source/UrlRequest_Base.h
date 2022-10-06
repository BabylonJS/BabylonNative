#pragma once

#include <UrlLib/UrlLib.h>
#include <arcana/threading/cancellation.h>
#include <string>
#include <unordered_map>

namespace UrlLib
{
    class UrlRequest::ImplBase
    {
    public:
        ~ImplBase()
        {
            Abort();
        }

        void Abort()
        {
            m_cancellationSource.cancel();
        }

        UrlResponseType ResponseType() const
        {
            return m_responseType;
        }

        void ResponseType(UrlResponseType value)
        {
            m_responseType = value;
        }

        UrlStatusCode StatusCode() const
        {
            return m_statusCode;
        }

        std::string_view ResponseUrl()
        {
            return m_responseUrl;
        }

        std::string_view ResponseString()
        {
            return m_responseString;
        }

        std::optional<std::string> GetResponseHeader(const std::string& headerName) const
        {
            const auto it = m_headers.find(ToLower(headerName.data()));
            if (it == m_headers.end())
            {
                return {};
            }

            return it->second;
        }

    protected:
        static std::string ToLower(const char* str)
        {
            std::string s{str};
            std::transform(s.cbegin(), s.cend(), s.begin(), [](auto c) { return std::tolower(c); });
            return s;
        }
        
        arcana::cancellation_source m_cancellationSource{};
        UrlResponseType m_responseType{UrlResponseType::String};
        UrlMethod m_method{UrlMethod::Get};
        UrlStatusCode m_statusCode{UrlStatusCode::None};
        std::string m_responseUrl{};
        std::string m_responseString{};
        std::unordered_map<std::string, std::string> m_headers;
    };
}
