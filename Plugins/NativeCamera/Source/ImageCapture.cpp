#include "ImageCapture.h"
#include "CameraDevice.h"
#include "MediaStream.h"
#include <Babylon/JsRuntime.h>
#include <Babylon/JsRuntimeScheduler.h>

#ifdef _MSC_VER
    #define DISABLE_UNREACHABLE_CODE_WARNINGS \
        __pragma(warning(push)) \
        __pragma(warning(disable : 4702))
    #define ENABLE_UNREACHABLE_CODE_WARNINGS __pragma(warning(pop))
#else
    #define DISABLE_UNREACHABLE_CODE_WARNINGS
    #define ENABLE_UNREACHABLE_CODE_WARNINGS
#endif

namespace Babylon::Plugins::Internal
{
    namespace
    {
        std::string RedEyeReductionToString(RedEyeReduction redEyeReduction)
        {
            return
                redEyeReduction == RedEyeReduction::Always ? "always" :
                redEyeReduction == RedEyeReduction::Controllable ? "controllable" : "never";
        }

        std::string FillLightModeToString(FillLightMode fillLightMode)
        {
           return
                fillLightMode == FillLightMode::Auto ? "auto" :
                fillLightMode == FillLightMode::Flash ? "flash" : "off";
        }

        Napi::Array FillLightModesToNapi(const Napi::Env& env, const std::set<FillLightMode>& fillLightModes)
        {
            auto arrayJS = Napi::Array::New(env, fillLightModes.size());
            uint32_t index = 0;
            for (FillLightMode fillLightMode : fillLightModes)
            {
                arrayJS.Set(index, FillLightModeToString(fillLightMode));
                index++;
            }

            return arrayJS;
        }

        Napi::Object PhotoCapabilitiesToNapi(const Napi::Env& env, const PhotoCapabilities& photoCapabilities)
        {
            auto imageWidthJS = Napi::Object::New(env);
            imageWidthJS.Set("min", photoCapabilities.MinWidth);
            imageWidthJS.Set("max", photoCapabilities.MaxWidth);
            imageWidthJS.Set("step", 1);

            auto imageHeightJS = Napi::Object::New(env);
            imageHeightJS.Set("min", photoCapabilities.MinHeight);
            imageHeightJS.Set("max", photoCapabilities.MaxHeight);
            imageHeightJS.Set("step", 1);

            auto photoCapabilitiesJS = Napi::Object::New(env);
            photoCapabilitiesJS.Set("redEyeReduction", RedEyeReductionToString(photoCapabilities.RedEyeReduction));
            photoCapabilitiesJS.Set("fillLightMode", FillLightModesToNapi(env, photoCapabilities.FillLightModes));
            photoCapabilitiesJS.Set("imageWidth", imageWidthJS);
            photoCapabilitiesJS.Set("imageHeight", imageHeightJS);

            return photoCapabilitiesJS;
        }

        Napi::Object PhotoSettingsToNapi(const Napi::Env& env, const PhotoSettings& photoSettings)
        {
            auto photoSettingsJS = Napi::Object::New(env);

            photoSettingsJS.Set("redEyeReduction", photoSettings.RedEyeReduction);
            photoSettingsJS.Set("fillLightMode", FillLightModeToString(photoSettings.FillLightMode));
            photoSettingsJS.Set("imageWidth", photoSettings.Width);
            photoSettingsJS.Set("imageHeight", photoSettings.Height);

            return photoSettingsJS;
        }
    };

    class ImageCapture : public Napi::ObjectWrap<ImageCapture>
    {
        static constexpr auto JS_CLASS_NAME = "ImageCapture";

    public:
        static void Initialize(Napi::Env env)
        {
            Napi::HandleScope scope{env};

            Napi::Function func = DefineClass(
                env,
                JS_CLASS_NAME,
                {
                    InstanceMethod("getPhotoCapabilities", &ImageCapture::GetPhotoCapabilities),
                    InstanceMethod("getPhotoSettings", &ImageCapture::GetPhotoSettings),
                    InstanceMethod("takePhoto", &ImageCapture::TakePhoto),
                    InstanceMethod("grabFrame", &ImageCapture::GrabFrame),
                });

            env.Global().Set(JS_CLASS_NAME, func);
        }

        ImageCapture(const Napi::CallbackInfo& info)
            : Napi::ObjectWrap<ImageCapture>{info}
            , m_runtimeScheduler{JsRuntime::GetFromJavaScript(info.Env())}
            , m_cameraDevice{MediaStream::Unwrap(info[0].As<Napi::Object>())->CameraDevice()}
            , m_photoSettings{m_cameraDevice->DefaultPhotoSettings()}
        {
        }

