#include <map>
#include <cstring>
#include <limits>
#include "CanvasDawn.h"
#include "ContextDawn.h"
#include "ImageDataDawn.h"

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif

#include "nanovg/nanovg.h"

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

namespace Babylon::Plugins::Internal
{
    static constexpr auto JS_IMAGEDATA_CONSTRUCTOR_NAME = "ImageDataDawn";

    Napi::Value ImageDataDawn::CreateInstance(Napi::Env env, ContextDawn* context, int32_t sx, int32_t sy, uint32_t width, uint32_t height)
    {
        // No Napi::HandleScope here: the object created by func.New() is returned to the caller.
        // A plain HandleScope would free the handle on close, which under the reference-counted
        // QuickJS Node-API port leaves the caller with a dangling value.
        Napi::Function func = DefineClass(
            env,
            JS_IMAGEDATA_CONSTRUCTOR_NAME,
            {
                InstanceAccessor("width", &ImageDataDawn::GetWidth, nullptr),
                InstanceAccessor("height", &ImageDataDawn::GetHeight, nullptr),
                InstanceAccessor("data", &ImageDataDawn::GetData, nullptr),
            });
        return func.New({Napi::External<ContextDawn>::New(env, context),
            Napi::Value::From(env, sx),
            Napi::Value::From(env, sy),
            Napi::Value::From(env, width),
            Napi::Value::From(env, height)});
    }

    ImageDataDawn::ImageDataDawn(const Napi::CallbackInfo& info)
        : Napi::ObjectWrap<ImageDataDawn>{info}
    {
        auto context{info[0].As<Napi::External<ContextDawn>>().Data()};
        const auto sx{info[1].As<Napi::Number>().Int32Value()};
        const auto sy{info[2].As<Napi::Number>().Int32Value()};
        m_width = info[3].As<Napi::Number>().Uint32Value();
        m_height = info[4].As<Napi::Number>().Uint32Value();

        // ContextDawn::GetImageData already rejects regions this large, but keep the invariant local
        // so the size_t multiplication below can never wrap (size_t is 32-bit on 32-bit ABIs).
        const uint64_t pixelCount{static_cast<uint64_t>(m_width) * m_height};
        if (pixelCount > std::numeric_limits<size_t>::max() / 4)
        {
            throw Napi::RangeError::New(info.Env(), "ImageDataDawn: requested region is too large.");
        }

        // Uint8ClampedArray, not Uint8Array: the spec clamps out-of-range writes
        // to 0..255, whereas a plain Uint8Array wraps them modulo 256, so
        // saturating arithmetic in JS (`data[i] = value + 40`) silently produces
        // a dark pixel instead of a bright one.
        const auto byteLength{static_cast<size_t>(pixelCount) * 4};
        auto data{Napi::Uint8Array::New(info.Env(), byteLength, napi_uint8_clamped_array)};
        if (context != nullptr && byteLength > 0)
        {
            context->ReadPixels(sx, sy, m_width, m_height, data.Data());
        }

        m_data = Napi::Persistent(data);
    }

    Napi::Value ImageDataDawn::GetWidth(const Napi::CallbackInfo&)
    {
        return Napi::Value::From(Env(), m_width);
    }

    Napi::Value ImageDataDawn::GetHeight(const Napi::CallbackInfo&)
    {
        return Napi::Value::From(Env(), m_height);
    }

    Napi::Value ImageDataDawn::GetData(const Napi::CallbackInfo&)
    {
        return m_data.Value();
    }
}
