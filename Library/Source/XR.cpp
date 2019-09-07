#include "XR.h"

#ifdef WIN32
#include <d3d11.h>
#define XR_USE_GRAPHICS_API_D3D11
#endif

#include <openxr/openxr.h>
#include <openxr/openxr_platform.h>

#define XR_DO(OPERATION) do { XrResult result = OPERATION; if (result != XrResult::XR_SUCCESS) return result; } while (false)
#define XR_CHECK(OPERATION) do { XrResult result = OPERATION; if (XR_FAILED(result)) throw std::exception{}; } while (false)

#include <gsl/span>

#include <algorithm>
#include <array>
#include <assert.h>
#include <thread>
#include <string>
#include <vector>

// XR platform-specific header stuff.
namespace
{
    auto CreateGraphicsBinding(XrInstance instance, XrSystemId systemId, void* graphicsDevice)
    {
        // Create the D3D11 device for the adapter associated with the system.
        XrGraphicsRequirementsD3D11KHR graphicsRequirements{ XR_TYPE_GRAPHICS_REQUIREMENTS_D3D11_KHR };
        XR_CHECK(xrGetD3D11GraphicsRequirementsKHR(instance, systemId, &graphicsRequirements));

        XrGraphicsBindingD3D11KHR graphicsBinding{ XR_TYPE_GRAPHICS_BINDING_D3D11_KHR };
        graphicsBinding.device = reinterpret_cast<ID3D11Device*>(graphicsDevice);
        return graphicsBinding;
    }

    using SwapchainFormat = DXGI_FORMAT;
    using SwapchainImage = XrSwapchainImageD3D11KHR;

    constexpr std::array<SwapchainFormat, 1> SUPPORTED_COLOR_FORMATS
    {
        DXGI_FORMAT_R8G8B8A8_UNORM
    };

    constexpr std::array<SwapchainFormat, 1> SUPPORTED_DEPTH_FORMATS
    {
        DXGI_FORMAT_D24_UNORM_S8_UINT
    };

    constexpr auto SWAPCHAIN_IMAGE_TYPE_ENUM{ XR_TYPE_SWAPCHAIN_IMAGE_D3D11_KHR };
}

namespace
{
    struct SupportedExtensions
    {
        SupportedExtensions()
            : Names{}
        {
            uint32_t extensionCount{};
            XR_CHECK(xrEnumerateInstanceExtensionProperties(nullptr, 0, &extensionCount, nullptr));
            std::vector<XrExtensionProperties> extensionProperties(extensionCount, { XR_TYPE_EXTENSION_PROPERTIES });
            XR_CHECK(xrEnumerateInstanceExtensionProperties(nullptr, extensionCount, &extensionCount, extensionProperties.data()));

            // D3D11 extension is required for this sample, so check if it's supported.
            assert(TryEnableExtension(XR_KHR_D3D11_ENABLE_EXTENSION_NAME, extensionProperties));

            // Additional optional extensions for enhanced functionality. Track whether enabled in m_optionalExtensions.
            DepthExtensionSupported = TryEnableExtension(XR_KHR_COMPOSITION_LAYER_DEPTH_EXTENSION_NAME, extensionProperties);
            UnboundedRefSpaceSupported = TryEnableExtension(XR_MSFT_UNBOUNDED_REFERENCE_SPACE_EXTENSION_NAME, extensionProperties);
            SpatialAnchorSupported = TryEnableExtension(XR_MSFT_SPATIAL_ANCHOR_EXTENSION_NAME, extensionProperties);
        }

        std::vector<const char*> Names{};
        bool DepthExtensionSupported{ false };
        bool UnboundedRefSpaceSupported{ false };
        bool SpatialAnchorSupported{ false };

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
}

namespace babylon
{
    struct HeadMountedDisplay::Impl
    {
        constexpr static XrFormFactor FORM_FACTOR{ XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY };
        constexpr static XrViewConfigurationType VIEW_CONFIGURATION_TYPE{ XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO };
        constexpr static uint32_t STEREO_VIEW_COUNT{ 2 }; // PRIMARY_STEREO view configuration always has 2 views

        XrInstance Instance{ XR_NULL_HANDLE };
        XrSystemId SystemId{ XR_NULL_SYSTEM_ID };

        std::unique_ptr<SupportedExtensions> Extensions{};

        XrEnvironmentBlendMode EnvironmentBlendMode{};
        float Near{};
        float Far{};

        std::string ApplicationName{};

