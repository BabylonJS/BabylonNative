#include "URL.h"
#include <sstream>
#include <regex>
#include <optional>

// NOTE: This is a platform agnostic implementation created with a lot of help from AI :)
//       In the future, we may want to consider using platform-specific URL parsing APIs instead.

namespace
{
    // Parsed URL components
    struct URLParts
    {
        std::string protocol;   // e.g., "https:"
        std::string username;   // e.g., "user"
        std::string password;   // e.g., "pass"
        std::string hostname;   // e.g., "example.com"
        std::string port;       // e.g., "8080"
        std::string pathname;   // e.g., "/path/to/resource"
        std::string search;     // e.g., "?query=value"
        std::string hash;       // e.g., "#fragment"
    };

    // URL format: scheme:[//[user:password@]host[:port]][/]path[?query][#fragment]
    // Regex breakdown:
    // ^([a-zA-Z][a-zA-Z0-9+.-]*):   - protocol/scheme (group 1)
    // (?:\/\/                       - optional authority starting with //
    //   (?:([^:@\/]*)               - username (group 2)
    //     (?::([^@\/]*))?           - password (group 3)
    //   @)?                         - end of credentials
    //   ([^:\/?#]*)                 - hostname (group 4)
    //   (?::(\d*))?                 - port (group 5)
    // )?                            - end of authority
    // ([^?#]*)                      - pathname (group 6)
    // (?:\?([^#]*))?                - query/search (group 7)
    // (?:#(.*))?$                   - fragment/hash (group 8)
    std::optional<URLParts> TryParseURL(const std::string& urlString)
    {
        static const std::regex urlRegex(
            R"(^([a-zA-Z][a-zA-Z0-9+.-]*):(?:\/\/(?:([^:@\/?#]*)(?::([^@\/?#]*))?@)?([^:\/?#]*)(?::(\d*))?)?([^?#]*)(?:\?([^#]*))?(?:#(.*))?$)",
            std::regex::ECMAScript
        );

        std::smatch match;
        if (!std::regex_match(urlString, match, urlRegex))
        {
            return std::nullopt;
        }

        URLParts parts;
        parts.protocol = match[1].str() + ":";
        parts.username = match[2].str();
        parts.password = match[3].str();
        parts.hostname = match[4].str();
        parts.port = match[5].str();
        parts.pathname = match[6].str();

        // Ensure pathname starts with / for URLs with authority
        if (!parts.hostname.empty() && (parts.pathname.empty() || parts.pathname[0] != '/'))
        {
            parts.pathname = "/" + parts.pathname;
        }

        // Store search with leading ? if present
        if (match[7].matched && !match[7].str().empty())
        {
            parts.search = "?" + match[7].str();
        }

        // Store hash with leading # if present
        if (match[8].matched && !match[8].str().empty())
        {
            parts.hash = "#" + match[8].str();
        }

        return parts;
    }

    // Remove dot segments from path (implements RFC 3986 remove_dot_segments)
    std::string RemoveDotSegments(const std::string& path)
    {
        std::string output;
        std::string input = path;

        while (!input.empty())
        {
            // A: If the input buffer begins with a prefix of "../" or "./"
            if (input.substr(0, 3) == "../")
            {
                input = input.substr(3);
            }
            else if (input.substr(0, 2) == "./")
            {
                input = input.substr(2);
            }
            // B: If the input buffer begins with a prefix of "/./" or "/."
            else if (input.substr(0, 3) == "/./")
            {
                input = "/" + input.substr(3);
            }
            else if (input == "/.")
            {
                input = "/";
            }
            // C: If the input buffer begins with a prefix of "/../" or "/.."
            else if (input.substr(0, 4) == "/../")
            {
                input = "/" + input.substr(4);
                // Remove last segment from output
                size_t lastSlash = output.rfind('/');
                if (lastSlash != std::string::npos)
                {
                    output = output.substr(0, lastSlash);
                }
            }
            else if (input == "/..")
            {
                input = "/";
                // Remove last segment from output
                size_t lastSlash = output.rfind('/');
                if (lastSlash != std::string::npos)
                {
                    output = output.substr(0, lastSlash);
                }
            }
            // D: If the input buffer consists only of "." or ".."
            else if (input == "." || input == "..")
            {
                input = "";
            }
            // E: Move the first path segment (including initial "/" if any) to output
            else
            {
                size_t segStart = 0;
                if (input[0] == '/')
                {
                    segStart = 1;
                }
                size_t segEnd = input.find('/', segStart);
                if (segEnd == std::string::npos)
                {
                    output += input;
                    input = "";
                }
                else
                {
                    output += input.substr(0, segEnd);
                    input = input.substr(segEnd);
                }
            }
        }

        return output;
    }

