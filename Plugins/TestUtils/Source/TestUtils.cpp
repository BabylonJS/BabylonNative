#include "TestUtils.h"

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <bimg/decode.h>
#include <bimg/encode.h>
#include <bx/file.h>
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

    Napi::Value TestUtils::GetStats(const Napi::CallbackInfo& info)
    {
        auto* stats = bgfx::getStats();
        auto env = info.Env();
        Napi::Object statObject = Napi::Object::New(env);

        statObject.Set("cpuTimeFrame", Napi::Number::New(env, static_cast<double>(stats->cpuTimeFrame)).As<Napi::Value>());
        statObject.Set("cpuTimeBegin", Napi::Number::New(env, static_cast<double>(stats->cpuTimeBegin)).As<Napi::Value>());
        statObject.Set("cpuTimeEnd", Napi::Number::New(env, static_cast<double>(stats->cpuTimeEnd)).As<Napi::Value>());
        statObject.Set("cpuTimerFreq", Napi::Number::New(env, static_cast<double>(stats->cpuTimerFreq)).As<Napi::Value>());

        statObject.Set("gpuTimeBegin", Napi::Number::New(env, static_cast<double>(stats->gpuTimeBegin)).As<Napi::Value>());
        statObject.Set("gpuTimeEnd", Napi::Number::New(env, static_cast<double>(stats->gpuTimeEnd)).As<Napi::Value>());
        statObject.Set("gpuTimerFreq", Napi::Number::New(env, static_cast<double>(stats->gpuTimerFreq)).As<Napi::Value>());

        statObject.Set("waitRender", Napi::Number::New(env, static_cast<double>(stats->waitRender)).As<Napi::Value>());
        statObject.Set("waitSubmit", Napi::Number::New(env, static_cast<double>(stats->waitSubmit)).As<Napi::Value>());

        statObject.Set("numDraw", Napi::Number::New(env, stats->numDraw).As<Napi::Value>());
        statObject.Set("numCompute", Napi::Number::New(env, stats->numCompute).As<Napi::Value>());
        statObject.Set("numBlit", Napi::Number::New(env, stats->numBlit).As<Napi::Value>());
        statObject.Set("maxGpuLatency", Napi::Number::New(env, stats->maxGpuLatency).As<Napi::Value>());

        statObject.Set("numDynamicIndexBuffers", Napi::Number::New(env, stats->numDynamicIndexBuffers).As<Napi::Value>());
        statObject.Set("numDynamicVertexBuffers", Napi::Number::New(env, stats->numDynamicVertexBuffers).As<Napi::Value>());
        statObject.Set("numFrameBuffers", Napi::Number::New(env, stats->numFrameBuffers).As<Napi::Value>());
        statObject.Set("numIndexBuffers", Napi::Number::New(env, stats->numIndexBuffers).As<Napi::Value>());
        statObject.Set("numOcclusionQueries", Napi::Number::New(env, stats->numOcclusionQueries).As<Napi::Value>());
        statObject.Set("numPrograms", Napi::Number::New(env, stats->numPrograms).As<Napi::Value>());
        statObject.Set("numShaders", Napi::Number::New(env, stats->numShaders).As<Napi::Value>());
        statObject.Set("numTextures", Napi::Number::New(env, stats->numTextures).As<Napi::Value>());
        statObject.Set("numUniforms", Napi::Number::New(env, stats->numUniforms).As<Napi::Value>());
        statObject.Set("numVertexBuffers", Napi::Number::New(env, stats->numVertexBuffers).As<Napi::Value>());
        statObject.Set("numVertexLayouts", Napi::Number::New(env, stats->numVertexLayouts).As<Napi::Value>());

        statObject.Set("textureMemoryUsed", Napi::Number::New(env, static_cast<double>(stats->textureMemoryUsed)).As<Napi::Value>());
        statObject.Set("rtMemoryUsed", Napi::Number::New(env, static_cast<double>(stats->rtMemoryUsed)).As<Napi::Value>());
        statObject.Set("transientVbUsed", Napi::Number::New(env, stats->transientVbUsed).As<Napi::Value>());
        statObject.Set("transientIbUsed", Napi::Number::New(env, stats->transientIbUsed).As<Napi::Value>());
        statObject.Set("gpuMemoryMax", Napi::Number::New(env, static_cast<double>(stats->gpuMemoryMax)).As<Napi::Value>());
        statObject.Set("gpuMemoryUsed", Napi::Number::New(env, static_cast<double>(stats->gpuMemoryUsed)).As<Napi::Value>());
        statObject.Set("numViews", Napi::Number::New(env, stats->numViews).As<Napi::Value>());
        
        return statObject;
    }

    void TestUtils::WriteString(const Napi::CallbackInfo& info)
    {
        const auto filename = info[0].As<Napi::String>().Utf8Value();
        const auto string = info[1].As<Napi::String>().Utf8Value();

        bx::FileWriter writer;
        bx::FilePath filepath(filename.c_str());
        bx::Error err;
        if (writer.open(filepath, false, &err))
        {
            writer.write(string.data(), static_cast<int32_t>(string.size()), &err);
        }
    }

    Napi::Value TestUtils::ReadString(const Napi::CallbackInfo& info)
    {
        const auto filename = info[0].As<Napi::String>().Utf8Value();

        bx::FileReader reader;
        bx::FilePath filepath(filename.c_str());
        bx::Error err;

        std::string str{};

        if (reader.open(filepath, &err))
        {
            auto size = reader.seek(0, bx::Whence::End);
            reader.seek(0, bx::Whence::Begin);
            
            str.resize(size);
            reader.read(str.data(), static_cast<int32_t>(size), &err);
        }
        return Napi::Value::From(info.Env(), str);
    }

}
