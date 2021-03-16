#include <XR.h>

#include "Windows/XrPlatform.h"
#include "XrSupportedExtensions.h"
#include "XrRegistry.h"
#include "SceneUnderstanding.h"

#include <assert.h>
#include <optional>
#include <arcana/threading/task.h>

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
            createInfo.applicationInfo = { "", 1, "OpenXR Sample", 1, XR_CURRENT_API_VERSION };
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
            std::vector<Swapchain> ColorSwapchains;
            std::vector<Swapchain> DepthSwapchains;
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
            static constexpr char* DEFAULT_XR_ACTION_SET_NAME{ "default_xr_action_set" };
            static constexpr char* DEFAULT_XR_ACTION_SET_LOCALIZED_NAME{ "Default XR Action Set" };
            XrActionSet ActionSet{};

            static constexpr std::array<const char*, 2> CONTROLLER_SUBACTION_PATH_PREFIXES
            {
                "/user/hand/left",
                "/user/hand/right"
            };
            std::array<XrPath, CONTROLLER_SUBACTION_PATH_PREFIXES.size()> ControllerSubactionPaths{};

            static constexpr char* CONTROLLER_GET_GRIP_POSE_ACTION_NAME{ "controller_get_pose_action" };
            static constexpr char* CONTROLLER_GET_GRIP_POSE_ACTION_LOCALIZED_NAME{ "Controller Pose" };
            static constexpr char* CONTROLLER_GET_GRIP_POSE_PATH_SUFFIX{ "/input/grip/pose" };
            XrAction ControllerGetGripPoseAction{};
            std::array<XrSpace, CONTROLLER_SUBACTION_PATH_PREFIXES.size()> ControllerGripPoseSpaces{};

            static constexpr char* CONTROLLER_GET_AIM_POSE_ACTION_NAME{ "controller_get_aim_action" };
            static constexpr char* CONTROLLER_GET_AIM_POSE_ACTION_LOCALIZED_NAME{ "Controller Aim" };
            static constexpr char* CONTROLLER_GET_AIM_POSE_PATH_SUFFIX{ "/input/aim/pose" };
            XrAction ControllerGetAimPoseAction{};
            std::array<XrSpace, CONTROLLER_SUBACTION_PATH_PREFIXES.size()> ControllerAimPoseSpaces{};

            static constexpr char* CONTROLLER_GET_TRIGGER_VALUE_ACTION_NAME{ "controller_get_trigger_action" };
            static constexpr char* CONTROLLER_GET_TRIGGER_VALUE_ACTION_LOCALIZED_NAME{ "Controller Trigger" };
            static constexpr char* CONTROLLER_GET_TRIGGER_VALUE_PATH_SUFFIX{ "/input/trigger/value" };
            XrAction ControllerGetTriggerValueAction{};

            static constexpr char* CONTROLLER_GET_SQUEEZE_CLICK_ACTION_NAME{ "controller_get_squeeze_action" };
            static constexpr char* CONTROLLER_GET_SQUEEZE_CLICK_ACTION_LOCALIZED_NAME{ "Controller Squeeze" };
            static constexpr char* CONTROLLER_GET_SQUEEZE_CLICK_PATH_SUFFIX{ "/input/squeeze/click" };
            XrAction ControllerGetSqueezeClickAction{};

            static constexpr char* CONTROLLER_GET_TRACKPAD_AXES_ACTION_NAME{ "controller_get_trackpad_axes_action" };
            static constexpr char* CONTROLLER_GET_TRACKPAD_AXES_ACTION_LOCALIZED_NAME{ "Controller Trackpad Axes" };
            static constexpr char* CONTROLLER_GET_TRACKPAD_AXES_PATH_SUFFIX{ "/input/trackpad" };
            XrAction ControllerGetTrackpadAxesAction{};

            static constexpr char* CONTROLLER_GET_TRACKPAD_CLICK_ACTION_NAME{ "controller_get_trackpad_click_action" };
            static constexpr char* CONTROLLER_GET_TRACKPAD_CLICK_ACTION_LOCALIZED_NAME{ "Controller Trackpad Click" };
            static constexpr char* CONTROLLER_GET_TRACKPAD_CLICK_PATH_SUFFIX{ "/input/trackpad/click" };
            XrAction ControllerGetTrackpadClickAction{};

            static constexpr char* CONTROLLER_GET_TRACKPAD_TOUCH_ACTION_NAME{ "controller_get_trackpad_touch_action" };
            static constexpr char* CONTROLLER_GET_TRACKPAD_TOUCH_ACTION_LOCALIZED_NAME{ "Controller Trackpad Touch" };
            static constexpr char* CONTROLLER_GET_TRACKPAD_TOUCH_PATH_SUFFIX{ "/input/trackpad/touch" };
            XrAction ControllerGetTrackpadTouchAction{};

            static constexpr char* CONTROLLER_GET_THUMBSTICK_AXES_ACTION_NAME{ "controller_get_thumbstick_axes_action" };
            static constexpr char* CONTROLLER_GET_THUMBSTICK_AXES_ACTION_LOCALIZED_NAME{ "Controller Thumbstick Axes" };
            static constexpr char* CONTROLLER_GET_THUMBSTICK_AXES_PATH_SUFFIX{ "/input/thumbstick" };
            XrAction ControllerGetThumbstickAxesAction{};

            static constexpr char* CONTROLLER_GET_THUMBSTICK_CLICK_ACTION_NAME{ "controller_get_thumbstick_click_action" };
            static constexpr char* CONTROLLER_GET_THUMBSTICK_CLICK_ACTION_LOCALIZED_NAME{ "Controller Thumbstick Click" };
            static constexpr char* CONTROLLER_GET_THUMBSTICK_CLICK_PATH_SUFFIX{ "/input/thumbstick/click" };
            XrAction ControllerGetThumbstickClickAction{};

            static constexpr char* HAND_GET_SELECT_ACTION_NAME{ "hand_get_select_action" };
            static constexpr char* HAND_GET_SELECT_ACTION_LOCALIZED_NAME{ "Hand Select" };
            static constexpr char* HAND_GET_SELECT_PATH_SUFFIX{ "/input/select/value" };
            XrAction HandGetSelectAction{};

            static constexpr char* HAND_GET_SQUEEZE_ACTION_NAME{ "hand_get_squeeze_action" };
            static constexpr char* HAND_GET_SQUEEZE_ACTION_LOCALIZED_NAME{ "Hand Squeeze" };
            static constexpr char* HAND_GET_SQUEEZE_PATH_SUFFIX{ "/input/squeeze/value" };
            XrAction HandGetSqueezeAction{};

            static constexpr char* DEFAULT_XR_INTERACTION_PROFILE{ "/interaction_profiles/khr/simple_controller" };
            static constexpr char* MICROSOFT_XR_INTERACTION_PROFILE{ "/interaction_profiles/microsoft/motion_controller" };
            static constexpr char* MICROSOFT_HAND_INTERACTION_PROFILE{ "/interaction_profiles/microsoft/hand_interaction" };

            std::vector<Frame::InputSource> ActiveInputSources{};
            std::vector<Frame::SceneObject> SceneObjects{};
            std::vector<Frame::Plane> Planes{};
            std::vector<Frame::Mesh> Meshes{};
            std::vector<FeaturePoint> FeaturePointCloud{};
        } ActionResources{};

        struct
        {
            static constexpr uint32_t TRIGGER_BUTTON = 0;
            static constexpr uint32_t SQUEEZE_BUTTON = 1;
            static constexpr uint32_t TRACKPAD_BUTTON = 2;
            static constexpr uint32_t THUMBSTICK_BUTTON = 3;

            static constexpr uint32_t TRACKPAD_X_AXIS = 0;
            static constexpr uint32_t TRACKPAD_Y_AXIS = 1;
            static constexpr uint32_t THUMBSTICK_X_AXIS = 2;
            static constexpr uint32_t THUMBSTICK_Y_AXIS = 3;
        } ControllerInfo;

        struct HandInfo
        {
            XrHandEXT Hand{};
            XrHandTrackerEXT HandTracker{};

            XrHandJointLocationEXT JointLocations[XR_HAND_JOINT_COUNT_EXT]{};
            XrHandJointLocationsEXT Locations{XR_TYPE_HAND_JOINT_LOCATIONS_EXT, nullptr, false, XR_HAND_JOINT_COUNT_EXT, JointLocations};

            // WebXR specs do not include joint 0, the palm joint, but OpenXR does
            static constexpr uint32_t UNUSED_HAND_JOINT_OFFSET{ 1 };
        };
        
        struct
        {
            std::array<HandInfo, 2> HandsInfo{};

            XrBool32 SupportsHandTracking{ false };
            XrBool32 HandsInitialized{ false };
        } HandData;

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
            InitializeActionResources(instance);
        }

        ~Impl()
        {
            if (HandData.HandsInitialized)
            {
                for (const HandInfo& handInfo : HandData.HandsInfo)
                {
                    XrCheck(HmdImpl.Context.Extensions()->xrDestroyHandTrackerEXT(handInfo.HandTracker));
                }

                HandData.HandsInitialized = false;
            }
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
            assert(viewState.Views.size() > 0);

            // Select color and depth swapchain pixel formats
            SwapchainFormat colorSwapchainFormat;
            SwapchainFormat depthSwapchainFormat;
            SelectSwapchainPixelFormats(colorSwapchainFormat, depthSwapchainFormat);

            uint32_t viewCount = static_cast<uint32_t>(viewState.Views.size());
            auto& renderResource = RenderResources.ResourceMap[viewState.Type];
            renderResource.ColorSwapchains.resize(viewCount);
            renderResource.DepthSwapchains.resize(viewCount);
            
            for (uint32_t idx = 0; idx < viewCount; ++idx)
            {
                const XrViewConfigurationView& view = viewState.ViewConfigViews[idx];
                    PopulateSwapchain(session,
                        colorSwapchainFormat,
                        view.recommendedImageRectWidth,
                        view.recommendedImageRectHeight,
                        1,
                        view.recommendedSwapchainSampleCount,
                        0,
                        XR_SWAPCHAIN_USAGE_SAMPLED_BIT | XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT,
                        viewState.Type,
                        renderResource.ColorSwapchains[idx]);
                    PopulateSwapchain(session,
                        depthSwapchainFormat,
                        view.recommendedImageRectWidth,
                        view.recommendedImageRectHeight,
                        1,
                        view.recommendedSwapchainSampleCount,
                        0,
                        XR_SWAPCHAIN_USAGE_SAMPLED_BIT | XR_SWAPCHAIN_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                        viewState.Type,
                        renderResource.DepthSwapchains[idx]);
            }
        }

        void CleanupSwapchains(ViewConfigurationState& viewState)
        {
            auto& renderResource = RenderResources.ResourceMap[viewState.Type];
            renderResource.ColorSwapchains.clear();
            renderResource.DepthSwapchains.clear();
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

            const auto nativeAnchorPtr = reinterpret_cast<NativeAnchorPtr>(anchorSpace.get());
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

        uintptr_t GetNativeXrContext()
        {
            return XrRegistry::GetNativeXrContext();
        }

        std::string GetNativeXrContextType()
        {
            return XrRegistry::GetNativeXrContextType();
        }

    private:
        static constexpr XrPosef IDENTITY_TRANSFORM{ XrQuaternionf{ 0.f, 0.f, 0.f, 1.f }, XrVector3f{ 0.f, 0.f, 0.f } };

        struct OpenXRAnchorSpace
        {
            xr::SpatialAnchorHandle Anchor;
            xr::SpaceHandle Space;
        };

        std::unordered_map<NativeAnchorPtr, std::shared_ptr<OpenXRAnchorSpace>> openXRAnchors{};

        void InitializeRenderResources(XrInstance instance, XrSystemId systemId)
        {
            // Read graphics properties for preferred swapchain length and logging, and hand tracking availability.
            XrSystemHandTrackingPropertiesEXT handTrackingSystemProperties{ XR_TYPE_SYSTEM_HAND_TRACKING_PROPERTIES_EXT };
            XrSystemProperties systemProperties{ XR_TYPE_SYSTEM_PROPERTIES, &handTrackingSystemProperties };
            XrCheck(xrGetSystemProperties(instance, systemId, &systemProperties));

            // Initialize the hand resources
            HandData.SupportsHandTracking = handTrackingSystemProperties.supportsHandTracking && HmdImpl.Context.Extensions()->HandTrackingSupported;
            InitializeHandResources();

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

        void InitializeHandResources()
        {
            if (!HandData.SupportsHandTracking || HandData.HandsInitialized)
            {
                return;
            }

            const auto& session = HmdImpl.Context.Session();
            constexpr std::array<XrHandEXT, 2> HANDEDNESS_EXT{XR_HAND_LEFT_EXT, XR_HAND_RIGHT_EXT};
            XrHandTrackerCreateInfoEXT trackerCreateInfo{XR_TYPE_HAND_TRACKER_CREATE_INFO_EXT};
            trackerCreateInfo.handJointSet = XR_HAND_JOINT_SET_DEFAULT_EXT;

            for (size_t i = 0; i < HandData.HandsInfo.size(); i++)
            {
                // Create the hand trackers
                HandData.HandsInfo[i].Hand = HANDEDNESS_EXT[i];
                trackerCreateInfo.hand = HANDEDNESS_EXT[i];
                XrCheck(HmdImpl.Context.Extensions()->xrCreateHandTrackerEXT(session, &trackerCreateInfo, &HandData.HandsInfo[i].HandTracker));
            }

            HandData.HandsInitialized = true;
        }

        void CreateControllerActionAndBinding(
            XrActionType controllerActionType,
            char* controllerActionName,
            char* controllerLocalizedActionName,
            const char* controllerActionSuffix,
            XrAction* controllerAction,
            std::vector<XrActionSuggestedBinding> &bindings,
            XrInstance instance)
        {
            XrActionCreateInfo actionInfo{ XR_TYPE_ACTION_CREATE_INFO };
            actionInfo.actionType = controllerActionType;
            strcpy_s(actionInfo.actionName, controllerActionName);
            strcpy_s(actionInfo.localizedActionName, controllerLocalizedActionName);
            actionInfo.countSubactionPaths = static_cast<uint32_t>(ActionResources.ControllerSubactionPaths.size());
            actionInfo.subactionPaths = ActionResources.ControllerSubactionPaths.data();
            XrCheck(xrCreateAction(ActionResources.ActionSet, &actionInfo, controllerAction));
            // For each controller subaction
            for (size_t idx = 0; idx < ActionResources.CONTROLLER_SUBACTION_PATH_PREFIXES.size(); ++idx)
            {
                // Create suggested binding
                std::string path{ ActionResources.CONTROLLER_SUBACTION_PATH_PREFIXES[idx] };
                path.append(controllerActionSuffix);
                bindings.push_back({*controllerAction});
                XrCheck(xrStringToPath(instance, path.data(), &bindings.back().binding));
            }
        }

        void InitializeActionResources(XrInstance instance)
        {
            const auto& session = HmdImpl.Context.Session();

            // Create action set
            XrActionSetCreateInfo actionSetInfo{ XR_TYPE_ACTION_SET_CREATE_INFO };
            std::strcpy(actionSetInfo.actionSetName, ActionResources.DEFAULT_XR_ACTION_SET_NAME);
            std::strcpy(actionSetInfo.localizedActionSetName, ActionResources.DEFAULT_XR_ACTION_SET_LOCALIZED_NAME);
            XrCheck(xrCreateActionSet(instance, &actionSetInfo, &ActionResources.ActionSet));

            // Cache paths for subactions
            for (size_t idx = 0; idx < ActionResources.CONTROLLER_SUBACTION_PATH_PREFIXES.size(); ++idx)
            {
                XrCheck(xrStringToPath(instance, ActionResources.CONTROLLER_SUBACTION_PATH_PREFIXES[idx], &ActionResources.ControllerSubactionPaths[idx]));
            }

            std::vector<XrActionSuggestedBinding> defaultBindings{};
            std::vector<XrActionSuggestedBinding> microsoftControllerBindings{};
            std::vector<XrActionSuggestedBinding> microsoftHandBindings{};

            // Create controller get grip pose action, suggested bindings, and spaces
            {
                XrActionCreateInfo actionInfo{ XR_TYPE_ACTION_CREATE_INFO };
                actionInfo.actionType = XR_ACTION_TYPE_POSE_INPUT;
                strcpy_s(actionInfo.actionName, ActionResources.CONTROLLER_GET_GRIP_POSE_ACTION_NAME);
                strcpy_s(actionInfo.localizedActionName, ActionResources.CONTROLLER_GET_GRIP_POSE_ACTION_LOCALIZED_NAME);
                actionInfo.countSubactionPaths = static_cast<uint32_t>(ActionResources.ControllerSubactionPaths.size());
                actionInfo.subactionPaths = ActionResources.ControllerSubactionPaths.data();
                XrCheck(xrCreateAction(ActionResources.ActionSet, &actionInfo, &ActionResources.ControllerGetGripPoseAction));
                // For each controller subaction
                for (size_t idx = 0; idx < ActionResources.CONTROLLER_SUBACTION_PATH_PREFIXES.size(); ++idx)
                {
                    // Create suggested binding
                    std::string path{ ActionResources.CONTROLLER_SUBACTION_PATH_PREFIXES[idx] };
                    path.append(ActionResources.CONTROLLER_GET_GRIP_POSE_PATH_SUFFIX);

                    defaultBindings.push_back({ ActionResources.ControllerGetGripPoseAction });
                    XrCheck(xrStringToPath(instance, path.data(), &defaultBindings.back().binding));

                    microsoftControllerBindings.push_back({ ActionResources.ControllerGetGripPoseAction });
                    XrCheck(xrStringToPath(instance, path.data(), &microsoftControllerBindings.back().binding));

                    if (HmdImpl.Context.Extensions()->HandInteractionSupported)
                    {
                        microsoftHandBindings.push_back({ ActionResources.ControllerGetGripPoseAction });
                        XrCheck(xrStringToPath(instance, path.data(), &microsoftHandBindings.back().binding));
                    }

                    // Create subaction space
                    XrActionSpaceCreateInfo actionSpaceCreateInfo{ XR_TYPE_ACTION_SPACE_CREATE_INFO };
                    actionSpaceCreateInfo.action = ActionResources.ControllerGetGripPoseAction;
                    actionSpaceCreateInfo.poseInActionSpace = IDENTITY_TRANSFORM;
                    actionSpaceCreateInfo.subactionPath = ActionResources.ControllerSubactionPaths[idx];
                    XrCheck(xrCreateActionSpace(session, &actionSpaceCreateInfo, &ActionResources.ControllerGripPoseSpaces[idx]));
                }
            }

            // Create controller get aim pose action, suggested bindings, and spaces
            {
                XrActionCreateInfo actionInfo{ XR_TYPE_ACTION_CREATE_INFO };
                actionInfo.actionType = XR_ACTION_TYPE_POSE_INPUT;
                strcpy_s(actionInfo.actionName, ActionResources.CONTROLLER_GET_AIM_POSE_ACTION_NAME);
                strcpy_s(actionInfo.localizedActionName, ActionResources.CONTROLLER_GET_AIM_POSE_ACTION_LOCALIZED_NAME);
                actionInfo.countSubactionPaths = static_cast<uint32_t>(ActionResources.ControllerSubactionPaths.size());
                actionInfo.subactionPaths = ActionResources.ControllerSubactionPaths.data();
                XrCheck(xrCreateAction(ActionResources.ActionSet, &actionInfo, &ActionResources.ControllerGetAimPoseAction));
                // For each controller subaction
                for (size_t idx = 0; idx < ActionResources.CONTROLLER_SUBACTION_PATH_PREFIXES.size(); ++idx)
                {
                    // Create suggested binding
                    std::string path{ ActionResources.CONTROLLER_SUBACTION_PATH_PREFIXES[idx] };
                    path.append(ActionResources.CONTROLLER_GET_AIM_POSE_PATH_SUFFIX);

                    defaultBindings.push_back({ ActionResources.ControllerGetAimPoseAction });
                    XrCheck(xrStringToPath(instance, path.data(), &defaultBindings.back().binding));
                    
                    microsoftControllerBindings.push_back({ ActionResources.ControllerGetAimPoseAction });
                    XrCheck(xrStringToPath(instance, path.data(), &microsoftControllerBindings.back().binding));

                    if (HmdImpl.Context.Extensions()->HandInteractionSupported)
                    {
                        microsoftHandBindings.push_back({ ActionResources.ControllerGetAimPoseAction });
                        XrCheck(xrStringToPath(instance, path.data(), &microsoftHandBindings.back().binding));
                    }

                    // Create subaction space
                    XrActionSpaceCreateInfo actionSpaceCreateInfo{ XR_TYPE_ACTION_SPACE_CREATE_INFO };
                    actionSpaceCreateInfo.action = ActionResources.ControllerGetAimPoseAction;
                    actionSpaceCreateInfo.poseInActionSpace = IDENTITY_TRANSFORM;
                    actionSpaceCreateInfo.subactionPath = ActionResources.ControllerSubactionPaths[idx];
                    XrCheck(xrCreateActionSpace(session, &actionSpaceCreateInfo, &ActionResources.ControllerAimPoseSpaces[idx]));
                }
            }

            // Create controller get trigger value action and suggested bindings=
            CreateControllerActionAndBinding(
                XR_ACTION_TYPE_FLOAT_INPUT, 
                ActionResources.CONTROLLER_GET_TRIGGER_VALUE_ACTION_NAME,
                ActionResources.CONTROLLER_GET_TRIGGER_VALUE_ACTION_LOCALIZED_NAME,
                ActionResources.CONTROLLER_GET_TRIGGER_VALUE_PATH_SUFFIX,
                &ActionResources.ControllerGetTriggerValueAction,
                microsoftControllerBindings,
                instance);

            // Create controller get squeeze click action and suggested bindings
            CreateControllerActionAndBinding(
                XR_ACTION_TYPE_BOOLEAN_INPUT, 
                ActionResources.CONTROLLER_GET_SQUEEZE_CLICK_ACTION_NAME,
                ActionResources.CONTROLLER_GET_SQUEEZE_CLICK_ACTION_LOCALIZED_NAME,
                ActionResources.CONTROLLER_GET_SQUEEZE_CLICK_PATH_SUFFIX,
                &ActionResources.ControllerGetSqueezeClickAction,
                microsoftControllerBindings,
                instance);

            // Create controller get trackpad axes action and suggested bindings
            CreateControllerActionAndBinding(
                XR_ACTION_TYPE_VECTOR2F_INPUT, 
                ActionResources.CONTROLLER_GET_TRACKPAD_AXES_ACTION_NAME,
                ActionResources.CONTROLLER_GET_TRACKPAD_AXES_ACTION_LOCALIZED_NAME,
                ActionResources.CONTROLLER_GET_TRACKPAD_AXES_PATH_SUFFIX,
                &ActionResources.ControllerGetTrackpadAxesAction,
                microsoftControllerBindings,
                instance);

            // Create controller get trackpad click action and suggested bindings
            CreateControllerActionAndBinding(
                XR_ACTION_TYPE_BOOLEAN_INPUT, 
                ActionResources.CONTROLLER_GET_TRACKPAD_CLICK_ACTION_NAME,
                ActionResources.CONTROLLER_GET_TRACKPAD_CLICK_ACTION_LOCALIZED_NAME,
                ActionResources.CONTROLLER_GET_TRACKPAD_CLICK_PATH_SUFFIX,
                &ActionResources.ControllerGetTrackpadClickAction,
                microsoftControllerBindings,
                instance);

            // Create controller get trackpad touch action and suggested bindings
            CreateControllerActionAndBinding(
                XR_ACTION_TYPE_BOOLEAN_INPUT, 
                ActionResources.CONTROLLER_GET_TRACKPAD_TOUCH_ACTION_NAME,
                ActionResources.CONTROLLER_GET_TRACKPAD_TOUCH_ACTION_LOCALIZED_NAME,
                ActionResources.CONTROLLER_GET_TRACKPAD_TOUCH_PATH_SUFFIX,
                &ActionResources.ControllerGetTrackpadTouchAction,
                microsoftControllerBindings,
                instance);

            // Create controller get thumbstick axes action and suggested bindings
            CreateControllerActionAndBinding(
                XR_ACTION_TYPE_VECTOR2F_INPUT, 
                ActionResources.CONTROLLER_GET_THUMBSTICK_AXES_ACTION_NAME,
                ActionResources.CONTROLLER_GET_THUMBSTICK_AXES_ACTION_LOCALIZED_NAME,
                ActionResources.CONTROLLER_GET_THUMBSTICK_AXES_PATH_SUFFIX,
                &ActionResources.ControllerGetThumbstickAxesAction,
                microsoftControllerBindings,
                instance);

            // Create controller get thumbstick click action and suggested bindings
            CreateControllerActionAndBinding(
                XR_ACTION_TYPE_BOOLEAN_INPUT, 
                ActionResources.CONTROLLER_GET_THUMBSTICK_CLICK_ACTION_NAME,
                ActionResources.CONTROLLER_GET_THUMBSTICK_CLICK_ACTION_LOCALIZED_NAME,
                ActionResources.CONTROLLER_GET_THUMBSTICK_CLICK_PATH_SUFFIX,
                &ActionResources.ControllerGetThumbstickClickAction,
                microsoftControllerBindings,
                instance);

            if (HmdImpl.Context.Extensions()->HandInteractionSupported)
            {
                // Create action and suggested bindings specific to hands
                CreateControllerActionAndBinding(
                    XR_ACTION_TYPE_BOOLEAN_INPUT, 
                    ActionResources.HAND_GET_SELECT_ACTION_NAME,
                    ActionResources.HAND_GET_SELECT_ACTION_LOCALIZED_NAME,
                    ActionResources.HAND_GET_SELECT_PATH_SUFFIX,
                    &ActionResources.HandGetSelectAction,
                    microsoftHandBindings,
                    instance);

                CreateControllerActionAndBinding(
                    XR_ACTION_TYPE_BOOLEAN_INPUT, 
                    ActionResources.HAND_GET_SQUEEZE_ACTION_NAME,
                    ActionResources.HAND_GET_SQUEEZE_ACTION_LOCALIZED_NAME,
                    ActionResources.HAND_GET_SQUEEZE_PATH_SUFFIX,
                    &ActionResources.HandGetSqueezeAction,
                    microsoftHandBindings,
                    instance);
            }

            // Provide default suggested bindings to instance
            XrInteractionProfileSuggestedBinding suggestedBindings{ XR_TYPE_INTERACTION_PROFILE_SUGGESTED_BINDING };
            XrCheck(xrStringToPath(instance, ActionResources.DEFAULT_XR_INTERACTION_PROFILE, &suggestedBindings.interactionProfile));
            suggestedBindings.suggestedBindings = defaultBindings.data();
            suggestedBindings.countSuggestedBindings = (uint32_t)defaultBindings.size();
            XrCheck(xrSuggestInteractionProfileBindings(instance, &suggestedBindings));

            // Provide Microsoft controller suggested binding to instance
            XrInteractionProfileSuggestedBinding microsoftControllerSuggestedBindings{ XR_TYPE_INTERACTION_PROFILE_SUGGESTED_BINDING };
            XrCheck(xrStringToPath(instance, ActionResources.MICROSOFT_XR_INTERACTION_PROFILE, &microsoftControllerSuggestedBindings.interactionProfile));
            microsoftControllerSuggestedBindings.suggestedBindings = microsoftControllerBindings.data();
            microsoftControllerSuggestedBindings.countSuggestedBindings = (uint32_t)microsoftControllerBindings.size();
            XrCheck(xrSuggestInteractionProfileBindings(instance, &microsoftControllerSuggestedBindings));

            if (HmdImpl.Context.Extensions()->HandInteractionSupported)
            {
                // Provide Microsoft hand suggested binding to instance
                XrInteractionProfileSuggestedBinding microsoftHandSuggestedBindings{ XR_TYPE_INTERACTION_PROFILE_SUGGESTED_BINDING };
                XrCheck(xrStringToPath(instance, ActionResources.MICROSOFT_HAND_INTERACTION_PROFILE, &microsoftHandSuggestedBindings.interactionProfile));
                microsoftHandSuggestedBindings.suggestedBindings = microsoftHandBindings.data();
                microsoftHandSuggestedBindings.countSuggestedBindings = (uint32_t)microsoftHandBindings.size();
                XrCheck(xrSuggestInteractionProfileBindings(instance, &microsoftHandSuggestedBindings));
            }

            XrSessionActionSetsAttachInfo attachInfo{ XR_TYPE_SESSION_ACTION_SETS_ATTACH_INFO };
            attachInfo.countActionSets = 1;
            attachInfo.actionSets = &ActionResources.ActionSet;
            XrCheck(xrAttachSessionActionSets(session, &attachInfo));
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
            if (HmdImpl.Context.Extensions()->SecondaryViewConfigurationSupported)
            {
                secondaryViewConfigCreateInfo.viewConfigurationType = viewConfigType;
                swapchainCreateInfo.next = &secondaryViewConfigCreateInfo;
            }

            XrCheck(xrCreateSwapchain(session, &swapchainCreateInfo, &swapchain.Handle));

            uint32_t chainLength;
            XrCheck(xrEnumerateSwapchainImages(swapchain.Handle, 0, &chainLength, nullptr));
            swapchain.Images.resize(chainLength, { SWAPCHAIN_IMAGE_TYPE_ENUM });
            XrCheck(xrEnumerateSwapchainImages(swapchain.Handle, static_cast<uint32_t>(swapchain.Images.size()), &chainLength,
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
                std::begin(swapchainFormats),
                std::end(swapchainFormats),
                std::begin(SUPPORTED_COLOR_FORMATS),
                std::end(SUPPORTED_COLOR_FORMATS));
            if (colorFormatPtr == std::end(swapchainFormats))
            {
                throw std::runtime_error{ "No runtime swapchain format is supported for color." };
            }

            auto depthFormatPtr = std::find_first_of(
                std::begin(swapchainFormats),
                std::end(swapchainFormats),
                std::begin(SUPPORTED_DEPTH_FORMATS),
                std::end(SUPPORTED_DEPTH_FORMATS));
            if (depthFormatPtr == std::end(swapchainFormats))
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

                XrSecondaryViewConfigurationSessionBeginInfoMSFT secondaryViewConfigInfo
                {
                    XR_TYPE_SECONDARY_VIEW_CONFIGURATION_SESSION_BEGIN_INFO_MSFT
                };

                if (HmdImpl.Context.Extensions()->SecondaryViewConfigurationSupported &&
                    HmdImpl.Context.Extensions()->FirstPersonObserverSupported &&
                    HmdImpl.SupportedSecondaryViewConfigurationTypes.size() > 0)
                {
                    secondaryViewConfigInfo.viewConfigurationCount = static_cast<uint32_t>(HmdImpl.SupportedSecondaryViewConfigurationTypes.size());
                    secondaryViewConfigInfo.enabledViewConfigurationTypes = HmdImpl.SupportedSecondaryViewConfigurationTypes.data();
                    InsertExtensionStruct(sessionBeginInfo, secondaryViewConfigInfo);
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
                case XR_TYPE_EVENT_DATA_REFERENCE_SPACE_CHANGE_PENDING:
                case XR_TYPE_EVENT_DATA_INTERACTION_PROFILE_CHANGED:
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
            assert(viewCountOutput == renderResource.ColorSwapchains.size());
            assert(viewCountOutput == renderResource.DepthSwapchains.size());

            renderResource.ProjectionLayerViews.resize(viewCountOutput);
            if (context.Extensions()->DepthExtensionSupported)
            {
                renderResource.DepthInfoViews.resize(viewCountOutput);
            }
        }

        void PopulateProjectionMatrix(const XrView& cachedView, xr::System::Session::Frame::View& view) {
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
            view.DepthTextureFormat = SwapchainFormatToTextureFormat(depthSwapchain.Format);
            view.DepthTexturePointer = depthSwapchain.Images[depthSwapchainImageIndex].texture;
            view.DepthTextureSize.Width = depthSwapchain.Width;
            view.DepthTextureSize.Height = depthSwapchain.Height;
            view.DepthNearZ = sessionImpl.DepthNearZ;
            view.DepthFarZ = sessionImpl.DepthFarZ;

            PopulateProjectionMatrix(cachedView, view);
        }

        void PopulateProjectionView(const XrView& cachedView,
            const xr::System::Session::Impl::Swapchain& colorSwapchain,
            const XrRect2Di imageRect,
            XrCompositionLayerProjectionView& projectionLayerView)
        {
            projectionLayerView.pose = cachedView.pose;
            projectionLayerView.fov = cachedView.fov;
            projectionLayerView.subImage.swapchain = colorSwapchain.Handle;
            projectionLayerView.subImage.imageRect = imageRect;
            projectionLayerView.subImage.imageArrayIndex = 0;
        }

        void PopulateDepthInfoView(const xr::System::Session::Impl::Swapchain& depthSwapchain,
            const XrRect2Di imageRect,
            XrCompositionLayerDepthInfoKHR& depthInfoView)
        {
            depthInfoView.minDepth = 0;
            depthInfoView.maxDepth = 1;
            depthInfoView.nearZ = sessionImpl.DepthNearZ;
            depthInfoView.farZ = sessionImpl.DepthFarZ;
            depthInfoView.subImage.swapchain = depthSwapchain.Handle;
            depthInfoView.subImage.imageRect = imageRect;
            depthInfoView.subImage.imageArrayIndex = 0;
        }

        // Returns true if the action is supported on the current input
        bool TryUpdateControllerFloatAction(XrAction controllerAction, XrSession session, float& currentFloatState)
        {
            // query input action state
            XrActionStateFloat floatActionState{XR_TYPE_ACTION_STATE_FLOAT};
            XrActionStateGetInfo getInfo{XR_TYPE_ACTION_STATE_GET_INFO};
            getInfo.action = controllerAction;
            XrCheck(xrGetActionStateFloat(session, &getInfo, &floatActionState));
            if (!floatActionState.isActive)
            {
                return false;
            }

            if (floatActionState.changedSinceLastSync)
            {
                currentFloatState = floatActionState.currentState;
            }

            return true;
        }

        // Returns true if the action is supported on the current input
        bool TryUpdateControllerBooleanAction(XrAction controllerAction, XrSession session, bool& currentBooleanState)
        {
            // query input action state
            XrActionStateBoolean booleanActionState{XR_TYPE_ACTION_STATE_BOOLEAN};
            XrActionStateGetInfo getInfo{XR_TYPE_ACTION_STATE_GET_INFO};
            getInfo.action = controllerAction;
            XrCheck(xrGetActionStateBoolean(session, &getInfo, &booleanActionState));
            if (!booleanActionState.isActive)
            {
                return false;
            }

            if (booleanActionState.changedSinceLastSync)
            {
               currentBooleanState = booleanActionState.currentState;
            }

            return true;
        }
        
        // Returns true if the action is supported on the current input
        bool TryUpdateControllerVector2fAction(XrAction controllerAction, XrSession session, float& currentXState, float& currentYState)
        {
            // query input action state
            XrActionStateVector2f vector2fActionState{XR_TYPE_ACTION_STATE_VECTOR2F};
            XrActionStateGetInfo getInfo{XR_TYPE_ACTION_STATE_GET_INFO};
            getInfo.action = controllerAction;
            XrCheck(xrGetActionStateVector2f(session, &getInfo, &vector2fActionState));
            if (!vector2fActionState.isActive)
            {
                return false;
            }

            if (vector2fActionState.changedSinceLastSync)
            {
                currentXState = vector2fActionState.currentState.x;
                currentYState = vector2fActionState.currentState.y;
            }

            return true;
        }

        void UpdatePoseData(xr::Pose& targetPose, XrPosef& sourcePose)
        {
            targetPose.Position.X = sourcePose.position.x;
            targetPose.Position.Y = sourcePose.position.y;
            targetPose.Position.Z = sourcePose.position.z;
            targetPose.Orientation.X = sourcePose.orientation.x;
            targetPose.Orientation.Y = sourcePose.orientation.y;
            targetPose.Orientation.Z = sourcePose.orientation.z;
            targetPose.Orientation.W = sourcePose.orientation.w;
        }
    };

    System::Session::Frame::Frame(Session::Impl& sessionImpl)
        : Views{ sessionImpl.RenderResources.ActiveFrameViews }
        , InputSources{ sessionImpl.ActionResources.ActiveInputSources }
        , Planes { sessionImpl.ActionResources.Planes }
        , Meshes { sessionImpl.ActionResources.Meshes }
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

        XrSecondaryViewConfigurationFrameStateMSFT secondaryViewConfigFrameState{XR_TYPE_SECONDARY_VIEW_CONFIGURATION_FRAME_STATE_MSFT};
        std::vector<XrSecondaryViewConfigurationStateMSFT> secondaryViewConfigStates;
        if (sessionImpl.HmdImpl.SupportedSecondaryViewConfigurationTypes.size() > 0)
        {
            uint32_t viewCount = 0;
            for (const auto& viewConfigurationType : sessionImpl.HmdImpl.SupportedSecondaryViewConfigurationTypes)
            {
                const auto& viewState = renderResources.ResourceMap.at(viewConfigurationType).ViewState;
                viewCount += static_cast<uint32_t>(viewState.Views.size());
            }

            secondaryViewConfigStates.resize(viewCount, { XR_TYPE_SECONDARY_VIEW_CONFIGURATION_STATE_MSFT });
            secondaryViewConfigFrameState.viewConfigurationCount = (uint32_t)secondaryViewConfigStates.size();
            secondaryViewConfigFrameState.viewConfigurationStates = secondaryViewConfigStates.data();
            InsertExtensionStruct(frameState, secondaryViewConfigFrameState);
        }

        XrCheck(xrWaitFrame(session, &frameWaitInfo, &frameState));
        m_impl->shouldRender = frameState.shouldRender;
        displayTime = frameState.predictedDisplayTime;

        XrFrameBeginInfo frameBeginInfo{ XR_TYPE_FRAME_BEGIN_INFO };
        XrCheck(xrBeginFrame(session, &frameBeginInfo));

        if (sessionImpl.HmdImpl.SupportedSecondaryViewConfigurationTypes.size() > 0)
        {
            for (const auto& secondaryViewConfigState : secondaryViewConfigStates)
            {
                auto& renderResource = renderResources.ResourceMap[secondaryViewConfigState.viewConfigurationType];
                auto& viewState = renderResource.ViewState;
                viewState.Active = secondaryViewConfigState.active;
                if (viewState.Active)
                {
                    std::vector<XrViewConfigurationView> viewConfigViews = EnumerateViewConfigurationViews(
                        sessionImpl.HmdImpl.Context.Instance(),
                        sessionImpl.HmdImpl.Context.SystemId(),
                        secondaryViewConfigState.viewConfigurationType);
                    
                    if (renderResource.ColorSwapchains.size() < viewConfigViews.size() ||
                        renderResource.DepthSwapchains.size() < viewConfigViews.size() ||
                        IsRecommendedSwapchainSizeChanged(viewState.ViewConfigViews, viewConfigViews))
                    {
                        viewState.ViewConfigViews = std::move(viewConfigViews);
                        sessionImpl.PopulateSwapchains(viewState);
                    }
                }
                else
                {
                    sessionImpl.CleanupSwapchains(viewState);
                }
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
            if (sessionImpl.HmdImpl.SupportedSecondaryViewConfigurationTypes.size() > 0)
            {
                for (const auto& viewConfigType : sessionImpl.HmdImpl.SupportedSecondaryViewConfigurationTypes)
                {
                    const auto& viewState = renderResources.ResourceMap[viewConfigType].ViewState;
                    if (viewState.Active)
                    {
                        uint32_t tempViewCount = 0;
                        m_impl->LocateViews(viewConfigType, tempViewCount);
                        secondaryViewCount += tempViewCount;
                    }
                }
            }

            totalViewCount += secondaryViewCount;
            Views.resize(totalViewCount);

            // Prepare rendering parameters of each view for swapchain texture arrays
            auto& primaryRenderResource = renderResources.ResourceMap[primaryType];
            for (uint32_t idx = 0; idx < primaryViewCount; ++idx)
            {
                const auto& colorSwapchain = primaryRenderResource.ColorSwapchains[idx];
                const auto& depthSwapchain = primaryRenderResource.DepthSwapchains[idx];
                const auto& cachedView = primaryRenderResource.ViewState.Views.at(idx);

                // Use the full range of recommended image size to achieve optimum resolution
                const XrRect2Di imageRect = { {0, 0}, { colorSwapchain.Width, colorSwapchain.Height } };
                assert(colorSwapchain.Width == depthSwapchain.Width);
                assert(colorSwapchain.Height == depthSwapchain.Height);

                const uint32_t colorSwapchainImageIndex = AquireAndWaitForSwapchainImage(colorSwapchain.Handle);
                const uint32_t depthSwapchainImageIndex = AquireAndWaitForSwapchainImage(depthSwapchain.Handle);

                // Populate the struct that consuming code will use for rendering.
                auto& view = Views[idx];
                m_impl->PopulateView(cachedView, colorSwapchain, colorSwapchainImageIndex, depthSwapchain, depthSwapchainImageIndex, view);
        
                primaryRenderResource.ProjectionLayerViews[idx] = { XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW };
                auto& projectionLayerView = primaryRenderResource.ProjectionLayerViews[idx];
                m_impl->PopulateProjectionView(cachedView, colorSwapchain, imageRect, projectionLayerView);

                if (depthSupported)
                {
                    primaryRenderResource.DepthInfoViews[idx] = { XR_TYPE_COMPOSITION_LAYER_DEPTH_INFO_KHR };
                    auto& depthInfoView = primaryRenderResource.DepthInfoViews[idx];
                    m_impl->PopulateDepthInfoView(depthSwapchain, imageRect, depthInfoView);
        
                    // Chain depth info struct to the corresponding projection layer views's next
                    projectionLayerView.next = &depthInfoView;
                }
            }

            if (sessionImpl.HmdImpl.SupportedSecondaryViewConfigurationTypes.size() > 0)
            {
                int index = primaryViewCount;
                for (const auto& viewConfigType : sessionImpl.HmdImpl.SupportedSecondaryViewConfigurationTypes)
                {
                    auto& secondaryRenderResource = renderResources.ResourceMap.at(viewConfigType);
                    ViewConfigurationState& viewConfigurationState = secondaryRenderResource.ViewState;
                    if (viewConfigurationState.Active)
                    {
                        const uint32_t viewCount = static_cast<uint32_t>(viewConfigurationState.Views.size());
                        for (uint32_t idx = 0; idx < viewCount; ++idx)
                        {
                            const auto& colorSwapchain = secondaryRenderResource.ColorSwapchains[idx];
                            const auto& depthSwapchain = secondaryRenderResource.DepthSwapchains[idx];
                            const auto& cachedView = secondaryRenderResource.ViewState.Views.at(idx);

                            // Use the full range of recommended image size to achieve optimum resolution
                            const XrRect2Di imageRect = { {0, 0}, { colorSwapchain.Width, colorSwapchain.Height } };
                            assert(colorSwapchain.Width == depthSwapchain.Width);
                            assert(colorSwapchain.Height == depthSwapchain.Height);

                            const uint32_t colorSwapchainImageIndex = AquireAndWaitForSwapchainImage(colorSwapchain.Handle);
                            const uint32_t depthSwapchainImageIndex = AquireAndWaitForSwapchainImage(depthSwapchain.Handle);

                            // Populate the struct that consuming code will use for rendering.
                            auto& view = Views[index];

                            m_impl->PopulateView(cachedView, colorSwapchain, colorSwapchainImageIndex, depthSwapchain, depthSwapchainImageIndex, view);

                            // Set is first person observer flag to true.
                            view.IsFirstPersonObserver = true;

                            secondaryRenderResource.ProjectionLayerViews[idx] = { XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW };
                            auto& projectionLayerView = secondaryRenderResource.ProjectionLayerViews[idx];
                            m_impl->PopulateProjectionView(cachedView, colorSwapchain, imageRect, projectionLayerView);

                            if (depthSupported)
                            {
                                secondaryRenderResource.DepthInfoViews[idx] = { XR_TYPE_COMPOSITION_LAYER_DEPTH_INFO_KHR };
                                auto& depthInfoView = secondaryRenderResource.DepthInfoViews[idx];
                                m_impl->PopulateDepthInfoView(depthSwapchain, imageRect, depthInfoView);

                                // Chain depth info struct to the corresponding projection layer views's next
                                projectionLayerView.next = &depthInfoView;
                            }

                            index++;
                        }
                    }
                }
            }

            const auto& su = m_impl->sessionImpl.HmdImpl.Context.SceneUnderstanding();
            SceneUnderstanding::UpdateFrameArgs args
            {
                sceneSpace,
                extensions,
                displayTime,
                UpdatedSceneObjects,
                RemovedSceneObjects,
                Planes,
                UpdatedPlanes,
                RemovedPlanes,
                Meshes,
                UpdatedMeshes,
                RemovedMeshes
            };
            su.UpdateFrame(args);

            // Locate all the things.
            auto& actionResources = m_impl->sessionImpl.ActionResources;

            std::vector<XrActiveActionSet> activeActionSets = { { actionResources.ActionSet, XR_NULL_PATH } };
            XrActionsSyncInfo syncInfo{ XR_TYPE_ACTIONS_SYNC_INFO };
            syncInfo.countActiveActionSets = (uint32_t)activeActionSets.size();
            syncInfo.activeActionSets = activeActionSets.data();
            XrCheck(xrSyncActions(session, &syncInfo));

            InputSources.resize(actionResources.CONTROLLER_SUBACTION_PATH_PREFIXES.size());
            for (size_t idx = 0; idx < InputSources.size(); ++idx)
            {
                // Get grip space
                {
                    XrSpace space = actionResources.ControllerGripPoseSpaces[idx];
                    XrSpaceLocation location{ XR_TYPE_SPACE_LOCATION };
                    XrCheck(xrLocateSpace(space, sceneSpace, displayTime, &location));

                    constexpr XrSpaceLocationFlags RequiredFlags =
                        XR_SPACE_LOCATION_POSITION_VALID_BIT |
                        XR_SPACE_LOCATION_ORIENTATION_VALID_BIT |
                        XR_SPACE_LOCATION_POSITION_TRACKED_BIT |
                        XR_SPACE_LOCATION_ORIENTATION_TRACKED_BIT;

                    auto& inputSource = InputSources[idx];
                    inputSource.TrackedThisFrame = (location.locationFlags & RequiredFlags) == RequiredFlags;
                    if (inputSource.TrackedThisFrame)
                    {
                        inputSource.Handedness = static_cast<InputSource::HandednessEnum>(idx);
                        m_impl->UpdatePoseData(inputSource.GripSpace.Pose, location.pose);
                    }
                }

                // Get aim space
                {
                    XrSpace space = actionResources.ControllerAimPoseSpaces[idx];
                    XrSpaceLocation location{ XR_TYPE_SPACE_LOCATION };
                    XrCheck(xrLocateSpace(space, sceneSpace, displayTime, &location));

                    constexpr XrSpaceLocationFlags RequiredFlags =
                        XR_SPACE_LOCATION_POSITION_VALID_BIT |
                        XR_SPACE_LOCATION_ORIENTATION_VALID_BIT |
                        XR_SPACE_LOCATION_POSITION_TRACKED_BIT |
                        XR_SPACE_LOCATION_ORIENTATION_TRACKED_BIT;

                    auto& inputSource = InputSources[idx];
                    inputSource.TrackedThisFrame = (location.locationFlags & RequiredFlags) == RequiredFlags;
                    if (inputSource.TrackedThisFrame)
                    {
                        inputSource.Handedness = static_cast<InputSource::HandednessEnum>(idx);
                        m_impl->UpdatePoseData(inputSource.AimSpace.Pose, location.pose);
                    }
                }

                // Get gamepad data 
                {
                    const auto& controllerInfo = sessionImpl.ControllerInfo;
                    auto& gamepadObject = InputSources[idx].GamepadObject;

                    // Update gamepad data
                    if ((m_impl->TryUpdateControllerFloatAction(actionResources.ControllerGetTriggerValueAction, session, gamepadObject.Buttons[controllerInfo.TRIGGER_BUTTON].Value)) &&
                        (m_impl->TryUpdateControllerBooleanAction(actionResources.ControllerGetSqueezeClickAction, session, gamepadObject.Buttons[controllerInfo.SQUEEZE_BUTTON].Pressed)) &&
                        (m_impl->TryUpdateControllerVector2fAction(actionResources.ControllerGetTrackpadAxesAction, session, gamepadObject.Axes[controllerInfo.TRACKPAD_X_AXIS], gamepadObject.Axes[controllerInfo.TRACKPAD_Y_AXIS])) &&
                        (m_impl->TryUpdateControllerBooleanAction(actionResources.ControllerGetTrackpadClickAction, session, gamepadObject.Buttons[controllerInfo.TRACKPAD_BUTTON].Pressed)) &&
                        (m_impl->TryUpdateControllerBooleanAction(actionResources.ControllerGetTrackpadTouchAction, session, gamepadObject.Buttons[controllerInfo.TRACKPAD_BUTTON].Touched)) &&
                        (m_impl->TryUpdateControllerVector2fAction(actionResources.ControllerGetThumbstickAxesAction, session, gamepadObject.Axes[controllerInfo.THUMBSTICK_X_AXIS], gamepadObject.Axes[controllerInfo.THUMBSTICK_Y_AXIS])) &&
                        (m_impl->TryUpdateControllerBooleanAction(actionResources.ControllerGetThumbstickClickAction, session, gamepadObject.Buttons[controllerInfo.THUMBSTICK_BUTTON].Pressed)))
                    {
                        // map the openxr values to populate other states of a button and axes that webxr expects
                        gamepadObject.Buttons[controllerInfo.TRIGGER_BUTTON].Pressed = (gamepadObject.Buttons[controllerInfo.TRIGGER_BUTTON].Value == 1);
                        gamepadObject.Buttons[controllerInfo.TRIGGER_BUTTON].Touched = (gamepadObject.Buttons[controllerInfo.TRIGGER_BUTTON].Value > 0);
                        gamepadObject.Buttons[controllerInfo.SQUEEZE_BUTTON].Value = (gamepadObject.Buttons[controllerInfo.SQUEEZE_BUTTON].Pressed);
                        gamepadObject.Buttons[controllerInfo.SQUEEZE_BUTTON].Touched = (gamepadObject.Buttons[controllerInfo.SQUEEZE_BUTTON].Pressed);
                        gamepadObject.Axes[controllerInfo.TRACKPAD_Y_AXIS] = -(gamepadObject.Axes[controllerInfo.TRACKPAD_Y_AXIS]);
                        gamepadObject.Buttons[controllerInfo.TRACKPAD_BUTTON].Value = (gamepadObject.Buttons[controllerInfo.TRACKPAD_BUTTON].Pressed);
                        gamepadObject.Axes[controllerInfo.THUMBSTICK_Y_AXIS] = -(gamepadObject.Axes[controllerInfo.THUMBSTICK_Y_AXIS]);
                        gamepadObject.Buttons[controllerInfo.THUMBSTICK_BUTTON].Value = (gamepadObject.Buttons[controllerInfo.THUMBSTICK_BUTTON].Pressed);
                        gamepadObject.Buttons[controllerInfo.THUMBSTICK_BUTTON].Touched = (gamepadObject.Axes[controllerInfo.THUMBSTICK_X_AXIS] != 0 || gamepadObject.Axes[controllerInfo.THUMBSTICK_Y_AXIS] != 0);

                        // Only signal that gamepad data is available if the actions were available
                        InputSources[idx].GamepadTrackedThisFrame = true;
                    }
                }

                // Get joint data
                if (sessionImpl.HandData.HandsInitialized)
                {
                    if (sessionImpl.HmdImpl.Context.Extensions()->HandInteractionSupported)
                    {
                        const auto& controllerInfo = sessionImpl.ControllerInfo;
                        auto& gamepadObject = InputSources[idx].GamepadObject;

                        // Get interaction data
                        if ((m_impl->TryUpdateControllerBooleanAction(actionResources.HandGetSelectAction, session, gamepadObject.Buttons[controllerInfo.TRIGGER_BUTTON].Pressed)) &&
                            (m_impl->TryUpdateControllerBooleanAction(actionResources.HandGetSqueezeAction, session, gamepadObject.Buttons[controllerInfo.SQUEEZE_BUTTON].Touched)))
                        {
                            gamepadObject.Buttons[controllerInfo.TRIGGER_BUTTON].Value = (gamepadObject.Buttons[controllerInfo.TRIGGER_BUTTON].Pressed);
                            gamepadObject.Buttons[controllerInfo.TRIGGER_BUTTON].Touched = (gamepadObject.Buttons[controllerInfo.TRIGGER_BUTTON].Pressed);
                            gamepadObject.Buttons[controllerInfo.SQUEEZE_BUTTON].Value = (gamepadObject.Buttons[controllerInfo.SQUEEZE_BUTTON].Pressed);
                            gamepadObject.Buttons[controllerInfo.SQUEEZE_BUTTON].Touched = (gamepadObject.Buttons[controllerInfo.SQUEEZE_BUTTON].Pressed);

                            InputSources[idx].GamepadTrackedThisFrame = true;
                        }
                    }

                    XrHandJointsLocateInfoEXT jointLocateInfo{XR_TYPE_HAND_JOINTS_LOCATE_INFO_EXT};
                    jointLocateInfo.baseSpace = sceneSpace;
                    jointLocateInfo.time = displayTime;

                    auto handInfo = sessionImpl.HandData.HandsInfo[idx];
                    XrCheck(sessionImpl.HmdImpl.Context.Extensions()->xrLocateHandJointsEXT(handInfo.HandTracker, &jointLocateInfo, &handInfo.Locations));
                    
                    auto& inputSource = InputSources[idx];
                    inputSource.JointsTrackedThisFrame = handInfo.Locations.isActive;
                        
                    // Set up the handJoints vector, skipping the palm joint as webXR doesn't support it
                    uint32_t JointCountWithoutPalm = handInfo.Locations.jointCount - handInfo.UNUSED_HAND_JOINT_OFFSET;

                    // xrLocateHandJointsEXT will always return the full joint set (or an error), so jointCount should never change
                    // We have to wait until here to initialize the vector though, as we don't "know" the number of joints until an input report comes in
                    if (inputSource.HandJoints.size() != JointCountWithoutPalm)
                    {
                        inputSource.HandJoints = std::vector<JointSpace>(JointCountWithoutPalm);
                    }
                    
                    // Set the joints to tracked, and populate the fields. Otherwise, set joints to untracked
                    if (inputSource.JointsTrackedThisFrame)
                    {
                        constexpr XrSpaceLocationFlags RequiredFlags =
                            XR_SPACE_LOCATION_POSITION_VALID_BIT |
                            XR_SPACE_LOCATION_ORIENTATION_VALID_BIT |
                            XR_SPACE_LOCATION_POSITION_TRACKED_BIT |
                            XR_SPACE_LOCATION_ORIENTATION_TRACKED_BIT;

                        for (uint32_t i = 0; i < JointCountWithoutPalm; i++)
                        {
                            auto joint = handInfo.JointLocations[i + handInfo.UNUSED_HAND_JOINT_OFFSET];

                            inputSource.HandJoints[i].PoseRadius = joint.radius;
                            inputSource.HandJoints[i].PoseTracked = (joint.locationFlags & RequiredFlags) == RequiredFlags;
                            m_impl->UpdatePoseData(inputSource.HandJoints[i].Pose, joint.pose);
                        }
                    }
                    else
                    {
                        for (auto& joint : inputSource.HandJoints)
                        {
                            joint.PoseTracked = false;
                        }
                    }
                }
            }
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

        std::vector<XrCompositionLayerProjection> layers{};
        const auto& context = m_impl->sessionImpl.HmdImpl.Context;
        const auto& session = context.Session();
        const auto& sceneSpace = context.Space();
        const auto& extensions = context.Extensions();
        const auto displayTime = context.DisplayTime();

        XrFrameEndInfo frameEndInfo{ XR_TYPE_FRAME_END_INFO };
        frameEndInfo.displayTime = displayTime;
        frameEndInfo.environmentBlendMode = m_impl->sessionImpl.HmdImpl.ViewProperties.at(m_impl->sessionImpl.HmdImpl.PrimaryViewConfigurationType).BlendMode;

        XrSecondaryViewConfigurationFrameEndInfoMSFT frameEndSecondaryViewConfigInfo
        {
            XR_TYPE_SECONDARY_VIEW_CONFIGURATION_FRAME_END_INFO_MSFT
        };
        std::vector<XrSecondaryViewConfigurationLayerInfoMSFT> activeSecondaryViewConfigLayerInfos;

        // Chain secondary view configuration layers data to endFrameInfo
        if (extensions->SecondaryViewConfigurationSupported &&
            m_impl->sessionImpl.HmdImpl.SupportedSecondaryViewConfigurationTypes.size() > 0) {
            for (auto& secondaryViewConfigType : m_impl->sessionImpl.HmdImpl.SupportedSecondaryViewConfigurationTypes) {
                auto& secondaryViewConfig = m_impl->sessionImpl.RenderResources.ResourceMap.at(secondaryViewConfigType).ViewState;
                if (secondaryViewConfig.Active) {
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

            if (activeSecondaryViewConfigLayerInfos.size() > 0) {
                frameEndSecondaryViewConfigInfo.viewConfigurationCount = (uint32_t)activeSecondaryViewConfigLayerInfos.size();
                frameEndSecondaryViewConfigInfo.viewConfigurationLayersInfo = activeSecondaryViewConfigLayerInfos.data();
                xr::InsertExtensionStruct(frameEndInfo, frameEndSecondaryViewConfigInfo);
            }
        }

        if (m_impl->shouldRender)
        {
            auto& renderResources = m_impl->sessionImpl.RenderResources;

            XrSwapchainImageReleaseInfo releaseInfo{ XR_TYPE_SWAPCHAIN_IMAGE_RELEASE_INFO };

            for (const auto& [type, renderResource] : renderResources.ResourceMap)
            {
                for (auto& swapchain : renderResource.ColorSwapchains)
                {
                    if (swapchain.Handle != XR_NULL_HANDLE)
                    {
                        XrAssert(xrReleaseSwapchainImage(swapchain.Handle, &releaseInfo));
                    }
                }

                for (auto& swapchain : renderResource.DepthSwapchains)
                {
                    if (swapchain.Handle != XR_NULL_HANDLE)
                    {
                        XrAssert(xrReleaseSwapchainImage(swapchain.Handle, &releaseInfo));
                    }
                }
            }

            layers.resize(1 + activeSecondaryViewConfigLayerInfos.size());

            // Inform the runtime to consider alpha channel during composition
            // The primary display on Hololens has additive environment blend mode. It will ignore alpha channel.
            // But mixed reality capture has alpha blend mode display and use alpha channel to blend content to environment.
            layers[0] = { XR_TYPE_COMPOSITION_LAYER_PROJECTION };
            auto& primaryLayer = layers[0];
            primaryLayer.layerFlags = XR_COMPOSITION_LAYER_BLEND_TEXTURE_SOURCE_ALPHA_BIT;
            primaryLayer.space = sceneSpace;
            const auto& primaryRenderResource = renderResources.ResourceMap.at(m_impl->sessionImpl.HmdImpl.PrimaryViewConfigurationType);
            primaryLayer.viewCount = static_cast<uint32_t>(primaryRenderResource.ProjectionLayerViews.size());
            primaryLayer.views = primaryRenderResource.ProjectionLayerViews.data();
            auto layersPtr = reinterpret_cast<XrCompositionLayerBaseHeader*>(&primaryLayer);
            frameEndInfo.layerCount = 1;
            frameEndInfo.layers = &layersPtr;
                    
            if (extensions->SecondaryViewConfigurationSupported &&
                activeSecondaryViewConfigLayerInfos.size() > 0) {

                for (size_t i = 0; i < activeSecondaryViewConfigLayerInfos.size(); i++) {
                    XrSecondaryViewConfigurationLayerInfoMSFT& secondaryViewLayerInfo = activeSecondaryViewConfigLayerInfos.at(i);
                    layers[i + 1] = { XR_TYPE_COMPOSITION_LAYER_PROJECTION };
                    XrCompositionLayerProjection& secondaryViewLayer = layers.at(i + 1);
                    secondaryViewLayer.layerFlags = XR_COMPOSITION_LAYER_BLEND_TEXTURE_SOURCE_ALPHA_BIT;
                    secondaryViewLayer.space = sceneSpace;
                    const auto& secondaryRenderResource = renderResources.ResourceMap.at(secondaryViewLayerInfo.viewConfigurationType);
                    secondaryViewLayer.viewCount = static_cast<uint32_t>(secondaryRenderResource.ProjectionLayerViews.size());
                    secondaryViewLayer.views = secondaryRenderResource.ProjectionLayerViews.data();
                    secondaryViewLayerInfo.layerCount = 1;
                    XrCompositionLayerBaseHeader* secondaryLayersPtr = reinterpret_cast<XrCompositionLayerBaseHeader*>(&secondaryViewLayer);
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

    arcana::task<std::shared_ptr<System::Session>, std::exception_ptr> System::Session::CreateAsync(System& system, void* graphicsDevice, std::function<void*()> windowProvider)
    {
        return arcana::task_from_result<std::exception_ptr>(std::make_shared<System::Session>(system, graphicsDevice, windowProvider));
    }

    System::Session::Session(System& headMountedDisplay, void* graphicsDevice, std::function<void*()>)
        : m_impl{ std::make_unique<System::Session::Impl>(*headMountedDisplay.m_impl, graphicsDevice) }
    {}

    System::Session::~Session() {}

    std::unique_ptr<System::Session::Frame> System::Session::GetNextFrame(bool& shouldEndSession, bool& shouldRestartSession, std::function<void(void* /*texturePointer*/)> /*deletedTextureCallback*/)
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

    uintptr_t System::Session::GetNativeXrContext()
    {
        return m_impl->GetNativeXrContext();
    }

    std::string System::Session::GetNativeXrContextType()
    {
        return m_impl->GetNativeXrContextType();
    }
}
