#pragma once

#include <Babylon/JsRuntimeScheduler.h>

#include <napi/napi.h>
#include "URLSearchParams.h"

namespace Babylon::Polyfills::Internal
{
    class URL final : public Napi::ObjectWrap<URL>
    {
    public:
        static void Initialize(Napi::Env env);
        static URL& GetFromJavaScript(Napi::Env env);

        explicit URL(const Napi::CallbackInfo& info);

    private:
        // Static methods (exposed to JavaScript)
        static Napi::Value CanParse(const Napi::CallbackInfo& info);
        static Napi::Value Parse(const Napi::CallbackInfo& info);
        static Napi::Value CreateObjectURL(const Napi::CallbackInfo& info);
        static Napi::Value RevokeObjectURL(const Napi::CallbackInfo& info);

        // Parse the URL string and populate all components including searchParams
        // Returns true if parsing succeeded, false otherwise
        // If baseString is provided, resolves urlString relative to it
        bool ParseURL(const std::string& urlString, const std::string& baseString = "");

        // Rebuild the full href from components
        std::string BuildHref() const;

        // Instance property getters
        Napi::Value GetHash(const Napi::CallbackInfo& info);
        Napi::Value GetHost(const Napi::CallbackInfo& info);
        Napi::Value GetHostname(const Napi::CallbackInfo& info);
        Napi::Value GetHref(const Napi::CallbackInfo& info);
        Napi::Value GetOrigin(const Napi::CallbackInfo& info);
        Napi::Value GetPassword(const Napi::CallbackInfo& info);
        Napi::Value GetPathname(const Napi::CallbackInfo& info);
        Napi::Value GetPort(const Napi::CallbackInfo& info);
        Napi::Value GetProtocol(const Napi::CallbackInfo& info);
        Napi::Value GetSearch(const Napi::CallbackInfo& info);
        Napi::Value GetSearchParams(const Napi::CallbackInfo& info);
        Napi::Value GetUsername(const Napi::CallbackInfo& info);

        // Instance property setters
        void SetHash(const Napi::CallbackInfo& info, const Napi::Value& value);
        void SetHost(const Napi::CallbackInfo& info, const Napi::Value& value);
        void SetHostname(const Napi::CallbackInfo& info, const Napi::Value& value);
        void SetHref(const Napi::CallbackInfo& info, const Napi::Value& value);
        void SetPassword(const Napi::CallbackInfo& info, const Napi::Value& value);
        void SetPathname(const Napi::CallbackInfo& info, const Napi::Value& value);
        void SetPort(const Napi::CallbackInfo& info, const Napi::Value& value);
        void SetProtocol(const Napi::CallbackInfo& info, const Napi::Value& value);
        void SetSearch(const Napi::CallbackInfo& info, const Napi::Value& value);
        void SetUsername(const Napi::CallbackInfo& info, const Napi::Value& value);

        // Instance methods
        Napi::Value ToString(const Napi::CallbackInfo& info);
        Napi::Value ToJSON(const Napi::CallbackInfo& info);

        // URL components
        std::string m_protocol;   // e.g., "https:"
        std::string m_username;   // e.g., "user"
        std::string m_password;   // e.g., "pass"
        std::string m_hostname;   // e.g., "example.com"
        std::string m_port;       // e.g., "8080"
        std::string m_pathname;   // e.g., "/path/to/resource"
        std::string m_hash;       // e.g., "#fragment"

        Napi::ObjectReference m_searchParamsReference;
    };
}
