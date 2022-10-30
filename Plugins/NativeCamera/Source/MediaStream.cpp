#include "MediaStream.h"
#include "NativeCameraImpl.h"

namespace Babylon::Plugins
{
    arcana::task<Napi::Object, std::exception_ptr> MediaStream::New(Napi::Env env, Napi::Object constraints)
    {
        auto ctor = DefineClass(
            env,
            JS_CLASS_NAME,
            {
                InstanceMethod("getTracks", &MediaStream::GetVideoTracks), // The only supported tracks are video tracks
                InstanceMethod("getVideoTracks", &MediaStream::GetVideoTracks),
                InstanceMethod("getAudioTracks", &MediaStream::GetAudioTracks),
                InstanceMethod("applyConstraints", &MediaStream::ApplyConstraints),
                InstanceMethod("getCapabilities", &MediaStream::GetCapabilities),
                InstanceMethod("getSettings", &MediaStream::GetSettings),
                InstanceMethod("getConstraints", &MediaStream::GetConstraints),
            });
        
        // Create a an object reference to the mediaStream javascript object so that it is not destructed during the async operation
        auto mediaStreamObject{ std::make_shared<Napi::ObjectReference>(Napi::Persistent(ctor.New({}))) };
        auto mediaStream{ MediaStream::Unwrap(mediaStreamObject->Value()) };
        
        return mediaStream->ApplyInitialConstraints(env, constraints).then(mediaStream->m_runtimeScheduler, arcana::cancellation::none(), [mediaStreamObject](const arcana::expected<void, std::exception_ptr>& result)
        {
            if (result.has_error())
            {
                throw result.error();
            }

            return mediaStreamObject->Value();
        });
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
        auto constraints = info[0].As<Napi::Object>();
        
        auto deferred{Napi::Promise::Deferred::New(env)};
        auto promise{deferred.Promise()};

        // Apply the constraints to the existing device
        if(!UpdateConstraints(env, constraints))
        {
            // Setting the constraint to the capability failed
            deferred.Reject(Napi::Error::New(env, std::runtime_error{"OverconstrainedError: Unable to match constraints to a supported camera configuration."}).Value());
            return static_cast<Napi::Value>(promise);
        }
        deferred.Resolve(env.Undefined());

        return static_cast<Napi::Value>(promise);
    }

    arcana::task<void, std::exception_ptr> MediaStream::ApplyInitialConstraints(Napi::Env env, Napi::Object constraints)
    {
        // A camera device hasn't been selected yet. Find the best device and open the stream
        auto bestCamera = FindBestCameraStream(constraints);
        m_cameraDevice = bestCamera.first;
        m_cameraResolution = bestCamera.second;

        // If m_cameraDevice is still null that means a camera device could not be found that meets the constraints
        if (m_cameraDevice == nullptr)
        {
            // If no device could be fount to satisfy the constraints throw a "ConstraintError" to match the browser implementation
            return arcana::task_from_error<void>(std::make_exception_ptr(std::runtime_error{"ConstraintError: Unable to match constraints to a supported camera configuration."}));
        }

        return m_cameraImpl->Open(m_cameraDevice, m_cameraResolution).then(m_runtimeScheduler, arcana::cancellation::none(), [this, env, constraints](const arcana::expected<Camera::Impl::CameraDimensions, std::exception_ptr>& result) {
            if (result.has_error())
            {
                // Re-throw the error from opening the camera
                throw result.error();
            }
            
            auto cameraDimensions{result.value()};
            this->Width = cameraDimensions.width;
            this->Height = cameraDimensions.height;
            
            if(!UpdateConstraints(env, constraints))
            {
                // Setting the constraint to the capability failed
                throw std::runtime_error{"ConstraintError: Unable to match constraints to a supported camera configuration."};
            }
            
            return;
        });
    }

    Napi::Value MediaStream::GetCapabilities(const Napi::CallbackInfo& info)
    {
        auto env = info.Env();
        
        if (m_cameraDevice == nullptr)
        {
            // We don't have a cameraDevice selected yet.
            return Napi::Object::New(env);
        }
        
        auto capabilities = Napi::Object::New(env);
        
        for (uint32_t i=0; i < m_cameraDevice->capabilities.size(); i++)
        {
            std::shared_ptr<CameraCapability> capability{ m_cameraDevice->capabilities[i] };
            capability->addAsCapability(capabilities);
        }
        
        return std::move(capabilities);
    }

