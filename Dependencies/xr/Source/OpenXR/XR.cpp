#include <XR.h>

#include "Windows/XrPlatform.h"
#include "XrSupportedExtensions.h"
#include "XrRegistry.h"
#include "SceneUnderstanding.h"

#include <assert.h>
#include <optional>
#include <arcana/threading/task.h>
#include <Input.h>

namespace xr
{
    namespace
    {
        uint32_t AquireAndWaitForSwapchainImage(XrSwapchain handle)
        {
            uint32_t swapchainImageIndex;
            XrSwapchainImageAcquireInfo acquireInfo{ XR_TYPE_SWAPCHAIN_IMAGE_ACQUIRE_INFO };
            XrCheck(xrAcquireSwapchainImage(handle, &acquireInfo, &swapchainImageIndex));

            XrSwapchainImageWaitInfo waitInfo{ XR_TYPE_SWAPCHAIN_IMAGE_WAIT_INFO };
            waitInfo.timeout = XR_INFINITE_DURATION;
            XrCheck(xrWaitSwapchainImage(handle, &waitInfo));

            return swapchainImageIndex;
        };

        std::unique_ptr<XrSessionContext> globalXrSessionContext{};
    }

    struct XrSessionContext::Impl
    {
        XrSessionContext::Impl()
            : Extensions(std::make_unique<XrSupportedExtensions>()) {}

        void PopulateExtensions()
        {
            if (Instance.Get() == XR_NULL_HANDLE)
            {
                throw std::runtime_error{ "Attempted to populate extensions when instance was null" };
            }

            Extensions->PopulateDispatchTable(Instance.Get());
        }

        InstanceHandle Instance;
        XrSystemId SystemId{ XR_NULL_SYSTEM_ID };
        SessionHandle Session;
        SpaceHandle SceneSpace;
        XrReferenceSpaceType SceneSpaceType{};
        std::atomic<XrSessionState> State{ XrSessionState::XR_SESSION_STATE_UNKNOWN };
        std::atomic<XrTime> DisplayTime{};
        std::unique_ptr<XrSupportedExtensions> Extensions;
        xr::SceneUnderstanding SceneUnderstanding{};
        bool IsSessionRunning{ false };
    };

    XrSessionContext::XrSessionContext()
        : ContextImpl(std::make_unique<Impl>()) {}

    XrSessionContext::~XrSessionContext() {}

    bool OPENXR_CONTEXT_INTERFACE_API XrSessionContext::IsInitialized() const
    {
        return ContextImpl->Session.Get() != XR_NULL_HANDLE &&
            ContextImpl->SceneSpace.Get() != XR_NULL_HANDLE &&
            ContextImpl->State != XrSessionState::XR_SESSION_STATE_UNKNOWN;
    }

    xr::ExtensionDispatchTable* OPENXR_CONTEXT_INTERFACE_API XrSessionContext::ExtensionDispatchTable() const
    {
        return ContextImpl->Extensions.get();
    }

    XrInstance OPENXR_CONTEXT_INTERFACE_API XrSessionContext::Instance() const
    {
        return ContextImpl->Instance.Get();
    }

    XrSystemId OPENXR_CONTEXT_INTERFACE_API XrSessionContext::SystemId() const
    {
        return ContextImpl->SystemId;
    }

    XrTime OPENXR_CONTEXT_INTERFACE_API XrSessionContext::DisplayTime() const
    {
        return ContextImpl->DisplayTime;
    }

    bool OPENXR_CONTEXT_INTERFACE_API XrSessionContext::TryEnableExtension(const char* name) const
    {
        return ContextImpl->Extensions->TryEnableExtension(name);
    }

    bool OPENXR_CONTEXT_INTERFACE_API XrSessionContext::IsExtensionEnabled(const char* name) const
    {
        return ContextImpl->Extensions->IsExtensionSupported(name);
    }

    XrSession OPENXR_CONTEXT_INTERFACE_API XrSessionContext::Session() const
    {
        return ContextImpl->Session.Get();
    }

    XrSessionState OPENXR_CONTEXT_INTERFACE_API XrSessionContext::State() const
    {
        return ContextImpl->State;
    }

    XrSpace OPENXR_CONTEXT_INTERFACE_API XrSessionContext::Space() const
    {
        return ContextImpl->SceneSpace.Get();
    }

    bool OPENXR_CONTEXT_INTERFACE_API XrSessionContext::IsSessionRunning() const
    {
        return ContextImpl->IsSessionRunning;
    }

    XrResult OPENXR_CONTEXT_INTERFACE_API XrSessionContext::GetInstanceProcAddr(const char* name, PFN_xrVoidFunction* function) const
    {
        return xrGetInstanceProcAddr(ContextImpl->Instance.Get(), name, function);
    }

    const std::unique_ptr<XrSupportedExtensions>& XrSessionContext::Extensions() const
    {
        return ContextImpl->Extensions;
    }

    const SceneUnderstanding& XrSessionContext::SceneUnderstanding() const
    {
        return ContextImpl->SceneUnderstanding;
    }

    void XrRegistry::Reset()
    {
        globalXrSessionContext = nullptr;
    }

    const XrSessionContext& XrRegistry::Context()
    {
        if (globalXrSessionContext == nullptr)
        {
            globalXrSessionContext = std::make_unique<XrSessionContext>();
        }

        return *globalXrSessionContext;
    }

    uintptr_t XrRegistry::GetNativeXrContext()
    {
        if (globalXrSessionContext == nullptr)
        {
            globalXrSessionContext = std::make_unique<XrSessionContext>();
        }

        return reinterpret_cast<uintptr_t>(globalXrSessionContext.get());
    }

    std::string XrRegistry::GetNativeXrContextType()
    {
        return "OpenXR";
    }

    struct System::Impl
    {
        static constexpr XrFormFactor FORM_FACTOR{ XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY };

        const XrSessionContext& Context;

        static constexpr XrViewConfigurationType PrimaryViewConfigurationType = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;
        std::vector<XrViewConfigurationType> SupportedSecondaryViewConfigurationTypes{};
        const std::vector<XrEnvironmentBlendMode> SupportedEnvironmentBlendModes = {
            XR_ENVIRONMENT_BLEND_MODE_ADDITIVE,
            XR_ENVIRONMENT_BLEND_MODE_OPAQUE,
            XR_ENVIRONMENT_BLEND_MODE_ALPHA_BLEND,
        };
        std::unordered_map<XrViewConfigurationType, ViewProperties> ViewProperties{};

        std::string ApplicationName{};

        Impl(const std::string& applicationName)
            : ApplicationName{ applicationName }
            , Context(XrRegistry::Context())
        {}

        bool IsInitialized() const
        {
            return Context.ContextImpl->Instance.Get() != XR_NULL_HANDLE && Context.ContextImpl->SystemId != XR_NULL_SYSTEM_ID;
        }

        bool TryInitialize()
        {
            assert(!IsInitialized());

            if (Context.ContextImpl->Instance.Get() == XR_NULL_HANDLE)
            {
                InitializeXrInstanceAndExtensions();
            }

            assert(Context.ContextImpl->Extensions != nullptr);
            assert(Context.ContextImpl->SystemId == XR_NULL_SYSTEM_ID);

            return TryInitializeXrSystemIdAndViewProperties();
        }

