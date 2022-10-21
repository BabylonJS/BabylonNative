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
        
        if (m_cameraDevice == nullptr || m_cameraResolution == nullptr)
        {
            auto bestCamera = FindBestCameraStream(constraints);
            m_cameraDevice = bestCamera.first;
            m_cameraResolution = bestCamera.second;
            
            // If m_cameraDevice is still null that means a camera device could not be found that meets the constraints
            if (m_cameraDevice == nullptr)
            {
                // If no device could be fount to satisfy the constraints throw a "ConstraintError" to match the browser implementation
                deferred.Reject(Napi::Error::New(env, std::runtime_error{"ConstraintError: Unable to match constraints to a supported camera configuration."}).Value());
                return static_cast<Napi::Value>(promise);
            }
            
            // This is the first time applying the constraints to the camera. First open up the stream
            m_cameraImpl->Open(m_cameraDevice, m_cameraResolution).then(m_runtimeScheduler, arcana::cancellation::none(), [this, env, deferred, constraints](const arcana::expected<Camera::Impl::CameraDimensions, std::exception_ptr>& result) {
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
        }

        auto torchConstraint{ constraints.Get("torch") };
        auto torchEnabled{ torchConstraint.IsBoolean() && torchConstraint.As<Napi::Boolean>().Value() };
        m_cameraImpl->SetCapability(CameraCapability::Torch, torchEnabled);

        return static_cast<Napi::Value>(promise);
    }

    std::pair<std::shared_ptr<CameraDevice>, std::shared_ptr<CameraTrack>> MediaStream::FindBestCameraStream(Napi::Object constraints)
    {
        // Get the available camera devices
        std::vector<std::shared_ptr<CameraDevice>> cameraDevices{ m_cameraImpl->GetCameraDevices() };
        
        int32_t maxWidth{256}, maxHeight{256};
        std::string facingMode{"environment"};

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
        
        std::shared_ptr<CameraDevice> bestCameraDevice{ nullptr };
        std::shared_ptr<CameraTrack> bestCameraResolution{nullptr };
        int32_t bestPixelCount{0};
        int32_t bestDimDiff{0};

        // The camera devices should be assumed to be sorted from best to worst. Pick the first camera device
        // that satisfies all constraints
        for (uint32_t i = 0; i < cameraDevices.size(); ++i)
        {
            auto cameraDevice = cameraDevices[i];

            if (cameraDevice->facingMode != facingMode)
            {
                // Ignore cameras facing the wrong direction
                continue;
            }

            for (uint32_t j = 0; j < cameraDevice->supportedResolutions.size(); j++) {
                auto resolution = cameraDevice->supportedResolutions[j];

                if (resolution->width > maxWidth || resolution->height > maxHeight)
                {
                    // The resolution exceeds our constraints. Skip it.
                    continue;
                }

                // Calculate pixel count and dimension differential and take the best qualifying one.
                int32_t pixelCount{resolution->width * resolution->height};
                int32_t dimDiff{(maxWidth - resolution->width) + (maxHeight - resolution->height)};
                if (bestCameraDevice == nullptr || pixelCount > bestPixelCount || (pixelCount == bestPixelCount && dimDiff < bestDimDiff))
                {
                    bestPixelCount = pixelCount;
                    bestCameraDevice = cameraDevice;
                    bestDimDiff = dimDiff;
                    bestCameraResolution = resolution;

                    // Check if we got an exact match exit the loop as no other resolution would be better
                    if (resolution->width == maxWidth && resolution->height == maxHeight)
                    {
                        break;
                    }
                }
            }

            if (bestCameraDevice != nullptr)
            {
                // We've found a camera device that meets all constraints no need to look further
                break;
            }
        }
        
        return {bestCameraDevice, bestCameraResolution};
    }

    void MediaStream::UpdateTexture(bgfx::TextureHandle textureHandle)
    {
        m_cameraImpl->UpdateCameraTexture(textureHandle);
    }
}
