#include "URLSearchParams.h"
#include <string>
#include <sstream>


namespace Babylon::Polyfills::Internal
{
    void URLSearchParams::Initialize(Napi::Env env)
    {
        Napi::HandleScope scope{env};

        Napi::Function func = DefineClass(
            env,
            JS_URL_SEARCH_PARAMS_CONSTRUCTOR_NAME,
            {
                InstanceMethod("get", &URLSearchParams::Get),
                InstanceMethod("set", &URLSearchParams::Set),
                InstanceMethod("has", &URLSearchParams::Has),
            });

        if (env.Global().Get(JS_URL_SEARCH_PARAMS_CONSTRUCTOR_NAME).IsUndefined())
        {
            env.Global().Set(JS_URL_SEARCH_PARAMS_CONSTRUCTOR_NAME, func);
        }

        JsRuntime::NativeObject::GetFromJavaScript(env).Set(JS_URL_SEARCH_PARAMS_CONSTRUCTOR_NAME, func);
    }

    URLSearchParams::URLSearchParams(const Napi::CallbackInfo& info)
        : Napi::ObjectWrap<URLSearchParams>{info}
    {
        if (info.Length() == 0) 
        {
            return;
        }
        std::string queryStr = info[0].As<Napi::String>();
        ParseInputQueryStr(queryStr);
    }

    void URLSearchParams::ParseKeyVal(const std::string& subStr)
    {
        auto equalSign = subStr.find("=");
        if (equalSign != std::string::npos)
        {
            auto key = subStr.substr(0, equalSign);
            auto value = subStr.substr(equalSign + 1);

            m_paramsVector.push_back(key);
            m_paramsMap[key] = value;
        }
    }

    void URLSearchParams::ParseInputQueryStr(const std::string& queryStr)
    {
        // find all the &'s to determine where each param ends
        // got positions for substring
        if (queryStr.length() <= 1)
        {
            return;
        }
        
        size_t start = queryStr[0] == '?' ? 1 : 0;
        size_t end = queryStr.find("&");
        // find the first &, in a while loop
        while (end != std::string::npos)
        {
            // get substring
            std::string mySub = queryStr.substr(start, end - start);
            ParseKeyVal(mySub);

            // update start, end
            start = end + 1;
            end = queryStr.find("&", start);
        }

        // now need to parse the last key val pair
        ParseKeyVal(queryStr.substr(start));
    }

    Napi::Value URLSearchParams::Get(const Napi::CallbackInfo& info)
    {
        std::string key = info[0].As<Napi::String>();

        auto element = m_paramsMap.find(key);

        // element is not found 
        if (element == m_paramsMap.end())
        {
            return Env().Null();
        }
        return Napi::Value::From(Env(), element->second);
    }

    std::string URLSearchParams::GetAllParams()
    {
        std::stringstream resultStringStream;

        for (int i = 0; i < m_paramsVector.size(); i++)
        {
            if (i > 0) 
            {
                resultStringStream << "&";
            }
            else 
            {
                resultStringStream << "?";
            }

            const std::string& key = m_paramsVector[i];
            const std::string& value = m_paramsMap[key];

            resultStringStream << key;
            resultStringStream << "=";
            resultStringStream << value;
        } 

        return resultStringStream.str();
    }


    void URLSearchParams::Set(const Napi::CallbackInfo& info)
    {
        if (info.Length() < 2) 
        {
            std::stringstream errorMessageStream;
            errorMessageStream << "Failed to execute 'set' on 'URLSearchParams': 2 arguments required, but only ";
            errorMessageStream << info.Length();
            errorMessageStream << " present";
            throw Napi::Error::New(info.Env(), errorMessageStream.str());
        }

        std::string key = info[0].As<Napi::String>();
        std::string value = info[1].ToString().Utf8Value();

        if (m_paramsMap.insert_or_assign(key, value).second) 
        {
            m_paramsVector.push_back(key);
        }
    }
    
    Napi::Value URLSearchParams::Has(const Napi::CallbackInfo& info)
    {
        std::string key = info[0].As<Napi::String>();
        return Napi::Value::From(Env(), m_paramsMap.find(key) != m_paramsMap.end());
    }
}