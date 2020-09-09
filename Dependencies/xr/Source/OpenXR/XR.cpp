#include <XR.h>

#include <XrPlatform.h>

#include <assert.h>
#include <optional>
#include <arcana/threading/task.h>

namespace xr
{
    namespace
    {
        struct SupportedExtensions : ExtensionDispatchTable
        {
            SupportedExtensions()
                : Names{}
            {
                uint32_t extensionCount{};
                XrCheck(xrEnumerateInstanceExtensionProperties(nullptr, 0, &extensionCount, nullptr));
                std::vector<XrExtensionProperties> extensionProperties(extensionCount, { XR_TYPE_EXTENSION_PROPERTIES });
                XrCheck(xrEnumerateInstanceExtensionProperties(nullptr, extensionCount, &extensionCount, extensionProperties.data()));

                // D3D11 extension is required for this sample, so check if it's supported.
                for (const char* extensionName : REQUIRED_EXTENSIONS)
                {
                    if (!TryEnableExtension(extensionName, extensionProperties))
                    {
                        throw std::runtime_error{ "Required extension not supported" };
                    }
                }

                // Additional optional extensions for enhanced functionality. Track whether enabled in m_optionalExtensions.
                DepthExtensionSupported = TryEnableExtension(XR_KHR_COMPOSITION_LAYER_DEPTH_EXTENSION_NAME, extensionProperties);
                UnboundedRefSpaceSupported = TryEnableExtension(XR_MSFT_UNBOUNDED_REFERENCE_SPACE_EXTENSION_NAME, extensionProperties);
                SpatialAnchorSupported = TryEnableExtension(XR_MSFT_SPATIAL_ANCHOR_EXTENSION_NAME, extensionProperties);
                SecondaryViewConfigurationSupported = TryEnableExtension(XR_MSFT_SECONDARY_VIEW_CONFIGURATION_EXTENSION_NAME, extensionProperties);
                FirstPersonObserverSupported = TryEnableExtension(XR_MSFT_FIRST_PERSON_OBSERVER_EXTENSION_NAME, extensionProperties);
            }

            std::vector<const char*> Names{};
            bool DepthExtensionSupported{ false };
            bool UnboundedRefSpaceSupported{ false };
            bool SpatialAnchorSupported{ false };
            bool SecondaryViewConfigurationSupported{ false };
            bool FirstPersonObserverSupported{ false };

        private:
            bool TryEnableExtension(
                const char* extensionName,
                const std::vector<XrExtensionProperties>& extensionProperties)
            {
                for (const auto& extensionProperty : extensionProperties)
                {
                    if (strcmp(extensionProperty.extensionName, extensionName) == 0)
                    {
                        Names.push_back(extensionName);
                        return true;
                    }
                }
                return false;
            };
        };

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
    }

    struct System::Impl
    {
        static constexpr XrFormFactor FORM_FACTOR{ XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY };

        XrInstance Instance{ XR_NULL_HANDLE };
        XrSystemId SystemId{ XR_NULL_SYSTEM_ID };

        std::unique_ptr<SupportedExtensions> Extensions{};
        
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
        {}

        bool IsInitialized() const
        {
            return Instance != XR_NULL_HANDLE && SystemId != XR_NULL_SYSTEM_ID;
        }

        bool TryInitialize()
        {
            assert(!IsInitialized());

            if (Instance == XR_NULL_HANDLE)
            {
                InitializeXrInstanceAndExtensions();
            }

            assert(Extensions != nullptr);
            assert(SystemId == XR_NULL_SYSTEM_ID);

            return TryInitializeXrSystemIdAndViewProperties();
        }

    private:
        // Phase one of initialization. Cannot fail without crashing.
        void InitializeXrInstanceAndExtensions()
        {
            Extensions = std::make_unique<SupportedExtensions>();

            XrInstanceCreateInfo createInfo{ XR_TYPE_INSTANCE_CREATE_INFO };
            createInfo.enabledExtensionCount = static_cast<uint32_t>(Extensions->Names.size());
            createInfo.enabledExtensionNames = Extensions->Names.data();
            createInfo.applicationInfo = { "", 1, "OpenXR Sample", 1, XR_CURRENT_API_VERSION };
            strcpy_s(createInfo.applicationInfo.applicationName, ApplicationName.c_str());
            XrCheck(xrCreateInstance(&createInfo, &Instance));

            Extensions->PopulateDispatchTable(Instance);
        }