    private:
        Napi::Value GetPhotoCapabilities(const Napi::CallbackInfo& info)
        {
            return PhotoCapabilitiesToNapi(info.Env(), m_cameraDevice->PhotoCapabilities());
        }

        Napi::Value GetPhotoSettings(const Napi::CallbackInfo& info)
        {
            return PhotoSettingsToNapi(info.Env(), m_photoSettings);
        }

        Napi::Value TakePhoto(const Napi::CallbackInfo& info)
        {
            // If the optional PhotoSettings are passed in, update the cached settings.
            if (info.Length() > 0)
            {
                const auto photoSettingsJS = info[0].As<Napi::Object>();
                if (photoSettingsJS.Has("redEyeReduction"))
                {
                    m_photoSettings.RedEyeReduction = photoSettingsJS.Get("redEyeReduction").ToBoolean();
                }

                if (photoSettingsJS.Has("imageWidth"))
                {
                    m_photoSettings.Width = photoSettingsJS.Get("imageWidth").ToNumber().Int32Value();
                }

                if (photoSettingsJS.Has("imageHeight"))
                {
                    m_photoSettings.Height = photoSettingsJS.Get("imageHeight").ToNumber().Int32Value();
                }

                if (photoSettingsJS.Has("fillLightMode"))
                {
                    const auto fillLightMode = photoSettingsJS.Get("fillLightMode").ToString().Utf8Value();
                    if (fillLightMode == "auto")
                    {
                        m_photoSettings.FillLightMode = FillLightMode::Auto;
                    }
                    else if (fillLightMode == "flash")
                    {
                        m_photoSettings.FillLightMode = FillLightMode::Flash;
                    }
                    else
                    {
                        m_photoSettings.FillLightMode = FillLightMode::Off;
                    }
                }
            }
            //unreachable code is detected below in x86 Release build. Not obvious fix so Warning as error doesn't break the build. Disabling warning for now.
DISABLE_UNREACHABLE_CODE_WARNINGS
            auto env = info.Env();
            auto deferred = Napi::Promise::Deferred::New(env);
            // Take a photo and synchronously (via inline_scheduler) make a copy of the data (since we know nothing about its lifetime) before
            // transitioning back to the JavaScript thread to complete the promise.
            m_cameraDevice->TakePhotoAsync(m_photoSettings).then(arcana::inline_scheduler, arcana::cancellation::none(), [](gsl::span<const uint8_t> result) {
                std::vector<uint8_t> bytes{};
                bytes.resize(result.size());
                std::memcpy(bytes.data(), result.data(), result.size());
                return bytes;
            }).then(m_runtimeScheduler.Get(), arcana::cancellation::none(), [env, deferred](const arcana::expected<std::vector<uint8_t>, std::exception_ptr>& result) {
                if (result.has_error())
                {
                    deferred.Reject(Napi::Error::New(env, result.error()).Value());
                    return;
                }

                // Create a JS ArrayBuffer and copy the image into the buffer.
                auto arrayBuffer = Napi::ArrayBuffer::New(env, result.value().size());
                std::memcpy(arrayBuffer.Data(), result.value().data(), result.value().size());

                // Ideally we'd have a real Blob polyfill, but we can also create a partial polyfill object inline here (enough to access the underlying ArrayBuffer).
                auto arrayBufferDeferred = Napi::Promise::Deferred::New(env);
                arrayBufferDeferred.Resolve(arrayBuffer);

                auto arrayBufferFunction = Napi::Function::New(env, [arrayBufferDeferred](const Napi::CallbackInfo&) -> Napi::Value {
                    return arrayBufferDeferred.Promise();
                }, "arrayBuffer");

                auto blob = Napi::Object::New(env);
                blob.Set("arrayBuffer", arrayBufferFunction);
                blob.Set("size", arrayBuffer.ByteLength());
                blob.Set("type", "image/jpeg");

                deferred.Resolve(blob);
            });
ENABLE_UNREACHABLE_CODE_WARNINGS
            return deferred.Promise();
        }

        Napi::Value GrabFrame(const Napi::CallbackInfo& info)
        {
            // Maybe implement this in the future if needed.
            throw Napi::Error::New(info.Env(), "Not implemented.");
        }

        JsRuntimeScheduler m_runtimeScheduler;
        const std::shared_ptr<Plugins::CameraDevice> m_cameraDevice{};
        PhotoSettings m_photoSettings{};
    };
}

namespace Babylon::Plugins::ImageCapture
{
    void Initialize(Napi::Env env)
    {
        Internal::ImageCapture::Initialize(env);
    }
}
