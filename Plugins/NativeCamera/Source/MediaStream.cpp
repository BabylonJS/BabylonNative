#if defined(_MSC_VER)
    // Disable the compiler warning for unreachable code due to CameraDevice being stubbed out on the windows platform
    #pragma warning(disable: 4702)
#endif

#include "MediaStream.h"
#include "CameraDevice.h"
#include "Constraint.h"
#include <assert.h>

namespace Babylon::Plugins
{
    arcana::task<Napi::Object, std::exception_ptr> MediaStream::NewAsync(Napi::Env env, Napi::Object constraints)
    {
        // Create a an object reference to the mediaStream javascript object so that it is not destructed during the async operation
        auto mediaStreamObject{Napi::Persistent(GetConstructor(env).New({}))};
        auto mediaStream{MediaStream::Unwrap(mediaStreamObject.Value())};
        
        return mediaStream->ApplyInitialConstraintsAsync(env, constraints).then(mediaStream->m_runtimeScheduler, arcana::cancellation::none(), [mediaStreamObject{std::move(mediaStreamObject)}]()
        {
            return mediaStreamObject.Value();
        });
    }

    Napi::Function MediaStream::GetConstructor(Napi::Env env)
    {
        Napi::Object _native{JsRuntime::NativeObject::GetFromJavaScript(env)};
        Napi::Value ctorValue{_native.Get(JS_CLASS_NAME)};
        Napi::Function ctor{ctorValue.IsFunction() ? ctorValue.As<Napi::Function>() : Napi::Function{}};

        if (ctor.IsEmpty() || ctor.IsUndefined())
        {
            // Initialize the persistent constructor
            ctor = DefineClass(
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
                    InstanceMethod("stop", &MediaStream::Stop),
                });
            
            _native.Set(JS_CLASS_NAME, ctor);
        }
        