        void InitializeViewConfigurations()
        {
            assert(Instance != nullptr);
            assert(Extensions != nullptr);

            const auto systemSupportedViewConfigurationTypes = EnumerateViewConfigurations(Instance, SystemId);
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
            XrSystemGetInfo systemInfo{ XR_TYPE_SYSTEM_GET_INFO };
            systemInfo.formFactor = FORM_FACTOR;

            XrResult result = xrGetSystem(Instance, &systemInfo, &SystemId);
            if (result == XR_ERROR_FORM_FACTOR_UNAVAILABLE)
            {
                SystemId = XR_NULL_SYSTEM_ID;
                return false;
            }
            else if(!XR_SUCCEEDED(result))
            {
                throw std::runtime_error{ "SystemId initialization failed with unexpected result type." };
            }

            // Initialize view configurations before assessing view properties.
            InitializeViewConfigurations();

            // Find the available environment blend modes.
            ViewProperties[PrimaryViewConfigurationType] = CreateViewProperties(Instance, SystemId, PrimaryViewConfigurationType, SupportedEnvironmentBlendModes);
            for (const auto& viewConfiguration : SupportedSecondaryViewConfigurationTypes)
            {
                ViewProperties[viewConfiguration] = CreateViewProperties(Instance, SystemId, viewConfiguration, SupportedEnvironmentBlendModes);
            }

            return true;
        }
    };

    struct System::Session::Impl
    {
        const System::Impl& HmdImpl;
        XrSession Session{ XR_NULL_HANDLE };

        XrSpace SceneSpace{ XR_NULL_HANDLE };
        XrReferenceSpaceType SceneSpaceType{};

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

            static constexpr char* DEFAULT_XR_INTERACTION_PROFILE{ "/interaction_profiles/khr/simple_controller" };

            std::vector<Frame::InputSource> ActiveInputSources{};
            std::vector<Frame::Plane> Planes{};
            std::vector<FeaturePoint> FeaturePointCloud{};
        } ActionResources{};

        float DepthNearZ{ DEFAULT_DEPTH_NEAR_Z };
        float DepthFarZ{ DEFAULT_DEPTH_FAR_Z };

        XrSessionState SessionState{ XR_SESSION_STATE_UNKNOWN };

        Impl(System::Impl& hmdImpl, void* graphicsContext)
            : HmdImpl{ hmdImpl }
        {
            assert(HmdImpl.IsInitialized());
            auto instance = HmdImpl.Instance;
            auto systemId = HmdImpl.SystemId;

            // Create the session
            auto graphicsBinding = CreateGraphicsBinding(*HmdImpl.Extensions, instance, systemId, graphicsContext);
            XrSessionCreateInfo createInfo{ XR_TYPE_SESSION_CREATE_INFO };
            createInfo.next = &graphicsBinding;
            createInfo.systemId = systemId;
            XrCheck(xrCreateSession(instance, &createInfo, &Session));

            // Initialize scene space
            if (HmdImpl.Extensions->UnboundedRefSpaceSupported)
            {
                SceneSpaceType = XR_REFERENCE_SPACE_TYPE_UNBOUNDED_MSFT;
            }
            else
            {
                SceneSpaceType = XR_REFERENCE_SPACE_TYPE_LOCAL;
            }
            XrReferenceSpaceCreateInfo spaceCreateInfo{ XR_TYPE_REFERENCE_SPACE_CREATE_INFO };
            spaceCreateInfo.referenceSpaceType = SceneSpaceType;
            spaceCreateInfo.poseInReferenceSpace = IDENTITY_TRANSFORM;
            XrCheck(xrCreateReferenceSpace(Session, &spaceCreateInfo, &SceneSpace));

            InitializeRenderResources(instance, systemId);
            InitializeActionResources(instance);
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
            xrRequestExitSession(Session);
        }

