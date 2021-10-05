#include "TestUtils.h"

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>


#include <bimg/decode.h>
#include <bimg/encode.h>



#include <functional>
#include <sstream>
#include <Babylon/JsRuntime.h>

namespace Babylon::Plugins::Internal
{
    void TestUtils::WritePNG(const Napi::CallbackInfo& info)
    {
        const auto buffer = info[0].As<Napi::Uint8Array>();
        const auto width = info[1].As<Napi::Number>().Uint32Value();
        const auto height = info[2].As<Napi::Number>().Uint32Value();
        const auto filename = info[3].As<Napi::String>().Utf8Value();

        if (buffer.ByteLength() < (width * height * 4))
        {
            return;
        }

        bx::MemoryBlock mb(&allocator);
        bx::FileWriter writer;
        bx::FilePath filepath(filename.c_str());
        bx::Error err;
        if (writer.open(filepath, false, &err))
        {
            bimg::imageWritePng(&writer, width, height, width * 4, buffer.Data(), bimg::TextureFormat::RGBA8, false);
            writer.close();
        }
    }

    Napi::Value TestUtils::DecodeImage(const Napi::CallbackInfo& info)
    {
        Image* image = new Image;
        const auto buffer = info[0].As<Napi::ArrayBuffer>();

        image->m_Image = bimg::imageParse(&allocator, buffer.Data(), static_cast<uint32_t>(buffer.ByteLength()));

        auto finalizer = [](Napi::Env, Image* image) { delete image; };
        return Napi::External<Image>::New(info.Env(), image, std::move(finalizer));
    }

    Napi::Value TestUtils::GetImageData(const Napi::CallbackInfo& info)
    {
        const auto imageData = info[0].As<Napi::External<Image>>().Data();

        if (!imageData || !imageData->m_Image || !imageData->m_Image->m_size)
        {
            return info.Env().Undefined();
        }

        auto data = Napi::Uint8Array::New(info.Env(), imageData->m_Image->m_size);
        const auto ptr = static_cast<uint8_t*>(imageData->m_Image->m_data);
        memcpy(data.Data(), ptr, imageData->m_Image->m_size);

        return Napi::Value::From(info.Env(), data);
    }
}

namespace Babylon::Plugins::TestUtils
{
    void Initialize(Napi::Env env, WindowType nativeWindowPtr)
    {
        Internal::TestUtils::CreateInstance(env, nativeWindowPtr);
    }
}