        Impl(const std::string& applicationName)
            : Near{ 20.f } // Number taken from sample. Seems weird, learn more.
            , Far{ 1.f }   // Number taken from sample. Seems weird, learn more.
            , ApplicationName{ applicationName }
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
                InitializeXrInstance();
                Extensions = std::make_unique<SupportedExtensions>();
            }

            assert(Extensions != nullptr);

            if (SystemId == XR_NULL_SYSTEM_ID)
            {
                return TryInitializeXrSystemIdAndBlendMode();
            }
        }

    private:
        // Phase one of initialization. Cannot fail without crashing.
        void InitializeXrInstance()
        {
            XrInstanceCreateInfo createInfo{ XR_TYPE_INSTANCE_CREATE_INFO };
            createInfo.enabledExtensionCount = static_cast<uint32_t>(Extensions->Names.size());
            createInfo.enabledExtensionNames = Extensions->Names.data();
            createInfo.applicationInfo = { "", 1, "OpenXR Sample", 1, XR_CURRENT_API_VERSION };
            strcpy_s(createInfo.applicationInfo.applicationName, ApplicationName.c_str());
            XR_CHECK(xrCreateInstance(&createInfo, &Instance));
        }

        // Phase two of initialization. Can fail and be retried without crashing.
        bool TryInitializeXrSystemIdAndBlendMode()
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
                throw std::exception{ "SystemId initialization failed with unexpected result type." };
            }

            // Find the available environment blend modes.
            uint32_t count;
            XR_CHECK(xrEnumerateEnvironmentBlendModes(Instance, SystemId, VIEW_CONFIGURATION_TYPE, 0, &count, nullptr));
            std::vector<XrEnvironmentBlendMode> environmentBlendModes(count);
            XR_CHECK(xrEnumerateEnvironmentBlendModes(Instance, SystemId, VIEW_CONFIGURATION_TYPE, count, &count, environmentBlendModes.data()));

            // TODO: Support preferentially targeting one blend mode over another.
            // For now, just choose the system's preferred blend mode.
            assert(environmentBlendModes.size() > 0);
            EnvironmentBlendMode = environmentBlendModes[0];
        }
    };

    struct HeadMountedDisplay::Session::Impl
    {
        const HeadMountedDisplay::Impl& HmdImpl;
        XrSession Session{ XR_NULL_HANDLE };

        XrSpace SceneSpace{ XR_NULL_HANDLE };
        XrReferenceSpaceType SceneSpaceType{};

        constexpr static uint32_t LeftSide = 0;
        constexpr static uint32_t RightSide = 1;

        struct SwapchainT {
            XrSwapchain Swapchain;
            SwapchainFormat Format{};
            int32_t Width{ 0 };
            int32_t Height{ 0 };
            uint32_t ArraySize{ 0 };
            std::vector<SwapchainImage> Images;
        };

        struct RenderResources {
            std::vector<XrView> Views;
            std::vector<XrViewConfigurationView> ConfigViews;
            SwapchainT ColorSwapchain;
            SwapchainT DepthSwapchain;
            std::vector<XrCompositionLayerProjectionView> ProjectionLayerViews;
            std::vector<XrCompositionLayerDepthInfoKHR> DepthInfoViews;
        };

        std::unique_ptr<RenderResources> Resources{};

        bool SessionRunning{ false };
        XrSessionState SessionState{ XR_SESSION_STATE_UNKNOWN };

        Impl(HeadMountedDisplay::Impl& hmdImpl, void* graphicsContext)
            : HmdImpl{ hmdImpl }
        {
            assert(HmdImpl.IsInitialized());
            auto instance = HmdImpl.Instance;
            auto systemId = HmdImpl.SystemId;

            // Create the session
            auto graphicsBinding = CreateGraphicsBinding(instance, systemId, graphicsContext);
            XrSessionCreateInfo createInfo{ XR_TYPE_SESSION_CREATE_INFO };
            createInfo.next = &graphicsBinding;
            createInfo.systemId = systemId;
            XR_CHECK(xrCreateSession(instance, &createInfo, &Session));

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
            XR_CHECK(xrCreateReferenceSpace(Session, &spaceCreateInfo, &SceneSpace));

            assert(Resources == nullptr);
            Resources = std::make_unique<RenderResources>();

            // Read graphics properties for preferred swapchain length and logging.
            XrSystemProperties systemProperties{ XR_TYPE_SYSTEM_PROPERTIES };
            XR_CHECK(xrGetSystemProperties(instance, systemId, &systemProperties));

            // Select color and depth swapchain pixel formats
            SwapchainFormat colorSwapchainFormat;
            SwapchainFormat depthSwapchainFormat;
            SelectSwapchainPixelFormats(colorSwapchainFormat, depthSwapchainFormat);

            // Query and cache view configuration views. Two-call idiom.
            uint32_t viewCount;
            XR_CHECK(xrEnumerateViewConfigurationViews(instance, systemId, HmdImpl.VIEW_CONFIGURATION_TYPE, 0, &viewCount, nullptr));
            assert(viewCount == HmdImpl.STEREO_VIEW_COUNT);
            Resources->ConfigViews.resize(viewCount, { XR_TYPE_VIEW_CONFIGURATION_VIEW });
            XR_CHECK(xrEnumerateViewConfigurationViews(instance, systemId, HmdImpl.VIEW_CONFIGURATION_TYPE, viewCount, &viewCount, Resources->ConfigViews.data()));

            // Using texture array for better performance, but requiring left/right views have identical sizes.
            const XrViewConfigurationView& view = Resources->ConfigViews[0];
            assert(Resources->ConfigViews[0].recommendedImageRectWidth == Resources->ConfigViews[1].recommendedImageRectWidth);
            assert(Resources->ConfigViews[0].recommendedImageRectHeight == Resources->ConfigViews[1].recommendedImageRectHeight);
            assert(Resources->ConfigViews[0].recommendedSwapchainSampleCount == Resources->ConfigViews[1].recommendedSwapchainSampleCount);

            // Create swapchains with texture array for color and depth images.
            // The texture array has the size of viewCount, and they are rendered in a single pass using VPRT.
            const uint32_t textureArraySize = viewCount;
            Resources->ColorSwapchain =
                CreateSwapchain(Session,
                    colorSwapchainFormat,
                    view.recommendedImageRectWidth,
                    view.recommendedImageRectHeight,
                    textureArraySize,
                    view.recommendedSwapchainSampleCount,
                    0,
                    XR_SWAPCHAIN_USAGE_SAMPLED_BIT | XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT);
            Resources->DepthSwapchain =
                CreateSwapchain(Session,
                    depthSwapchainFormat,
                    view.recommendedImageRectWidth,
                    view.recommendedImageRectHeight,
                    textureArraySize,
                    view.recommendedSwapchainSampleCount,
                    0,
                    XR_SWAPCHAIN_USAGE_SAMPLED_BIT | XR_SWAPCHAIN_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);

            // Preallocate view buffers for xrLocateViews later inside frame loop.
            Resources->Views.resize(viewCount, { XR_TYPE_VIEW });
        }

    private:
        static constexpr XrPosef IDENTITY_TRANSFORM{ XrQuaternionf{ 0.f, 0.f, 0.f, 1.f }, XrVector3f{ 0.f, 0.f, 0.f } };

        SwapchainT CreateSwapchain(XrSession session,
            SwapchainFormat format,
            int32_t width,
            int32_t height,
            uint32_t arraySize,
            uint32_t sampleCount,
            XrSwapchainCreateFlags createFlags,
            XrSwapchainUsageFlags usageFlags)
        {
            SwapchainT swapchain;
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

            XR_CHECK(xrCreateSwapchain(session, &swapchainCreateInfo, &swapchain.Swapchain));

            uint32_t chainLength;
            XR_CHECK(xrEnumerateSwapchainImages(swapchain.Swapchain, 0, &chainLength, nullptr));
            swapchain.Images.resize(chainLength, { SWAPCHAIN_IMAGE_TYPE_ENUM });
            XR_CHECK(xrEnumerateSwapchainImages(swapchain.Swapchain, (uint32_t)swapchain.Images.size(), &chainLength,
                reinterpret_cast<XrSwapchainImageBaseHeader*>(swapchain.Images.data())));

            return swapchain;
        }

        void SelectSwapchainPixelFormats(SwapchainFormat& colorFormat, SwapchainFormat& depthFormat)
        {
            // Query runtime preferred swapchain formats. Two-call idiom.
            uint32_t swapchainFormatCount;
            XR_CHECK(xrEnumerateSwapchainFormats(Session, 0, &swapchainFormatCount, nullptr));
            std::vector<int64_t> swapchainFormats(swapchainFormatCount);
            XR_CHECK(xrEnumerateSwapchainFormats(Session, (uint32_t)swapchainFormats.size(), &swapchainFormatCount, swapchainFormats.data()));

            auto colorFormatPtr = std::find_first_of(
                std::begin(swapchainFormats),
                std::end(swapchainFormats),
                std::begin(SUPPORTED_COLOR_FORMATS),
                std::end(SUPPORTED_COLOR_FORMATS));
            if (colorFormatPtr == std::end(swapchainFormats))
            {
                throw std::exception{ "No runtime swapchain format is supported for color." };
            }

            auto depthFormatPtr = std::find_first_of(
                std::begin(swapchainFormats),
                std::end(swapchainFormats),
                std::begin(SUPPORTED_DEPTH_FORMATS),
                std::end(SUPPORTED_DEPTH_FORMATS));
            if (depthFormatPtr == std::end(swapchainFormats))
            {
                throw std::exception{ "No runtime swapchain format is supported for depth." };
            }

            colorFormat = static_cast<SwapchainFormat>(*colorFormatPtr);
            depthFormat = static_cast<SwapchainFormat>(*depthFormatPtr);
        }

        bool TryReadNextEvent(XrEventDataBuffer& buffer) const
        {
            // Reset buffer header for every xrPollEvent function call.
            buffer = { XR_TYPE_EVENT_DATA_BUFFER };
            const XrResult xr = xrPollEvent(HmdImpl.Instance, &buffer);
            if (xr == XR_EVENT_UNAVAILABLE)
            {
                return false;
            }
            else
            {
                return true;
            }
        }

        void ProcessEvents(bool* exitRenderLoop, bool* requestRestart)
        {
            *exitRenderLoop = *requestRestart = false;

            XrEventDataBuffer buffer{ XR_TYPE_EVENT_DATA_BUFFER };
            XrEventDataBaseHeader* header = reinterpret_cast<XrEventDataBaseHeader*>(&buffer);

            // Process all pending messages.
            while (TryReadNextEvent(buffer))
            {
                switch (header->type)
                {
                case XR_TYPE_EVENT_DATA_INSTANCE_LOSS_PENDING:
                    *exitRenderLoop = true;
                    *requestRestart = false;
                    return;
                case XR_TYPE_EVENT_DATA_SESSION_STATE_CHANGED:
                    const auto stateEvent = *reinterpret_cast<const XrEventDataSessionStateChanged*>(header);
                    assert(Session != XR_NULL_HANDLE && Session == stateEvent.session);
                    SessionState = stateEvent.state;
                    switch (SessionState)
                    {
                    case XR_SESSION_STATE_READY:
                    {
                        assert(Session != XR_NULL_HANDLE);
                        XrSessionBeginInfo sessionBeginInfo{ XR_TYPE_SESSION_BEGIN_INFO };
                        sessionBeginInfo.primaryViewConfigurationType = HmdImpl.VIEW_CONFIGURATION_TYPE;
                        XR_CHECK(xrBeginSession(Session, &sessionBeginInfo));
                        SessionRunning = true;
                        break;
                    }
                    case XR_SESSION_STATE_STOPPING:
                        SessionRunning = false;
                        XR_CHECK(xrEndSession(Session));
                        break;
                    case XR_SESSION_STATE_EXITING:
                        // Do not attempt to restart because user closed this session.
                        *exitRenderLoop = true;
                        *requestRestart = false;
                        break;
                    case XR_SESSION_STATE_LOSS_PENDING:
                        // Poll for a new systemId
                        *exitRenderLoop = true;
                        *requestRestart = true;
                        break;
                    }
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

    // Choppied and copied out of the OpenXR VS SDK
    struct OpenXR
    {
        // ------------------------------ Start CreateInstance ----------------------------
        std::vector<const char*> SelectExtensions() {
            // Fetch the list of extensions supported by the runtime.
            uint32_t extensionCount;
            XR_CHECK(xrEnumerateInstanceExtensionProperties(nullptr, 0, &extensionCount, nullptr));
            std::vector<XrExtensionProperties> extensionProperties(extensionCount, { XR_TYPE_EXTENSION_PROPERTIES });
            XR_CHECK(xrEnumerateInstanceExtensionProperties(nullptr, extensionCount, &extensionCount, extensionProperties.data()));

            std::vector<const char*> enabledExtensions;

            // Add a specific extension to the list of extensions to be enabled, if it is supported.
            auto EnableExtentionIfSupported = [&](const char* extensionName) {
                for (uint32_t i = 0; i < extensionCount; i++) {
                    if (strcmp(extensionProperties[i].extensionName, extensionName) == 0) {
                        enabledExtensions.push_back(extensionName);
                        return true;
                    }
                }
                return false;
            };

            // D3D11 extension is required for this sample, so check if it's supported.
            assert(EnableExtentionIfSupported(XR_KHR_D3D11_ENABLE_EXTENSION_NAME));

            // Additional optional extensions for enhanced functionality. Track whether enabled in m_optionalExtensions.
            m_optionalExtensions.DepthExtensionSupported = EnableExtentionIfSupported(XR_KHR_COMPOSITION_LAYER_DEPTH_EXTENSION_NAME);
            m_optionalExtensions.UnboundedRefSpaceSupported = EnableExtentionIfSupported(XR_MSFT_UNBOUNDED_REFERENCE_SPACE_EXTENSION_NAME);
            m_optionalExtensions.SpatialAnchorSupported = EnableExtentionIfSupported(XR_MSFT_SPATIAL_ANCHOR_EXTENSION_NAME);

            return enabledExtensions;
        }

        void CreateInstance()
        {
            assert(m_instance == XR_NULL_HANDLE);

            // Build out the extensions to enable. Some extensions are required and some are optional.
            const std::vector<const char*> enabledExtensions = SelectExtensions();

            // Create the instance with desired extensions.
            XrInstanceCreateInfo createInfo{ XR_TYPE_INSTANCE_CREATE_INFO };
            createInfo.enabledExtensionCount = (uint32_t)enabledExtensions.size();
            createInfo.enabledExtensionNames = enabledExtensions.data();

            createInfo.applicationInfo = { "", 1, "OpenXR Sample", 1, XR_CURRENT_API_VERSION };
            strcpy_s(createInfo.applicationInfo.applicationName, m_applicationName.c_str());
            XR_CHECK(xrCreateInstance(&createInfo, &m_instance));
        }
        // ------------------------------ End CreateInstance ----------------------------
        // ------------------------------ Start InitializeSystem ------------------------------
        void InitializeSystem() {
            assert(m_instance != XR_NULL_HANDLE);
            assert(m_systemId == XR_NULL_SYSTEM_ID);

            XrSystemGetInfo systemInfo{ XR_TYPE_SYSTEM_GET_INFO };
            systemInfo.formFactor = FORM_FACTOR;
            while (true) {
                XrResult result = xrGetSystem(m_instance, &systemInfo, &m_systemId);
                if (XR_SUCCEEDED(result)) {
                    break;
                }
                else if (result == XR_ERROR_FORM_FACTOR_UNAVAILABLE) {
                    // DEBUG_PRINT("No headset detected.  Trying again in one second...");
                    using namespace std::chrono_literals;
                    std::this_thread::sleep_for(1s);
                }
                else {
                    // CHECK_XRRESULT(result, "xrGetSystem");
                    throw std::exception{}; // TODO
                }
            };

            // Choose an environment blend mode.
            {
                // Query the list of supported environment blend modes for the current system
                uint32_t count;
                XR_CHECK(xrEnumerateEnvironmentBlendModes(m_instance, m_systemId, VIEW_CONFIGURATION_TYPE, 0, &count, nullptr));
                assert(count > 0); // A system must support at least one environment blend mode.

                std::vector<XrEnvironmentBlendMode> environmentBlendModes(count);
                XR_CHECK(xrEnumerateEnvironmentBlendModes(
                    m_instance, m_systemId, VIEW_CONFIGURATION_TYPE, count, &count, environmentBlendModes.data()));

                // This sample supports all modes, pick the system's preferred one.
                m_environmentBlendMode = environmentBlendModes[0];
            }

            // Choose a reasonable depth range can help improve hologram visual quality.
            // Use reversed Z (near > far) for more uniformed Z resolution.
            m_near = 20.f;
            m_far = 0.1f;
        }
        // ------------------------------ End InitializeSystem ------------------------------

        // *************************** EVERYTHING ABOVE HERE IS IN AN OBJECT NOW ***************************

        void InitializeSession() {
            assert(m_instance != XR_NULL_HANDLE);
            assert(m_systemId != XR_NULL_SYSTEM_ID);
            assert(m_session == XR_NULL_HANDLE);

            // Create the D3D11 device for the adapter associated with the system.
            XrGraphicsRequirementsD3D11KHR graphicsRequirements{ XR_TYPE_GRAPHICS_REQUIREMENTS_D3D11_KHR };
            XR_CHECK(xrGetD3D11GraphicsRequirementsKHR(m_instance, m_systemId, &graphicsRequirements));

            // Create a list of feature levels which are both supported by the OpenXR runtime and this application.
            std::vector<D3D_FEATURE_LEVEL> featureLevels = { D3D_FEATURE_LEVEL_12_1,
                                                            D3D_FEATURE_LEVEL_12_0,
                                                            D3D_FEATURE_LEVEL_11_1,
                                                            D3D_FEATURE_LEVEL_11_0,
                                                            D3D_FEATURE_LEVEL_10_1,
                                                            D3D_FEATURE_LEVEL_10_0 };
            featureLevels.erase(std::remove_if(featureLevels.begin(),
                featureLevels.end(),
                [&](D3D_FEATURE_LEVEL fl) { return fl < graphicsRequirements.minFeatureLevel; }),
                featureLevels.end());
            assert(featureLevels.size() != 0); // "Unsupported minimum feature level!"

            ID3D11Device* device = nullptr;//TODO m_graphicsPlugin->InitializeDevice(graphicsRequirements.adapterLuid, featureLevels);

            XrGraphicsBindingD3D11KHR graphicsBinding{ XR_TYPE_GRAPHICS_BINDING_D3D11_KHR };
            graphicsBinding.device = device;

            XrSessionCreateInfo createInfo{ XR_TYPE_SESSION_CREATE_INFO };
            createInfo.next = &graphicsBinding;
            createInfo.systemId = m_systemId;
            XR_CHECK(xrCreateSession(m_instance, &createInfo, &m_session));

            CreateSpaces();
            CreateSwapchains();
        }

        void CreateSpaces() {
            assert(m_session != XR_NULL_HANDLE);

            // Create a space to place a cube in the world.
            {
                if (m_optionalExtensions.UnboundedRefSpaceSupported) {
                    // Unbounded reference space provides the best scene space for world-scale experiences.
                    m_sceneSpaceType = XR_REFERENCE_SPACE_TYPE_UNBOUNDED_MSFT;
                }
                else {
                    // If running on a platform that does not support world-scale experiences, fall back to local space.
                    m_sceneSpaceType = XR_REFERENCE_SPACE_TYPE_LOCAL;
                }

                XrReferenceSpaceCreateInfo spaceCreateInfo{ XR_TYPE_REFERENCE_SPACE_CREATE_INFO };
                spaceCreateInfo.referenceSpaceType = m_sceneSpaceType;
                // spaceCreateInfo.poseInReferenceSpace = xr::math::Pose::Identity();
                XR_CHECK(xrCreateReferenceSpace(m_session, &spaceCreateInfo, &m_sceneSpace));
            }
        }

        std::tuple<DXGI_FORMAT, DXGI_FORMAT> SelectSwapchainPixelFormats()
        {
            assert(m_session != XR_NULL_HANDLE);

            // Query runtime preferred swapchain formats.
            uint32_t swapchainFormatCount;
            XR_CHECK(xrEnumerateSwapchainFormats(m_session, 0, &swapchainFormatCount, nullptr));

            std::vector<int64_t> swapchainFormats(swapchainFormatCount);
            XR_CHECK(xrEnumerateSwapchainFormats(
                m_session, (uint32_t)swapchainFormats.size(), &swapchainFormatCount, swapchainFormats.data()));

            // Choose the first runtime preferred format that this app supports.
            auto SelectPixelFormat = [](const std::vector<int64_t>& runtimePreferredFormats,
                const std::vector<DXGI_FORMAT>& applicationSupportedFormats) {
                auto found = std::find_first_of(std::begin(runtimePreferredFormats),
                    std::end(runtimePreferredFormats),
                    std::begin(applicationSupportedFormats),
                    std::end(applicationSupportedFormats));
                if (found == std::end(runtimePreferredFormats)) {
                    throw std::exception{ "No runtime swapchain format is supported." };
                }
                return (DXGI_FORMAT)* found;
            };

            // DXGI_FORMAT colorSwapchainFormat = SelectPixelFormat(swapchainFormats, m_graphicsPlugin->SupportedColorFormats());
            // DXGI_FORMAT depthSwapchainFormat = SelectPixelFormat(swapchainFormats, m_graphicsPlugin->SupportedDepthFormats());

            // return { colorSwapchainFormat, depthSwapchainFormat };
        }

        void CreateSwapchains()
        {
            assert(m_session != XR_NULL_HANDLE);
            assert(m_renderResources == nullptr);

            m_renderResources = std::make_unique<RenderResources>();

            // Read graphics properties for preferred swapchain length and logging.
            XrSystemProperties systemProperties{ XR_TYPE_SYSTEM_PROPERTIES };
            XR_CHECK(xrGetSystemProperties(m_instance, m_systemId, &systemProperties));

            // Select color and depth swapchain pixel formats
            const auto [colorSwapchainFormat, depthSwapchainFormat] = SelectSwapchainPixelFormats();

            // Query and cache view configuration views.
            uint32_t viewCount;
            XR_CHECK(xrEnumerateViewConfigurationViews(m_instance, m_systemId, VIEW_CONFIGURATION_TYPE, 0, &viewCount, nullptr));
            assert(viewCount == STEREO_VIEW_COUNT);

            m_renderResources->ConfigViews.resize(viewCount, { XR_TYPE_VIEW_CONFIGURATION_VIEW });
            XR_CHECK(xrEnumerateViewConfigurationViews(
                m_instance, m_systemId, VIEW_CONFIGURATION_TYPE, viewCount, &viewCount, m_renderResources->ConfigViews.data()));

            // Using texture array for better performance, but requiring left/right views have identical sizes.
            const XrViewConfigurationView& view = m_renderResources->ConfigViews[0];
            assert(m_renderResources->ConfigViews[0].recommendedImageRectWidth ==
                m_renderResources->ConfigViews[1].recommendedImageRectWidth);
            assert(m_renderResources->ConfigViews[0].recommendedImageRectHeight ==
                m_renderResources->ConfigViews[1].recommendedImageRectHeight);
            assert(m_renderResources->ConfigViews[0].recommendedSwapchainSampleCount ==
                m_renderResources->ConfigViews[1].recommendedSwapchainSampleCount);

            // Create swapchains with texture array for color and depth images.
            // The texture array has the size of viewCount, and they are rendered in a single pass using VPRT.
            const uint32_t textureArraySize = viewCount;
            m_renderResources->ColorSwapchain =
                CreateSwapchainD3D11(m_session,
                    colorSwapchainFormat,
                    view.recommendedImageRectWidth,
                    view.recommendedImageRectHeight,
                    textureArraySize,
                    view.recommendedSwapchainSampleCount,
                    0,
                    XR_SWAPCHAIN_USAGE_SAMPLED_BIT | XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT);

            m_renderResources->DepthSwapchain =
                CreateSwapchainD3D11(m_session,
                    depthSwapchainFormat,
                    view.recommendedImageRectWidth,
                    view.recommendedImageRectHeight,
                    textureArraySize,
                    view.recommendedSwapchainSampleCount,
                    0,
                    XR_SWAPCHAIN_USAGE_SAMPLED_BIT | XR_SWAPCHAIN_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);

            // Preallocate view buffers for xrLocateViews later inside frame loop.
            m_renderResources->Views.resize(viewCount, { XR_TYPE_VIEW });
        }

        struct SwapchainD3D11;
        SwapchainD3D11 CreateSwapchainD3D11(XrSession session,
            DXGI_FORMAT format,
            int32_t width,
            int32_t height,
            uint32_t arraySize,
            uint32_t sampleCount,
            XrSwapchainCreateFlags createFlags,
            XrSwapchainUsageFlags usageFlags) {
            SwapchainD3D11 swapchain;
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

            XR_CHECK(xrCreateSwapchain(session, &swapchainCreateInfo, &swapchain.Swapchain));

            uint32_t chainLength;
            XR_CHECK(xrEnumerateSwapchainImages(swapchain.Swapchain, 0, &chainLength, nullptr));
            swapchain.Images.resize(chainLength, { SWAPCHAIN_IMAGE_TYPE_ENUM });
            XR_CHECK(xrEnumerateSwapchainImages(swapchain.Swapchain, static_cast<uint32_t>(swapchain.Images.size()), &chainLength,
                reinterpret_cast<XrSwapchainImageBaseHeader*>(swapchain.Images.data())));

            return swapchain;
        }

        // Return true if an event is available, otherwise return false.
        bool TryReadNextEvent(XrEventDataBuffer* buffer) const {
            // Reset buffer header for every xrPollEvent function call.
            *buffer = { XR_TYPE_EVENT_DATA_BUFFER };
            const XrResult xr = xrPollEvent(m_instance, buffer);
            if (xr == XR_EVENT_UNAVAILABLE) {
                return false;
            }
            else {
                return true;
            }
        }

        void ProcessEvents(bool* exitRenderLoop, bool* requestRestart) {
            *exitRenderLoop = *requestRestart = false;

            XrEventDataBuffer buffer{ XR_TYPE_EVENT_DATA_BUFFER };
            XrEventDataBaseHeader* header = reinterpret_cast<XrEventDataBaseHeader*>(&buffer);

            // Process all pending messages.
            while (TryReadNextEvent(&buffer)) {
                switch (header->type) {
                case XR_TYPE_EVENT_DATA_INSTANCE_LOSS_PENDING: {
                    *exitRenderLoop = true;
                    *requestRestart = false;
                    return;
                }
                case XR_TYPE_EVENT_DATA_SESSION_STATE_CHANGED: {
                    const auto stateEvent = *reinterpret_cast<const XrEventDataSessionStateChanged*>(header);
                    assert(m_session != XR_NULL_HANDLE && m_session == stateEvent.session);
                    m_sessionState = stateEvent.state;
                    switch (m_sessionState) {
                    case XR_SESSION_STATE_READY: {
                        assert(m_session != XR_NULL_HANDLE);
                        XrSessionBeginInfo sessionBeginInfo{ XR_TYPE_SESSION_BEGIN_INFO };
                        sessionBeginInfo.primaryViewConfigurationType = VIEW_CONFIGURATION_TYPE;
                        XR_CHECK(xrBeginSession(m_session, &sessionBeginInfo));
                        m_sessionRunning = true;
                        break;
                    }
                    case XR_SESSION_STATE_STOPPING: {
                        m_sessionRunning = false;
                        XR_CHECK(xrEndSession(m_session));
                        break;
                    }
                    case XR_SESSION_STATE_EXITING: {
                        // Do not attempt to restart because user closed this session.
                        *exitRenderLoop = true;
                        *requestRestart = false;
                        break;
                    }
                    case XR_SESSION_STATE_LOSS_PENDING: {
                        // Poll for a new systemId
                        *exitRenderLoop = true;
                        *requestRestart = true;
                        break;
                    }
                    }
                    break;
                }
                case XR_TYPE_EVENT_DATA_REFERENCE_SPACE_CHANGE_PENDING:
                case XR_TYPE_EVENT_DATA_INTERACTION_PROFILE_CHANGED:
                default: {
                    // DEBUG_PRINT("Ignoring event type %d", header->type);
                    break;
                }
                }
            }
        }
        // ------------------- VARIABLES ----------------------
        constexpr static XrFormFactor FORM_FACTOR{ XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY };
        constexpr static XrViewConfigurationType VIEW_CONFIGURATION_TYPE{ XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO };
        constexpr static uint32_t STEREO_VIEW_COUNT{ 2 }; // PRIMARY_STEREO view configuration always has 2 views

        const std::string m_applicationName;

        XrInstance m_instance;
        XrSystemId m_systemId{ XR_NULL_SYSTEM_ID };
        XrSession m_session;

        struct {
            bool DepthExtensionSupported{ false };
            bool UnboundedRefSpaceSupported{ false };
            bool SpatialAnchorSupported{ false };
        } m_optionalExtensions;

        XrSpace m_sceneSpace;
        XrReferenceSpaceType m_sceneSpaceType{};

        constexpr static uint32_t LeftSide = 0;
        constexpr static uint32_t RightSide = 1;

        XrEnvironmentBlendMode m_environmentBlendMode{};
        float m_near{};
        float m_far{};

        struct SwapchainD3D11 {
            XrSwapchain Swapchain;
            DXGI_FORMAT Format{ DXGI_FORMAT_UNKNOWN };
            int32_t Width{ 0 };
            int32_t Height{ 0 };
            uint32_t ArraySize{ 0 };
            std::vector<XrSwapchainImageD3D11KHR> Images;
        };

        struct RenderResources {
            std::vector<XrView> Views;
            std::vector<XrViewConfigurationView> ConfigViews;
            SwapchainD3D11 ColorSwapchain;
            SwapchainD3D11 DepthSwapchain;
            std::vector<XrCompositionLayerProjectionView> ProjectionLayerViews;
            std::vector<XrCompositionLayerDepthInfoKHR> DepthInfoViews;
        };

        std::unique_ptr<RenderResources> m_renderResources{};

        bool m_sessionRunning{ false };
        XrSessionState m_sessionState{ XR_SESSION_STATE_UNKNOWN };
    };
}