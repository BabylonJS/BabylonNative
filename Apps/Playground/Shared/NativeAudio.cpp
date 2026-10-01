#include "NativeAudio.h"

#pragma warning(push)
#pragma warning(disable : 4244)
#pragma warning(disable : 4100)
#pragma warning(disable : 4458)
#pragma warning(disable : 4267)
#define MINIAUDIO_IMPLEMENTATION
#include "../../../Dependencies/miniaudio.h"
#pragma warning(pop)

#include <iostream>
#include <string>

#if defined(__ANDROID__) || defined(ANDROID)
#include <android/log.h>
#define LOG_AUDIO(fmt, ...) __android_log_print(ANDROID_LOG_INFO, "NativeAudio", fmt, ##__VA_ARGS__)
#else
#define LOG_AUDIO(fmt, ...) printf("[NativeAudio] " fmt "\n", ##__VA_ARGS__)
#endif

namespace
{
    void LogStatus(const std::string& msg)
    {
        LOG_AUDIO("%s", msg.c_str());
    }
}

namespace Babylon::Plugins::Internal
{
    struct NativeSound {
        ma_sound sound;
        ma_decoder decoder;
        bool hasDecoder = false;
        bool loaded = false;
    };

    NativeAudio::NativeAudio(const Napi::CallbackInfo& info)
        : Napi::ObjectWrap<NativeAudio>(info)
    {
        LogStatus("NativeAudio constructed");
        engine = new ma_engine();
        if (ma_engine_init(NULL, engine) != MA_SUCCESS) {
            LogStatus("Engine init failed");
            delete engine;
            engine = nullptr;
        } else {
            LogStatus("Engine init success");
        }
    }

    NativeAudio::~NativeAudio()
    {
        LogStatus("NativeAudio destructed");
        for (auto& pair : sounds) {
            if (pair.second->loaded) {
                ma_sound_uninit(&pair.second->sound);
            }
            if (pair.second->hasDecoder) {
                ma_decoder_uninit(&pair.second->decoder);
            }
            delete pair.second;
        }
        sounds.clear();

        if (engine) {
            ma_engine_uninit(engine);
            delete engine;
            engine = nullptr;
        }
    }

    void NativeAudio::CreateInstance(Napi::Env env)
    {
        Napi::Function func = DefineClass(
            env,
            "NativeAudioClass",
            {
                InstanceMethod("play", &NativeAudio::Play),
                InstanceMethod("stop", &NativeAudio::Stop),
                InstanceMethod("loadSound", &NativeAudio::LoadSound),
                InstanceMethod("unloadSound", &NativeAudio::UnloadSound),
                InstanceMethod("log", &NativeAudio::Log),
            });

        env.Global().Set("NativeAudio", func.New({}));
        LogStatus("NativeAudio instance exposed to JavaScript");
    }

    Napi::Value NativeAudio::Log(const Napi::CallbackInfo& info)
    {
        if (info.Length() >= 1 && info[0].IsString()) {
            std::string msg = info[0].As<Napi::String>().Utf8Value();
            LogStatus("[JS] " + msg);
        }
        return info.Env().Undefined();
    }

    Napi::Value NativeAudio::Play(const Napi::CallbackInfo& info)
    {
        if (info.Length() < 1 || !info[0].IsString()) {
            LogStatus("Invalid arguments for play()");
            return info.Env().Undefined();
        }

        std::string path = info[0].As<Napi::String>().Utf8Value();

        if (!engine) {
            LogStatus("Engine is null");
            return info.Env().Undefined();
        }

        float volume = 1.0f;
        if (info.Length() >= 2 && info[1].IsNumber()) {
            volume = (float)info[1].As<Napi::Number>().DoubleValue();
            if (volume < 0.0f) volume = 0.0f;
            if (volume > 1.0f) volume = 1.0f;
        }

        bool loop = false;
        if (info.Length() >= 3 && info[2].IsBoolean()) {
            loop = info[2].As<Napi::Boolean>().Value();
        }

        if (sounds.find(path) == sounds.end()) {
            NativeSound* ns = new NativeSound();
            ma_result result = MA_ERROR;

            if (m_audioCache.find(path) != m_audioCache.end()) {
                result = ma_decoder_init_memory(m_audioCache[path].data(), m_audioCache[path].size(), NULL, &ns->decoder);
                if (result == MA_SUCCESS) {
                    ns->hasDecoder = true;
                    result = ma_sound_init_from_data_source(engine, &ns->decoder, 0, NULL, &ns->sound);
                }
            } else {
                result = ma_sound_init_from_file(engine, path.c_str(), 0, NULL, NULL, &ns->sound);
            }

            if (result == MA_SUCCESS) {
                ns->loaded = true;
                sounds[path] = ns;
                LogStatus("Sound loaded: " + path);
            } else {
                LogStatus("Failed to load sound: " + path);
                if (ns->hasDecoder) {
                    ma_decoder_uninit(&ns->decoder);
                }
                delete ns;
                return info.Env().Undefined();
            }
        }

        NativeSound* ns = sounds[path];
        if (ns && ns->loaded) {
            ma_sound_set_volume(&ns->sound, volume);
            ma_sound_set_looping(&ns->sound, loop ? MA_TRUE : MA_FALSE);

            ma_sound_stop(&ns->sound);
            if (ns->hasDecoder) {
                ma_decoder_seek_to_pcm_frame(&ns->decoder, 0);
            }
            ma_sound_seek_to_pcm_frame(&ns->sound, 0);
            ma_sound_set_at_end(&ns->sound, MA_FALSE);

            ma_sound_start(&ns->sound);
            LogStatus("Playing '" + path + "', vol=" + std::to_string(volume) + ", loop=" + (loop ? "true" : "false"));
        }

        return info.Env().Undefined();
    }

