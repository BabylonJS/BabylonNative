#include <XR.h>
#include <XRHelpers.h>

#include <AndroidExtensions/Globals.h>

#define LOG_TAG "LoonyOpenXR"
#include <android/log.h>

#include <EGL/egl.h>
#include <GLES3/gl3.h>

#include <openxr/openxr.h>
#include <openxr/openxr_platform.h>

#include <arcana/threading/task.h>
#include <arcana/threading/task_schedulers.h>

#include <algorithm>
#include <array>
#include <cassert>
#include <chrono>
#include <cstring>
#include <sstream>
#include <stdexcept>
#include <thread>
#include <vector>

#include "Include/IXrContextOpenXR.h"

using namespace android;
using namespace android::global;

namespace
{
    template <typename T>
    T MakeXrStruct(XrStructureType type)
    {
        T value{};
        value.type = type;
        return value;
    }

    template <typename T>
    std::vector<T> MakeXrStructVector(size_t count, XrStructureType type)
    {
        std::vector<T> values(count);
        for (auto& value : values)
        {
            value.type = type;
        }
        return values;
    }
}

namespace
{
    // For the XR_KHR_opengl_es_enable graphics binding, xrEnumerateSwapchainFormats
    // returns raw OpenGL(ES) sized-internal-format tokens (the same enum space as
    // glTexStorage2D's internalformat) -- NOT Vulkan VkFormat numeric values. Only the
    // Vulkan graphics binding extension enumerates VkFormat values.
    constexpr int64_t OPENXR_COLOR_FORMAT_SRGB = 0x8C43;    // GL_SRGB8_ALPHA8
    constexpr int64_t OPENXR_COLOR_FORMAT_UNORM = 0x8058;   // GL_RGBA8
    constexpr int64_t OPENXR_DEPTH_FORMAT_D24S8 = 0x88F0;   // GL_DEPTH24_STENCIL8
    constexpr int64_t OPENXR_DEPTH_FORMAT_D32 = 0x8CAC;     // GL_DEPTH_COMPONENT32F

    constexpr XrPosef IDENTITY_POSE{
        {0.f, 0.f, 0.f, 1.f},
        {0.f, 0.f, 0.f},
    };

    constexpr std::array<int64_t, 2> SUPPORTED_COLOR_FORMATS{
        OPENXR_COLOR_FORMAT_SRGB,
        OPENXR_COLOR_FORMAT_UNORM,
    };

    constexpr std::array<int64_t, 2> SUPPORTED_DEPTH_FORMATS{
        OPENXR_DEPTH_FORMAT_D24S8,
        OPENXR_DEPTH_FORMAT_D32,
    };

