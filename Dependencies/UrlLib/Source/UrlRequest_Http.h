// HTTP response rules shared by the platform backends. They take and return plain strings, with
// no platform or UrlRequest types, so they can be unit tested on their own (Tests/HttpSemantics.cpp).

#pragma once

#include <algorithm>
#include <string>
#include <string_view>
#include <unordered_map>

namespace UrlLib::Http
{
    // What a backend knows about the reason phrase of the response's status line.
    enum class ReasonPhrase
    {
        // The platform API does not report the phrase (NSURLSession, HttpURLConnection through the
        // Android wrapper), or the response did not come from HTTP (file://, app://, a scheme
        // resolver). The status text is then the phrase the caller supplied if there is one, and
        // otherwise the standard phrase for the status code.
        Unknown,
        // The backend read the phrase from an HTTP/1.x status line. It is reported as it was sent,
        // including a non-standard or an empty one.
        OnWire,
        // The response arrived over HTTP/2 or HTTP/3, which have no reason phrase.
        None,
    };

    // Standard reason phrases, for a response whose phrase the platform does not report. Returns ""
    // for codes not in the table.
    inline std::string_view StandardReasonPhrase(int statusCode)
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

    // The status text of a response (the "status message" of the Fetch standard), given what the
    // backend knows about its reason phrase. `phrase` must outlive the returned view.
    inline std::string_view StatusText(ReasonPhrase reasonPhrase, std::string_view phrase, int statusCode)
    {
        switch (reasonPhrase)
        {
            case ReasonPhrase::None:
                return "";
            case ReasonPhrase::OnWire:
                return phrase;
            case ReasonPhrase::Unknown:
                break;
        }

        return phrase.empty() ? StandardReasonPhrase(statusCode) : phrase;
    }

    // Whether an ALPN protocol name, as NSURLSessionTaskTransactionMetrics.networkProtocolName
    // reports it ("http/1.1", "h2", "h3", and drafts such as "h3-29"), is HTTP/2 or later.
    inline bool IsHttp2OrLater(std::string_view protocolName)
    {
        return protocolName.size() >= 2 && protocolName[0] == 'h' && protocolName[1] >= '2' && protocolName[1] <= '9';
    }

    // The parts of a status line ("HTTP/1.1 200 OK", "HTTP/2 200") that decide the status text.
    struct StatusLine
    {
        // 0 when the line is not a status line.
        int majorVersion{0};
        ReasonPhrase reasonPhrase{ReasonPhrase::Unknown};
        std::string_view phrase{};
    };

    // Parses a status line as libcurl hands it to the header callback, with or without its line
    // ending. libcurl writes "HTTP/2 200" and "HTTP/3 200" for responses that have no status line
    // on the wire. `line` must outlive the result.
    inline StatusLine ParseStatusLine(std::string_view line)
    {
        constexpr std::string_view prefix{"HTTP/"};
        if (line.substr(0, prefix.size()) != prefix || line.size() == prefix.size() || line[prefix.size()] < '0' || line[prefix.size()] > '9')
        {
            return {};
        }

        const auto isSpace = [](char c) { return c == ' ' || c == '\t'; };
        const auto isLineEnd = [](char c) { return c == '\r' || c == '\n'; };

        StatusLine statusLine{};
        statusLine.majorVersion = line[prefix.size()] - '0';

        size_t cursor = prefix.size();
        for (; cursor < line.size() && !isSpace(line[cursor]) && !isLineEnd(line[cursor]); ++cursor) {} // version
        for (; cursor < line.size() && isSpace(line[cursor]); ++cursor) {}
        for (; cursor < line.size() && !isSpace(line[cursor]) && !isLineEnd(line[cursor]); ++cursor) {} // status code
        for (; cursor < line.size() && isSpace(line[cursor]); ++cursor) {}

        size_t end = line.size();
        for (; end > cursor && (isSpace(line[end - 1]) || isLineEnd(line[end - 1])); --end) {}

        if (statusLine.majorVersion >= 2)
        {
            statusLine.reasonPhrase = ReasonPhrase::None;
        }
        else
        {
            statusLine.reasonPhrase = ReasonPhrase::OnWire;
            statusLine.phrase = line.substr(cursor, end - cursor);
        }

        return statusLine;
    }

    // A URL without its fragment, as the "response URL" of XMLHttpRequest and fetch is serialized.
    inline std::string_view WithoutFragment(std::string_view url)
    {
        return url.substr(0, url.find('#'));
    }

    inline std::string ToLower(std::string_view text)
    {
        std::string lower{text};
        std::transform(lower.begin(), lower.end(), lower.begin(), [](char c) {
            return (c >= 'A' && c <= 'Z') ? static_cast<char>(c - 'A' + 'a') : c;
        });
        return lower;
    }

    // Adds a response header to `headers`, which is keyed by lower-cased name. A name that is
    // already present gets the value appended after ", ", in the order the values arrive, which is
    // how the Fetch standard combines the values of a repeated header.
    inline void AddHeader(std::unordered_map<std::string, std::string>& headers, std::string_view name, std::string_view value)
    {
        const auto [it, inserted] = headers.try_emplace(ToLower(name), value);
        if (!inserted)
        {
            it->second.append(", ");
            it->second.append(value);
        }
    }
}