        Size GetWidthAndHeightForViewIndex(size_t viewIndex) const
        {
            auto& primaryRenderResource = RenderResources.ResourceMap.at(HmdImpl.PrimaryViewConfigurationType);
            auto primaryViewConfigTypeSwapchainCount = primaryRenderResource.ColorSwapchains.size();
            if (viewIndex < primaryViewConfigTypeSwapchainCount)
            {
                const auto& swapchain = primaryRenderResource.ColorSwapchains.at(viewIndex);
                return{ static_cast<size_t>(swapchain.Width), static_cast<size_t>(swapchain.Height) };
            }
            else if (HmdImpl.Extensions->SecondaryViewConfigurationSupported &&
                HmdImpl.SupportedSecondaryViewConfigurationTypes.size() > 0)
            {
                size_t secondaryViewIndex = viewIndex - primaryViewConfigTypeSwapchainCount;
                for (const auto& viewConfigType : HmdImpl.SupportedSecondaryViewConfigurationTypes)
                {
                    const auto& state = RenderResources.ResourceMap.at(viewConfigType).ViewState;
                    if (state.Views.size() <= secondaryViewIndex)
                    {
                        secondaryViewIndex -= state.Views.size();
                        continue;
                    }

                    const auto& viewConfig = state.ViewConfigViews.at(secondaryViewIndex);
                    return { viewConfig.recommendedImageRectWidth, viewConfig.recommendedImageRectHeight };
                }
            }

            // On emulators, SecondaryViewConfigurationTypes aren't populated but still need a size
            return { 0, 0 };
        }