    void XrCheck(XrResult result, const char* context)
    {
        if (XR_FAILED(result))
        {
            std::ostringstream message;
            message << context << " failed with XrResult " << result;
            __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "%s", message.str().c_str());
            throw std::runtime_error(message.str());
        }
    }

    xr::TextureFormat SwapchainFormatToTextureFormat(int64_t format)
    {
        switch (format)
        {
            case OPENXR_COLOR_FORMAT_SRGB:
            case OPENXR_COLOR_FORMAT_UNORM:
                return xr::TextureFormat::RGBA8_SRGB;
            case OPENXR_DEPTH_FORMAT_D24S8:
                return xr::TextureFormat::D24S8;
            case OPENXR_DEPTH_FORMAT_D32:
                return xr::TextureFormat::D16;
            default:
                return xr::TextureFormat::RGBA8_SRGB;
        }
    }

    void PopulateProjectionMatrix(const XrView& view, float depthNear, float depthFar, std::array<float, 16>& projectionMatrix)
    {
        const float n = depthNear;
        const float f = depthFar;

        const float l = std::tanf(view.fov.angleLeft) * n;
        const float r = std::tanf(view.fov.angleRight) * n;
        const float t = std::tanf(view.fov.angleUp) * n;
        const float b = std::tanf(view.fov.angleDown) * n;

        const float invDiffRL = 1.f / (r - l);
        const float invDiffTB = 1.f / (t - b);
        const float ww = 2.f * n * invDiffRL;
        const float hh = 2.f * n * invDiffTB;
        const float xx = (r + l) * invDiffRL;
        const float yy = (t + b) * invDiffTB;

        // OpenGL ES uses a [-1, 1] depth range.
        constexpr bool homogeneousDepth = true;

        const float diffFN = f - n;
        const float za = -(homogeneousDepth ? (f + n) / diffFN : f / diffFN);
        const float zb = -(homogeneousDepth ? (2.f * f * n) / diffFN : n * -za);
        constexpr float zc = -1.f;

        projectionMatrix = {
            ww, 0, 0, 0,
            0, hh, 0, 0,
            xx, yy, za, zc,
            0, 0, zb, 0,
        };
    }

    uint32_t AcquireAndWaitForSwapchainImage(XrSwapchain handle)
    {
        uint32_t swapchainImageIndex{};
        XrSwapchainImageAcquireInfo acquireInfo = MakeXrStruct<XrSwapchainImageAcquireInfo>(XR_TYPE_SWAPCHAIN_IMAGE_ACQUIRE_INFO);
        XrCheck(xrAcquireSwapchainImage(handle, &acquireInfo, &swapchainImageIndex), "xrAcquireSwapchainImage");

        XrSwapchainImageWaitInfo waitInfo = MakeXrStruct<XrSwapchainImageWaitInfo>(XR_TYPE_SWAPCHAIN_IMAGE_WAIT_INFO);
        waitInfo.timeout = XR_INFINITE_DURATION;
        XrCheck(xrWaitSwapchainImage(handle, &waitInfo), "xrWaitSwapchainImage");

        return swapchainImageIndex;
    }

    // android::global::GetAppContext()/GetCurrentActivity() return short-lived C++ wrapper
    // objects whose destructor deletes the JNI global reference they hold. OpenXR retains
    // the raw jobject beyond the lifetime of that wrapper (the loader and runtime may use it
    // asynchronously), so we must promote it to a global ref we own for the life of the
    // process before handing it to the OpenXR API.
    jobject ToPersistentGlobalRef(jobject transient)
    {
        return GetEnvForCurrentThread()->NewGlobalRef(transient);
    }

    void InitializeLoader()
    {
        PFN_xrInitializeLoaderKHR initializeLoader = nullptr;
        XrCheck(
            xrGetInstanceProcAddr(XR_NULL_HANDLE, "xrInitializeLoaderKHR", reinterpret_cast<PFN_xrVoidFunction*>(&initializeLoader)),
            "xrGetInstanceProcAddr(xrInitializeLoaderKHR)");

        JavaVM* javaVm = nullptr;
        if (GetEnvForCurrentThread()->GetJavaVM(&javaVm) != JNI_OK || javaVm == nullptr)
        {
            throw std::runtime_error("Failed to obtain JavaVM for OpenXR loader initialization.");
        }

        XrLoaderInitInfoAndroidKHR loaderInitInfo = MakeXrStruct<XrLoaderInitInfoAndroidKHR>(XR_TYPE_LOADER_INIT_INFO_ANDROID_KHR);
        loaderInitInfo.applicationVM = javaVm;
        loaderInitInfo.applicationContext = ToPersistentGlobalRef(GetAppContext());
        XrCheck(initializeLoader(reinterpret_cast<XrLoaderInitInfoBaseHeaderKHR*>(&loaderInitInfo)), "xrInitializeLoaderKHR");
    }

    std::vector<const char*> SelectEnabledInstanceExtensions()
    {
        uint32_t extensionCount = 0;
        XrCheck(xrEnumerateInstanceExtensionProperties(nullptr, 0, &extensionCount, nullptr), "xrEnumerateInstanceExtensionProperties(count)");
        std::vector<XrExtensionProperties> extensionProperties = MakeXrStructVector<XrExtensionProperties>(extensionCount, XR_TYPE_EXTENSION_PROPERTIES);
        XrCheck(
            xrEnumerateInstanceExtensionProperties(nullptr, extensionCount, &extensionCount, extensionProperties.data()),
            "xrEnumerateInstanceExtensionProperties");

        const char* desiredExtensions[] = {
            XR_KHR_ANDROID_CREATE_INSTANCE_EXTENSION_NAME,
            XR_KHR_OPENGL_ES_ENABLE_EXTENSION_NAME,
        };

        std::vector<const char*> enabledExtensions;
        for (const char* desired : desiredExtensions)
        {
            const auto found = std::find_if(
                extensionProperties.begin(),
                extensionProperties.end(),
                [desired](const XrExtensionProperties& properties) {
                    return std::strcmp(properties.extensionName, desired) == 0;
                });
            if (found != extensionProperties.end())
            {
                enabledExtensions.push_back(desired);
            }
        }

        if (enabledExtensions.size() != std::size(desiredExtensions))
        {
            throw std::runtime_error("Required OpenXR instance extensions are unavailable on this device.");
        }

        return enabledExtensions;
    }

    int64_t SelectSwapchainFormat(XrSession session, const std::array<int64_t, 2>& candidates)
    {
        uint32_t formatCount = 0;
        XrCheck(xrEnumerateSwapchainFormats(session, 0, &formatCount, nullptr), "xrEnumerateSwapchainFormats(count)");
        std::vector<int64_t> runtimeFormats(formatCount);
        XrCheck(
            xrEnumerateSwapchainFormats(session, formatCount, &formatCount, runtimeFormats.data()),
            "xrEnumerateSwapchainFormats");

        for (const auto candidate : candidates)
        {
            if (std::find(runtimeFormats.begin(), runtimeFormats.end(), candidate) != runtimeFormats.end())
            {
                return candidate;
            }
        }

        throw std::runtime_error("No compatible OpenXR swapchain format found.");
    }
}

namespace xr
{
    struct XrContextOpenXR : public IXrContextOpenXR
    {
        XrInstance InstanceHandle{XR_NULL_HANDLE};
        XrSystemId SystemId{XR_NULL_SYSTEM_ID};
        XrSession SessionHandle{XR_NULL_HANDLE};
        XrSpace ReferenceSpace{XR_NULL_HANDLE};
        XrSessionState SessionState{XR_SESSION_STATE_UNKNOWN};
        bool SessionRunning{false};
        bool Initialized{false};

        bool IsInitialized() const override
        {
            return Initialized;
        }

        XrInstance Instance() const override
        {
            return InstanceHandle;
        }

        XrSession Session() const override
        {
            return SessionHandle;
        }

        ~XrContextOpenXR() override = default;
    };

    namespace
    {
        // OpenXR runtimes (including Quest's) do not support multiple concurrently-live
        // XrInstance handles in one process. Babylon probes availability via the static
        // System::IsSessionSupportedAsync before the "real" session is ever created, so
        // every System::Impl must share a single process-wide instance/system/session
        // rather than each creating its own.
        std::shared_ptr<XrContextOpenXR>& GlobalXrContext()
        {
            static std::shared_ptr<XrContextOpenXR> context{std::make_shared<XrContextOpenXR>()};
            return context;
        }
    }

    struct System::Impl
    {
        std::shared_ptr<XrContextOpenXR> XrContext{GlobalXrContext()};
        std::string ApplicationName;

        explicit Impl(const std::string& applicationName)
            : ApplicationName{applicationName.empty() ? "Loony Quest" : applicationName}
        {
        }

        bool IsInitialized() const
        {
            return XrContext->InstanceHandle != XR_NULL_HANDLE && XrContext->SystemId != XR_NULL_SYSTEM_ID;
        }

