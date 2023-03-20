#include "URL.h"
#include <sstream>

namespace Babylon::Polyfills::Internal
{
    void URL::Initialize(Napi::Env env)
    {
        Napi::HandleScope scope{env};

        Napi::Function func = DefineClass(
            env,
            JS_URL_CONSTRUCTOR_NAME,
            {
                InstanceAccessor("search", &URL::GetSearch, &URL::SetSearch),
                InstanceAccessor("href", &URL::GetHref, &URL::SetHref),
                InstanceAccessor("origin", &URL::GetOrigin, nullptr),
                InstanceAccessor("pathname", &URL::GetPathname, nullptr),
                InstanceAccessor("hostname", &URL::GetHostname, nullptr),
                InstanceAccessor("searchParams", &URL::GetSearchParams, nullptr),
            });

        if (env.Global().Get(JS_URL_CONSTRUCTOR_NAME).IsUndefined())
        {
            env.Global().Set(JS_URL_CONSTRUCTOR_NAME, func);
        }

        JsRuntime::NativeObject::GetFromJavaScript(env).Set(JS_URL_CONSTRUCTOR_NAME, func);
    }

    URL& URL::GetFromJavaScript(Napi::Env env)
    {
        return *URL::Unwrap(JsRuntime::NativeObject::GetFromJavaScript(env).Get(JS_URL_CONSTRUCTOR_NAME).As<Napi::Object>());
    }

    Napi::Value URL::GetSearch(const Napi::CallbackInfo&)
    {
        std::string allParams = GetSearchQuery();
        return Napi::Value::From(Env(), allParams);
    }

    void URL::SetSearch(const Napi::CallbackInfo&, const Napi::Value& value)
    {
        m_search = value.As<Napi::String>();
    }

    Napi::Value URL::GetHref(const Napi::CallbackInfo&)
    {
        std::stringstream resultHref;
        resultHref << m_origin;
        resultHref << m_pathname;

        std::string allParams = GetSearchQuery();
        resultHref << allParams;

        return Napi::Value::From(Env(), resultHref.str());
    }

    void URL::SetHref(const Napi::CallbackInfo&, const Napi::Value& value)
    {
        m_href = value.As<Napi::String>();
    }

    Napi::Value URL::GetOrigin(const Napi::CallbackInfo&)
    {
        return Napi::Value::From(Env(), m_origin);
    }

    Napi::Value URL::GetPathname(const Napi::CallbackInfo&)
    {
        return Napi::Value::From(Env(), m_pathname);
    }

    Napi::Value URL::GetHostname(const Napi::CallbackInfo&)
    {
        return Napi::Value::From(Env(), m_hostname);
    }

    // gets search params from UrlSearchParams class
    std::string URL::GetSearchQuery()
    {
        auto searchParamsObj = URLSearchParams::Unwrap(m_searchParamsReference.Value());
        return searchParamsObj->GetAllParams();
    }

    Napi::Value URL::GetSearchParams(const Napi::CallbackInfo&)
    {
        return m_searchParamsReference.Value();
    }

    // TODO current URL constructor is incomplete, it only supports one argument
    // and the url parsing is limited, this logic should be moved to UrlLib and use platform
    // specific functions to parse the URL and get the parts
    URL::URL(const Napi::CallbackInfo& info)
        : Napi::ObjectWrap<URL>{info}
    {
        if (!info.Length())
        {
            return;
        }

        // Store Entire URL
        m_href = info[0].As<Napi::String>();

        // Get Position of ? to store search var
        const size_t qIndex = m_href.find_last_of('?');

        if (qIndex != std::string::npos)
        {
            m_search = m_href.substr(qIndex, m_href.size() - qIndex);
        }

        // get UrlSearchParams object
        const Napi::Object searchParams = info.Env().Global().Get(URLSearchParams::JS_URL_SEARCH_PARAMS_CONSTRUCTOR_NAME).As<Napi::Function>().New({Napi::Value::From(info.Env(), m_search)});
        m_searchParamsReference = Napi::Persistent(searchParams);

        // Get URL Domain
        const size_t start = m_href.find("//");
        const size_t originStart = m_href.find("h");
        const size_t index = m_href.find_first_of("/", start + 2);

        m_origin = m_href.substr(originStart, index);
        m_hostname = m_origin.substr(start + 2, index);

        if (index != std::string::npos)
        {
            m_pathname = m_href.substr(index, qIndex - index);
        }
    }
}

namespace Babylon::Polyfills::URL
{
    void Initialize(Napi::Env env)
    {
        Internal::URL::Initialize(env);
        Internal::URLSearchParams::Initialize(env);
    }
}
