#include <Babylon/Polyfills/Console.h>

#include <array>
#include <functional>
#include <sstream>
#include <cmath>

namespace
{
    constexpr const char* JS_INSTANCE_NAME{"console"};

    void Call(Napi::Function func, const Napi::CallbackInfo& info)
    {
        std::array<Napi::Value, 6> staticArgs{};
        const size_t argc = info.Length();

        if (info.Length() < std::size(staticArgs))
        {
            for (size_t i = 0; i < argc; ++i)
            {
                staticArgs[i] = info[i];
            }

            func.Call(argc, staticArgs.data());
        }
        else
        {
            std::vector<Napi::Value> args(argc);
            for (size_t i = 0; i < argc; ++i)
            {
                args[i] = info[i];
            }

            func.Call(argc, args.data());
        }
    }

    void InvokeCallback(Babylon::Polyfills::Console::CallbackT callback, const Napi::CallbackInfo& info, Babylon::Polyfills::Console::LogLevel logLevel)
    {
        std::ostringstream ss{};
        if (info.Length() > 0)
        {
            std::string firstArg = info[0].ToString();
            size_t currArgIndex = 1;

            std::size_t j = 0;
            while (j < firstArg.size())
            {
                const char currChar = firstArg[j];
                // When a '%' is encountered, check the next character to determine the type of string we have
                if (currChar == '%' && j < firstArg.size() - 1 && currArgIndex < info.Length())
                {
                    char nextChar = firstArg[j + 1];
                    Napi::Value currArg = info[currArgIndex];
                    // the next character can be one of: [soO], when the substitution string specifies a string
                    if (nextChar == 'o' || nextChar == 'O' || nextChar == 's')
                    {
                        ss << currArg.ToString().Utf8Value();
                        currArgIndex++;
                    }
                    // or [dif], when it specifies a number
                    else if (nextChar == 'd' || nextChar == 'i' || nextChar == 'f')
                    {
                        double d = currArg.ToNumber().DoubleValue();
                        if (std::isnan(d))
                        {
                            ss << "NaN";
                        }
                        else if (nextChar == 'd' || nextChar == 'i')
                        {
                            int64_t i = static_cast<int64_t>(d);
                            ss << i;
                        }
                        else
                        {
                            ss << d;
                        }
                        currArgIndex++;
                    }
                    // otherwise it's an invalid format string, just dump it on the stream
                    else
                    {
                        ss << currChar << nextChar;
                    }
                    // walk forward two characters
                    j += 2;
                }
                else
                {
                    // walk forward one character and print it on the stream
                    ss << currChar;
                    j++;
                }
            }

            // if any arguments are remaining after we done all substitutions we could, then dump them into the stream
            for (; currArgIndex < info.Length(); currArgIndex++)
            {
                ss << " ";
                Napi::Value currArg = info[currArgIndex];
                ss << currArg.ToString().Utf8Value();
            }
        }

        callback(ss.str().c_str(), logLevel);
    }

    void AddMethod(Napi::Object& console, const char* functionName, Babylon::Polyfills::Console::LogLevel logLevel, Babylon::Polyfills::Console::CallbackT callback)
    {
        auto existingFunction = std::make_shared<Napi::FunctionReference>(Napi::Persistent(console.Get(functionName).As<Napi::Function>()));
        console.Set(functionName,
            Napi::Function::New(
                console.Env(), [callback, existingFunction = std::move(existingFunction), logLevel](const Napi::CallbackInfo& info) {
                    InvokeCallback(callback, info, logLevel);

                    if (!existingFunction->Value().IsUndefined())
                    {
                        Call(existingFunction->Value(), info);
                    }
                },
                functionName));
    }
}

namespace Babylon::Polyfills::Console
{
    void BABYLON_API Initialize(Napi::Env env, CallbackT callback)
    {
        Napi::HandleScope scope{env};

        auto console = env.Global().Get(JS_INSTANCE_NAME).As<Napi::Object>();
        if (console.IsUndefined())
        {
            console = Napi::Object::New(env);
            env.Global().Set(JS_INSTANCE_NAME, console);
        }

        AddMethod(console, "log", LogLevel::Log, callback);
        AddMethod(console, "warn", LogLevel::Warn, callback);
        AddMethod(console, "error", LogLevel::Error, callback);
    }
}