        bool TryInitialize()
        {
            if (IsInitialized())
            {
                return true;
            }

            InitializeLoader();

            const auto enabledExtensions = SelectEnabledInstanceExtensions();

            JavaVM* javaVm = nullptr;
            if (GetEnvForCurrentThread()->GetJavaVM(&javaVm) != JNI_OK || javaVm == nullptr)
            {
                throw std::runtime_error("Failed to obtain JavaVM for OpenXR instance creation.");
            }

            XrInstanceCreateInfoAndroidKHR instanceCreateInfoAndroid = MakeXrStruct<XrInstanceCreateInfoAndroidKHR>(XR_TYPE_INSTANCE_CREATE_INFO_ANDROID_KHR);
            instanceCreateInfoAndroid.applicationVM = javaVm;
            instanceCreateInfoAndroid.applicationActivity = ToPersistentGlobalRef(GetCurrentActivity());

            XrInstanceCreateInfo createInfo = MakeXrStruct<XrInstanceCreateInfo>(XR_TYPE_INSTANCE_CREATE_INFO);
            createInfo.next = &instanceCreateInfoAndroid;
            createInfo.enabledExtensionCount = static_cast<uint32_t>(enabledExtensions.size());
            createInfo.enabledExtensionNames = enabledExtensions.data();
            std::strncpy(createInfo.applicationInfo.applicationName, ApplicationName.c_str(), XR_MAX_APPLICATION_NAME_SIZE - 1);
            createInfo.applicationInfo.applicationVersion = 1;
            std::strncpy(createInfo.applicationInfo.engineName, "BabylonNative", XR_MAX_ENGINE_NAME_SIZE - 1);
            createInfo.applicationInfo.engineVersion = 1;
            createInfo.applicationInfo.apiVersion = XR_CURRENT_API_VERSION;

            XrCheck(xrCreateInstance(&createInfo, &XrContext->InstanceHandle), "xrCreateInstance");

            XrSystemGetInfo systemInfo = MakeXrStruct<XrSystemGetInfo>(XR_TYPE_SYSTEM_GET_INFO);
            systemInfo.formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;
            const XrResult systemResult = xrGetSystem(XrContext->InstanceHandle, &systemInfo, &XrContext->SystemId);
            if (systemResult == XR_ERROR_FORM_FACTOR_UNAVAILABLE)
            {
                return false;
            }
            XrCheck(systemResult, "xrGetSystem");

            return true;
        }
    };

    struct System::Session::Impl
    {
        struct Swapchain
        {
            XrSwapchain Handle{XR_NULL_HANDLE};
            int64_t Format{};
            uint32_t Width{};
            uint32_t Height{};
            uint32_t ArraySize{};
            std::vector<XrSwapchainImageOpenGLESKHR> Images{};
        };

        System::Impl& SystemImpl;
        std::shared_ptr<XrContextOpenXR> XrContext;
        std::function<void*()> WindowProvider;

        EGLDisplay Display{EGL_NO_DISPLAY};
        EGLConfig Config{};
        EGLContext Context{EGL_NO_CONTEXT};

        std::vector<Frame::View> ActiveFrameViews{};
        std::vector<Frame::InputSource> InputSources{};
        std::vector<FeaturePoint> FeaturePointCloud{};
        std::optional<Space> EyeTrackerSpace{};

        std::vector<XrView> CachedViews{};
        std::vector<XrViewConfigurationView> ViewConfigViews{};
        Swapchain ColorSwapchain{};
        Swapchain DepthSwapchain{};
        std::vector<XrCompositionLayerProjectionView> ProjectionLayerViews{};

        XrEnvironmentBlendMode BlendMode{XR_ENVIRONMENT_BLEND_MODE_OPAQUE};
        XrTime PredictedDisplayTime{};
        bool ShouldRender{false};
        bool SessionEndRequested{false};
        // Tracks whether BeginFrame() actually called xrBeginFrame() for the frame in
        // progress. xrEndFrame() must never be called without a matching xrBeginFrame() —
        // doing so violates the OpenXR frame-loop contract and has been observed to wedge
        // the Quest runtime's internal frame-pacing state machine (silent, CPU-idle hang).
        bool FrameBegun{false};

        float DepthNearZ{DEFAULT_DEPTH_NEAR_Z};
        float DepthFarZ{DEFAULT_DEPTH_FAR_Z};

        Impl(System::Impl& systemImpl, void* graphicsContext, std::function<void*()> windowProvider)
            : SystemImpl{systemImpl}
            , XrContext{systemImpl.XrContext}
            , WindowProvider{std::move(windowProvider)}
            , Context{reinterpret_cast<EGLContext>(graphicsContext)}
        {
        }

        ~Impl()
        {
            DestroySession();
        }

        void DestroySession()
        {
            if (!XrContext->Initialized)
            {
                return;
            }

            ColorSwapchain = {};
            DepthSwapchain = {};

            if (XrContext->ReferenceSpace != XR_NULL_HANDLE)
            {
                xrDestroySpace(XrContext->ReferenceSpace);
                XrContext->ReferenceSpace = XR_NULL_HANDLE;
            }

            if (XrContext->SessionHandle != XR_NULL_HANDLE)
            {
                if (XrContext->SessionRunning)
                {
                    xrEndSession(XrContext->SessionHandle);
                    XrContext->SessionRunning = false;
                }
                xrDestroySession(XrContext->SessionHandle);
                XrContext->SessionHandle = XR_NULL_HANDLE;
            }

            XrContext->Initialized = false;
        }