        void PopulateSwapchains(ViewConfigurationState& viewState)
        {
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
                    PopulateSwapchain(Session,
                        colorSwapchainFormat,
                        view.recommendedImageRectWidth,
                        view.recommendedImageRectHeight,
                        1,
                        view.recommendedSwapchainSampleCount,
                        0,
                        XR_SWAPCHAIN_USAGE_SAMPLED_BIT | XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT,
                        viewState.Type,
                        renderResource.ColorSwapchains[idx]);
                    PopulateSwapchain(Session,
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
            const auto& apiExtensions = *HmdImpl.Extensions;

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
            anchorCreateInfo.space = SceneSpace;
            anchorCreateInfo.pose = { pose.Orientation.X, pose.Orientation.Y, pose.Orientation.Z, pose.Orientation.W,
                                      pose.Position.X, pose.Position.Y, pose.Position.Z };
            anchorCreateInfo.time = time;

            CHECK_XRCMD(apiExtensions.xrCreateSpatialAnchorMSFT(
                Session,
                &anchorCreateInfo,
                anchorSpace->Anchor.Put(apiExtensions.xrDestroySpatialAnchorMSFT)));

            XrSpatialAnchorSpaceCreateInfoMSFT anchorSpaceCreateInfo{XR_TYPE_SPATIAL_ANCHOR_SPACE_CREATE_INFO_MSFT};
            anchorSpaceCreateInfo.anchor = anchorSpace->Anchor.Get();
            anchorSpaceCreateInfo.poseInAnchorSpace = xr::math::Pose::Identity();
            CHECK_XRCMD(apiExtensions.xrCreateSpatialAnchorSpaceMSFT(Session, &anchorSpaceCreateInfo, anchorSpace->Space.Put()));

            const auto nativeAnchorPtr = reinterpret_cast<NativeAnchorPtr>(anchorSpace.get());
            openXRAnchors[nativeAnchorPtr] = anchorSpace;
            return { pose, nativeAnchorPtr };
        }

        void UpdateAnchor(Anchor& anchor, XrTime time) const
        {
            const auto anchorEntry = openXRAnchors.find(anchor.NativeAnchor);
            if (anchorEntry == openXRAnchors.end())
            {
                throw std::runtime_error("Could not find this anchor in the OpenXR collection. "
                    "Perhaps the anchor was deleted or the anchor was created with a different XR implementation.");
            }

            auto anchorSpace = anchorEntry->second;

            XrSpaceLocation spaceLocation{XR_TYPE_SPACE_LOCATION};
            auto spaceLocationResult = xrLocateSpace(anchorSpace->Space.Get(), SceneSpace, time, &spaceLocation);
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
        static constexpr XrPosef IDENTITY_TRANSFORM{ XrQuaternionf{ 0.f, 0.f, 0.f, 1.f }, XrVector3f{ 0.f, 0.f, 0.f } };

        struct OpenXRAnchorSpace
        {
            xr::SpatialAnchorHandle Anchor;
            xr::SpaceHandle Space;
        };

        std::unordered_map<NativeAnchorPtr, std::shared_ptr<OpenXRAnchorSpace>> openXRAnchors{};

        void InitializeRenderResources(XrInstance instance, XrSystemId systemId)
        {
            // Read graphics properties for preferred swapchain length and logging.
            XrSystemProperties systemProperties{ XR_TYPE_SYSTEM_PROPERTIES };
            XrCheck(xrGetSystemProperties(instance, systemId, &systemProperties));

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

        void InitializeActionResources(XrInstance instance)
        {
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

            std::vector<XrActionSuggestedBinding> bindings{};

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
                    bindings.push_back({ ActionResources.ControllerGetGripPoseAction });
                    XrCheck(xrStringToPath(instance, path.data(), &bindings.back().binding));

                    // Create subaction space
                    XrActionSpaceCreateInfo actionSpaceCreateInfo{ XR_TYPE_ACTION_SPACE_CREATE_INFO };
                    actionSpaceCreateInfo.action = ActionResources.ControllerGetGripPoseAction;
                    actionSpaceCreateInfo.poseInActionSpace = IDENTITY_TRANSFORM;
                    actionSpaceCreateInfo.subactionPath = ActionResources.ControllerSubactionPaths[idx];
                    XrCheck(xrCreateActionSpace(Session, &actionSpaceCreateInfo, &ActionResources.ControllerGripPoseSpaces[idx]));
                }
            }

            // Create controller controller get aim pose action, suggested bindings, and spaces
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
                    bindings.push_back({ ActionResources.ControllerGetAimPoseAction });
                    XrCheck(xrStringToPath(instance, path.data(), &bindings.back().binding));

                    // Create subaction space
                    XrActionSpaceCreateInfo actionSpaceCreateInfo{ XR_TYPE_ACTION_SPACE_CREATE_INFO };
                    actionSpaceCreateInfo.action = ActionResources.ControllerGetAimPoseAction;
                    actionSpaceCreateInfo.poseInActionSpace = IDENTITY_TRANSFORM;
                    actionSpaceCreateInfo.subactionPath = ActionResources.ControllerSubactionPaths[idx];
                    XrCheck(xrCreateActionSpace(Session, &actionSpaceCreateInfo, &ActionResources.ControllerAimPoseSpaces[idx]));
                }
            }

            // Provide suggested bindings to instance
            XrInteractionProfileSuggestedBinding suggestedBindings{ XR_TYPE_INTERACTION_PROFILE_SUGGESTED_BINDING };
            XrCheck(xrStringToPath(instance, ActionResources.DEFAULT_XR_INTERACTION_PROFILE, &suggestedBindings.interactionProfile));
            suggestedBindings.suggestedBindings = bindings.data();
            suggestedBindings.countSuggestedBindings = (uint32_t)bindings.size();
            XrCheck(xrSuggestInteractionProfileBindings(instance, &suggestedBindings));

