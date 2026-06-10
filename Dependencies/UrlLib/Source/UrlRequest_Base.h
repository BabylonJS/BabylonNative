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

        // Returns the HTTP status reason phrase. Prefers the phrase carried on the wire
        // (HTTP/1.x status line, captured by the platform backend into `m_statusText`).
        // When the transport carries no reason phrase (HTTP/2+, or a backend that does not
        // expose it), falls back to a canonical code->text table. Returns "" for unknown codes.
        std::string_view StatusText() const
        {
            if (!m_statusText.empty())
            {
                return m_statusText;
            }

            return ReasonPhraseForStatusCode(static_cast<int>(m_statusCode));
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

        // Canonical HTTP reason phrases, used as a fallback when the transport does not carry
        // a reason phrase on the wire (HTTP/2+ status lines omit it, and some platform HTTP
        // stacks don't surface it). Returns "" for codes not in the table.
        static std::string_view ReasonPhraseForStatusCode(int statusCode)
        {
            switch (statusCode)
            {
                case 100: return "Continue";
                case 101: return "Switching Protocols";
                case 200: return "OK";
                case 201: return "Created";
                case 202: return "Accepted";
                case 203: return "Non-Authoritative Information";
                case 204: return "No Content";
                case 205: return "Reset Content";
                case 206: return "Partial Content";
                case 300: return "Multiple Choices";
                case 301: return "Moved Permanently";
                case 302: return "Found";
                case 303: return "See Other";
                case 304: return "Not Modified";
                case 307: return "Temporary Redirect";
                case 308: return "Permanent Redirect";
                case 400: return "Bad Request";
                case 401: return "Unauthorized";
                case 402: return "Payment Required";
                case 403: return "Forbidden";
                case 404: return "Not Found";
                case 405: return "Method Not Allowed";
                case 406: return "Not Acceptable";
                case 407: return "Proxy Authentication Required";
                case 408: return "Request Timeout";
                case 409: return "Conflict";
                case 410: return "Gone";
                case 411: return "Length Required";
                case 412: return "Precondition Failed";
                case 413: return "Payload Too Large";
                case 414: return "URI Too Long";
                case 415: return "Unsupported Media Type";
                case 416: return "Range Not Satisfiable";
                case 417: return "Expectation Failed";
                case 426: return "Upgrade Required";
                case 428: return "Precondition Required";
                case 429: return "Too Many Requests";
                case 431: return "Request Header Fields Too Large";
                case 451: return "Unavailable For Legal Reasons";
                case 500: return "Internal Server Error";
                case 501: return "Not Implemented";
                case 502: return "Bad Gateway";
                case 503: return "Service Unavailable";
                case 504: return "Gateway Timeout";
                case 505: return "HTTP Version Not Supported";
                case 511: return "Network Authentication Required";
                default: return "";
            }
        }

        // Reset the per-request response state that lives in ImplBase. Each platform's `Open()`
        // calls this at the start so a single `UrlRequest` can be reused across requests without
        // leaking prior status / URL / body / headers. Platform-specific response buffers live in
        // each `Impl` (different types per backend) and must be reset by the platform's `Open()`
        // alongside this call.
        void ResetForOpen()
        {
            m_statusCode = UrlStatusCode::None;
            m_statusText.clear();
            m_responseUrl.clear();
            m_responseString.clear();
            m_headers.clear();
        }

        arcana::cancellation_source m_cancellationSource{};
        UrlResponseType m_responseType{UrlResponseType::String};
        UrlMethod m_method{UrlMethod::Get};
        UrlStatusCode m_statusCode{UrlStatusCode::None};
        std::string m_statusText{};
        std::string m_responseUrl{};
        std::string m_responseString{};
        std::unordered_map<std::string, std::string> m_headers;
        std::string m_requestBody{};
        std::unordered_map<std::string, std::string> m_requestHeaders;
    };
}