    Napi::Value NativeAudio::Stop(const Napi::CallbackInfo& info)
    {
        if (info.Length() < 1 || !info[0].IsString()) {
            return info.Env().Undefined();
        }

        std::string path = info[0].As<Napi::String>().Utf8Value();
        if (sounds.find(path) != sounds.end()) {
            NativeSound* ns = sounds[path];
            if (ns && ns->loaded) {
                ma_sound_stop(&ns->sound);
                if (ns->hasDecoder) {
                    ma_decoder_seek_to_pcm_frame(&ns->decoder, 0);
                }
                ma_sound_seek_to_pcm_frame(&ns->sound, 0);
                ma_sound_set_at_end(&ns->sound, MA_FALSE);
                LogStatus("Stopped sound: " + path);
            }
        }
        return info.Env().Undefined();
    }

    Napi::Value NativeAudio::LoadSound(const Napi::CallbackInfo& info)
    {
        if (info.Length() < 2 || !info[0].IsString() || !info[1].IsArrayBuffer()) {
            LogStatus("Invalid arguments for loadSound(name, arrayBuffer)");
            return info.Env().Undefined();
        }

        std::string name = info[0].As<Napi::String>().Utf8Value();
        Napi::ArrayBuffer buffer = info[1].As<Napi::ArrayBuffer>();

        size_t byteLength = buffer.ByteLength();
        uint8_t* dataPtr = static_cast<uint8_t*>(buffer.Data());

        if (sounds.find(name) != sounds.end()) {
            NativeSound* oldSound = sounds[name];
            if (oldSound->loaded) {
                ma_sound_stop(&oldSound->sound);
                ma_sound_uninit(&oldSound->sound);
            }
            if (oldSound->hasDecoder) {
                ma_decoder_uninit(&oldSound->decoder);
            }
            delete oldSound;
            sounds.erase(name);
        }

        m_audioCache[name] = std::vector<uint8_t>(dataPtr, dataPtr + byteLength);

        if (engine && engine->pResourceManager) {
            ma_resource_manager_register_encoded_data(
                engine->pResourceManager,
                name.c_str(),
                m_audioCache[name].data(),
                m_audioCache[name].size()
            );
        }

        LogStatus("Loaded sound to memory: " + name + " (" + std::to_string(byteLength) + " bytes)");
        return info.Env().Undefined();
    }

    Napi::Value NativeAudio::UnloadSound(const Napi::CallbackInfo& info)
    {
        if (info.Length() < 1 || !info[0].IsString()) {
            return info.Env().Undefined();
        }

        std::string name = info[0].As<Napi::String>().Utf8Value();

        if (sounds.find(name) != sounds.end()) {
            NativeSound* ns = sounds[name];
            if (ns->loaded) {
                ma_sound_stop(&ns->sound);
                ma_sound_uninit(&ns->sound);
            }
            if (ns->hasDecoder) {
                ma_decoder_uninit(&ns->decoder);
            }
            delete ns;
            sounds.erase(name);
        }

        if (m_audioCache.find(name) != m_audioCache.end()) {
            m_audioCache.erase(name);
        }

        if (engine && engine->pResourceManager) {
            ma_resource_manager_unregister_data(engine->pResourceManager, name.c_str());
        }

        LogStatus("Unloaded sound: " + name);
        return info.Env().Undefined();
    }
}

namespace Babylon::Plugins::NativeAudio
{
    void BABYLON_API Initialize(Napi::Env env)
    {
        Internal::NativeAudio::CreateInstance(env);
    }
}
