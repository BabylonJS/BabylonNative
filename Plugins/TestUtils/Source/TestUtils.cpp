#include "TestUtils.h"

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <bimg/decode.h>
#include <bimg/encode.h>
#include <bx/file.h>
#include <functional>
#include <sstream>
#include <Babylon/JsRuntime.h>
#include <Babylon/Graphics/DeviceContext.h>

#define STRINGIZEX(x) #x
#define STRINGIZE(x) STRINGIZEX(x)

namespace Babylon::Plugins::Internal
{
    Napi::Value TestUtils::GetGraphicsApiName(const Napi::CallbackInfo& info)
    {
        return Napi::Value::From(info.Env(), STRINGIZE(GRAPHICS_API));
    }

    void TestUtils::WritePNG(const Napi::CallbackInfo& info)
    {
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
    }

    Napi::Value TestUtils::DecodeImage(const Napi::CallbackInfo& info)
    {
        Image* image = new Image;
        const auto buffer = info[0].As<Napi::ArrayBuffer>();

        image->m_Image = bimg::imageParse(&Graphics::DeviceContext::GetDefaultAllocator(), buffer.Data(), static_cast<uint32_t>(buffer.ByteLength()));

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

    void TestUtils::GetFrameBufferData(const Napi::CallbackInfo& info)
    {
        const auto callback{ info[0].As<Napi::Function>() };

        auto callbackPtr{ std::make_shared<Napi::FunctionReference>(Napi::Persistent(callback)) };
        m_deviceContext.RequestScreenShot([this, callbackPtr{ std::move(callbackPtr) }](std::vector<uint8_t> array) {
            m_runtime.Dispatch([callbackPtr{ std::move(callbackPtr) }, array{ std::move(array) }](Napi::Env env) {
                auto arrayBuffer{ Napi::ArrayBuffer::New(env, const_cast<uint8_t*>(array.data()), array.size()) };
                auto typedArray{ Napi::Uint8Array::New(env, array.size(), arrayBuffer, 0) };
                callbackPtr->Value().Call({ typedArray });
                });
            });
    }
}