    // Merge base path with relative path per RFC 3986
    std::string MergePaths(const std::string& basePath, const std::string& relativePath, bool baseHasAuthority)
    {
        if (baseHasAuthority && basePath.empty())
        {
            // If base has authority and empty path, result is "/" + relative
            return "/" + relativePath;
        }
        else
        {
            // Remove everything after the last "/" in base path
            size_t lastSlash = basePath.rfind('/');
            if (lastSlash != std::string::npos)
            {
                return basePath.substr(0, lastSlash + 1) + relativePath;
            }
            else
            {
                return relativePath;
            }
        }
    }

    // Resolve a URL against a base URL per RFC 3986
    std::optional<URLParts> ResolveURL(const std::string& urlString, const std::string& baseString = "")
    {
        // First, try to parse the URL as absolute
        auto url = TryParseURL(urlString);
        if (url)
        {
            // URL is absolute, use it directly (normalize path)
            url->pathname = RemoveDotSegments(url->pathname);
            return url;
        }

        // URL is relative, we need a valid base
        if (baseString.empty())
        {
            return std::nullopt;
        }

        auto base = TryParseURL(baseString);
        if (!base)
        {
            return std::nullopt;
        }

        URLParts result;

        // Handle different types of relative references
        if (urlString.empty())
        {
            // Empty reference - inherit everything from base
            result = *base;
        }
        else if (urlString.substr(0, 2) == "//")
        {
            // Network-path reference (starts with //)
            // Parse as scheme-relative URL
            std::string withScheme = base->protocol + urlString;
            auto parsed = TryParseURL(withScheme);
            if (!parsed)
            {
                return std::nullopt;
            }
            result = *parsed;
            result.pathname = RemoveDotSegments(result.pathname);
        }
        else if (urlString[0] == '/')
        {
            // Absolute-path reference (starts with /)
            result.protocol = base->protocol;
            result.username = base->username;
            result.password = base->password;
            result.hostname = base->hostname;
            result.port = base->port;

            // Extract path, query, fragment from relative URL
            std::string remaining = urlString;

            // Extract fragment
            size_t hashPos = remaining.find('#');
            if (hashPos != std::string::npos)
            {
                result.hash = remaining.substr(hashPos);
                remaining = remaining.substr(0, hashPos);
            }

            // Extract query
            size_t queryPos = remaining.find('?');
            if (queryPos != std::string::npos)
            {
                result.search = remaining.substr(queryPos);
                remaining = remaining.substr(0, queryPos);
            }

            result.pathname = RemoveDotSegments(remaining);
        }
        else if (urlString[0] == '?' )
        {
            // Query-only reference
            result.protocol = base->protocol;
            result.username = base->username;
            result.password = base->password;
            result.hostname = base->hostname;
            result.port = base->port;
            result.pathname = base->pathname;

            std::string remaining = urlString;
            size_t hashPos = remaining.find('#');
            if (hashPos != std::string::npos)
            {
                result.hash = remaining.substr(hashPos);
                result.search = remaining.substr(0, hashPos);
            }
            else
            {
                result.search = remaining;
            }
        }
        else if (urlString[0] == '#')
        {
            // Fragment-only reference
            result = *base;
            result.hash = urlString;
        }
        else
        {
            // Relative-path reference
            result.protocol = base->protocol;
            result.username = base->username;
            result.password = base->password;
            result.hostname = base->hostname;
            result.port = base->port;

            std::string remaining = urlString;

            // Extract fragment
            size_t hashPos = remaining.find('#');
            if (hashPos != std::string::npos)
            {
                result.hash = remaining.substr(hashPos);
                remaining = remaining.substr(0, hashPos);
            }

            // Extract query
            size_t queryPos = remaining.find('?');
            if (queryPos != std::string::npos)
            {
                result.search = remaining.substr(queryPos);
                remaining = remaining.substr(0, queryPos);
            }

            // Merge paths
            std::string merged = MergePaths(base->pathname, remaining, !base->hostname.empty());
            result.pathname = RemoveDotSegments(merged);
        }

        return result;
    }
}

