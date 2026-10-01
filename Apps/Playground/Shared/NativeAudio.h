#pragma once

#include <napi/env.h>
#include <Babylon/JsRuntime.h>
#include <map>
#include <string>
#include <vector>

// Forward declaration to avoid exposing miniaudio types in the header
struct ma_engine;

namespace Babylon::Plugins::Internal
{
    struct NativeSound;

    class NativeAudio final : public Napi::ObjectWrap<NativeAudio>
    {
    public:
        using ParentT = Napi::ObjectWrap<NativeAudio>;

        static void CreateInstance(Napi::Env env);

        NativeAudio(const Napi::CallbackInfo& info);
        ~NativeAudio();

    private:
        Napi::Value Play(const Napi::CallbackInfo& info);
        Napi::Value Stop(const Napi::CallbackInfo& info);
        Napi::Value LoadSound(const Napi::CallbackInfo& info);
        Napi::Value UnloadSound(const Napi::CallbackInfo& info);
        Napi::Value Log(const Napi::CallbackInfo& info);

        ma_engine* engine = nullptr;
        std::map<std::string, NativeSound*> sounds;
        std::map<std::string, std::vector<uint8_t>> m_audioCache;
    };
}

namespace Babylon::Plugins::NativeAudio
{
    void BABYLON_API Initialize(Napi::Env env);
}