        return ctor;
    }

    MediaStream::MediaStream(const Napi::CallbackInfo& info)
        : Napi::ObjectWrap<MediaStream>{info}
        , m_runtimeScheduler{JsRuntime::GetFromJavaScript(info.Env())}
    {
    }

    MediaStream::~MediaStream()
    {
        if (m_cameraDevice != nullptr)
        {
            // The cameraDevice should be destroyed on the JS thread as it may need to access main thread resources
            // move ownership of the cameraDevice to a lambda and dispatch it with the runtimeScheduler so the destructor
            // is called from that thread.
            m_runtimeScheduler([cameraDevice = std::move(m_cameraDevice)](){});
        }
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
        if (!UpdateConstraints(env, constraints))
        {
            // Setting the constraint to the capability failed
            deferred.Reject(Napi::Error::New(env, "OverconstrainedError: Unable to match constraints to a supported camera configuration.").Value());
            return static_cast<Napi::Value>(promise);
        }
        deferred.Resolve(env.Undefined());

        return static_cast<Napi::Value>(promise);
    }

    arcana::task<void, std::exception_ptr> MediaStream::ApplyInitialConstraintsAsync(Napi::Env env, Napi::Object constraints)
    {
        // A camera device hasn't been selected yet. Find the best device and open the stream
        auto bestCamera = FindBestCameraStream(env, constraints);

        // If m_cameraDevice is still null that means a camera device could not be found that meets the constraints
        if (!bestCamera.has_value())
        {
            // If no device could be fount to satisfy the constraints throw a "ConstraintError" to match the browser implementation
            throw std::runtime_error{"ConstraintError: Unable to match constraints to a supported camera configuration."};
        }

        // We need to create a shared_ptr to the CameraDevice because internally it calls shared_from_this
        m_cameraDevice = std::make_shared<CameraDevice>(std::move(bestCamera->first));

        // Create a persistent ref to the constraints object so it isn't destructed during our async work
        auto constraintsRef{Napi::Persistent(constraints)};

        return m_cameraDevice->OpenAsync(bestCamera.value().second).then(m_runtimeScheduler, arcana::cancellation::none(), [this, env, constraintsRef{std::move(constraintsRef)}](CameraDevice::CameraDimensions cameraDimensions) {
            this->Width = cameraDimensions.width;
            this->Height = cameraDimensions.height;
            
            if (!UpdateConstraints(env, constraintsRef.Value()))
            {
                // Setting the constraint to the capability failed
                throw std::runtime_error{"ConstraintError: Unable to match constraints to a supported camera configuration."};
            }
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
        
        for (const auto& capability : m_cameraDevice->Capabilities())
        {
            capability->AddAsCapability(capabilities);
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
        
        for (const auto& capability : m_cameraDevice->Capabilities())
        {
            capability->AddAsSetting(settings);
        }
        
        return std::move(settings);
    }

    Napi::Value MediaStream::GetConstraints(const Napi::CallbackInfo& info)
    {
        auto env = info.Env();
        
        // return a clone of the currently applied constraints
        return Napi::Object::From(env, m_currentConstraints.Value());
    }

    std::optional<std::pair<CameraDevice, const CameraTrack&>> MediaStream::FindBestCameraStream(Napi::Env env, Napi::Object constraints)
    {
        // Get the available camera devices
        std::vector<CameraDevice> cameraDevices{CameraDevice::GetCameraDevices(env)};
        
        std::optional<std::pair<CameraDevice, const CameraTrack&>> bestCameraConfiguration{};
        int32_t bestFullySatisfiedCapabilityCount{0};
        
        // The camera devices should be assumed to be sorted from best to worst. Pick the first camera device that fully
        // satisfies the most constraints without failing any.
        for (auto& cameraDevice : cameraDevices)
        {
            bool failedAConstraint{false};
            int32_t fullySatisfiedCapabilityCount{0};
            for (const auto& capability : cameraDevice.Capabilities())
            {
                Capability::MeetsConstraint constraintSatifaction{capability->MeetsConstraints(constraints)};
                switch (constraintSatifaction)
                {
                    case Capability::MeetsConstraint::FullySatisfied:
                        fullySatisfiedCapabilityCount++;
                        break;
                    case Capability::MeetsConstraint::PartiallySatisfied:
                    case Capability::MeetsConstraint::Unconstrained:
                        // Don't weight partialy satisfied or unconstrained capabilites any higher than another device
                        break;
                    case Capability::MeetsConstraint::Unsatisfied:
                    default:
                        failedAConstraint = true;
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
            const CameraTrack* bestResolution{nullptr};
            int32_t bestWidthDiff{INT32_MAX};
            int32_t bestHeightDiff{INT32_MAX};
            
            auto widthConstraint{Constraint::ParseConstraint<int32_t>(constraints.Get("width"))};
            auto heightConstraint{Constraint::ParseConstraint<int32_t>(constraints.Get("height"))};
            
            // Set the targetWidth and targetHeight as a fallback through the values exact, ideal, max, min
            auto targetWidth = widthConstraint.exact.has_value() ? widthConstraint.exact
                : widthConstraint.ideal.has_value() ? widthConstraint.ideal
                : widthConstraint.max.has_value() ? widthConstraint.max
                : widthConstraint.min;
            auto targetHeight = heightConstraint.exact.has_value() ? heightConstraint.exact
                : heightConstraint.ideal.has_value() ? heightConstraint.ideal
                : heightConstraint.max.has_value() ? heightConstraint.max
                : heightConstraint.min;

            for (uint32_t j = 0; j < cameraDevice.SupportedResolutions().size(); j++) {
                auto& resolution = cameraDevice.SupportedResolutions()[j];
                
                auto meetsWidthRequirements =
                    (!widthConstraint.exact.has_value() || widthConstraint.exact.value() == resolution.Width()) &&
                    (!widthConstraint.min.has_value() || widthConstraint.min.value() <= resolution.Width()) &&
                    (!widthConstraint.max.has_value() || widthConstraint.max.value() >= resolution.Width());
                
                auto meetsHeightRequirements =
                    (!heightConstraint.exact.has_value() || heightConstraint.exact.value() == resolution.Height()) &&
                    (!heightConstraint.min.has_value() || heightConstraint.min.value() <= resolution.Height()) &&
                    (!heightConstraint.max.has_value() || heightConstraint.max.value() >= resolution.Height());
                
                if (!meetsWidthRequirements || !meetsHeightRequirements)
                {
                    // The resolution doesn't meet the constraint requirements
                    continue;
                }
                
                int32_t widthDiff = targetWidth.has_value() ? abs(static_cast<int32_t>(targetWidth.value() - resolution.Width())) : 0;
                int32_t heightDiff = targetHeight.has_value() ? abs(static_cast<int32_t>(targetHeight.value() - resolution.Height())) : 0;
                
                if (bestResolution == nullptr || widthDiff + heightDiff < bestWidthDiff + bestHeightDiff)
                {
                    bestWidthDiff = widthDiff;
                    bestHeightDiff = heightDiff;
                    bestResolution = &resolution;

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
            if (!bestCameraConfiguration.has_value() || fullySatisfiedCapabilityCount > bestFullySatisfiedCapabilityCount)
            {
                bestCameraConfiguration.emplace(std::move(cameraDevice), *bestResolution);
                bestFullySatisfiedCapabilityCount = fullySatisfiedCapabilityCount;
            }
        }
        
        return bestCameraConfiguration;
    }

    bool MediaStream::UpdateConstraints(Napi::Env env, Napi::Object constraints)
    {
        bool allConstraintsSatisfied{true};
        m_currentConstraints = Napi::Persistent(Napi::Object::New(env));

        if (m_cameraDevice == nullptr)
        {
            // We don't have a cameraDevice selected yet.
            return false;
        }
        
        for(auto& capability : m_cameraDevice->Capabilities())
        {
            Capability::MeetsConstraint constraintSatisfaction{capability->MeetsConstraints(constraints)};
            
            switch (constraintSatisfaction)
            {
                case Capability::MeetsConstraint::FullySatisfied:
                case Capability::MeetsConstraint::PartiallySatisfied:
                    m_currentConstraints.Set(capability->GetName(), constraints.Get(capability->GetName()));
                    break;
                case Capability::MeetsConstraint::Unconstrained:
                    // Still apply the unconstrained capability so that it resets to it's default in case it was previously set
                    break;
                case Capability::MeetsConstraint::Unsatisfied:
                default:
                    // The constraint couldn't be satisfied ignore it and continue applying the remaining constraints
                    allConstraintsSatisfied = false;
                    continue;;
            }
            
            if(!capability->ApplyConstraints(constraints))
            {
                // Setting the constraint to the capability failed
                allConstraintsSatisfied = false;
            }
        }
        
        return allConstraintsSatisfied;
    }

    void MediaStream::Stop(const Napi::CallbackInfo& /*info*/)
    {
        // Clear out the cameraDevice which will disconnect the camera stream
        m_cameraDevice = nullptr;
    }

    bool MediaStream::UpdateTexture(bgfx::TextureHandle textureHandle)
    {
        bool dimensionsChanged = false;

        if (m_cameraDevice == nullptr)
        {
            // We don't have a cameraDevice selected yet.
            return dimensionsChanged;
        }

        auto cameraDimensions{m_cameraDevice->UpdateCameraTexture(textureHandle)};

        if (this->Width != cameraDimensions.width || this->Height != cameraDimensions.height)
        {
            dimensionsChanged = true;
            this->Width = cameraDimensions.width;
            this->Height = cameraDimensions.height;
        }

        return dimensionsChanged;
    }
}