namespace Babylon::Polyfills::Internal
{
    static constexpr auto JS_URL_CONSTRUCTOR_NAME = "URL";

    void URL::Initialize(Napi::Env env)
    {
        if (env.Global().Get(JS_URL_CONSTRUCTOR_NAME).IsUndefined())
        {
            Napi::Function func = DefineClass(
                env,
                JS_URL_CONSTRUCTOR_NAME,
                {
                    // All URL instance properties per MDN spec
                    InstanceAccessor("hash", &URL::GetHash, &URL::SetHash),
                    InstanceAccessor("host", &URL::GetHost, &URL::SetHost),
                    InstanceAccessor("hostname", &URL::GetHostname, &URL::SetHostname),
                    InstanceAccessor("href", &URL::GetHref, &URL::SetHref),
                    InstanceAccessor("origin", &URL::GetOrigin, nullptr), // read-only
                    InstanceAccessor("password", &URL::GetPassword, &URL::SetPassword),
                    InstanceAccessor("pathname", &URL::GetPathname, &URL::SetPathname),
                    InstanceAccessor("port", &URL::GetPort, &URL::SetPort),
                    InstanceAccessor("protocol", &URL::GetProtocol, &URL::SetProtocol),
                    InstanceAccessor("search", &URL::GetSearch, &URL::SetSearch),
                    InstanceAccessor("searchParams", &URL::GetSearchParams, nullptr), // read-only
                    InstanceAccessor("username", &URL::GetUsername, &URL::SetUsername),
                    // Instance methods
                    InstanceMethod("toString", &URL::ToString),
                    InstanceMethod("toJSON", &URL::ToJSON),
                    // Static methods
                    StaticMethod("canParse", &URL::CanParse),
                    StaticMethod("parse", &URL::Parse),
                });

            env.Global().Set(JS_URL_CONSTRUCTOR_NAME, func);
        }
    }

    URL& URL::GetFromJavaScript(Napi::Env env)
    {
        return *URL::Unwrap(JsRuntime::NativeObject::GetFromJavaScript(env).Get(JS_URL_CONSTRUCTOR_NAME).As<Napi::Object>());
    }

    bool URL::ParseURL(const std::string& urlString, const std::string& baseString)
    {
        auto parts = ResolveURL(urlString, baseString);
        if (!parts)
        {
            return false;
        }

        m_protocol = std::move(parts->protocol);
        m_username = std::move(parts->username);
        m_password = std::move(parts->password);
        m_hostname = std::move(parts->hostname);
        m_port = std::move(parts->port);
        m_pathname = std::move(parts->pathname);
        m_hash = std::move(parts->hash);

        // Create URLSearchParams object from the search string (without leading ?)
        std::string searchWithoutQuestion = parts->search.empty() ? "" : parts->search.substr(1);
        const Napi::Object searchParams = Env().Global()
            .Get(URLSearchParams::JS_URL_SEARCH_PARAMS_CONSTRUCTOR_NAME)
            .As<Napi::Function>()
            .New({Napi::Value::From(Env(), searchWithoutQuestion)});
        m_searchParamsReference = Napi::Persistent(searchParams);

        return true;
    }

    std::string URL::BuildHref() const
    {
        std::stringstream href;

        href << m_protocol;

        // Add authority if hostname exists
        if (!m_hostname.empty())
        {
            href << "//";

            // Add credentials if present
            if (!m_username.empty())
            {
                href << m_username;
                if (!m_password.empty())
                {
                    href << ":" << m_password;
                }
                href << "@";
            }

            href << m_hostname;

            if (!m_port.empty())
            {
                href << ":" << m_port;
            }
        }

        href << m_pathname;

        // Get search from URLSearchParams to stay in sync
        auto searchParamsObj = URLSearchParams::Unwrap(m_searchParamsReference.Value());
        href << searchParamsObj->GetAllParams();
        
        href << m_hash;

        return href.str();
    }

