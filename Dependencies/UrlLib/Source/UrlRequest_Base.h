#pragma once

#include <UrlLib/UrlLib.h>
#include <arcana/threading/cancellation.h>
#include <string>
#include <cctype>
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

        void SetRequestBody(std::string requestBody) {
            m_requestBody = requestBody;
        }

        void SetRequestHeader(std::string name, std::string value)
        {
            m_requestHeaders[name] = value;
        }

        const std::unordered_map<std::string, std::string>& GetAllResponseHeaders() const
        {
            return m_headers;
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

    protected:
        static std::string ToLower(const char* str)
        {
            std::string s{str};
            std::transform(s.cbegin(), s.cend(), s.begin(), [](auto c) { return static_cast<decltype(c)>(std::tolower(c)); });
            return s;
        }
        
        static void ToLower(std::string& s)
        {
            std::transform(s.cbegin(), s.cend(), s.begin(), [](auto c) { return static_cast<decltype(c)>(std::tolower(c)); });
        }

        arcana::cancellation_source m_cancellationSource{};
        UrlResponseType m_responseType{UrlResponseType::String};
        UrlMethod m_method{UrlMethod::Get};
        UrlStatusCode m_statusCode{UrlStatusCode::None};
        std::string m_responseUrl{};
        std::string m_responseString{};
        std::unordered_map<std::string, std::string> m_headers;
        std::string m_requestBody{};
        std::unordered_map<std::string, std::string> m_requestHeaders;
    };
}
