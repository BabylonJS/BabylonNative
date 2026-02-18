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

    Napi::Value ImageData::CreateInstance(Napi::Env env, Context* context, uint32_t width, uint32_t height)
    {
        Napi::HandleScope scope{env};
        Napi::Function func = DefineClass(
            env,
            JS_IMAGEDATA_CONSTRUCTOR_NAME,
            {
                InstanceAccessor("width", &ImageData::GetWidth, nullptr),
                InstanceAccessor("height", &ImageData::GetHeight, nullptr),
                InstanceAccessor("data", &ImageData::GetData, nullptr),
            });
        return func.New({Napi::External<Context>::New(env, context), Napi::Value::From(env, width), Napi::Value::From(env, height)});
    }

    ImageData::ImageData(const Napi::CallbackInfo& info)
        : Napi::ObjectWrap<ImageData>{info}
    {
        (void)info[0].As<Napi::External<Context>>().Data();
        auto width{info[1].As<Napi::Number>().Uint32Value()};
        auto height{info[2].As<Napi::Number>().Uint32Value()};
        m_width = width;
        m_height = height;
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
        // TODO(getImageData): Currently returns a zeroed-out buffer instead of
        // actual pixel data. Implementing this requires GPU texture readback:
        // copy the relevant region of the canvas render target into a staging
        // buffer, map it to CPU memory, and fill the returned Uint8Array with
        // the RGBA pixel values. This is needed for any JS code that inspects
        // canvas contents (e.g. hit-testing, image processing, snapshots).
        const auto size{m_width * m_height * 4};
        auto data{Napi::Uint8Array::New(info.Env(), size)};
        memset(data.Data(), 0, size);
        return Napi::Value::From(info.Env(), data);
    }
}
