#pragma once

#include <functional>
#include <memory>
#include <string>

namespace Napi
{
    class Env;
}

namespace Babylon
{
    class PluginHost
    {
    public:
        static PluginHost& GetRuntimeImplFromJavaScript(Napi::Env env);

        virtual std::string GetAbsoluteUrl(const std::string& url) = 0;
        virtual void AddTask(std::function<void()>&& function) = 0;

        virtual void LoadUrlAsync(const std::string& url, std::function<void(const std::string & data)>&& function) = 0;
        virtual void LoadUrlAsync(const std::string& url, std::function<void(const std::vector<char> & data)>&& function) = 0;
    };
}