        void Initialize()
        {
            if (XrContext->Initialized)
            {
                return;
            }

            __android_log_print(ANDROID_LOG_INFO, LOG_TAG, "Session::Impl::Initialize starting");

            if (!SystemImpl.IsInitialized() && !SystemImpl.TryInitialize())
            {
                throw std::runtime_error("OpenXR system is not initialized.");
            }

            InitializeEglFromCurrentContext();
            __android_log_print(ANDROID_LOG_INFO, LOG_TAG, "InitializeEglFromCurrentContext done");
            CreateSession();
            __android_log_print(ANDROID_LOG_INFO, LOG_TAG, "CreateSession done");
            CreateReferenceSpace();
            InitializeViewConfiguration();
            CreateSwapchains();
            __android_log_print(ANDROID_LOG_INFO, LOG_TAG, "CreateSwapchains done, Initialize complete");

            XrContext->Initialized = true;
        }

        void InitializeEglFromCurrentContext()
        {
            Display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
            if (Display == EGL_NO_DISPLAY)
            {
                throw std::runtime_error("No default EGL display.");
            }

            EGLSurface currentDrawSurface = eglGetCurrentSurface(EGL_DRAW);
            if (currentDrawSurface == EGL_NO_SURFACE)
            {
                throw std::runtime_error("No current EGL draw surface.");
            }

            EGLint configId{};
            if (!eglQuerySurface(Display, currentDrawSurface, EGL_CONFIG_ID, &configId))
            {
                throw std::runtime_error("Failed to query EGL surface config id.");
            }

            EGLint numConfigs{};
            if (!eglGetConfigs(Display, nullptr, 0, &numConfigs))
            {
                throw std::runtime_error("Failed to enumerate EGL configs.");
            }

            std::vector<EGLConfig> configs(static_cast<size_t>(numConfigs));
            if (!eglGetConfigs(Display, configs.data(), numConfigs, &numConfigs))
            {
                throw std::runtime_error("Failed to get EGL configs.");
            }

            const auto configIt = std::find_if(configs.begin(), configs.end(), [&](const EGLConfig& candidate) {
                EGLint id{};
                if (!eglGetConfigAttrib(Display, candidate, EGL_CONFIG_ID, &id))
                {
                    return false;
                }
                return id == configId;
            });

            if (configIt == configs.end())
            {
                throw std::runtime_error("EGL config not found for current surface.");
            }

            Config = *configIt;
            if (Context == EGL_NO_CONTEXT)
            {
                Context = eglGetCurrentContext();
            }
        }

        void CreateSession()
        {
            PFN_xrGetOpenGLESGraphicsRequirementsKHR getOpenGLESGraphicsRequirements = nullptr;
            XrCheck(
                xrGetInstanceProcAddr(
                    XrContext->InstanceHandle,
                    "xrGetOpenGLESGraphicsRequirementsKHR",
                    reinterpret_cast<PFN_xrVoidFunction*>(&getOpenGLESGraphicsRequirements)),
                "xrGetInstanceProcAddr(xrGetOpenGLESGraphicsRequirementsKHR)");

            XrGraphicsRequirementsOpenGLESKHR graphicsRequirements = MakeXrStruct<XrGraphicsRequirementsOpenGLESKHR>(XR_TYPE_GRAPHICS_REQUIREMENTS_OPENGL_ES_KHR);
            XrCheck(
                getOpenGLESGraphicsRequirements(XrContext->InstanceHandle, XrContext->SystemId, &graphicsRequirements),
                "xrGetOpenGLESGraphicsRequirementsKHR");

            XrGraphicsBindingOpenGLESAndroidKHR graphicsBinding = MakeXrStruct<XrGraphicsBindingOpenGLESAndroidKHR>(XR_TYPE_GRAPHICS_BINDING_OPENGL_ES_ANDROID_KHR);
            graphicsBinding.display = Display;
            graphicsBinding.config = Config;
            graphicsBinding.context = Context;

            XrSessionCreateInfo sessionCreateInfo = MakeXrStruct<XrSessionCreateInfo>(XR_TYPE_SESSION_CREATE_INFO);
            sessionCreateInfo.next = &graphicsBinding;
            sessionCreateInfo.systemId = XrContext->SystemId;
            XrCheck(xrCreateSession(XrContext->InstanceHandle, &sessionCreateInfo, &XrContext->SessionHandle), "xrCreateSession");
        }

        void CreateReferenceSpace()
        {
            XrReferenceSpaceCreateInfo referenceSpaceCreateInfo = MakeXrStruct<XrReferenceSpaceCreateInfo>(XR_TYPE_REFERENCE_SPACE_CREATE_INFO);
            referenceSpaceCreateInfo.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_LOCAL;
            referenceSpaceCreateInfo.poseInReferenceSpace = IDENTITY_POSE;
            XrCheck(
                xrCreateReferenceSpace(XrContext->SessionHandle, &referenceSpaceCreateInfo, &XrContext->ReferenceSpace),
                "xrCreateReferenceSpace");
        }

        void InitializeViewConfiguration()
        {
            uint32_t viewCount = 0;
            XrCheck(
                xrEnumerateViewConfigurationViews(
                    XrContext->InstanceHandle,
                    XrContext->SystemId,
                    XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO,
                    0,
                    &viewCount,
                    nullptr),
                "xrEnumerateViewConfigurationViews(count)");
            ViewConfigViews = MakeXrStructVector<XrViewConfigurationView>(viewCount, XR_TYPE_VIEW_CONFIGURATION_VIEW);
            XrCheck(
                xrEnumerateViewConfigurationViews(
                    XrContext->InstanceHandle,
                    XrContext->SystemId,
                    XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO,
                    viewCount,
                    &viewCount,
                    ViewConfigViews.data()),
                "xrEnumerateViewConfigurationViews");

            CachedViews = MakeXrStructVector<XrView>(viewCount, XR_TYPE_VIEW);
            ProjectionLayerViews = MakeXrStructVector<XrCompositionLayerProjectionView>(viewCount, XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW);
            ActiveFrameViews.assign(viewCount, {});

            uint32_t blendModeCount = 0;
            XrCheck(
                xrEnumerateEnvironmentBlendModes(
                    XrContext->InstanceHandle,
                    XrContext->SystemId,
                    XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO,
                    0,
                    &blendModeCount,
                    nullptr),
                "xrEnumerateEnvironmentBlendModes(count)");
            std::vector<XrEnvironmentBlendMode> blendModes(blendModeCount);
            XrCheck(
                xrEnumerateEnvironmentBlendModes(
                    XrContext->InstanceHandle,
                    XrContext->SystemId,
                    XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO,
                    blendModeCount,
                    &blendModeCount,
                    blendModes.data()),
                "xrEnumerateEnvironmentBlendModes");
            BlendMode = blendModes.empty() ? XR_ENVIRONMENT_BLEND_MODE_OPAQUE : blendModes.front();
        }

