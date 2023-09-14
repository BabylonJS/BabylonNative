#include "ImageCapture.h"
#include "CameraDevice.h"
#include "MediaStream.h"

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

        Napi::Object PhotoCapabilitiesToNapi(const Napi::Env& env, PhotoCapabilities photoCapabilities)
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
            photoCapabilitiesJS.Set("fillLightMode", FillLightModeToString(photoCapabilities.FillLightMode));
            photoCapabilitiesJS.Set("imageWidth", imageWidthJS);
            photoCapabilitiesJS.Set("imageHeight", imageHeightJS);

            return photoCapabilitiesJS;
        }

        Napi::Object PhotoSettingsToNapi(const Napi::Env& env, PhotoSettings photoSettings)
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
                m_photoSettings.RedEyeReduction = photoSettingsJS.Get("redEyeReduction").ToBoolean();
                m_photoSettings.Width = photoSettingsJS.Get("imageWidth").ToNumber().Int32Value();
                m_photoSettings.Height = photoSettingsJS.Get("imageHeight").ToNumber().Int32Value();

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

            // TODO
            // 1. Call m_cameraDevice->TakePhoto(m_photoSettings)
            // 2. Use a Napi::Deferred to return a promise
            // 3. From the task returned from m_cameraDevice->TakePhoto, create a continuation that copies the resulting std::vector<uint8_t>
            //    (the raw bytes of the jpeg) into a Napi::ArrayBuffer, and return a Blob polyfill with an arrayBuffer() function that returns
            //    a completed Promise with the Napi::ArrayBuffer

            return info.Env().Undefined();
        }

        Napi::Value GrabFrame(const Napi::CallbackInfo& info)
        {
            // Maybe implement this in the future if needed.
            throw Napi::Error::New(info.Env(), "Not implemented.");
        }

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
