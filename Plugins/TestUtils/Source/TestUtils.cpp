#include "TestUtils.h"
#include <Babylon/Plugins/TestUtils.h>

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#ifdef BABYLON_NATIVE_PLUGIN_NATIVEENGINE_LOAD_IMAGES
#include <bimg/decode.h>
#include <bimg/encode.h>
#endif
#include <bx/file.h>

#include <Babylon/JsRuntime.h>
#include <Babylon/Graphics/DeviceContext.h>
#include <Babylon/Graphics/Platform.h>

#include <algorithm>
#include <filesystem>
#include <functional>
#include <gsl/span>
#include <memory>
#include <sstream>
#include <system_error>
#include <vector>

#if defined(_WIN32)
#include <Windows.h>
#elif defined(__APPLE__)
#include <mach-o/dyld.h>
#elif defined(__linux__) || defined(__ANDROID__)
#include <unistd.h>
#endif

#define STRINGIZEX(x) #x
#define STRINGIZE(x) STRINGIZEX(x)

namespace
{
    // Returns the directory containing the running executable. Mirrors the per-platform
    // helpers already used by the Playground / UnitTests; folded into TestUtils.cpp so the
    // listFiles API does not need a new entry point per platform file.
    std::filesystem::path GetExecutableDirectory()
    {
#if defined(_WIN32)
        wchar_t buffer[MAX_PATH]{};
        ::GetModuleFileNameW(nullptr, buffer, MAX_PATH);
        return std::filesystem::path{buffer}.parent_path();
#elif defined(__APPLE__)
        char buffer[1024]{};
        uint32_t size = sizeof(buffer);
        if (_NSGetExecutablePath(buffer, &size) != 0)
        {
            return {};
        }
        return std::filesystem::path{buffer}.parent_path();
#elif defined(__linux__) || defined(__ANDROID__)
        char buffer[1024]{};
        const auto len = ::readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
        if (len <= 0)
        {
            return {};
        }
        buffer[len] = 0;
        return std::filesystem::path{buffer}.parent_path();
#else
        return {};
#endif
    }
}

namespace Babylon::Plugins::Internal
{
    Napi::Value TestUtils::GetGraphicsApiName(const Napi::CallbackInfo& info)
    {
        return Napi::Value::From(info.Env(), STRINGIZE(GRAPHICS_API));
    }

    void TestUtils::WritePNG(const Napi::CallbackInfo& info)
    {
#ifndef BABYLON_NATIVE_PLUGIN_NATIVEENGINE_LOAD_IMAGES
        throw Napi::Error::New(info.Env(), "Image loading is disabled in this build (BABYLON_NATIVE_PLUGIN_NATIVEENGINE_LOAD_IMAGES=OFF).");
#else
        const auto buffer = info[0].As<Napi::Uint8Array>();
        const auto width = info[1].As<Napi::Number>().Uint32Value();
        const auto height = info[2].As<Napi::Number>().Uint32Value();
        const auto filename = info[3].As<Napi::String>().Utf8Value();

        if (buffer.ByteLength() < width * height * 4)
        {
            throw Napi::Error::New(info.Env(), "Buffer byte length is invalid for width and height");
        }

        bx::MemoryBlock mb(&Graphics::DeviceContext::GetDefaultAllocator());
        bx::FileWriter writer;
        bx::FilePath filepath(filename.c_str());
        bx::FilePath filedir(filepath.getPath());
        bx::makeAll(filedir);
        bx::Error err;
        if (writer.open(filepath, false, &err))
        {
            bimg::imageWritePng(&writer, width, height, width * 4, buffer.Data(), bimg::TextureFormat::RGBA8, false);
            writer.close();
        }
#endif
    }

    Napi::Value TestUtils::DecodeImage(const Napi::CallbackInfo& info)
    {
#ifndef BABYLON_NATIVE_PLUGIN_NATIVEENGINE_LOAD_IMAGES
        throw Napi::Error::New(info.Env(), "Image loading is disabled in this build (BABYLON_NATIVE_PLUGIN_NATIVEENGINE_LOAD_IMAGES=OFF).");
#else
        Image* image = new Image;
        const auto buffer = info[0].As<Napi::ArrayBuffer>();

        image->m_Image = bimg::imageParse(&Graphics::DeviceContext::GetDefaultAllocator(), buffer.Data(), static_cast<uint32_t>(buffer.ByteLength()));

        auto finalizer = [](Napi::Env, Image* image) { delete image; };
        return Napi::External<Image>::New(info.Env(), image, std::move(finalizer));
#endif
    }