        void CreateSwapchain(
            int64_t format,
            uint32_t width,
            uint32_t height,
            uint32_t arraySize,
            XrSwapchainUsageFlags usageFlags,
            Swapchain& swapchain)
        {
            swapchain.Format = format;
            swapchain.Width = width;
            swapchain.Height = height;
            swapchain.ArraySize = arraySize;

            XrSwapchainCreateInfo swapchainCreateInfo = MakeXrStruct<XrSwapchainCreateInfo>(XR_TYPE_SWAPCHAIN_CREATE_INFO);
            swapchainCreateInfo.usageFlags = usageFlags;
            swapchainCreateInfo.format = format;
            swapchainCreateInfo.sampleCount = 1;
            swapchainCreateInfo.width = width;
            swapchainCreateInfo.height = height;
            swapchainCreateInfo.faceCount = 1;
            swapchainCreateInfo.arraySize = arraySize;
            swapchainCreateInfo.mipCount = 1;
            XrCheck(xrCreateSwapchain(XrContext->SessionHandle, &swapchainCreateInfo, &swapchain.Handle), "xrCreateSwapchain");

            uint32_t imageCount = 0;
            XrCheck(xrEnumerateSwapchainImages(swapchain.Handle, 0, &imageCount, nullptr), "xrEnumerateSwapchainImages(count)");
            swapchain.Images = MakeXrStructVector<XrSwapchainImageOpenGLESKHR>(imageCount, XR_TYPE_SWAPCHAIN_IMAGE_OPENGL_ES_KHR);
            XrCheck(
                xrEnumerateSwapchainImages(
                    swapchain.Handle,
                    imageCount,
                    &imageCount,
                    reinterpret_cast<XrSwapchainImageBaseHeader*>(swapchain.Images.data())),
                "xrEnumerateSwapchainImages");
        }

        void CreateSwapchains()
        {
            const auto& viewConfig = ViewConfigViews.front();
            const uint32_t viewCount = static_cast<uint32_t>(CachedViews.size());
            const int64_t colorFormat = SelectSwapchainFormat(XrContext->SessionHandle, SUPPORTED_COLOR_FORMATS);
            const int64_t depthFormat = SelectSwapchainFormat(XrContext->SessionHandle, SUPPORTED_DEPTH_FORMATS);

            CreateSwapchain(
                colorFormat,
                viewConfig.recommendedImageRectWidth,
                viewConfig.recommendedImageRectHeight,
                viewCount,
                XR_SWAPCHAIN_USAGE_SAMPLED_BIT | XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT,
                ColorSwapchain);
            CreateSwapchain(
                depthFormat,
                viewConfig.recommendedImageRectWidth,
                viewConfig.recommendedImageRectHeight,
                viewCount,
                XR_SWAPCHAIN_USAGE_SAMPLED_BIT | XR_SWAPCHAIN_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                DepthSwapchain);
        }

        void ProcessSessionStateChanged(const XrEventDataSessionStateChanged& stateChanged)
        {
            __android_log_print(ANDROID_LOG_INFO, LOG_TAG, "Session state changed to %d", static_cast<int>(stateChanged.state));
            XrContext->SessionState = stateChanged.state;
            switch (stateChanged.state)
            {
                case XR_SESSION_STATE_READY:
                {
                    XrSessionBeginInfo beginInfo = MakeXrStruct<XrSessionBeginInfo>(XR_TYPE_SESSION_BEGIN_INFO);
                    beginInfo.primaryViewConfigurationType = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;
                    XrCheck(xrBeginSession(XrContext->SessionHandle, &beginInfo), "xrBeginSession");
                    XrContext->SessionRunning = true;
                    __android_log_print(ANDROID_LOG_INFO, LOG_TAG, "xrBeginSession succeeded, session running");
                    break;
                }
                case XR_SESSION_STATE_STOPPING:
                    XrCheck(xrEndSession(XrContext->SessionHandle), "xrEndSession");
                    XrContext->SessionRunning = false;
                    break;
                default:
                    break;
            }
        }