    Napi::Value MediaStream::GetSettings(const Napi::CallbackInfo& info)
    {
        auto env = info.Env();
        
        if (m_cameraDevice == nullptr)
        {
            // We don't have a cameraDevice selected yet.
            return Napi::Object::New(env);
        }
        
        auto settings = Napi::Object::New(env);
        
        for (uint32_t i=0; i < m_cameraDevice->capabilities.size(); i++)
        {
            std::shared_ptr<CameraCapability> capability{ m_cameraDevice->capabilities[i] };
            capability->addAsSetting(settings);
        }
        
        return std::move(settings);
    }

    Napi::Value MediaStream::GetConstraints(const Napi::CallbackInfo& info)
    {
        auto env = info.Env();
        
        // return a clone of the currently applied constraints
        return Napi::Object::From(env, m_currentConstraints.Value());
    }

    std::pair<std::shared_ptr<CameraDevice>, std::shared_ptr<CameraTrack>> MediaStream::FindBestCameraStream(Napi::Object constraints)
    {
        // Get the available camera devices
        std::vector<std::shared_ptr<CameraDevice>> cameraDevices{ m_cameraImpl->GetCameraDevices() };
        
        std::shared_ptr<CameraDevice> bestCameraDevice{ nullptr };
        std::shared_ptr<CameraTrack> bestCameraResolution{ nullptr };
        int32_t bestFullySatisfiedCapabilityCount{0};
        
        // The camera devices should be assumed to be sorted from best to worst. Pick the first camera device that fully
        // satisfies the most constraints without failing any.
        for (uint32_t i = 0; i < cameraDevices.size(); ++i)
        {
            auto cameraDevice = cameraDevices[i];
            
            bool failedAConstraint{ false };
            int32_t fullySatisfiedCapabilityCount{ 0 };
            for (uint32_t j = 0; j < cameraDevice->capabilities.size(); j++ )
            {
                CameraCapability::MeetsConstraint constraintSatifaction{ cameraDevice->capabilities[j]->meetsConstraints(constraints) };
                switch (constraintSatifaction)
                {
                    case CameraCapability::MeetsConstraint::Unsatisfied:
                        failedAConstraint = true;
                        break;
                    case CameraCapability::MeetsConstraint::FullySatisfied:
                        fullySatisfiedCapabilityCount++;
                        break;
                    case CameraCapability::MeetsConstraint::PartiallySatisfied:
                    case CameraCapability::MeetsConstraint::Unconstrained:
                        // Don't weight partialy satisfied or unconstrained capabilites any higher than another device
                        break;
                }
                
                // Don't bother continuing to count capabilities if we've failed on
                if (failedAConstraint)
                {
                    break;
                }
            }
            
            if (failedAConstraint)
            {
                // The device fails at least one of the constraint requirements. Skip it.
                continue;
            }
            
            // Ensure the width/height constraints can be met and find the best resolution available within
            // the constraints
            std::shared_ptr<CameraTrack> bestResolution{ nullptr };
            int32_t bestWidthDiff{INT32_MAX};
            int32_t bestHeightDiff{INT32_MAX};
            
            auto widthConstraint{ CameraCapability::parseConstraint<uint32_t>(constraints.Get("width")) };
            auto heightConstraint{ CameraCapability::parseConstraint<uint32_t>(constraints.Get("height")) };
            
            // Set the targetWidth and targetHeight as a fallback through the values exact, ideal, max, min
            auto targetWidth = widthConstraint.exact.has_value() ? widthConstraint.exact
                    : widthConstraint.ideal.has_value() ? widthConstraint.ideal
                    : widthConstraint.max.has_value() ? widthConstraint.max
                    : widthConstraint.min;
            auto targetHeight = heightConstraint.exact.has_value() ? heightConstraint.exact
                    : heightConstraint.ideal.has_value() ? heightConstraint.ideal
                    : heightConstraint.max.has_value() ? heightConstraint.max
                    : heightConstraint.min;

            for (uint32_t j = 0; j < cameraDevice->supportedResolutions.size(); j++) {
                auto resolution = cameraDevice->supportedResolutions[j];
                uint32_t width{ static_cast<uint32_t>(resolution->width) };
                uint32_t height{ static_cast<uint32_t>(resolution->height) };
                
                auto meetsWidthRequirements =
                    (!widthConstraint.exact.has_value() || widthConstraint.exact.value() == width) &&
                    (!widthConstraint.min.has_value() || widthConstraint.min.value() <= width) &&
                    (!widthConstraint.max.has_value() || widthConstraint.max.value() >= width);
                
                auto meetsHeightRequirements =
                    (!heightConstraint.exact.has_value() || heightConstraint.exact.value() == height) &&
                    (!heightConstraint.min.has_value() || heightConstraint.min.value() <= height) &&
                    (!heightConstraint.max.has_value() || heightConstraint.max.value() >= height);
                
                if (!meetsWidthRequirements || !meetsHeightRequirements)
                {
                    // The resolution doesn't meet the constraint requirements
                    continue;
                }
                
                int32_t widthDiff = targetWidth.has_value() ? abs(static_cast<int32_t>(targetWidth.value() - width)) : 0;
                int32_t heightDiff = targetHeight.has_value() ? abs(static_cast<int32_t>(targetHeight.value() - width)) : 0;
                
                if (bestResolution == nullptr || widthDiff + heightDiff < bestWidthDiff + bestHeightDiff)
                {
                    bestWidthDiff = widthDiff;
                    bestHeightDiff = heightDiff;
                    bestResolution = resolution;

                    // Check if we got an exact match exit the loop as no other resolution would be better
                    if (widthDiff + heightDiff == 0)
                    {
                        break;
                    }
                }
            }
            
            if (bestResolution == nullptr)
            {
                // This device doesn't meet the width/height constraints
                continue;
            }
            
            // Count a fully satisfied width or height constraint towards the overal device satisfaction
            fullySatisfiedCapabilityCount += bestWidthDiff == 0 ? 1 : 0;
            fullySatisfiedCapabilityCount += bestHeightDiff == 0 ? 1 : 0;

            // At this point we have a device that fully satisfies all given constraints, we'll have to search
            // all devices to find the one that meets the most constraints
            if (bestCameraDevice == nullptr || fullySatisfiedCapabilityCount > bestFullySatisfiedCapabilityCount)
            {
                bestCameraDevice = cameraDevice;
                bestCameraResolution = bestResolution;
                bestFullySatisfiedCapabilityCount = fullySatisfiedCapabilityCount;
            }
        }
        
        return {bestCameraDevice, bestCameraResolution};
    }

