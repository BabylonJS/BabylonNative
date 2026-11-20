#include <Babylon/Polyfills/Streams.h>

#include "StreamsScripts.h"

#include <array>
#include <string_view>

namespace Babylon::Polyfills::Streams
{
    void BABYLON_API Initialize(Napi::Env env)
    {
        Napi::HandleScope scope{env};
        auto global = env.Global();

        static constexpr std::array<std::string_view, 13> constructorNames{
            "ReadableStream",
            "ReadableStreamDefaultController",
            "ReadableByteStreamController",
            "ReadableStreamBYOBRequest",
            "ReadableStreamDefaultReader",
            "ReadableStreamBYOBReader",
            "WritableStream",
            "WritableStreamDefaultController",
            "WritableStreamDefaultWriter",
            "ByteLengthQueuingStrategy",
            "CountQueuingStrategy",
            "TransformStream",
            "TransformStreamDefaultController",
        };

        bool needsPonyfill{};
        for (const auto name : constructorNames)
        {
            const auto constructor = global.Get(name.data());
            if (constructor.IsUndefined() || constructor.IsNull())
            {
                needsPonyfill = true;
                break;
            }
        }

        if (!needsPonyfill)
        {
            return;
        }

        const auto exports = Napi::Eval(env, Internal::StreamsScripts::Ponyfill.data(), "jsruntimehost://web-streams-polyfill.js").As<Napi::Object>();
        for (const auto name : constructorNames)
        {
            global.Set(name.data(), exports.Get(name.data()));
        }
    }
}