        void PollEvents(bool& shouldEndSession, bool& shouldRestartSession)
        {
            shouldEndSession = false;
            shouldRestartSession = false;

            XrEventDataBuffer eventData = MakeXrStruct<XrEventDataBuffer>(XR_TYPE_EVENT_DATA_BUFFER);
            while (true)
            {
                const XrResult pollResult = xrPollEvent(XrContext->InstanceHandle, &eventData);
                if (pollResult == XR_EVENT_UNAVAILABLE)
                {
                    break;
                }
                XrCheck(pollResult, "xrPollEvent");

                switch (eventData.type)
                {
                    case XR_TYPE_EVENT_DATA_INSTANCE_LOSS_PENDING:
                        shouldEndSession = true;
                        shouldRestartSession = false;
                        return;
                    case XR_TYPE_EVENT_DATA_SESSION_STATE_CHANGED:
                    {
                        const auto& stateChanged = *reinterpret_cast<XrEventDataSessionStateChanged*>(&eventData);
                        if (stateChanged.session == XrContext->SessionHandle)
                        {
                            ProcessSessionStateChanged(stateChanged);
                            if (stateChanged.state == XR_SESSION_STATE_EXITING)
                            {
                                shouldEndSession = true;
                                shouldRestartSession = false;
                            }
                            else if (stateChanged.state == XR_SESSION_STATE_LOSS_PENDING)
                            {
                                shouldEndSession = true;
                                shouldRestartSession = true;
                            }
                        }
                        break;
                    }
                    default:
                        break;
                }

                eventData = MakeXrStruct<XrEventDataBuffer>(XR_TYPE_EVENT_DATA_BUFFER);
            }

            if (SessionEndRequested)
            {
                xrRequestExitSession(XrContext->SessionHandle);
                SessionEndRequested = false;
            }
        }

        void LocateViews()
        {
            XrViewState viewState = MakeXrStruct<XrViewState>(XR_TYPE_VIEW_STATE);
            XrViewLocateInfo locateInfo = MakeXrStruct<XrViewLocateInfo>(XR_TYPE_VIEW_LOCATE_INFO);
            locateInfo.viewConfigurationType = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;
            locateInfo.displayTime = PredictedDisplayTime;
            locateInfo.space = XrContext->ReferenceSpace;

            uint32_t viewCount = 0;
            XrCheck(
                xrLocateViews(
                    XrContext->SessionHandle,
                    &locateInfo,
                    &viewState,
                    static_cast<uint32_t>(CachedViews.size()),
                    &viewCount,
                    CachedViews.data()),
                "xrLocateViews");
            assert(viewCount == CachedViews.size());

            // Tracking isn't guaranteed to be valid yet (e.g. the first frame or two after
            // xrBeginSession), and the runtime is not required to overwrite pose fields when
            // it isn't: submitting the resulting {0,0,0,0} orientation (not a unit quaternion)
            // to xrEndFrame is rejected with XR_ERROR_LAYER_INVALID. Fall back to identity.
            const bool orientationValid = (viewState.viewStateFlags & XR_VIEW_STATE_ORIENTATION_VALID_BIT) != 0;
            const bool positionValid = (viewState.viewStateFlags & XR_VIEW_STATE_POSITION_VALID_BIT) != 0;
            if (!orientationValid || !positionValid)
            {
                for (auto& view : CachedViews)
                {
                    if (!orientationValid)
                    {
                        view.pose.orientation = IDENTITY_POSE.orientation;
                    }
                    if (!positionValid)
                    {
                        view.pose.position = IDENTITY_POSE.position;
                    }
                }
            }
        }

        void PopulateViews()
        {
            const uint32_t colorImageIndex = AcquireAndWaitForSwapchainImage(ColorSwapchain.Handle);
            const uint32_t depthImageIndex = AcquireAndWaitForSwapchainImage(DepthSwapchain.Handle);
            const XrRect2Di imageRect{{0, 0}, {static_cast<int32_t>(ColorSwapchain.Width), static_cast<int32_t>(ColorSwapchain.Height)}};

            for (uint32_t viewIndex = 0; viewIndex < CachedViews.size(); ++viewIndex)
            {
                auto& view = ActiveFrameViews[viewIndex];
                const auto& cachedView = CachedViews[viewIndex];

                view.Space.Pose.Position.X = cachedView.pose.position.x;
                view.Space.Pose.Position.Y = cachedView.pose.position.y;
                view.Space.Pose.Position.Z = cachedView.pose.position.z;
                view.Space.Pose.Orientation.X = cachedView.pose.orientation.x;
                view.Space.Pose.Orientation.Y = cachedView.pose.orientation.y;
                view.Space.Pose.Orientation.Z = cachedView.pose.orientation.z;
                view.Space.Pose.Orientation.W = cachedView.pose.orientation.w;

                view.ColorTextureFormat = SwapchainFormatToTextureFormat(ColorSwapchain.Format);
                view.ColorTexturePointer = reinterpret_cast<void*>(static_cast<uintptr_t>(ColorSwapchain.Images[colorImageIndex].image));
                view.ColorTextureSize = {ColorSwapchain.Width, ColorSwapchain.Height, ColorSwapchain.ArraySize};

                view.DepthTextureFormat = SwapchainFormatToTextureFormat(DepthSwapchain.Format);
                view.DepthTexturePointer = reinterpret_cast<void*>(static_cast<uintptr_t>(DepthSwapchain.Images[depthImageIndex].image));
                view.DepthTextureSize = {DepthSwapchain.Width, DepthSwapchain.Height, DepthSwapchain.ArraySize};

                view.DepthNearZ = DepthNearZ;
                view.DepthFarZ = DepthFarZ;
                view.RequiresAppClear = true;
                view.IsFirstPersonObserver = false;

                PopulateProjectionMatrix(cachedView, DepthNearZ, DepthFarZ, view.ProjectionMatrix);

                auto& projectionLayerView = ProjectionLayerViews[viewIndex];
                projectionLayerView = MakeXrStruct<XrCompositionLayerProjectionView>(XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW);
                projectionLayerView.pose = cachedView.pose;
                projectionLayerView.fov = cachedView.fov;
                projectionLayerView.subImage.swapchain = ColorSwapchain.Handle;
                projectionLayerView.subImage.imageRect = imageRect;
                projectionLayerView.subImage.imageArrayIndex = viewIndex;
            }
        }

