#pragma once

#include <Babylon/JsRuntimeScheduler.h>
#include <napi/napi.h>
#include <unordered_map>
#include <vector>

namespace Babylon::Polyfills::Internal
{
    class URLSearchParams final : public Napi::ObjectWrap<URLSearchParams>
    {
    public:
        static void Initialize(Napi::Env env);
        explicit URLSearchParams(const Napi::CallbackInfo& info);
        static constexpr auto JS_URL_SEARCH_PARAMS_CONSTRUCTOR_NAME = "URLSearchParams";
        std::string GetAllParams();

    private:
        void Set(const Napi::CallbackInfo& info);
        Napi::Value Has(const Napi::CallbackInfo& info);
        Napi::Value Get(const Napi::CallbackInfo& info);
        void ParseKeyVal(const std::string& subStr);
        void ParseInputQueryStr(const std::string& queryStr);

        std::unordered_map<std::string, std::string> m_paramsMap;
        std::vector<std::string> m_paramsVector;
    };
}