    private:
        // Phase one of initialization. Cannot fail without crashing.
        void InitializeXrInstanceAndExtensions()
        {
            auto& extensions = Context.ContextImpl->Extensions;
            auto& instanceHandle = Context.ContextImpl->Instance;
            
            XrInstanceCreateInfo createInfo{ XR_TYPE_INSTANCE_CREATE_INFO };
            createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions->Names.size());
            createInfo.enabledExtensionNames = extensions->Names.data();
            createInfo.applicationInfo = { "", 1, "BabylonNative", 1, XR_CURRENT_API_VERSION };
            strcpy_s(createInfo.applicationInfo.applicationName, ApplicationName.c_str());
            XrCheck(xrCreateInstance(&createInfo, instanceHandle.Put()));

            Context.ContextImpl->PopulateExtensions();
        }

        void InitializeViewConfigurations()
        {
            const auto& instance = Context.Instance();
            const auto& systemId = Context.SystemId();
            
            assert(instance != XR_NULL_HANDLE);
            assert(systemId != XR_NULL_SYSTEM_ID);

            const auto systemSupportedViewConfigurationTypes = EnumerateViewConfigurations(instance, systemId);
            assert(std::find(systemSupportedViewConfigurationTypes.begin(), systemSupportedViewConfigurationTypes.end(), PrimaryViewConfigurationType) != systemSupportedViewConfigurationTypes.end());

            for (const auto& viewConfigType : systemSupportedViewConfigurationTypes)
            {
                if (viewConfigType == PrimaryViewConfigurationType)
                {
                    continue;
                }

                SupportedSecondaryViewConfigurationTypes.push_back(viewConfigType);
            }
        }