        void BeginFrame()
        {
            FrameBegun = false;

            if (!XrContext->SessionRunning)
            {
                ShouldRender = false;
                return;
            }

            XrFrameWaitInfo waitInfo = MakeXrStruct<XrFrameWaitInfo>(XR_TYPE_FRAME_WAIT_INFO);
            XrFrameState frameState = MakeXrStruct<XrFrameState>(XR_TYPE_FRAME_STATE);
            XrCheck(xrWaitFrame(XrContext->SessionHandle, &waitInfo, &frameState), "xrWaitFrame");
            ShouldRender = frameState.shouldRender;
            PredictedDisplayTime = frameState.predictedDisplayTime;

            XrFrameBeginInfo beginInfo = MakeXrStruct<XrFrameBeginInfo>(XR_TYPE_FRAME_BEGIN_INFO);
            XrCheck(xrBeginFrame(XrContext->SessionHandle, &beginInfo), "xrBeginFrame");
            FrameBegun = true;

            if (ShouldRender && XrContext->SessionRunning)
            {
                LocateViews();
                PopulateViews();
            }
        }

        void EndFrame()
        {
            // xrEndFrame() must be paired 1:1 with a preceding xrBeginFrame(); calling it
            // when BeginFrame() bailed out early (session not yet running) is an OpenXR
            // contract violation that can hang the Quest runtime's frame-pacing state.
            if (!FrameBegun)
            {
                return;
            }
            FrameBegun = false;

            XrFrameEndInfo endInfo = MakeXrStruct<XrFrameEndInfo>(XR_TYPE_FRAME_END_INFO);
            endInfo.displayTime = PredictedDisplayTime;
            endInfo.environmentBlendMode = BlendMode;

            XrCompositionLayerProjection layer = MakeXrStruct<XrCompositionLayerProjection>(XR_TYPE_COMPOSITION_LAYER_PROJECTION);
            const XrCompositionLayerBaseHeader* layers[1]{};

            if (ShouldRender && XrContext->SessionRunning)
            {
                // The app has finished rendering into the images PopulateViews() acquired
                // via xrAcquireSwapchainImage/xrWaitSwapchainImage; they must be released
                // before being referenced by a composition layer submitted to xrEndFrame,
                // or the runtime rejects the layer as invalid.
                XrSwapchainImageReleaseInfo releaseInfo = MakeXrStruct<XrSwapchainImageReleaseInfo>(XR_TYPE_SWAPCHAIN_IMAGE_RELEASE_INFO);
                XrCheck(xrReleaseSwapchainImage(ColorSwapchain.Handle, &releaseInfo), "xrReleaseSwapchainImage(color)");
                XrCheck(xrReleaseSwapchainImage(DepthSwapchain.Handle, &releaseInfo), "xrReleaseSwapchainImage(depth)");

                layer.space = XrContext->ReferenceSpace;
                layer.viewCount = static_cast<uint32_t>(ProjectionLayerViews.size());
                layer.views = ProjectionLayerViews.data();
                layers[0] = reinterpret_cast<const XrCompositionLayerBaseHeader*>(&layer);
                endInfo.layerCount = 1;
                endInfo.layers = layers;
            }
            else
            {
                endInfo.layerCount = 0;
                endInfo.layers = nullptr;
            }

            XrCheck(xrEndFrame(XrContext->SessionHandle, &endInfo), "xrEndFrame");
        }

        std::unique_ptr<Session::Frame> GetNextFrame(
            bool& shouldEndSession,
            bool& shouldRestartSession,
            std::function<arcana::task<void, std::exception_ptr>(void*)> /*deletedTextureAsyncCallback*/)
        {
            if (!XrContext->Initialized)
            {
                Initialize();
            }

            // Pump events until the session reaches a runnable state (or exits).
            for (int attempt = 0; attempt < 100 && !XrContext->SessionRunning; ++attempt)
            {
                bool ignoredEnd = false;
                bool ignoredRestart = false;
                PollEvents(ignoredEnd, ignoredRestart);
                if (ignoredEnd)
                {
                    shouldEndSession = true;
                    shouldRestartSession = ignoredRestart;
                    return nullptr;
                }
                if (XrContext->SessionRunning)
                {
                    break;
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(16));
            }

            PollEvents(shouldEndSession, shouldRestartSession);
            if (shouldEndSession)
            {
                return nullptr;
            }

            if (!XrContext->SessionRunning)
            {
                __android_log_print(ANDROID_LOG_WARN, LOG_TAG, "GetNextFrame proceeding without a running session (state=%d)", static_cast<int>(XrContext->SessionState));
            }

            BeginFrame();
            return std::make_unique<Session::Frame>(*this);
        }

        void RequestEndSession()
        {
            SessionEndRequested = true;
        }

        void ClearSwapchainViews(const float color[4]) const
        {
            if (!ShouldRender || !XrContext->SessionRunning)
            {
                return;
            }

            for (uint32_t viewIndex = 0; viewIndex < ActiveFrameViews.size(); ++viewIndex)
            {
                const auto& view = ActiveFrameViews[viewIndex];
                const auto colorTexture = static_cast<GLuint>(reinterpret_cast<uintptr_t>(view.ColorTexturePointer));
                const auto depthTexture = static_cast<GLuint>(reinterpret_cast<uintptr_t>(view.DepthTexturePointer));

                GLuint framebuffer{};
                glGenFramebuffers(1, &framebuffer);
                glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
                glFramebufferTextureLayer(
                    GL_FRAMEBUFFER,
                    GL_COLOR_ATTACHMENT0,
                    colorTexture,
                    0,
                    static_cast<GLint>(viewIndex));
                glFramebufferTextureLayer(
                    GL_FRAMEBUFFER,
                    GL_DEPTH_ATTACHMENT,
                    depthTexture,
                    0,
                    static_cast<GLint>(viewIndex));

                glViewport(0, 0, static_cast<GLsizei>(view.ColorTextureSize.Width), static_cast<GLsizei>(view.ColorTextureSize.Height));
                glDisable(GL_SCISSOR_TEST);
                glDisable(GL_BLEND);
                glEnable(GL_DEPTH_TEST);
                glClearColor(color[0], color[1], color[2], color[3]);
                glClearDepthf(1.f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                glBindFramebuffer(GL_FRAMEBUFFER, 0);
                glDeleteFramebuffers(1, &framebuffer);
            }
        }
    };

