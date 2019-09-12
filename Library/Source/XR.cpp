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

// XR platform-specific header stuff. Move to header ASAP.
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
                Extensions = std::make_unique<SupportedExtensions>();
                InitializeXrInstance();
            }

            assert(Extensions != nullptr);
            assert(SystemId == XR_NULL_SYSTEM_ID);

            return TryInitializeXrSystemIdAndBlendMode();
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

            return true;
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

        std::unique_ptr<HeadMountedDisplay::Session::XrFrame> GetNextFrame()
        {
            // TODO: Separate this?
            bool foo;
            bool bar;
            ProcessEvents(&foo, &bar);

            return std::make_unique<XrFrame>(*this);
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

    HeadMountedDisplay::Session::XrFrame::XrFrame(Session::Impl& sessionImpl)
        : m_sessionImpl{ sessionImpl }
    {
        auto session = m_sessionImpl.Session;

        XrFrameWaitInfo frameWaitInfo{ XR_TYPE_FRAME_WAIT_INFO };
        XrFrameState frameState{ XR_TYPE_FRAME_STATE };
        XR_CHECK(xrWaitFrame(session, &frameWaitInfo, &frameState));
        m_shouldRender = frameState.shouldRender;
        m_displayTime = frameState.predictedDisplayTime;

        XrFrameBeginInfo frameBeginInfo{ XR_TYPE_FRAME_BEGIN_INFO };
        XR_CHECK(xrBeginFrame(session, &frameBeginInfo));

        auto& renderResources = m_sessionImpl.Resources;
        const auto& colorSwapchain = renderResources->ColorSwapchain;
        const auto& depthSwapchain = renderResources->DepthSwapchain;

        // Only render when session is visible. otherwise submit zero layers
        if (m_shouldRender)
        {
            uint32_t viewCapacityInput = (uint32_t)renderResources->Views.size();
            uint32_t viewCountOutput;
        
            XrViewState viewState{ XR_TYPE_VIEW_STATE };
            XrViewLocateInfo viewLocateInfo{ XR_TYPE_VIEW_LOCATE_INFO };
            viewLocateInfo.viewConfigurationType = HeadMountedDisplay::Impl::VIEW_CONFIGURATION_TYPE;
            viewLocateInfo.displayTime = m_displayTime;
            viewLocateInfo.space = m_sessionImpl.SceneSpace;
            XR_CHECK(xrLocateViews(session, &viewLocateInfo, &viewState, viewCapacityInput, &viewCountOutput, renderResources->Views.data()));
            assert(viewCountOutput == viewCapacityInput);
            assert(viewCountOutput == renderResources->ConfigViews.size());
            assert(viewCountOutput == renderResources->ColorSwapchain.ArraySize);
            assert(viewCountOutput == renderResources->DepthSwapchain.ArraySize);
        
            renderResources->ProjectionLayerViews.resize(viewCountOutput);
            if (m_sessionImpl.HmdImpl.Extensions->DepthExtensionSupported)
            {
                renderResources->DepthInfoViews.resize(viewCountOutput);
            }
        
            // Use the full range of recommended image size to achieve optimum resolution
            const XrRect2Di imageRect = { {0, 0}, {colorSwapchain.Width, colorSwapchain.Height} };
            assert(colorSwapchain.Width == depthSwapchain.Width);
            assert(colorSwapchain.Height == depthSwapchain.Height);
        
            auto AquireAndWaitForSwapchainImage = [](XrSwapchain handle)
            {
                uint32_t swapchainImageIndex;
                XrSwapchainImageAcquireInfo acquireInfo{ XR_TYPE_SWAPCHAIN_IMAGE_ACQUIRE_INFO };
                XR_CHECK(xrAcquireSwapchainImage(handle, &acquireInfo, &swapchainImageIndex));
        
                XrSwapchainImageWaitInfo waitInfo{ XR_TYPE_SWAPCHAIN_IMAGE_WAIT_INFO };
                waitInfo.timeout = XR_INFINITE_DURATION;
                XR_CHECK(xrWaitSwapchainImage(handle, &waitInfo));
        
                return swapchainImageIndex;
            };
        
            const uint32_t colorSwapchainImageIndex = AquireAndWaitForSwapchainImage(colorSwapchain.Swapchain);
            const uint32_t depthSwapchainImageIndex = AquireAndWaitForSwapchainImage(depthSwapchain.Swapchain);
        
            // Prepare rendering parameters of each view for swapchain texture arrays
            for (uint32_t i = 0; i < viewCountOutput; i++)
            {
                // TODO: Actually fill out the views with real stuff.
                Views.emplace_back(/*{ m_renderResources->Views[i].pose, m_renderResources->Views[i].fov, m_nearFar }*/);
        
                renderResources->ProjectionLayerViews[i] = { XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW };
                renderResources->ProjectionLayerViews[i].pose = renderResources->Views[i].pose;
                renderResources->ProjectionLayerViews[i].fov = renderResources->Views[i].fov;
                renderResources->ProjectionLayerViews[i].subImage.swapchain = colorSwapchain.Swapchain;
                renderResources->ProjectionLayerViews[i].subImage.imageRect = imageRect;
                renderResources->ProjectionLayerViews[i].subImage.imageArrayIndex = i;
        
                if (sessionImpl.HmdImpl.Extensions->DepthExtensionSupported)
                {
                    renderResources->DepthInfoViews[i] = { XR_TYPE_COMPOSITION_LAYER_DEPTH_INFO_KHR };
                    renderResources->DepthInfoViews[i].minDepth = 0;
                    renderResources->DepthInfoViews[i].maxDepth = 1;
                    renderResources->DepthInfoViews[i].nearZ = sessionImpl.HmdImpl.Near;
                    renderResources->DepthInfoViews[i].farZ = sessionImpl.HmdImpl.Far;
                    renderResources->DepthInfoViews[i].subImage.swapchain = depthSwapchain.Swapchain;
                    renderResources->DepthInfoViews[i].subImage.imageRect = imageRect;
                    renderResources->DepthInfoViews[i].subImage.imageArrayIndex = i;
        
                    // Chain depth info struct to the corresponding projection layer views's next
                    renderResources->ProjectionLayerViews[i].next = &renderResources->DepthInfoViews[i];
                }
            }
        }
    }

    HeadMountedDisplay::Session::XrFrame::~XrFrame()
    {

        // EndFrame can submit mutiple layers
        std::vector<XrCompositionLayerBaseHeader*> layers{};

        // The projection layer consists of projection layer views.
        XrCompositionLayerProjection layer{ XR_TYPE_COMPOSITION_LAYER_PROJECTION };

        if (m_shouldRender)
        {
            auto& renderResources = m_sessionImpl.Resources;
        
            XrSwapchainImageReleaseInfo releaseInfo{ XR_TYPE_SWAPCHAIN_IMAGE_RELEASE_INFO };
            /*XR_CHECK(*/xrReleaseSwapchainImage(renderResources->ColorSwapchain.Swapchain, &releaseInfo)/*)*/;
            /*XR_CHECK(*/xrReleaseSwapchainImage(renderResources->DepthSwapchain.Swapchain, &releaseInfo)/*)*/;
        
            // Inform the runtime to consider alpha channel during composition
            // The primary display on Hololens has additive environment blend mode. It will ignore alpha channel.
            // But mixed reality capture has alpha blend mode display and use alpha channel to blend content to environment.
            layer.layerFlags = XR_COMPOSITION_LAYER_BLEND_TEXTURE_SOURCE_ALPHA_BIT;
        
            layer.space = m_sessionImpl.SceneSpace;
            layer.viewCount = (uint32_t)renderResources->ProjectionLayerViews.size();
            layer.views = renderResources->ProjectionLayerViews.data();
        
            layers.push_back(reinterpret_cast<XrCompositionLayerBaseHeader*>(&layer));
        }

        // Submit the composition layers for the predicted display time.
        XrFrameEndInfo frameEndInfo{ XR_TYPE_FRAME_END_INFO };
        frameEndInfo.displayTime = m_displayTime;
        frameEndInfo.environmentBlendMode = m_sessionImpl.HmdImpl.EnvironmentBlendMode;
        frameEndInfo.layerCount = (uint32_t)layers.size();
        frameEndInfo.layers = layers.data();
        auto result = /*XR_CHECK(*/xrEndFrame(m_sessionImpl.Session, &frameEndInfo)/*)*/;
        assert(!XR_FAILED(result));
    }

    HeadMountedDisplay::HeadMountedDisplay()
        : m_impl{ std::make_unique<HeadMountedDisplay::Impl>("APP NAME HERE") }
    {}

    HeadMountedDisplay::~HeadMountedDisplay() {}

    bool HeadMountedDisplay::TryInitialize()
    {
        return m_impl->TryInitialize();
    }

    std::unique_ptr<HeadMountedDisplay::Session> HeadMountedDisplay::CreateSession(void* graphicsDevice)
    {
        //return std::make_unique<Session>(*this);
        return std::unique_ptr<Session>{ new Session(*this, graphicsDevice) };
    }

    HeadMountedDisplay::Session::Session(HeadMountedDisplay& headMountedDisplay, void* graphicsDevice)
        : m_impl{ std::make_unique<HeadMountedDisplay::Session::Impl>(*headMountedDisplay.m_impl, graphicsDevice) }
    {}

    HeadMountedDisplay::Session::~Session() {}

    std::unique_ptr<HeadMountedDisplay::Session::XrFrame> HeadMountedDisplay::Session::GetNextFrame()
    {
        return m_impl->GetNextFrame();
    }
}