        // Phase two of initialization. Can fail and be retried without crashing.
        bool TryInitializeXrSystemIdAndViewProperties()
        {
            const auto& instance = Context.Instance();
            auto& systemId = Context.ContextImpl->SystemId;

            XrSystemGetInfo systemInfo{ XR_TYPE_SYSTEM_GET_INFO };
            systemInfo.formFactor = FORM_FACTOR;

            XrResult result = xrGetSystem(instance, &systemInfo, &systemId);
            if (result == XR_ERROR_FORM_FACTOR_UNAVAILABLE)
            {
                systemId = XR_NULL_SYSTEM_ID;
                return false;
            }
            else if(XR_FAILED(result))
            {
                throw std::runtime_error{ "SystemId initialization failed with unexpected result type." };
            }

            // Initialize view configurations before assessing view properties.
            InitializeViewConfigurations();

            // Find the available environment blend modes.
            ViewProperties[PrimaryViewConfigurationType] = CreateViewProperties(instance, systemId, PrimaryViewConfigurationType, SupportedEnvironmentBlendModes);
            for (const auto& viewConfiguration : SupportedSecondaryViewConfigurationTypes)
            {
                ViewProperties[viewConfiguration] = CreateViewProperties(instance, systemId, viewConfiguration, SupportedEnvironmentBlendModes);
            }

            return true;
        }
    };

    struct System::Session::Impl
    {
        const System::Impl& HmdImpl;

        static constexpr uint32_t LeftSide = 0;
        static constexpr uint32_t RightSide = 1;

        struct Swapchain
        {
            XrSwapchain Handle{};
            SwapchainFormat Format{};
            int32_t Width{ 0 };
            int32_t Height{ 0 };
            uint32_t ArraySize{ 0 };
            std::vector<SwapchainImage> Images{};
        };

        struct RenderResource
        {
            ViewConfigurationState ViewState;
            Swapchain ColorSwapchain{};
            Swapchain DepthSwapchain{};
            std::vector<XrCompositionLayerProjectionView> ProjectionLayerViews;
            std::vector<XrCompositionLayerDepthInfoKHR> DepthInfoViews;
        };

        struct
        {
            std::unordered_map<XrViewConfigurationType, RenderResource> ResourceMap{};
            std::vector<Frame::View> ActiveFrameViews{};
        } RenderResources{};

        struct
        {
            std::vector<System::Session::Frame::InputSource> ActiveInputSources{};
            std::optional<System::Session::Frame::Space> EyeTrackerSpace{};
            std::vector<FeaturePoint> FeaturePointCloud{};
        } ActionResources{};

        const Input InputManager;

        float DepthNearZ{ DEFAULT_DEPTH_NEAR_Z };
        float DepthFarZ{ DEFAULT_DEPTH_FAR_Z };

        Impl(System::Impl& hmdImpl, void* graphicsContext)
            : HmdImpl{ hmdImpl }
        {
            assert(HmdImpl.IsInitialized());
            const auto& context = HmdImpl.Context;
            const auto& instance = context.Instance();
            const auto& systemId = context.SystemId();
            const auto& extensions = context.Extensions();
            auto& sessionHandle = context.ContextImpl->Session;
            auto& sceneSpaceHandle = context.ContextImpl->SceneSpace;
            auto& sceneSpaceType = context.ContextImpl->SceneSpaceType;

            // Create the session
            auto graphicsBinding = CreateGraphicsBinding(*extensions, instance, systemId, graphicsContext);
            XrSessionCreateInfo createInfo{ XR_TYPE_SESSION_CREATE_INFO };
            createInfo.next = &graphicsBinding;
            createInfo.systemId = systemId;
            XrCheck(xrCreateSession(instance, &createInfo, sessionHandle.Put()));

            // Initialize scene space
            if (extensions->UnboundedRefSpaceSupported)
            {
                sceneSpaceType = XR_REFERENCE_SPACE_TYPE_UNBOUNDED_MSFT;
            }
            else
            {
                sceneSpaceType = XR_REFERENCE_SPACE_TYPE_LOCAL;
            }
            XrReferenceSpaceCreateInfo spaceCreateInfo{ XR_TYPE_REFERENCE_SPACE_CREATE_INFO };
            spaceCreateInfo.referenceSpaceType = sceneSpaceType;
            spaceCreateInfo.poseInReferenceSpace = IDENTITY_TRANSFORM;
            XrCheck(xrCreateReferenceSpace(sessionHandle.Get(), &spaceCreateInfo, sceneSpaceHandle.Put()));

            InitializeRenderResources(instance, systemId);
        }

        ~Impl()
        {
            // Reset OpenXR Context to release session handle and exit immersive mode
            XrRegistry::Reset();
        }

        std::unique_ptr<System::Session::Frame> GetNextFrame(bool& shouldEndSession, bool& shouldRestartSession)
        {
            ProcessEvents(shouldEndSession, shouldRestartSession);

            if (!shouldEndSession)
            {
                return std::make_unique<Frame>(*this);
            }
            else
            {
                return nullptr;
            }
        }

        void RequestEndSession()
        {
            const auto& session = HmdImpl.Context.Session();
            xrRequestExitSession(session);
        }

        void PopulateSwapchains(ViewConfigurationState& viewState)
        {
            const auto& session = HmdImpl.Context.Session();
            const auto viewCount = static_cast<uint32_t>(viewState.Views.size());
            assert(viewCount > 0);

            // Select color and depth swapchain pixel formats
            SwapchainFormat colorSwapchainFormat;
            SwapchainFormat depthSwapchainFormat;
            SelectSwapchainPixelFormats(colorSwapchainFormat, depthSwapchainFormat);

            auto& renderResource = RenderResources.ResourceMap[viewState.Type];
            
            // All of the view config views should have the same recommended sizes,
            // as long as we're working with stereoscopic HMD's.
            const auto& viewConfigView = viewState.ViewConfigViews[0];

            PopulateSwapchain(session,
                colorSwapchainFormat,
                viewConfigView.recommendedImageRectWidth,
                viewConfigView.recommendedImageRectHeight,
                viewCount,
                viewConfigView.recommendedSwapchainSampleCount,
                0,
                XR_SWAPCHAIN_USAGE_SAMPLED_BIT | XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT,
                viewState.Type,
                renderResource.ColorSwapchain);
            PopulateSwapchain(session,
                depthSwapchainFormat,
                viewConfigView.recommendedImageRectWidth,
                viewConfigView.recommendedImageRectHeight,
                viewCount,
                viewConfigView.recommendedSwapchainSampleCount,
                0,
                XR_SWAPCHAIN_USAGE_SAMPLED_BIT | XR_SWAPCHAIN_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                viewState.Type,
                renderResource.DepthSwapchain);
        }

        void CleanupSwapchains(ViewConfigurationState& viewState)
        {
            auto& renderResource = RenderResources.ResourceMap[viewState.Type];
            renderResource.ColorSwapchain = {};
            renderResource.DepthSwapchain = {};
        }

        Anchor CreateAnchor(Pose pose, NativeTrackablePtr trackable, XrTime time)
        {
            const auto& session = HmdImpl.Context.Session();
            const auto& apiExtensions = *HmdImpl.Context.Extensions();
            const auto& sceneSpace = HmdImpl.Context.Space();

            if (!apiExtensions.SpatialAnchorSupported)
            {
                throw std::runtime_error("Spatial anchors are not supported for this device.");
            }

            if (trackable != nullptr)
            {
                throw std::runtime_error("Anchors created from hit test results not yet implemented for OpenXR.");
            }

            auto anchorSpace = std::make_shared<OpenXRAnchorSpace>();

            XrSpatialAnchorCreateInfoMSFT anchorCreateInfo{XR_TYPE_SPATIAL_ANCHOR_CREATE_INFO_MSFT};
            anchorCreateInfo.space = sceneSpace;
            anchorCreateInfo.pose = { pose.Orientation.X, pose.Orientation.Y, pose.Orientation.Z, pose.Orientation.W,
                                      pose.Position.X, pose.Position.Y, pose.Position.Z };
            anchorCreateInfo.time = time;

            CHECK_XRCMD(apiExtensions.xrCreateSpatialAnchorMSFT(
                session,
                &anchorCreateInfo,
                anchorSpace->Anchor.Put(apiExtensions.xrDestroySpatialAnchorMSFT)));

            XrSpatialAnchorSpaceCreateInfoMSFT anchorSpaceCreateInfo{XR_TYPE_SPATIAL_ANCHOR_SPACE_CREATE_INFO_MSFT};
            anchorSpaceCreateInfo.anchor = anchorSpace->Anchor.Get();
            anchorSpaceCreateInfo.poseInAnchorSpace = xr::math::Pose::Identity();
            CHECK_XRCMD(apiExtensions.xrCreateSpatialAnchorSpaceMSFT(session, &anchorSpaceCreateInfo, anchorSpace->Space.Put()));

            const auto nativeAnchorPtr = reinterpret_cast<NativeAnchorPtr>(anchorSpace->Anchor.Get());
            openXRAnchors[nativeAnchorPtr] = anchorSpace;
            return { pose, nativeAnchorPtr };
        }

        void UpdateAnchor(Anchor& anchor, XrTime time) const
        {
            const auto& sceneSpace = HmdImpl.Context.Space();
            const auto anchorEntry = openXRAnchors.find(anchor.NativeAnchor);
            if (anchorEntry == openXRAnchors.end())
            {
                throw std::runtime_error("Could not find this anchor in the OpenXR collection. "
                    "Perhaps the anchor was deleted or the anchor was created with a different XR implementation.");
            }

            auto anchorSpace = anchorEntry->second;

            XrSpaceLocation spaceLocation{XR_TYPE_SPACE_LOCATION};
            auto spaceLocationResult = xrLocateSpace(anchorSpace->Space.Get(), sceneSpace, time, &spaceLocation);
            if (XR_FAILED(spaceLocationResult))
            {
                anchor.IsValid = false;
                return;
            }

            if (xr::math::Pose::IsPoseValid(spaceLocation)) 
            {
                anchor.Pose = {
                    spaceLocation.pose.position.x, spaceLocation.pose.position.y, spaceLocation.pose.position.z,
                    spaceLocation.pose.orientation.x, spaceLocation.pose.orientation.y, spaceLocation.pose.orientation.z, spaceLocation.pose.orientation.w 
                };
            }
        }

        void DeleteAnchor(Anchor& anchor)
        {
            openXRAnchors.erase(anchor.NativeAnchor);
        }

    private:
        struct OpenXRAnchorSpace
        {
            xr::SpatialAnchorHandle Anchor;
            xr::SpaceHandle Space;
        };

        std::unordered_map<NativeAnchorPtr, std::shared_ptr<OpenXRAnchorSpace>> openXRAnchors{};

        void InitializeRenderResources(XrInstance instance, XrSystemId systemId)
        {
            // Read graphics properties for preferred swapchain length and logging, and hand tracking availability.
            XrSystemEyeGazeInteractionPropertiesEXT eyeTrackingSystemProperties{ XR_TYPE_SYSTEM_EYE_GAZE_INTERACTION_PROPERTIES_EXT};
            XrSystemHandTrackingPropertiesEXT handTrackingSystemProperties{ XR_TYPE_SYSTEM_HAND_TRACKING_PROPERTIES_EXT, &eyeTrackingSystemProperties };
            XrSystemProperties systemProperties{ XR_TYPE_SYSTEM_PROPERTIES, &handTrackingSystemProperties };
            XrCheck(xrGetSystemProperties(instance, systemId, &systemProperties));

            // TODO: Moving the xrGetSystemProperties call into its own function might make this make more sense.
            const Input::InitOptions inputInitOptions = {
                HmdImpl.Context.Instance(),
                HmdImpl.Context.Session(),
                *HmdImpl.Context.Extensions(),
                handTrackingSystemProperties,
                eyeTrackingSystemProperties
            };
            InputManager.Initialize(inputInitOptions);

            const XrViewConfigurationType primaryType = HmdImpl.PrimaryViewConfigurationType;
            auto& primaryRenderResource = RenderResources.ResourceMap[primaryType];
            primaryRenderResource.ViewState = CreateViewConfigurationState(primaryType, instance, systemId);
            for (const auto& viewConfiguration : HmdImpl.SupportedSecondaryViewConfigurationTypes)
            {
                RenderResources.ResourceMap[viewConfiguration].ViewState = CreateViewConfigurationState(viewConfiguration, instance, systemId);
            }

            // Create the swapchains for the primary view configuration type. Secondary view configuration type swapchains will be populated once activated.
            PopulateSwapchains(primaryRenderResource.ViewState);
        }

        void PopulateSwapchain(XrSession session,
            SwapchainFormat format,
            int32_t width,
            int32_t height,
            uint32_t arraySize,
            uint32_t sampleCount,
            XrSwapchainCreateFlags createFlags,
            XrSwapchainUsageFlags usageFlags,
            XrViewConfigurationType viewConfigType,
            Swapchain& swapchain)
        {
            swapchain.Format = format;
            swapchain.Width = width;
            swapchain.Height = height;
            swapchain.ArraySize = arraySize;

            XrSwapchainCreateInfo swapchainCreateInfo{ XR_TYPE_SWAPCHAIN_CREATE_INFO };
            swapchainCreateInfo.arraySize = arraySize;
            swapchainCreateInfo.format = format;
            swapchainCreateInfo.width = width;
            swapchainCreateInfo.height = height;
            swapchainCreateInfo.mipCount = 1;
            swapchainCreateInfo.faceCount = 1;
            swapchainCreateInfo.sampleCount = sampleCount;
            swapchainCreateInfo.createFlags = createFlags;
            swapchainCreateInfo.usageFlags = usageFlags;

            XrSecondaryViewConfigurationSwapchainCreateInfoMSFT secondaryViewConfigCreateInfo
            {
                XR_TYPE_SECONDARY_VIEW_CONFIGURATION_SWAPCHAIN_CREATE_INFO_MSFT
            };
            if (HmdImpl.Context.Extensions()->SecondaryViewConfigurationSupported && 
                viewConfigType == XR_VIEW_CONFIGURATION_TYPE_SECONDARY_MONO_FIRST_PERSON_OBSERVER_MSFT)
            {
                secondaryViewConfigCreateInfo.viewConfigurationType = viewConfigType;
                swapchainCreateInfo.next = &secondaryViewConfigCreateInfo;
            }

            XrCheck(xrCreateSwapchain(session, &swapchainCreateInfo, &swapchain.Handle));

            uint32_t imageCount{ 0 };
            XrCheck(xrEnumerateSwapchainImages(swapchain.Handle, 0, &imageCount, nullptr));
            swapchain.Images.resize(imageCount, { SWAPCHAIN_IMAGE_TYPE_ENUM });
            XrCheck(xrEnumerateSwapchainImages(swapchain.Handle, static_cast<uint32_t>(swapchain.Images.size()), &imageCount,
                reinterpret_cast<XrSwapchainImageBaseHeader*>(swapchain.Images.data())));
        }

        void SelectSwapchainPixelFormats(SwapchainFormat& colorFormat, SwapchainFormat& depthFormat)
        {
            const auto& session = HmdImpl.Context.Session();

            // Query runtime preferred swapchain formats. Two-call idiom.
            uint32_t swapchainFormatCount;
            XrCheck(xrEnumerateSwapchainFormats(session, 0, &swapchainFormatCount, nullptr));
            std::vector<int64_t> swapchainFormats(swapchainFormatCount);
            XrCheck(xrEnumerateSwapchainFormats(session, static_cast<uint32_t>(swapchainFormats.size()), &swapchainFormatCount, swapchainFormats.data()));

            auto colorFormatPtr = std::find_first_of(
                std::begin(SUPPORTED_COLOR_FORMATS),
                std::end(SUPPORTED_COLOR_FORMATS),
                std::begin(swapchainFormats),
                std::end(swapchainFormats));
            if (colorFormatPtr == std::end(SUPPORTED_COLOR_FORMATS))
            {
                throw std::runtime_error{ "No runtime swapchain format is supported for color." };
            }

            auto depthFormatPtr = std::find_first_of(
                std::begin(SUPPORTED_DEPTH_FORMATS),
                std::end(SUPPORTED_DEPTH_FORMATS),
                std::begin(swapchainFormats),
                std::end(swapchainFormats));
            if (depthFormatPtr == std::end(SUPPORTED_DEPTH_FORMATS))
            {
                throw std::runtime_error{ "No runtime swapchain format is supported for depth." };
            }

            colorFormat = static_cast<SwapchainFormat>(*colorFormatPtr);
            depthFormat = static_cast<SwapchainFormat>(*depthFormatPtr);
        }

        bool TryReadNextEvent(XrEventDataBuffer& buffer) const
        {
            // Reset buffer header for every xrPollEvent function call.
            buffer = { XR_TYPE_EVENT_DATA_BUFFER };
            const XrResult xr = xrPollEvent(HmdImpl.Context.Instance(), &buffer);
            return xr != XR_EVENT_UNAVAILABLE;
        }

        void ProcessSessionState(bool& exitRenderLoop, bool& requestRestart)
        {
            const auto& session = HmdImpl.Context.Session();
            const auto& sessionState = HmdImpl.Context.State();
            auto& isSessionRunning = HmdImpl.Context.ContextImpl->IsSessionRunning;
            switch (sessionState)
            {
            case XR_SESSION_STATE_READY:
            {
                assert(session != XR_NULL_HANDLE);
                XrSessionBeginInfo sessionBeginInfo{ XR_TYPE_SESSION_BEGIN_INFO };
                sessionBeginInfo.primaryViewConfigurationType = HmdImpl.PrimaryViewConfigurationType;

                XrSecondaryViewConfigurationSessionBeginInfoMSFT secondaryViewConfigSessionBeginInfo
                {
                    XR_TYPE_SECONDARY_VIEW_CONFIGURATION_SESSION_BEGIN_INFO_MSFT
                };
                const auto& supportedSecondaryViewConfigTypes = HmdImpl.SupportedSecondaryViewConfigurationTypes;
                if (HmdImpl.Context.Extensions()->SecondaryViewConfigurationSupported &&
                    HmdImpl.Context.Extensions()->FirstPersonObserverSupported &&
                    supportedSecondaryViewConfigTypes.size() > 0)
                {
                    secondaryViewConfigSessionBeginInfo.viewConfigurationCount = static_cast<uint32_t>(supportedSecondaryViewConfigTypes.size());
                    secondaryViewConfigSessionBeginInfo.enabledViewConfigurationTypes = supportedSecondaryViewConfigTypes.data();
                    InsertExtensionStruct(sessionBeginInfo, secondaryViewConfigSessionBeginInfo);
                }

                XrCheck(xrBeginSession(session, &sessionBeginInfo));
                isSessionRunning = true;
                break;
            }
            case XR_SESSION_STATE_STOPPING:
                XrCheck(xrEndSession(session));
                isSessionRunning = false;
                break;
            case XR_SESSION_STATE_EXITING:
                // Do not attempt to restart because user closed this session.
                exitRenderLoop = true;
                requestRestart = false;
                break;
            case XR_SESSION_STATE_LOSS_PENDING:
                // Poll for a new systemId
                exitRenderLoop = true;
                requestRestart = true;
                break;
            }
        }

        void ProcessEvents(bool& exitRenderLoop, bool& requestRestart)
        {
            auto& sessionState = HmdImpl.Context.ContextImpl->State;
            exitRenderLoop = false;
            requestRestart = false;

            XrEventDataBuffer buffer{ XR_TYPE_EVENT_DATA_BUFFER };
            XrEventDataBaseHeader* header = reinterpret_cast<XrEventDataBaseHeader*>(&buffer);

            // Process all pending messages.
            while (TryReadNextEvent(buffer))
            {
                switch (header->type)
                {
                case XR_TYPE_EVENT_DATA_INSTANCE_LOSS_PENDING:
                    exitRenderLoop = true;
                    requestRestart = false;
                    return;
                case XR_TYPE_EVENT_DATA_SESSION_STATE_CHANGED:
                    const auto stateEvent = *reinterpret_cast<const XrEventDataSessionStateChanged*>(header);
                    assert(HmdImpl.Context.Session() != XR_NULL_HANDLE && HmdImpl.Context.Session() == stateEvent.session);
                    sessionState = stateEvent.state;
                    ProcessSessionState(exitRenderLoop, requestRestart);
                    break;
                case XR_TYPE_EVENT_DATA_INTERACTION_PROFILE_CHANGED:
                    InputManager.RefreshInputSources(
                        HmdImpl.Context.Instance(), 
                        HmdImpl.Context.Session(), 
                        ActionResources.ActiveInputSources);
                    break;
                case XR_TYPE_EVENT_DATA_REFERENCE_SPACE_CHANGE_PENDING:
                default:
                    // DEBUG_PRINT("Ignoring event type %d", header->type);
                    break;
                }
            }
        }
    };

    struct System::Session::Frame::Impl
    {
        Impl(Session::Impl& sessionImpl)
            : sessionImpl{sessionImpl}
        {
        }

        Session::Impl& sessionImpl;
        bool shouldRender{};

        void LocateViews(const XrViewConfigurationType viewConfigType, uint32_t& viewCountOutput)
        {
            const auto& context = sessionImpl.HmdImpl.Context;
            const auto& session = context.Session();
            const auto& sceneSpace = context.Space();
            const auto displayTime = context.DisplayTime();
            auto& renderResource = sessionImpl.RenderResources.ResourceMap.at(viewConfigType);

            ViewConfigurationState& viewConfigurationState = renderResource.ViewState;
            uint32_t viewCapacityInput = static_cast<uint32_t>(viewConfigurationState.Views.size());
            XrViewState viewState{ XR_TYPE_VIEW_STATE };
            XrViewLocateInfo viewLocateInfo{ XR_TYPE_VIEW_LOCATE_INFO };
            viewLocateInfo.viewConfigurationType = viewConfigType;
            viewLocateInfo.displayTime = displayTime;
            viewLocateInfo.space = sceneSpace;
            XrCheck(xrLocateViews(session, &viewLocateInfo, &viewState, viewCapacityInput, &viewCountOutput, viewConfigurationState.Views.data()));

            assert(viewCountOutput == viewCapacityInput);
            assert(viewCountOutput == renderResource.ColorSwapchain.ArraySize);
            assert(viewCountOutput == renderResource.DepthSwapchain.ArraySize);

            renderResource.ProjectionLayerViews.resize(viewCountOutput);
            if (context.Extensions()->DepthExtensionSupported)
            {
                renderResource.DepthInfoViews.resize(viewCountOutput);
            }
        }

        void PopulateViewConfigurationState(
            System::Session::Impl::RenderResource& renderResource,
            std::vector<xr::System::Session::Frame::View>::iterator viewsStart,
            std::vector<xr::System::Session::Frame::View>::iterator viewsEnd,
            const bool depthSupported,
            const bool isPrimaryObserver)
        {
            const auto& colorSwapchain = renderResource.ColorSwapchain;
            const auto& depthSwapchain = renderResource.DepthSwapchain;
            const auto colorSwapchainImageIndex = AquireAndWaitForSwapchainImage(colorSwapchain.Handle);
            const auto depthSwapchainImageIndex = AquireAndWaitForSwapchainImage(depthSwapchain.Handle);

            uint32_t viewIdx = 0;
            for (auto viewIter = viewsStart; viewIter < viewsEnd; viewIter++, viewIdx++)
            {
                auto& currentView = *viewIter;
                const auto& cachedView = renderResource.ViewState.Views.at(viewIdx);

                // Use the full range of recommended image size to achieve optimum resolution
                const XrRect2Di imageRect = { {0, 0}, { colorSwapchain.Width, colorSwapchain.Height } };
                assert(colorSwapchain.Width == depthSwapchain.Width);
                assert(colorSwapchain.Height == depthSwapchain.Height);

                // Populate the struct that consuming code will use for rendering.
                PopulateView(cachedView, colorSwapchain, colorSwapchainImageIndex, depthSwapchain, depthSwapchainImageIndex, currentView);
        
                // Set is first person observer flag to true.
                currentView.IsFirstPersonObserver = isPrimaryObserver;

                renderResource.ProjectionLayerViews[viewIdx] = { XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW };
                auto& projectionLayerView = renderResource.ProjectionLayerViews[viewIdx];
                PopulateProjectionView(cachedView, colorSwapchain, imageRect, viewIdx, projectionLayerView);

                if (depthSupported)
                {
                    renderResource.DepthInfoViews[viewIdx] = { XR_TYPE_COMPOSITION_LAYER_DEPTH_INFO_KHR };
                    auto& depthInfoView = renderResource.DepthInfoViews[viewIdx];
                    PopulateDepthInfoView(depthSwapchain, imageRect, viewIdx, depthInfoView);
        
                    // Chain depth info struct to the corresponding projection layer views's next
                    projectionLayerView.next = &depthInfoView;
                }
            }
        }

        void PopulateProjectionMatrix(const XrView& cachedView, xr::System::Session::Frame::View& view) 
        {
            const float n{sessionImpl.DepthNearZ};
            const float f{sessionImpl.DepthFarZ};

            const float l{std::tanf(cachedView.fov.angleLeft)  * n};
            const float r{std::tanf(cachedView.fov.angleRight) * n};
            const float t{std::tanf(cachedView.fov.angleUp)    * n};
            const float b{std::tanf(cachedView.fov.angleDown)  * n};

            // Taken from BGFX math mtxProj().
            // See also D3DXMatrixPerspectiveOffCenterRH.
            const float invDiffRL = 1.f / (r - l);
            const float invDiffTB = 1.f / (t - b);
            const float ww  =  2.f * n * invDiffRL;
            const float hh  =  2.f * n * invDiffTB;
            const float xx  = (r + l) * invDiffRL;
            const float yy  = (t + b) * invDiffTB;

            // TODO: Set this based on the graphics API in use.
            // If true, the NDC depth ranges from [-1, 1] (OpenGL/Vulkan)
            // If false, the NDC depth ranges from [0, 1] (D3D)
            constexpr bool homogeneousDepth = false;

            // We negate the values za and zc to create a right-handed projection matrix,
            // since the supplied nearZ and farZ are positive.
            const float diffFN = f - n;
            const float za = -(homogeneousDepth ? (f + n) / diffFN : f / diffFN);
            const float zb = -(homogeneousDepth ? (2.f * f * n) / diffFN : n * -za);
            constexpr float zc = -1.f;

            view.ProjectionMatrix = {
                ww, 0,  0,  0,
                0,  hh, 0,  0,
                xx, yy, za, zc,
                0,  0,  zb, 0
            };
        }

        void PopulateView(const XrView& cachedView,
            const xr::System::Session::Impl::Swapchain& colorSwapchain,
            const uint32_t colorSwapchainImageIndex,
            const xr::System::Session::Impl::Swapchain& depthSwapchain,
            const uint32_t depthSwapchainImageIndex,
            xr::System::Session::Frame::View& view)
        {
            view.Space.Pose.Position.X = cachedView.pose.position.x;
            view.Space.Pose.Position.Y = cachedView.pose.position.y;
            view.Space.Pose.Position.Z = cachedView.pose.position.z;
            view.Space.Pose.Orientation.X = cachedView.pose.orientation.x;
            view.Space.Pose.Orientation.Y = cachedView.pose.orientation.y;
            view.Space.Pose.Orientation.Z = cachedView.pose.orientation.z;
            view.Space.Pose.Orientation.W = cachedView.pose.orientation.w;
            view.ColorTextureFormat = SwapchainFormatToTextureFormat(colorSwapchain.Format);
            view.ColorTexturePointer = colorSwapchain.Images[colorSwapchainImageIndex].texture;
            view.ColorTextureSize.Width = colorSwapchain.Width;
            view.ColorTextureSize.Height = colorSwapchain.Height;
            view.ColorTextureSize.Depth = colorSwapchain.ArraySize;
            view.DepthTextureFormat = SwapchainFormatToTextureFormat(depthSwapchain.Format);
            view.DepthTexturePointer = depthSwapchain.Images[depthSwapchainImageIndex].texture;
            view.DepthTextureSize.Width = depthSwapchain.Width;
            view.DepthTextureSize.Height = depthSwapchain.Height;
            view.DepthTextureSize.Depth = depthSwapchain.ArraySize;
            view.DepthNearZ = sessionImpl.DepthNearZ;
            view.DepthFarZ = sessionImpl.DepthFarZ;

            PopulateProjectionMatrix(cachedView, view);
        }

        void PopulateProjectionView(const XrView& cachedView,
            const xr::System::Session::Impl::Swapchain& colorSwapchain,
            const XrRect2Di imageRect,
            const uint32_t imageArrayIndex,
            XrCompositionLayerProjectionView& projectionLayerView)
        {
            projectionLayerView.pose = cachedView.pose;
            projectionLayerView.fov = cachedView.fov;
            projectionLayerView.subImage.swapchain = colorSwapchain.Handle;
            projectionLayerView.subImage.imageRect = imageRect;
            projectionLayerView.subImage.imageArrayIndex = imageArrayIndex;
        }

        void PopulateDepthInfoView(const xr::System::Session::Impl::Swapchain& depthSwapchain,
            const XrRect2Di imageRect,
            const uint32_t imageArrayIndex,
            XrCompositionLayerDepthInfoKHR& depthInfoView)
        {
            depthInfoView.minDepth = 0;
            depthInfoView.maxDepth = 1;
            depthInfoView.nearZ = sessionImpl.DepthNearZ;
            depthInfoView.farZ = sessionImpl.DepthFarZ;
            depthInfoView.subImage.swapchain = depthSwapchain.Handle;
            depthInfoView.subImage.imageRect = imageRect;
            depthInfoView.subImage.imageArrayIndex = imageArrayIndex;
        }
    };

    System::Session::Frame::Frame(Session::Impl& sessionImpl)
        : Views{ sessionImpl.RenderResources.ActiveFrameViews }
        , InputSources{ sessionImpl.ActionResources.ActiveInputSources }
        , EyeTrackerSpace{ sessionImpl.ActionResources.EyeTrackerSpace }
        , FeaturePointCloud{ sessionImpl.ActionResources.FeaturePointCloud } // NYI
        , UpdatedSceneObjects{}
        , RemovedSceneObjects{}
        , UpdatedPlanes{}
        , RemovedPlanes{}
        , UpdatedMeshes{}
        , RemovedMeshes{}
        // TODO - https://github.com/BabylonJS/BabylonNative/issues/505
        // Plumb tracking states from OpenXR. For now this will maintain the current behavior where BabylonJS assumes tracking is always available.
        , IsTracking{true}
        , m_impl{ std::make_unique<System::Session::Frame::Impl>(sessionImpl) }
    {
        if (!m_impl->sessionImpl.HmdImpl.Context.IsSessionRunning())
        {
            return;
        }

        const auto& session = m_impl->sessionImpl.HmdImpl.Context.Session();
        const auto& extensions = *m_impl->sessionImpl.HmdImpl.Context.Extensions();
        auto& displayTime = m_impl->sessionImpl.HmdImpl.Context.ContextImpl->DisplayTime;
        auto& renderResources = m_impl->sessionImpl.RenderResources;

        XrFrameWaitInfo frameWaitInfo{ XR_TYPE_FRAME_WAIT_INFO };
        XrFrameState frameState{ XR_TYPE_FRAME_STATE };

        const auto& supportedSecondaryViewConfigTypes = m_impl->sessionImpl.HmdImpl.SupportedSecondaryViewConfigurationTypes;
        const auto secondaryViewConfigCount = supportedSecondaryViewConfigTypes.size();
        std::vector<XrSecondaryViewConfigurationStateMSFT> secondaryViewConfigStates(secondaryViewConfigCount, { XR_TYPE_SECONDARY_VIEW_CONFIGURATION_STATE_MSFT });
        XrSecondaryViewConfigurationFrameStateMSFT secondaryViewConfigFrameState{ XR_TYPE_SECONDARY_VIEW_CONFIGURATION_FRAME_STATE_MSFT };
        if (secondaryViewConfigCount > 0)
        {
            secondaryViewConfigFrameState.viewConfigurationCount = (uint32_t)secondaryViewConfigStates.size();
            secondaryViewConfigFrameState.viewConfigurationStates = secondaryViewConfigStates.data();
            InsertExtensionStruct(frameState, secondaryViewConfigFrameState);
        }

        XrCheck(xrWaitFrame(session, &frameWaitInfo, &frameState));
        m_impl->shouldRender = frameState.shouldRender;
        displayTime = frameState.predictedDisplayTime;

        XrFrameBeginInfo frameBeginInfo{ XR_TYPE_FRAME_BEGIN_INFO };
        XrCheck(xrBeginFrame(session, &frameBeginInfo));

        for (const auto& secondaryViewConfigState : secondaryViewConfigStates)
        {
            auto& renderResource = renderResources.ResourceMap[secondaryViewConfigState.viewConfigurationType];
            auto& viewState = renderResource.ViewState;
            viewState.Active = secondaryViewConfigState.active;
            const auto& colorSwapchainHandle = renderResource.ColorSwapchain.Handle;
            if (viewState.Active)
            {
                std::vector<XrViewConfigurationView> viewConfigViews = EnumerateViewConfigurationViews(
                    sessionImpl.HmdImpl.Context.Instance(),
                    sessionImpl.HmdImpl.Context.SystemId(),
                    secondaryViewConfigState.viewConfigurationType);

                if (IsRecommendedSwapchainSizeChanged(viewState.ViewConfigViews, viewConfigViews) ||
                    colorSwapchainHandle == XR_NULL_HANDLE)
                {
                    viewState.ViewConfigViews = std::move(viewConfigViews);
                    sessionImpl.PopulateSwapchains(viewState);
                }
            }
            else if(colorSwapchainHandle != XR_NULL_HANDLE)
            {
                sessionImpl.CleanupSwapchains(viewState);
            }
        }

        // Only render when session is visible. otherwise submit zero layers
        if (m_impl->shouldRender)
        {
            const auto& context = sessionImpl.HmdImpl.Context;
            const auto& sceneSpace = context.Space();
            const auto depthSupported = context.Extensions()->DepthExtensionSupported;

            uint32_t totalViewCount = 0;
            uint32_t primaryViewCount;
            const XrViewConfigurationType primaryType = sessionImpl.HmdImpl.PrimaryViewConfigurationType;
            m_impl->LocateViews(primaryType, primaryViewCount);
            totalViewCount += primaryViewCount;

            uint32_t secondaryViewCount = 0;
            for (const auto& viewConfigType : supportedSecondaryViewConfigTypes)
            {
                const auto& viewState = renderResources.ResourceMap[viewConfigType].ViewState;
                if (viewState.Active)
                {
                    uint32_t tempViewCount = 0;
                    m_impl->LocateViews(viewConfigType, tempViewCount);
                    secondaryViewCount += tempViewCount;
                }
            }

            totalViewCount += secondaryViewCount;

            Views.resize(totalViewCount);

            auto& primaryRenderResource = renderResources.ResourceMap.at(primaryType);
            m_impl->PopulateViewConfigurationState(
                primaryRenderResource, 
                Views.begin(), Views.begin() + primaryViewCount,
                depthSupported, false);            

            int viewStartIdx = primaryViewCount;
            for (const auto& viewConfigType : supportedSecondaryViewConfigTypes)
            {
                auto& secondaryRenderResource = renderResources.ResourceMap.at(viewConfigType);
                ViewConfigurationState& viewConfigurationState = secondaryRenderResource.ViewState;
                if (viewConfigurationState.Active)
                {
                    const uint32_t viewCount = static_cast<uint32_t>(viewConfigurationState.Views.size());

                    m_impl->PopulateViewConfigurationState(
                        secondaryRenderResource, 
                        Views.begin() + viewStartIdx, Views.begin() + viewStartIdx + viewCount,
                        depthSupported, true); 

                    viewStartIdx += viewCount;
                }
            }

            const auto& sceneUnderstanding = m_impl->sessionImpl.HmdImpl.Context.SceneUnderstanding();
            SceneUnderstanding::UpdateFrameArgs suUpdateArgs
            {
                sceneSpace,
                extensions,
                displayTime,
                UpdatedSceneObjects,
                RemovedSceneObjects,
                UpdatedPlanes,
                RemovedPlanes,
                UpdatedMeshes,
                RemovedMeshes
            };
            sceneUnderstanding.UpdateFrame(suUpdateArgs);

            const Input::UpdateFrameArgs inputUpdateArgs
            {
                m_impl->sessionImpl.HmdImpl.Context.Instance(),
                m_impl->sessionImpl.HmdImpl.Context.Session(),
                m_impl->sessionImpl.HmdImpl.Context.Space(),
                m_impl->sessionImpl.HmdImpl.Context.DisplayTime(),
                *m_impl->sessionImpl.HmdImpl.Context.Extensions(),
                InputSources,
                EyeTrackerSpace
            };
            sessionImpl.InputManager.UpdateFrame(inputUpdateArgs);
        }
    }

    void System::Session::Frame::GetHitTestResults(std::vector<HitResult>&, Ray, xr::HitTestTrackableType) const {
        // Stubbed out for now, should be implemented if we want to support OpenXR based passthrough AR devices.
    }

    System::Session::Frame::~Frame()
    {
        if (!m_impl->sessionImpl.HmdImpl.Context.IsSessionRunning())
        {
            return;
        }

        const auto& context = m_impl->sessionImpl.HmdImpl.Context;
        const auto& session = context.Session();
        const auto& sceneSpace = context.Space();
        const auto& extensions = context.Extensions();
        const auto displayTime = context.DisplayTime();

        XrFrameEndInfo frameEndInfo{ XR_TYPE_FRAME_END_INFO };
        frameEndInfo.displayTime = displayTime;
        frameEndInfo.environmentBlendMode = m_impl->sessionImpl.HmdImpl.ViewProperties.at(m_impl->sessionImpl.HmdImpl.PrimaryViewConfigurationType).BlendMode;

        // Chain secondary view configuration layers data to endFrameInfo
        const auto& supportedSecondaryViewConfigTypes = m_impl->sessionImpl.HmdImpl.SupportedSecondaryViewConfigurationTypes;
        std::vector<XrSecondaryViewConfigurationLayerInfoMSFT> activeSecondaryViewConfigLayerInfos;
        XrSecondaryViewConfigurationFrameEndInfoMSFT frameEndSecondaryViewConfigInfo{ XR_TYPE_SECONDARY_VIEW_CONFIGURATION_FRAME_END_INFO_MSFT };
        if (extensions->SecondaryViewConfigurationSupported && supportedSecondaryViewConfigTypes.size() > 0) 
        {
            activeSecondaryViewConfigLayerInfos.reserve(supportedSecondaryViewConfigTypes.size());
            const auto& resourceMap = m_impl->sessionImpl.RenderResources.ResourceMap;
            for (auto& secondaryViewConfigType : supportedSecondaryViewConfigTypes) 
            {
                const auto& secondaryViewConfig = resourceMap.at(secondaryViewConfigType).ViewState;
                if (secondaryViewConfig.Active) 
                {
                    activeSecondaryViewConfigLayerInfos.emplace_back(
                        XrSecondaryViewConfigurationLayerInfoMSFT
                        {
                            XR_TYPE_SECONDARY_VIEW_CONFIGURATION_LAYER_INFO_MSFT,
                            nullptr,
                            secondaryViewConfigType,
                            m_impl->sessionImpl.HmdImpl.ViewProperties.at(secondaryViewConfigType).BlendMode
                        });
                }
            }

            if (activeSecondaryViewConfigLayerInfos.size() > 0) 
            {
                frameEndSecondaryViewConfigInfo.viewConfigurationCount = static_cast<uint32_t>(activeSecondaryViewConfigLayerInfos.size());
                frameEndSecondaryViewConfigInfo.viewConfigurationLayersInfo = activeSecondaryViewConfigLayerInfos.data();
                xr::InsertExtensionStruct(frameEndInfo, frameEndSecondaryViewConfigInfo);
            }
        }

        // Allocate the layer structs up here so that they're still alive for the xrEndFrame call.
        XrCompositionLayerProjection primaryLayer{ XR_TYPE_COMPOSITION_LAYER_PROJECTION };
        std::vector<XrCompositionLayerProjection> secondaryLayers(activeSecondaryViewConfigLayerInfos.size(), { XR_TYPE_COMPOSITION_LAYER_PROJECTION });

        if (m_impl->shouldRender)
        {
            auto& renderResources = m_impl->sessionImpl.RenderResources;

            XrSwapchainImageReleaseInfo releaseInfo{ XR_TYPE_SWAPCHAIN_IMAGE_RELEASE_INFO };

            for (const auto& [viewConfigType, renderResource] : renderResources.ResourceMap)
            {
                const auto& colorSwapchain = renderResource.ColorSwapchain;
                const auto& depthSwapchain = renderResource.DepthSwapchain;

                if (colorSwapchain.Handle != XR_NULL_HANDLE)
                {
                    XrAssert(xrReleaseSwapchainImage(colorSwapchain.Handle, &releaseInfo));
                }

                if (depthSwapchain.Handle != XR_NULL_HANDLE)
                {
                    XrAssert(xrReleaseSwapchainImage(depthSwapchain.Handle, &releaseInfo));
                }
            }

            // Inform the runtime to consider alpha channel during composition
            // The primary display on Hololens has additive environment blend mode. It will ignore alpha channel.
            // But mixed reality capture has alpha blend mode display and use alpha channel to blend content to environment.
            primaryLayer.layerFlags = XR_COMPOSITION_LAYER_BLEND_TEXTURE_SOURCE_ALPHA_BIT;
            primaryLayer.space = sceneSpace;
            const auto& primaryRenderResource = renderResources.ResourceMap.at(m_impl->sessionImpl.HmdImpl.PrimaryViewConfigurationType);
            primaryLayer.viewCount = static_cast<uint32_t>(primaryRenderResource.ProjectionLayerViews.size());
            primaryLayer.views = primaryRenderResource.ProjectionLayerViews.data();
            auto layersPtr = reinterpret_cast<XrCompositionLayerBaseHeader*>(&primaryLayer);
            frameEndInfo.layerCount = 1;
            frameEndInfo.layers = &layersPtr;

            if (extensions->SecondaryViewConfigurationSupported &&
                activeSecondaryViewConfigLayerInfos.size() > 0) 
            {
                for (size_t i = 0; i < activeSecondaryViewConfigLayerInfos.size(); i++) 
                {
                    auto& secondaryViewLayerInfo = activeSecondaryViewConfigLayerInfos.at(i);
                    auto& secondaryViewLayer = secondaryLayers.at(i);
                    const auto& secondaryRenderResource = renderResources.ResourceMap.at(secondaryViewLayerInfo.viewConfigurationType);
                    const auto secondaryLayersPtr = reinterpret_cast<XrCompositionLayerBaseHeader*>(&secondaryViewLayer);

                    secondaryViewLayer.layerFlags = XR_COMPOSITION_LAYER_BLEND_TEXTURE_SOURCE_ALPHA_BIT;
                    secondaryViewLayer.space = sceneSpace;
                    secondaryViewLayer.viewCount = static_cast<uint32_t>(secondaryRenderResource.ProjectionLayerViews.size());
                    secondaryViewLayer.views = secondaryRenderResource.ProjectionLayerViews.data();
                    secondaryViewLayerInfo.layerCount = 1;
                    secondaryViewLayerInfo.layers = &secondaryLayersPtr;
                }
            }
        }

#ifdef _DEBUG
        if (!Views.empty())
        {
            // 'SetPrivateData' is intercepted by the special fork of RenderDoc in order to detect that
            // a frame is being presented. This can be done to any swapchain image texture returned
            // by xrEnumerateSwapchainImages. This should be done after all rendering is complete for the
            // frame (and not for each eye backbuffer).
            ID3D11Texture2D* texture = reinterpret_cast<ID3D11Texture2D*>(Views.front().ColorTexturePointer);
            int dummy;
            texture->SetPrivateData({0xD4544440, 0x90B9, 0x4815, 0x8B, 0x99, 0x18, 0xC0, 0x23, 0xA5, 0x73, 0xF1}, sizeof(dummy), &dummy);
        }
#endif
        
        XrAssert(xrEndFrame(session, &frameEndInfo));
    }

    System::System(const char* appName)
        : m_impl{ std::make_unique<System::Impl>(appName) }
    {}

    System::~System() {}

    bool System::IsInitialized() const
    {
        return m_impl->IsInitialized();
    }

    bool System::TryInitialize()
    {
        return m_impl->TryInitialize();
    }

    arcana::task<bool, std::exception_ptr> System::IsSessionSupportedAsync(SessionType sessionType)
    {
        // Only immersive_VR is supported for now.
        return arcana::task_from_result<std::exception_ptr>(sessionType == SessionType::IMMERSIVE_VR);
    }

    uintptr_t System::GetNativeXrContext()
    {
        return XrRegistry::GetNativeXrContext();
    }

    std::string System::GetNativeXrContextType()
    {
        return XrRegistry::GetNativeXrContextType();
    }

    arcana::task<std::shared_ptr<System::Session>, std::exception_ptr> System::Session::CreateAsync(System& system, void* graphicsDevice, std::function<void*()> windowProvider)
    {
        return arcana::task_from_result<std::exception_ptr>(std::make_shared<System::Session>(system, graphicsDevice, windowProvider));
    }

    System::Session::Session(System& headMountedDisplay, void* graphicsDevice, std::function<void*()>)
        : m_impl{ std::make_unique<System::Session::Impl>(*headMountedDisplay.m_impl, graphicsDevice) }
    {}

    System::Session::~Session() {}

    std::unique_ptr<System::Session::Frame> System::Session::GetNextFrame(bool& shouldEndSession, bool& shouldRestartSession, std::function<arcana::task<void, std::exception_ptr>(void*)> /*deletedTextureAsyncCallback*/)
    {
        return m_impl->GetNextFrame(shouldEndSession, shouldRestartSession);
    }

    void System::Session::RequestEndSession()
    {
        m_impl->RequestEndSession();
    }

    void System::Session::SetDepthsNearFar(float depthNear, float depthFar)
    {
        m_impl->DepthNearZ = depthNear;
        m_impl->DepthFarZ = depthFar;
    }

    Anchor System::Session::Frame::CreateAnchor(Pose pose, NativeTrackablePtr trackable) const
    {
        const auto displayTime = m_impl->sessionImpl.HmdImpl.Context.DisplayTime();
        return m_impl->sessionImpl.CreateAnchor(pose, trackable, displayTime);
    }

    Anchor System::Session::Frame::DeclareAnchor(NativeAnchorPtr /*anchor*/) const {
        throw std::runtime_error("not implemented"); 
    }

    void System::Session::Frame::UpdateAnchor(Anchor& anchor) const
    {
        const auto displayTime = m_impl->sessionImpl.HmdImpl.Context.DisplayTime();
        m_impl->sessionImpl.UpdateAnchor(anchor, displayTime);
    }

    void System::Session::Frame::DeleteAnchor(Anchor& anchor) const
    {
        m_impl->sessionImpl.DeleteAnchor(anchor);
    }

    System::Session::Frame::SceneObject& System::Session::Frame::GetSceneObjectByID(System::Session::Frame::SceneObject::Identifier id) const
    {
        const auto& su = m_impl->sessionImpl.HmdImpl.Context.SceneUnderstanding();
        return su.GetSceneObjectByID(id);
    }

    System::Session::Frame::Plane& System::Session::Frame::GetPlaneByID(System::Session::Frame::Plane::Identifier id) const
    {
        const auto& su = m_impl->sessionImpl.HmdImpl.Context.SceneUnderstanding();
        return su.GetPlaneByID(id);
    }

    System::Session::Frame::Mesh& System::Session::Frame::GetMeshByID(System::Session::Frame::Mesh::Identifier id) const
    {
        const auto& su = m_impl->sessionImpl.HmdImpl.Context.SceneUnderstanding();
        return su.GetMeshByID(id);
    }

    void System::Session::SetPlaneDetectionEnabled(bool enabled) const
    {
        if (enabled)
        {
            const auto& session = m_impl->HmdImpl.Context.Session();
            const auto& extensions = *m_impl->HmdImpl.Context.Extensions();
            auto& su = m_impl->HmdImpl.Context.SceneUnderstanding();
            su.Initialize(SceneUnderstanding::InitOptions{ session, extensions });
        }
    }

    bool System::Session::TrySetFeaturePointCloudEnabled(bool) const
    {
        // Point cloud system not yet supported.
        return false;
    }

    bool System::Session::TrySetPreferredPlaneDetectorOptions(const xr::GeometryDetectorOptions& detectorOptions)
    {
        const auto& session = m_impl->HmdImpl.Context.Session();
        const auto& extensions = *m_impl->HmdImpl.Context.Extensions();
        auto& su = m_impl->HmdImpl.Context.SceneUnderstanding();

        SceneUnderstanding::InitOptions initOptions
        {
            session,
            extensions,
            detectorOptions.DetectionBoundary,
            detectorOptions.UpdateInterval
        };

        su.Initialize(initOptions);

        return true;
    }

    bool System::Session::TrySetMeshDetectorEnabled(const bool enabled)
    {
        if (enabled)
        {
            const auto& session = m_impl->HmdImpl.Context.Session();
            const auto& extensions = *m_impl->HmdImpl.Context.Extensions();
            auto& su = m_impl->HmdImpl.Context.SceneUnderstanding();
            SceneUnderstanding::InitOptions initOptions{ session, extensions };
            su.Initialize(initOptions);
        }

        return true;
    }

    bool System::Session::TrySetPreferredMeshDetectorOptions(const xr::GeometryDetectorOptions& detectorOptions)
    {
        const auto& session = m_impl->HmdImpl.Context.Session();
        const auto& extensions = *m_impl->HmdImpl.Context.Extensions();
        auto& su = m_impl->HmdImpl.Context.SceneUnderstanding();

        SceneUnderstanding::InitOptions initOptions
        {
            session,
            extensions,
            detectorOptions.DetectionBoundary,
            detectorOptions.UpdateInterval
        };

        su.Initialize(initOptions);

        return true;
    }
}
