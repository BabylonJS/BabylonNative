#include "MediaStream.h"
#include "NativeCameraImpl.h"

namespace Babylon::Plugins
{
    static constexpr auto JS_CLASS_NAME = "_MediaStream";

    void MediaStream::Initialize(Napi::Env& env)
    {
        Napi::Function ctor = DefineClass(
          env,
          JS_CLASS_NAME,
          {
              InstanceMethod("getTracks", &MediaStream::GetVideoTracks), // The only supported tracks are video tracks
              InstanceMethod("getVideoTracks", &MediaStream::GetVideoTracks),
              InstanceMethod("getAudioTracks", &MediaStream::GetAudioTracks),
              InstanceMethod("applyConstraints", &MediaStream::ApplyConstraints),
          });
        
        constructor = Napi::Persistent(ctor);
    }

    MediaStream::MediaStream(const Napi::CallbackInfo& info)
        : Napi::ObjectWrap<MediaStream>{info}
        , m_cameraImpl{std::make_shared<Camera::Impl>(info.Env(), false /* should finish removing overrideTexture */)}
        , m_runtimeScheduler{JsRuntime::GetFromJavaScript(info.Env())}
    {
    }

    Napi::Value MediaStream::GetVideoTracks(const Napi::CallbackInfo& info)
    {
        // To simply the implementation (because only a single track is supported by the native implementation) the same
        // ObjectWrap fullfills the MediaStream and MediaStreamTrack interfaces
        auto tracks = Napi::Array::New(info.Env(), 1);
        tracks.Set(uint32_t{0}, info.This());
        return std::move(tracks);
    }

    Napi::Value MediaStream::GetAudioTracks(const Napi::CallbackInfo& info)
    {
        // Only video tracks are currently supported
        return Napi::Array::New(info.Env(), 0);
    }

    Napi::Value MediaStream::ApplyConstraints(const Napi::CallbackInfo& info)
    {
        auto env = info.Env();
        auto deferred{Napi::Promise::Deferred::New(env)};
        auto promise{deferred.Promise()};
        
        auto constraints = info[0].As<Napi::Object>();
        uint32_t maxWidth{256}, maxHeight{256};
        std::string facingMode{};

        auto maxWidthValue{constraints.Get("maxWidth")};
        auto maxHeightValue{constraints.Get("maxHeight")};
        auto facingModeValue{constraints.Get("facingMode")};
        if (maxWidthValue.IsNumber())
        {
            maxWidth = maxWidthValue.As<Napi::Number>().Uint32Value();
        }
        if (maxHeightValue.IsNumber())
        {
            maxHeight = maxHeightValue.As<Napi::Number>().Uint32Value();
        }
        if (facingModeValue.IsString())
        {
            facingMode = facingModeValue.As<Napi::String>().Utf8Value();
        }
                
        m_cameraImpl->Open(maxWidth, maxHeight, facingMode == "user").then(m_runtimeScheduler, arcana::cancellation::none(), [this, env, deferred, constraints](const arcana::expected<Camera::Impl::CameraDimensions, std::exception_ptr>& result) {
            if (result.has_error())
            {
                deferred.Reject(Napi::Error::New(env, result.error()).Value());
            }
            else
            {
                auto cameraDimensions{result.value()};
                this->Width = cameraDimensions.width;
                this->Height = cameraDimensions.height;

                deferred.Resolve(env.Undefined());
            }
        });

        return static_cast<Napi::Value>(promise);
    }

    void MediaStream::UpdateTexture(bgfx::TextureHandle textureHandle)
    {
        m_cameraImpl->UpdateCameraTexture(textureHandle);
    }
}