    bool MediaStream::UpdateConstraints(Napi::Env env, Napi::Object constraints)
    {
        bool allConstraintsSatisfied{ true };
        m_currentConstraints = Napi::Persistent(Napi::Object::New(env));
        
        for(uint32_t i=0; i < m_cameraDevice->capabilities.size(); i++)
        {
            auto capability{ m_cameraDevice->capabilities[i] };
            CameraCapability::MeetsConstraint constraintSatisfaction{ capability->meetsConstraints(constraints) };
            
            switch (constraintSatisfaction)
            {
                case CameraCapability::MeetsConstraint::Unsatisfied:
                    // The constraint couldn't be satisfied ignore it and continue applying the remaining constraints
                    allConstraintsSatisfied = false;
                    continue;
                case CameraCapability::MeetsConstraint::FullySatisfied:
                case CameraCapability::MeetsConstraint::PartiallySatisfied:
                    m_currentConstraints.Set(capability->getName(), constraints.Get(capability->getName()));
                    break;
                case CameraCapability::MeetsConstraint::Unconstrained:
                    // Still apply the unconstrained capability so that it resets to it's default in case it was previously set
                    break;
            }
            
            if(!capability->applyConstraints(constraints))
            {
                // Setting the constraint to the capability failed
                allConstraintsSatisfied = false;
            }
        }
        
        return allConstraintsSatisfied;
    }

    void MediaStream::UpdateTexture(bgfx::TextureHandle textureHandle)
    {
        m_cameraImpl->UpdateCameraTexture(textureHandle);
    }
}