            XrSessionActionSetsAttachInfo attachInfo{ XR_TYPE_SESSION_ACTION_SETS_ATTACH_INFO };
            attachInfo.countActionSets = 1;
            attachInfo.actionSets = &ActionResources.ActionSet;
            XrCheck(xrAttachSessionActionSets(Session, &attachInfo));
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
            if (HmdImpl.Extensions->SecondaryViewConfigurationSupported)
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
            // Query runtime preferred swapchain formats. Two-call idiom.
            uint32_t swapchainFormatCount;
            XrCheck(xrEnumerateSwapchainFormats(Session, 0, &swapchainFormatCount, nullptr));
            std::vector<int64_t> swapchainFormats(swapchainFormatCount);
            XrCheck(xrEnumerateSwapchainFormats(Session, static_cast<uint32_t>(swapchainFormats.size()), &swapchainFormatCount, swapchainFormats.data()));

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
            const XrResult xr = xrPollEvent(HmdImpl.Instance, &buffer);
            return xr != XR_EVENT_UNAVAILABLE;
        }

        void ProcessSessionState(bool& exitRenderLoop, bool& requestRestart)
        {
            switch (SessionState)
            {
            case XR_SESSION_STATE_READY:
            {
                assert(Session != XR_NULL_HANDLE);
                XrSessionBeginInfo sessionBeginInfo{ XR_TYPE_SESSION_BEGIN_INFO };
                sessionBeginInfo.primaryViewConfigurationType = HmdImpl.PrimaryViewConfigurationType;

                XrSecondaryViewConfigurationSessionBeginInfoMSFT secondaryViewConfigInfo
                {
                    XR_TYPE_SECONDARY_VIEW_CONFIGURATION_SESSION_BEGIN_INFO_MSFT
                };

                if (HmdImpl.Extensions->SecondaryViewConfigurationSupported &&
                    HmdImpl.Extensions->FirstPersonObserverSupported &&
                    HmdImpl.SupportedSecondaryViewConfigurationTypes.size() > 0)
                {
                    secondaryViewConfigInfo.viewConfigurationCount = static_cast<uint32_t>(HmdImpl.SupportedSecondaryViewConfigurationTypes.size());
                    secondaryViewConfigInfo.enabledViewConfigurationTypes = HmdImpl.SupportedSecondaryViewConfigurationTypes.data();
                    InsertExtensionStruct(sessionBeginInfo, secondaryViewConfigInfo);
                }

                XrCheck(xrBeginSession(Session, &sessionBeginInfo));
                break;
            }
            case XR_SESSION_STATE_STOPPING:
                XrCheck(xrEndSession(Session));
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
                    assert(Session != XR_NULL_HANDLE && Session == stateEvent.session);
                    SessionState = stateEvent.state;
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
        int64_t displayTime{};

        void LocateViews(const XrViewConfigurationType viewConfigType, uint32_t& viewCountOutput)
        {
            auto& renderResource = sessionImpl.RenderResources.ResourceMap.at(viewConfigType);

            ViewConfigurationState& viewConfigurationState = renderResource.ViewState;
            uint32_t viewCapacityInput = static_cast<uint32_t>(viewConfigurationState.Views.size());
            XrViewState viewState{ XR_TYPE_VIEW_STATE };
            XrViewLocateInfo viewLocateInfo{ XR_TYPE_VIEW_LOCATE_INFO };
            viewLocateInfo.viewConfigurationType = viewConfigType;
            viewLocateInfo.displayTime = displayTime;
            viewLocateInfo.space = sessionImpl.SceneSpace;
            XrCheck(xrLocateViews(sessionImpl.Session, &viewLocateInfo, &viewState, viewCapacityInput, &viewCountOutput, viewConfigurationState.Views.data()));

            assert(viewCountOutput == viewCapacityInput);
            assert(viewCountOutput == renderResource.ColorSwapchains.size());
            assert(viewCountOutput == renderResource.DepthSwapchains.size());

            renderResource.ProjectionLayerViews.resize(viewCountOutput);
            if (sessionImpl.HmdImpl.Extensions->DepthExtensionSupported)
            {
                renderResource.DepthInfoViews.resize(viewCountOutput);
            }
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
            view.FieldOfView.AngleUp = cachedView.fov.angleUp;
            view.FieldOfView.AngleDown = cachedView.fov.angleDown;
            view.FieldOfView.AngleLeft = cachedView.fov.angleLeft;
            view.FieldOfView.AngleRight = cachedView.fov.angleRight;
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
    };

    System::Session::Frame::Frame(Session::Impl& sessionImpl)
        : Views{ sessionImpl.RenderResources.ActiveFrameViews }
        , InputSources{ sessionImpl.ActionResources.ActiveInputSources }
        , Planes { sessionImpl.ActionResources.Planes } // NYI
        , FeaturePointCloud{ sessionImpl.ActionResources.FeaturePointCloud } // NYI
        , UpdatedPlanes{} // NYI
        , RemovedPlanes{} // NYI
        , m_impl{ std::make_unique<System::Session::Frame::Impl>(sessionImpl) }
    {
        auto session = m_impl->sessionImpl.Session;
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
        m_impl->displayTime = frameState.predictedDisplayTime;

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
                        sessionImpl.HmdImpl.Instance,
                        sessionImpl.HmdImpl.SystemId,
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

                if (sessionImpl.HmdImpl.Extensions->DepthExtensionSupported)
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

                            if (sessionImpl.HmdImpl.Extensions->DepthExtensionSupported)
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

            // Locate all the things.
            auto& actionResources = m_impl->sessionImpl.ActionResources;

            std::vector<XrActiveActionSet> activeActionSets = { { actionResources.ActionSet, XR_NULL_PATH } };
            XrActionsSyncInfo syncInfo{ XR_TYPE_ACTIONS_SYNC_INFO };
            syncInfo.countActiveActionSets = (uint32_t)activeActionSets.size();
            syncInfo.activeActionSets = activeActionSets.data();
            XrCheck(xrSyncActions(m_impl->sessionImpl.Session, &syncInfo));

            InputSources.resize(actionResources.CONTROLLER_SUBACTION_PATH_PREFIXES.size());
            for (size_t idx = 0; idx < InputSources.size(); ++idx)
            {
                // Get grip space
                {
                    XrSpace space = actionResources.ControllerGripPoseSpaces[idx];
                    XrSpaceLocation location{ XR_TYPE_SPACE_LOCATION };
                    XrCheck(xrLocateSpace(space, m_impl->sessionImpl.SceneSpace, m_impl->displayTime, &location));

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
                        inputSource.GripSpace.Pose.Position.X = location.pose.position.x;
                        inputSource.GripSpace.Pose.Position.Y = location.pose.position.y;
                        inputSource.GripSpace.Pose.Position.Z = location.pose.position.z;
                        inputSource.GripSpace.Pose.Orientation.X = location.pose.orientation.x;
                        inputSource.GripSpace.Pose.Orientation.Y = location.pose.orientation.y;
                        inputSource.GripSpace.Pose.Orientation.Z = location.pose.orientation.z;
                        inputSource.GripSpace.Pose.Orientation.W = location.pose.orientation.w;
                    }
                }

                // Get aim space
                {
                    XrSpace space = actionResources.ControllerAimPoseSpaces[idx];
                    XrSpaceLocation location{ XR_TYPE_SPACE_LOCATION };
                    XrCheck(xrLocateSpace(space, m_impl->sessionImpl.SceneSpace, m_impl->displayTime, &location));

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
                        inputSource.AimSpace.Pose.Position.X = location.pose.position.x;
                        inputSource.AimSpace.Pose.Position.Y = location.pose.position.y;
                        inputSource.AimSpace.Pose.Position.Z = location.pose.position.z;
                        inputSource.AimSpace.Pose.Orientation.X = location.pose.orientation.x;
                        inputSource.AimSpace.Pose.Orientation.Y = location.pose.orientation.y;
                        inputSource.AimSpace.Pose.Orientation.Z = location.pose.orientation.z;
                        inputSource.AimSpace.Pose.Orientation.W = location.pose.orientation.w;
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
        std::vector<XrCompositionLayerProjection> layers{};

        XrFrameEndInfo frameEndInfo{ XR_TYPE_FRAME_END_INFO };
        frameEndInfo.displayTime = m_impl->displayTime;
        frameEndInfo.environmentBlendMode = m_impl->sessionImpl.HmdImpl.ViewProperties.at(m_impl->sessionImpl.HmdImpl.PrimaryViewConfigurationType).BlendMode;

        XrSecondaryViewConfigurationFrameEndInfoMSFT frameEndSecondaryViewConfigInfo
        {
            XR_TYPE_SECONDARY_VIEW_CONFIGURATION_FRAME_END_INFO_MSFT
        };
        std::vector<XrSecondaryViewConfigurationLayerInfoMSFT> activeSecondaryViewConfigLayerInfos;

        // Chain secondary view configuration layers data to endFrameInfo
        if (m_impl->sessionImpl.HmdImpl.Extensions->SecondaryViewConfigurationSupported &&
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
            primaryLayer.space = m_impl->sessionImpl.SceneSpace;
            const auto& primaryRenderResource = renderResources.ResourceMap.at(m_impl->sessionImpl.HmdImpl.PrimaryViewConfigurationType);
            primaryLayer.viewCount = static_cast<uint32_t>(primaryRenderResource.ProjectionLayerViews.size());
            primaryLayer.views = primaryRenderResource.ProjectionLayerViews.data();
            auto layersPtr = reinterpret_cast<XrCompositionLayerBaseHeader*>(&primaryLayer);
            frameEndInfo.layerCount = 1;
            frameEndInfo.layers = &layersPtr;
                    
            if (m_impl->sessionImpl.HmdImpl.Extensions->SecondaryViewConfigurationSupported &&
                activeSecondaryViewConfigLayerInfos.size() > 0) {

                for (size_t i = 0; i < activeSecondaryViewConfigLayerInfos.size(); i++) {
                    XrSecondaryViewConfigurationLayerInfoMSFT& secondaryViewLayerInfo = activeSecondaryViewConfigLayerInfos.at(i);
                    layers[i + 1] = { XR_TYPE_COMPOSITION_LAYER_PROJECTION };
                    XrCompositionLayerProjection& secondaryViewLayer = layers.at(i + 1);
                    secondaryViewLayer.layerFlags = XR_COMPOSITION_LAYER_BLEND_TEXTURE_SOURCE_ALPHA_BIT;
                    secondaryViewLayer.space = m_impl->sessionImpl.SceneSpace;
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
        
        XrAssert(xrEndFrame(m_impl->sessionImpl.Session, &frameEndInfo));
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

    arcana::task<std::shared_ptr<System::Session>, std::exception_ptr> System::Session::CreateAsync(System& system, void* graphicsDevice, void* window)
    {
        return arcana::task_from_result<std::exception_ptr>(std::make_shared<System::Session>(system, graphicsDevice, window));
    }

    System::Session::Session(System& headMountedDisplay, void* graphicsDevice, void*)
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

    Size System::Session::GetWidthAndHeightForViewIndex(size_t viewIndex) const
    {
        return m_impl->GetWidthAndHeightForViewIndex(viewIndex);
    }

    void System::Session::SetDepthsNearFar(float depthNear, float depthFar)
    {
        m_impl->DepthNearZ = depthNear;
        m_impl->DepthFarZ = depthFar;
    }

    Anchor System::Session::Frame::CreateAnchor(Pose pose, NativeTrackablePtr trackable) const
    {
        return m_impl->sessionImpl.CreateAnchor(pose, trackable, m_impl->displayTime);
    }

    void System::Session::Frame::UpdateAnchor(Anchor& anchor) const
    {
        m_impl->sessionImpl.UpdateAnchor(anchor, m_impl->displayTime);
    }

    void System::Session::Frame::DeleteAnchor(Anchor& anchor) const
    {
        m_impl->sessionImpl.DeleteAnchor(anchor);
    }

    System::Session::Frame::Plane& System::Session::Frame::GetPlaneByID(System::Session::Frame::Plane::Identifier) const
    {
        throw std::runtime_error("Planes not yet implemented for OpenXR.");
    }

    void System::Session::SetPlaneDetectionEnabled(bool) const
    {
        throw std::runtime_error("Planes not yet implemented for OpenXR");
    }

    bool System::Session::TrySetFeaturePointCloudEnabled(bool) const
    {
        // Point cloud system not yet supported.
        return false;
    }
}