    struct System::Session::Frame::Impl
    {
        Session::Impl& SessionImpl;

        explicit Impl(Session::Impl& sessionImpl)
            : SessionImpl{sessionImpl}
        {
        }
    };

    System::Session::Frame::Frame(Session::Impl& sessionImpl)
        : Views{sessionImpl.ActiveFrameViews}
        , InputSources{sessionImpl.InputSources}
        , FeaturePointCloud{sessionImpl.FeaturePointCloud}
        , EyeTrackerSpace{sessionImpl.EyeTrackerSpace}
        , UpdatedSceneObjects{}
        , RemovedSceneObjects{}
        , UpdatedPlanes{}
        , RemovedPlanes{}
        , UpdatedMeshes{}
        , RemovedMeshes{}
        , UpdatedImageTrackingResults{}
        , IsTracking{sessionImpl.XrContext->SessionRunning}
        , m_impl{std::make_unique<Session::Frame::Impl>(sessionImpl)}
    {
    }

    System::Session::Frame::~Frame() {}

    void System::Session::Frame::GetHitTestResults(std::vector<HitResult>&, Ray, HitTestTrackableType) const {}

    Anchor System::Session::Frame::CreateAnchor(Pose, NativeTrackablePtr) const
    {
        throw std::runtime_error("Anchors are not supported on the Android OpenXR backend yet.");
    }

    Anchor System::Session::Frame::DeclareAnchor(NativeAnchorPtr) const
    {
        throw std::runtime_error("Anchors are not supported on the Android OpenXR backend yet.");
    }

    void System::Session::Frame::UpdateAnchor(Anchor&) const {}

    void System::Session::Frame::DeleteAnchor(Anchor&) const {}

    System::Session::Frame::SceneObject& System::Session::Frame::GetSceneObjectByID(System::Session::Frame::SceneObject::Identifier) const
    {
        throw std::runtime_error("Scene object detection is not supported on the Android OpenXR backend.");
    }

    System::Session::Frame::Plane& System::Session::Frame::GetPlaneByID(System::Session::Frame::Plane::Identifier) const
    {
        throw std::runtime_error("Plane detection is not supported on the Android OpenXR backend.");
    }

    System::Session::Frame::Mesh& System::Session::Frame::GetMeshByID(System::Session::Frame::Mesh::Identifier) const
    {
        throw std::runtime_error("Mesh detection is not supported on the Android OpenXR backend.");
    }

    System::Session::Frame::ImageTrackingResult& System::Session::Frame::GetImageTrackingResultByID(
        System::Session::Frame::ImageTrackingResult::Identifier) const
    {
        throw std::runtime_error("Image tracking is not supported on the Android OpenXR backend.");
    }

    void System::Session::Frame::Render()
    {
        m_impl->SessionImpl.EndFrame();
    }

    System::System(const char* appName)
        : m_impl{std::make_unique<System::Impl>(appName ? appName : "")}
    {
    }

    System::~System() = default;

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
        if (sessionType != SessionType::IMMERSIVE_VR)
        {
            return arcana::task_from_result<std::exception_ptr>(false);
        }

        try
        {
            System probe("Loony Quest");
            return arcana::task_from_result<std::exception_ptr>(probe.TryInitialize());
        }
        catch (...)
        {
            return arcana::task_from_result<std::exception_ptr>(false);
        }
    }

    uintptr_t System::GetNativeXrContext()
    {
        return reinterpret_cast<uintptr_t>(m_impl->XrContext.get());
    }

    std::string System::GetNativeXrContextType()
    {
        return "OpenXR";
    }

    arcana::task<std::shared_ptr<System::Session>, std::exception_ptr> System::Session::CreateAsync(
        System& system,
        void* graphicsDevice,
        void* commandQueue,
        std::function<void*()> windowProvider)
    {
        (void)commandQueue;
        return arcana::task_from_result<std::exception_ptr>(
            std::make_shared<System::Session>(system, graphicsDevice, commandQueue, std::move(windowProvider)));
    }

    System::Session::Session(System& system, void* graphicsDevice, void*, std::function<void*()> windowProvider)
        : m_impl{std::make_unique<System::Session::Impl>(*system.m_impl, graphicsDevice, std::move(windowProvider))}
    {
    }

    System::Session::~Session() = default;

    std::unique_ptr<System::Session::Frame> System::Session::GetNextFrame(
        bool& shouldEndSession,
        bool& shouldRestartSession,
        std::function<arcana::task<void, std::exception_ptr>(void*)> deletedTextureAsyncCallback)
    {
        return m_impl->GetNextFrame(shouldEndSession, shouldRestartSession, std::move(deletedTextureAsyncCallback));
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

    void System::Session::SetPlaneDetectionEnabled(bool) const {}

    bool System::Session::TrySetFeaturePointCloudEnabled(bool) const
    {
        return false;
    }

    bool System::Session::TrySetPreferredPlaneDetectorOptions(const GeometryDetectorOptions&)
    {
        return false;
    }

    bool System::Session::TrySetMeshDetectorEnabled(const bool)
    {
        return false;
    }

    bool System::Session::TrySetPreferredMeshDetectorOptions(const GeometryDetectorOptions&)
    {
        return false;
    }

    std::vector<ImageTrackingScore>* System::Session::GetImageTrackingScores() const
    {
        return nullptr;
    }

    void System::Session::CreateAugmentedImageDatabase(const std::vector<ImageTrackingRequest>&) const {}
}
