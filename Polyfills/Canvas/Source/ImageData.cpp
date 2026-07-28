#include <bgfx/bgfx.h>
#include <map>
#include <cstring>
#include <limits>
#include "Canvas.h"
#include "Context.h"
#include "ImageData.h"

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif

#include "nanovg/nanovg.h"

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

namespace Babylon::Polyfills::Internal
{
    static constexpr auto JS_IMAGEDATA_CONSTRUCTOR_NAME = "ImageData";

    Napi::Value ImageData::CreateInstance(Napi::Env env, Context* context, int32_t sx, int32_t sy, uint32_t width, uint32_t height)
    {
        // No Napi::HandleScope here: the object created by func.New() is returned to the caller.
        // A plain HandleScope would free the handle on close, which under the reference-counted
        // QuickJS Node-API port leaves the caller with a dangling value.
        Napi::Function func = DefineClass(
            env,
            JS_IMAGEDATA_CONSTRUCTOR_NAME,
            {
                InstanceAccessor("width", &ImageData::GetWidth, nullptr),
                InstanceAccessor("height", &ImageData::GetHeight, nullptr),
                InstanceAccessor("data", &ImageData::GetData, nullptr),
            });
        return func.New({Napi::External<Context>::New(env, context),
            Napi::Value::From(env, sx),
            Napi::Value::From(env, sy),
            Napi::Value::From(env, width),
            Napi::Value::From(env, height)});
    }

    ImageData::ImageData(const Napi::CallbackInfo& info)
        : Napi::ObjectWrap<ImageData>{info}
    {
        auto context{info[0].As<Napi::External<Context>>().Data()};
        const auto sx{info[1].As<Napi::Number>().Int32Value()};
        const auto sy{info[2].As<Napi::Number>().Int32Value()};
        m_width = info[3].As<Napi::Number>().Uint32Value();
        m_height = info[4].As<Napi::Number>().Uint32Value();

        // Context::GetImageData already rejects regions this large, but keep the invariant local
        // so the size_t multiplication below can never wrap (size_t is 32-bit on 32-bit ABIs).
        const uint64_t pixelCount{static_cast<uint64_t>(m_width) * m_height};
        if (pixelCount > std::numeric_limits<size_t>::max() / 4)
        {
            throw Napi::RangeError::New(info.Env(), "ImageData: requested region is too large.");
        }

        m_pixels.resize(static_cast<size_t>(pixelCount) * 4);
        if (context != nullptr && !m_pixels.empty())
        {
            context->ReadPixels(sx, sy, m_width, m_height, m_pixels.data());
        }
    }

    Napi::Value ImageData::GetWidth(const Napi::CallbackInfo&)
    {
        return Napi::Value::From(Env(), m_width);
    }

    Napi::Value ImageData::GetHeight(const Napi::CallbackInfo&)
    {
        return Napi::Value::From(Env(), m_height);
    }

    Napi::Value ImageData::GetData(const Napi::CallbackInfo& info)
    {
        const auto size{m_pixels.size()};
        auto data{Napi::Uint8Array::New(info.Env(), size)};
        if (size > 0)
        {
            std::memcpy(data.Data(), m_pixels.data(), size);
        }
        return Napi::Value::From(info.Env(), data);
    }
}