    URL::URL(const Napi::CallbackInfo& info)
        : Napi::ObjectWrap<URL>{info}
    {
        if (!info.Length())
        {
            throw Napi::TypeError::New(info.Env(), "Failed to construct 'URL': 1 argument required, but only 0 present.");
        }

        std::string urlString = info[0].As<Napi::String>();
        std::string baseString;

        if (info.Length() > 1 && !info[1].IsUndefined())
        {
            baseString = info[1].As<Napi::String>();
        }

        if (!ParseURL(urlString, baseString))
        {
            throw Napi::TypeError::New(info.Env(), "Failed to construct 'URL': Invalid URL");
        }
    }

    // Property getters

    Napi::Value URL::GetHash(const Napi::CallbackInfo&)
    {
        return Napi::String::New(Env(), m_hash);
    }

    Napi::Value URL::GetHost(const Napi::CallbackInfo&)
    {
        // host = hostname:port (port only if non-empty)
        if (m_port.empty())
        {
            return Napi::String::New(Env(), m_hostname);
        }
        return Napi::String::New(Env(), m_hostname + ":" + m_port);
    }

    Napi::Value URL::GetHostname(const Napi::CallbackInfo&)
    {
        return Napi::String::New(Env(), m_hostname);
    }

    Napi::Value URL::GetHref(const Napi::CallbackInfo&)
    {
        return Napi::String::New(Env(), BuildHref());
    }

    Napi::Value URL::GetOrigin(const Napi::CallbackInfo&)
    {
        // origin = protocol + "//" + host (for http/https/ws/wss/ftp)
        if (m_hostname.empty())
        {
            return Napi::String::New(Env(), "null");
        }

        std::stringstream origin;
        origin << m_protocol << "//" << m_hostname;
        if (!m_port.empty())
        {
            origin << ":" << m_port;
        }
        return Napi::String::New(Env(), origin.str());
    }

    Napi::Value URL::GetPassword(const Napi::CallbackInfo&)
    {
        return Napi::String::New(Env(), m_password);
    }

    Napi::Value URL::GetPathname(const Napi::CallbackInfo&)
    {
        return Napi::String::New(Env(), m_pathname);
    }

    Napi::Value URL::GetPort(const Napi::CallbackInfo&)
    {
        return Napi::String::New(Env(), m_port);
    }

    Napi::Value URL::GetProtocol(const Napi::CallbackInfo&)
    {
        return Napi::String::New(Env(), m_protocol);
    }

    Napi::Value URL::GetSearch(const Napi::CallbackInfo&)
    {
        // Return search params from URLSearchParams to stay in sync
        auto searchParamsObj = URLSearchParams::Unwrap(m_searchParamsReference.Value());
        std::string params = searchParamsObj->GetAllParams();
        return Napi::String::New(Env(), params);
    }

    Napi::Value URL::GetSearchParams(const Napi::CallbackInfo&)
    {
        return m_searchParamsReference.Value();
    }

    Napi::Value URL::GetUsername(const Napi::CallbackInfo&)
    {
        return Napi::String::New(Env(), m_username);
    }

    // Property setters

    void URL::SetHash(const Napi::CallbackInfo&, const Napi::Value& value)
    {
        std::string hash = value.As<Napi::String>();
        // Ensure hash starts with # if non-empty
        if (!hash.empty() && hash[0] != '#')
        {
            m_hash = "#" + hash;
        }
        else
        {
            m_hash = hash;
        }
    }

    void URL::SetHost(const Napi::CallbackInfo&, const Napi::Value& value)
    {
        std::string host = value.As<Napi::String>();
        // Parse host into hostname and port
        size_t colonPos = host.find(':');
        if (colonPos != std::string::npos)
        {
            m_hostname = host.substr(0, colonPos);
            m_port = host.substr(colonPos + 1);
        }
        else
        {
            m_hostname = host;
            m_port = "";
        }
    }

    void URL::SetHostname(const Napi::CallbackInfo&, const Napi::Value& value)
    {
        m_hostname = value.As<Napi::String>();
    }

