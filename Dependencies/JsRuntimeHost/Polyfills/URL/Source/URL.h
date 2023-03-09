#pragma once

#include <Babylon/JsRuntimeScheduler.h>

#include <napi/napi.h>
#include "URLSearchParams.h"

namespace Babylon::Polyfills::Internal
{
    class URL final : public Napi::ObjectWrap<URL>
    {
        static constexpr auto JS_URL_CONSTRUCTOR_NAME = "URL";

    public:
        static void Initialize(Napi::Env env);
        static URL& GetFromJavaScript(Napi::Env env);

        explicit URL(const Napi::CallbackInfo& info);

    private:
        Napi::Value GetSearch(const Napi::CallbackInfo& info);
        void SetSearch(const Napi::CallbackInfo& info, const Napi::Value& value);

        Napi::Value GetHref(const Napi::CallbackInfo& info);
        void SetHref(const Napi::CallbackInfo& info, const Napi::Value& value);

        Napi::Value GetOrigin(const Napi::CallbackInfo& info);
        Napi::Value GetPathname(const Napi::CallbackInfo& info);
        Napi::Value GetHostname(const Napi::CallbackInfo& info);

        std::string GetSearchQuery();
        Napi::Value GetSearchParams(const Napi::CallbackInfo& info);

        std::unordered_map<std::string, std::vector<Napi::FunctionReference>> m_eventHandlerRefs;
        std::string m_search;
        std::string m_href;
        std::string m_origin;
        std::string m_pathname;
        std::string m_hostname;
        Napi::ObjectReference m_searchParamsReference;
    };
}