    Napi::Value TestUtils::GetImageData(const Napi::CallbackInfo& info)
    {
#ifndef BABYLON_NATIVE_PLUGIN_NATIVEENGINE_LOAD_IMAGES
        throw Napi::Error::New(info.Env(), "Image loading is disabled in this build (BABYLON_NATIVE_PLUGIN_NATIVEENGINE_LOAD_IMAGES=OFF).");
#else
        const auto imageData = info[0].As<Napi::External<Image>>().Data();

        if (!imageData || !imageData->m_Image || !imageData->m_Image->m_size)
        {
            return info.Env().Undefined();
        }

        auto data = Napi::Uint8Array::New(info.Env(), imageData->m_Image->m_size);
        const auto ptr = static_cast<uint8_t*>(imageData->m_Image->m_data);
        memcpy(data.Data(), ptr, imageData->m_Image->m_size);

        return Napi::Value::From(info.Env(), data);
#endif
    }

    void TestUtils::GetFrameBufferData(const Napi::CallbackInfo& info)
    {
        const auto callback{ info[0].As<Napi::Function>() };

        auto callbackPtr{ std::make_shared<Napi::FunctionReference>(Napi::Persistent(callback)) };
        m_deviceContext.RequestScreenShot([this, callbackPtr{ std::move(callbackPtr) }](std::vector<uint8_t> array) {
            m_runtime.Dispatch([callbackPtr{ std::move(callbackPtr) }, array{ std::move(array) }](Napi::Env env) mutable {
                auto span = gsl::span<uint8_t>{array};
                auto arrayBuffer{ Napi::ArrayBuffer::New(env, span.data(), span.size(), [array = std::move(array)](Napi::Env, void*) {}) };
                auto typedArray{ Napi::Uint8Array::New(env, span.size(), arrayBuffer, 0) };
                callbackPtr->Value().Call({ typedArray });
            });
        });
    }

    void TestUtils::CaptureNextFrame(const Napi::CallbackInfo& /*info*/)
    {
        m_deviceContext.RequestCaptureNextFrame();
    }

    // listFiles(relativePath): returns the basenames of regular files in
    // <exe-dir>/<relativePath>, sorted lexicographically. Non-existent or non-directory
    // paths return an empty array (not an error) so callers can probe optional asset
    // folders without exception handling. Subdirectories are not recursed.
    Napi::Value TestUtils::ListFiles(const Napi::CallbackInfo& info)
    {
        const auto relPath = info[0].As<Napi::String>().Utf8Value();
        const auto dir = GetExecutableDirectory() / relPath;

        std::vector<std::string> names;
        std::error_code ec;
        if (std::filesystem::is_directory(dir, ec))
        {
            for (const auto& entry : std::filesystem::directory_iterator(dir, ec))
            {
                std::error_code entryEc;
                if (entry.is_regular_file(entryEc))
                {
                    names.push_back(entry.path().filename().generic_string());
                }
            }
        }
        std::sort(names.begin(), names.end());

        auto arr = Napi::Array::New(info.Env(), names.size());
        for (uint32_t i = 0; i < names.size(); ++i)
        {
            arr.Set(i, Napi::String::New(info.Env(), names[i]));
        }
        return arr;
    }
}

namespace
{
    Babylon::Plugins::TestUtils::ExitCallback& ExitCallbackStorage()
    {
        static Babylon::Plugins::TestUtils::ExitCallback s_callback;
        return s_callback;
    }
}

namespace Babylon::Plugins::TestUtils
{
    void BABYLON_API Initialize(Napi::Env env, Graphics::WindowT window)
    {
        Internal::TestUtils::CreateInstance(env, window);
    }

    void BABYLON_API SetExitCallback(ExitCallback callback)
    {
        ExitCallbackStorage() = std::move(callback);
    }
}

namespace Babylon::Plugins::Internal
{
    // Bridges per-platform TestUtils::Exit() to the host-registered callback.
    void InvokeExitCallback(int exitCode)
    {
        auto& cb = ExitCallbackStorage();
        if (cb)
        {
            cb(exitCode);
        }
    }
}