    void URL::SetHref(const Napi::CallbackInfo& info, const Napi::Value& value)
    {
        std::string urlString = value.As<Napi::String>();
        if (!ParseURL(urlString))
        {
            throw Napi::TypeError::New(info.Env(), "Invalid URL");
        }
    }

    void URL::SetPassword(const Napi::CallbackInfo&, const Napi::Value& value)
    {
        m_password = value.As<Napi::String>();
    }

    void URL::SetPathname(const Napi::CallbackInfo&, const Napi::Value& value)
    {
        std::string pathname = value.As<Napi::String>();
        // Ensure pathname starts with / if we have a host
        if (!m_hostname.empty() && !pathname.empty() && pathname[0] != '/')
        {
            m_pathname = "/" + pathname;
        }
        else
        {
            m_pathname = pathname;
        }
    }

    void URL::SetPort(const Napi::CallbackInfo&, const Napi::Value& value)
    {
        m_port = value.As<Napi::String>();
    }

    void URL::SetProtocol(const Napi::CallbackInfo&, const Napi::Value& value)
    {
        std::string protocol = value.As<Napi::String>();
        // Ensure protocol ends with :
        if (!protocol.empty() && protocol.back() != ':')
        {
            m_protocol = protocol + ":";
        }
        else
        {
            m_protocol = protocol;
        }
    }

    void URL::SetSearch(const Napi::CallbackInfo& info, const Napi::Value& value)
    {
        std::string search = value.As<Napi::String>();
        
        // Normalize the search string (ensure it starts with ? if non-empty)
        std::string searchWithoutQuestion;
        if (!search.empty())
        {
            searchWithoutQuestion = (search[0] == '?') ? search.substr(1) : search;
        }

        // Update search params object
        const Napi::Object searchParams = info.Env().Global()
            .Get(URLSearchParams::JS_URL_SEARCH_PARAMS_CONSTRUCTOR_NAME)
            .As<Napi::Function>()
            .New({Napi::Value::From(info.Env(), searchWithoutQuestion)});
        m_searchParamsReference = Napi::Persistent(searchParams);
    }

    void URL::SetUsername(const Napi::CallbackInfo&, const Napi::Value& value)
    {
        m_username = value.As<Napi::String>();
    }

    // Instance methods

    Napi::Value URL::ToString(const Napi::CallbackInfo&)
    {
        return Napi::String::New(Env(), BuildHref());
    }

    Napi::Value URL::ToJSON(const Napi::CallbackInfo&)
    {
        return Napi::String::New(Env(), BuildHref());
    }

    // Static methods

    Napi::Value URL::CanParse(const Napi::CallbackInfo& info)
    {
        if (!info.Length())
        {
            return Napi::Boolean::New(info.Env(), false);
        }

        std::string urlString = info[0].As<Napi::String>();
        std::string baseString;

        if (info.Length() > 1 && !info[1].IsUndefined())
        {
            baseString = info[1].As<Napi::String>();

            // If base is provided, first check if base itself is valid
            auto baseParts = TryParseURL(baseString);
            if (!baseParts.has_value())
            {
                return Napi::Boolean::New(info.Env(), false);
            }

            // Resolve URL against base and check if result is valid
            std::optional<URLParts> resolved = ResolveURL(urlString, baseString);
            return Napi::Boolean::New(info.Env(), resolved.has_value());
        }

        return Napi::Boolean::New(info.Env(), TryParseURL(urlString).has_value());
    }

    Napi::Value URL::Parse(const Napi::CallbackInfo& info)
    {
        if (!info.Length())
        {
            return info.Env().Null();
        }

        try
        {
            Napi::Function urlConstructor = info.Env().Global()
                .Get(JS_URL_CONSTRUCTOR_NAME)
                .As<Napi::Function>();

            if (info.Length() == 1)
            {
                return urlConstructor.New({info[0]});
            }
            else if (info.Length() >= 2)
            {
                return urlConstructor.New({info[0], info[1]});
            }
            else
            {
                return info.Env().Null();
            }
        }
        catch (const Napi::Error&)
        {
            return info.Env().Null();
        }
    }
}

namespace Babylon::Polyfills::URL
{
    void BABYLON_API Initialize(Napi::Env env)
    {
        Internal::URL::Initialize(env);
        Internal::URLSearchParams::Initialize(env);
    }
}
