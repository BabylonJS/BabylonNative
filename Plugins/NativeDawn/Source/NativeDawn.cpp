#include <Babylon/Plugins/NativeDawn.h>

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

#include <webgpu/webgpu.h>

// jsruntimehost's V8 N-API enqueues GC finalizers into napi_env__::pending_finalizers
// but, unlike real Node's event-loop-backed env, never drains that queue at runtime.
// NativeDawn creates many short-lived N-API wrappers per frame (one per WebGPU object:
// command encoders, passes, views, bind groups...), so their finalizers — and the
// wgpu handles they would release — accumulate unbounded until the process OOMs.
// When built against V8 we reach into the internal env to nudge GC and drain the queue
// (see PumpJsFinalizers). NATIVEDAWN_V8_FINALIZER_DRAIN is defined by CMake only when
// NAPI_JAVASCRIPT_ENGINE == V8; other engines finalize through their own mechanisms.
#if defined(NATIVEDAWN_V8_FINALIZER_DRAIN)
#include "js_native_api_v8.h"
#endif

#include <bimg/encode.h>
#include <bx/allocator.h>
#include <bx/error.h>
#include <bx/file.h>
#include <bx/filepath.h>

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <array>
#include <cmath>
#include <algorithm>
#include <cctype>
#include <filesystem>
#include <memory>
#include <string>
#include <string_view>
#include <functional>
#include <thread>
#include <utility>
#include <vector>

#include "CanvasDawn.h"

namespace Babylon::Plugins::NativeDawn
{
    // Defined in ImageDecode.cpp (bimg): encoded image bytes -> RGBA8.
    bool DecodeRGBA(const uint8_t* data, size_t size, std::vector<uint8_t>& out, int& width, int& height);

    namespace
    {
        // Global Dawn state for the experiment. Single window / single device.
        // Handles are raw C objects; ownership is tracked explicitly (see
        // WgpuOps / MakeExt below) rather than by the C++ wrapper's RAII.
        struct State
        {
            WGPUInstance instance = nullptr;
            WGPUAdapter adapter = nullptr;
            WGPUDevice device = nullptr;
            WGPUQueue queue = nullptr;
            WGPUSurface surface = nullptr;
            WGPUTextureFormat surfaceFormat = WGPUTextureFormat_BGRA8Unorm;
            WGPUTexture currentSurfaceTexture = nullptr;
            void* hwnd = nullptr;
            uint32_t width = 0;
            uint32_t height = 0;
            bool ready = false;
        };

        State g_state;

        std::string SvToStr(const WGPUStringView& sv)
        {
            if (sv.data == nullptr) return {};
            if (sv.length == static_cast<size_t>(-1)) return std::string(sv.data);
            return std::string(sv.data, sv.length);
        }

        // ---- Diagnostics -----------------------------------------------------
        // Everything NativeDawn reports -- Dawn validation/device errors included
        // -- goes through here so it lands in the same place as the rest of
        // Babylon Native's diagnostics: the JS console. Hosts already install a
        // Babylon::Polyfills::Console callback to capture engine output, so
        // routing through console.error/warn/log means Dawn's validation messages
        // show up in the app log, in the Playground's test output, and in any
        // host-side log sink, instead of vanishing onto raw stderr where nothing
        // is listening.
        //
        // Calling into JS is only safe on the JS thread with no exception in
        // flight, and Dawn can surface errors before the environment exists (for
        // example while the adapter is still being created). Whenever that is the
        // case we fall back to stderr so a message is never simply dropped.
        enum class LogLevel
        {
            Log,
            Warn,
            Error,
        };

        napi_env g_logEnv{};
        std::thread::id g_jsThreadId{};
        bool g_inLogCallback = false;

        void LogToStderr(LogLevel level, const char* msg)
        {
            const char* tag = level == LogLevel::Error ? "error" : (level == LogLevel::Warn ? "warning" : "info");
            std::fprintf(stderr, "[NativeDawn] %s: %s\n", tag, msg);
            std::fflush(stderr);
        }

        void DawnLog(LogLevel level, const std::string& message)
        {
            // Re-entrancy guard: console.* is JS, and anything it triggers could
            // fault back into Dawn and log again.
            if (g_logEnv == nullptr || std::this_thread::get_id() != g_jsThreadId || g_inLogCallback)
            {
                LogToStderr(level, message.c_str());
                return;
            }

            g_inLogCallback = true;
            bool delivered = false;
            try
            {
                Napi::Env env{g_logEnv};
                Napi::HandleScope scope{env};
                if (!env.IsExceptionPending())
                {
                    Napi::Value consoleV = env.Global().Get("console");
                    if (consoleV.IsObject())
                    {
                        const char* method = level == LogLevel::Error ? "error" : (level == LogLevel::Warn ? "warn" : "log");
                        Napi::Value fnV = consoleV.As<Napi::Object>().Get(method);
                        if (fnV.IsFunction())
                        {
                            fnV.As<Napi::Function>().Call(consoleV, {Napi::String::New(env, "[NativeDawn] " + message)});
                            delivered = true;
                        }
                    }
                }
            }
            catch (...)
            {
                delivered = false;
            }
            g_inLogCallback = false;

            if (!delivered)
            {
                LogToStderr(level, message.c_str());
            }
        }

        // printf-style convenience so the existing call sites stay readable.
        template<typename... Args>
        void DawnLogF(LogLevel level, const char* fmt, Args... args)
        {
            std::array<char, 2048> buf{};
            const int n = std::snprintf(buf.data(), buf.size(), fmt, args...);
            DawnLog(level, n < 0 ? fmt : std::string{buf.data(), static_cast<size_t>(std::min<int>(n, static_cast<int>(buf.size()) - 1))});
        }

        constexpr WGPUStringView EmptyStringView()
        {
            return {
                .data = nullptr,
                .length = WGPU_STRLEN,
            };
        }

        // Deferred GPU-resource destruction. Calling wgpu Destroy() the instant
        // Babylon disposes a buffer/texture/queryset can trigger a Dawn
        // "used in submit while destroyed" device error when an in-flight command
        // (or a render bundle recorded this frame) still references it. Those
        // errors accumulate over a long in-process validation run and eventually
        // wedge the device (every later test renders black). Defer the real
        // Destroy() a few frames so the GPU has retired the referencing work.
        std::vector<std::pair<int, std::function<void()>>> g_pendingDestroy;
        void DeferDestroy(std::function<void()> fn)
        {
            g_pendingDestroy.emplace_back(3, std::move(fn));
        }
        void FlushPendingDestroy()
        {
            size_t w = 0;
            for (size_t r = 0; r < g_pendingDestroy.size(); ++r)
            {
                if (--g_pendingDestroy[r].first <= 0) { g_pendingDestroy[r].second(); }
                else { g_pendingDestroy[w++] = std::move(g_pendingDestroy[r]); }
            }
            g_pendingDestroy.resize(w);
        }

        void LogDeviceError(WGPUDevice const*, WGPUErrorType type, WGPUStringView message, void*, void*)
        {
            const char* kind = "device error";
            switch (type)
            {
                case WGPUErrorType_Validation: kind = "validation error"; break;
                case WGPUErrorType_OutOfMemory: kind = "out-of-memory error"; break;
                case WGPUErrorType_Internal: kind = "internal error"; break;
                case WGPUErrorType_Unknown: kind = "unknown error"; break;
                default: break;
            }

            std::string text = std::string{kind} + ": " + SvToStr(message);

            // A broken pipeline usually re-raises the same error on every frame,
            // and each one costs a Diagnostics banner plus a native callstack, so
            // collapse consecutive duplicates and report the count once the error
            // changes. The first occurrence is always logged in full.
            static std::string lastText;
            static uint64_t repeats = 0;
            if (text == lastText)
            {
                ++repeats;
                return;
            }
            if (repeats > 0)
            {
                DawnLogF(LogLevel::Error, "previous error repeated %llu more time(s)",
                    static_cast<unsigned long long>(repeats));
            }
            lastText = text;
            repeats = 0;

            DawnLog(LogLevel::Error, text);
        }

        // Blocks the calling thread until `f` resolves. The C API has no
        // convenience overload, so the wait info has to be built explicitly.
        void WaitFuture(WGPUFuture f)
        {
            WGPUFutureWaitInfo wait{
                .future = f,
            };
            wgpuInstanceWaitAny(g_state.instance, 1, &wait, UINT64_MAX);
        }

        bool CreateDeviceAndSurface(void* window, uint32_t width, uint32_t height)
        {
            g_state.hwnd = window;
            g_state.width = width;
            g_state.height = height;

            // Instance with synchronous WaitAny support.
            static const auto kTimedWaitAny = WGPUInstanceFeatureName_TimedWaitAny;
            WGPUInstanceDescriptor instDesc{
                .requiredFeatureCount = 1,
                .requiredFeatures = &kTimedWaitAny,
            };
            g_state.instance = wgpuCreateInstance(&instDesc);
            if (!g_state.instance)
            {
                DawnLog(LogLevel::Error, "CreateInstance failed");
                return false;
            }

            // Blocks the calling thread until `f` resolves.
            auto waitFuture = [](WGPUFuture f) { WaitFuture(f); };

            // Adapter (high performance).
            WGPURequestAdapterOptions adapterOpts{
                .powerPreference = WGPUPowerPreference_HighPerformance,
            };
            WGPURequestAdapterCallbackInfo adapterCb{
                .mode = WGPUCallbackMode_WaitAnyOnly,
                .callback = [](WGPURequestAdapterStatus status, WGPUAdapter adapter,
                                WGPUStringView message, void* out, void*) {
                    if (status == WGPURequestAdapterStatus_Success)
                    {
                        // Takes ownership of the reference handed to the callback.
                        *static_cast<WGPUAdapter*>(out) = adapter;
                    }
                    else
                    {
                        DawnLog(LogLevel::Error, "RequestAdapter failed: " + SvToStr(message));
                    }
                },
                .userdata1 = &g_state.adapter,
            };
            waitFuture(wgpuInstanceRequestAdapter(g_state.instance, &adapterOpts, adapterCb));
            if (!g_state.adapter)
            {
                return false;
            }

            // Request every limit the adapter actually supports.
            //
            // WebGPU deliberately hands out a device with the *default* (i.e.
            // lowest-common-denominator) limits unless the caller opts in via
            // requiredLimits. Babylon.js reports the device limits up through
            // engine caps and builds render targets accordingly, so leaving
            // this unset silently caps us far below the hardware: a G-buffer of
            // RGBA32Float + RGBA16Float + RGBA16Float + RGBA8Unorm needs
            // maxColorAttachmentBytesPerSample >= 40, while the default is 32
            // and typical desktop adapters support 128. Dawn then rejects the
            // render pass, every command buffer built from it becomes invalid,
            // and the scene surfaces an opaque "Invalid argument" to JS.
            // Passing the adapter's own limits back as the required limits is
            // always valid (they are by definition supported) and is the
            // standard way to say "give me everything this adapter has".
            WGPULimits supportedLimits{WGPU_LIMITS_INIT};
            const bool haveLimits =
                wgpuAdapterGetLimits(g_state.adapter, &supportedLimits) == WGPUStatus_Success;
            if (!haveLimits)
            {
                DawnLog(LogLevel::Warn, "wgpuAdapterGetLimits failed -- falling back to default device limits");
            }
            else
            {
                // The chain belongs to the adapter query; requesting it back as
                // a required limit would ask Dawn to honour extension structs we
                // did not populate.
                supportedLimits.nextInChain = nullptr;
            }

            // Features are opt-in like limits, and this is the only device the JS
            // layer ever gets (requestDevice hands back this one), so anything not
            // requested here is permanently invisible to Babylon.js -- notably
            // float32-filterable, without which sampling an r32float texture with a
            // filtering sampler is a validation error.
            // Dawn's own extensions start at 0x00050000; several change device
            // semantics or need matching toggles, so keep to standard features.
            std::vector<WGPUFeatureName> requiredFeatures;
            {
                WGPUSupportedFeatures adapterFeatures{WGPU_SUPPORTED_FEATURES_INIT};
                wgpuAdapterGetFeatures(g_state.adapter, &adapterFeatures);
                for (size_t i = 0; i < adapterFeatures.featureCount; ++i)
                {
                    const WGPUFeatureName feature = adapterFeatures.features[i];
                    if (static_cast<uint32_t>(feature) < 0x00050000u)
                    {
                        requiredFeatures.push_back(feature);
                    }
                }
                wgpuSupportedFeaturesFreeMembers(adapterFeatures);
            }

            // Device.
            WGPUDeviceDescriptor devDesc{
                .label = EmptyStringView(),
                .requiredFeatureCount = requiredFeatures.size(),
                .requiredFeatures = requiredFeatures.empty() ? nullptr : requiredFeatures.data(),
                .requiredLimits = haveLimits ? &supportedLimits : nullptr,
                .defaultQueue = {
                    .label = EmptyStringView(),
                },
                .uncapturedErrorCallbackInfo = {
                    .callback = &LogDeviceError,
                },
            };

            // WARP (software D3D12) MSAA-resolve workaround.
            // On the software adapter used by CI runners (no discrete GPU),
            // Dawn's lazy resource clearing wipes an MSAA resolve target's
            // content *after* the resolve but *before* the next pass samples it
            // (the resolved subresource isn't flagged initialized on this path).
            // The net effect is that any FrameGraph doing MSAA-render -> resolve
            // -> post-process (e.g. "FrameGraph image processing") reads an empty
            // resolve target and produces a fully transparent/black frame, while
            // the exact same graph is correct on real GPUs. Forcing resources to
            // be eagerly cleared at creation marks them initialized up-front and
            // sidesteps the bad lazy clear. Scope it to software adapters so real
            // GPUs are unaffected. Detection is deliberately broad: Dawn does not
            // consistently report the CI WARP adapter as AdapterType::CPU, so we
            // also match the Microsoft vendor id (0x1414) and WARP / "Basic
            // Render Driver" device/description strings.
            WGPUAdapterInfo adapterInfo{
                .nextInChain = nullptr,
                .vendor = EmptyStringView(),
                .architecture = EmptyStringView(),
                .device = EmptyStringView(),
                .description = EmptyStringView(),
                .backendType = WGPUBackendType_Undefined,
            };
            wgpuAdapterGetInfo(g_state.adapter, &adapterInfo);

            auto svContains = [](WGPUStringView sv, const char* needle) -> bool {
                if (sv.data == nullptr || sv.length == 0) { return false; }
                std::string s(sv.data, sv.length);
                std::string n(needle);
                std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
                std::transform(n.begin(), n.end(), n.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
                return s.find(n) != std::string::npos;
            };
            const bool isSoftwareAdapter =
                adapterInfo.adapterType == WGPUAdapterType_CPU ||
                adapterInfo.vendorID == 0x1414 /* Microsoft (WARP) */ ||
                svContains(adapterInfo.device, "warp") ||
                svContains(adapterInfo.device, "basic render") ||
                svContains(adapterInfo.description, "warp") ||
                svContains(adapterInfo.description, "basic render");

            DawnLogF(LogLevel::Log, "adapter: device='%s' vendor='%s' vendorID=0x%x type=%d backend=%d software=%d",
                SvToStr(adapterInfo.device).c_str(),
                SvToStr(adapterInfo.vendor).c_str(),
                static_cast<unsigned>(adapterInfo.vendorID),
                static_cast<int>(adapterInfo.adapterType), static_cast<int>(adapterInfo.backendType),
                isSoftwareAdapter ? 1 : 0);

            // Strings in adapterInfo are owned by Dawn; the C API requires an
            // explicit free (the C++ wrapper did this in its destructor).
            wgpuAdapterInfoFreeMembers(adapterInfo);

            WGPUDawnTogglesDescriptor toggles{
                .chain = {
                    .sType = WGPUSType_DawnTogglesDescriptor,
                },
            };
            const char* kWarpWorkaround = "nonzero_clear_resources_on_creation_for_testing";
            if (isSoftwareAdapter)
            {
                toggles.enabledToggleCount = 1;
                toggles.enabledToggles = &kWarpWorkaround;
                devDesc.nextInChain = &toggles.chain;
                DawnLog(LogLevel::Warn, "software adapter detected -- enabling MSAA-resolve lazy-clear workaround");
            }
            WGPURequestDeviceCallbackInfo deviceCb{
                .mode = WGPUCallbackMode_WaitAnyOnly,
                .callback = [](WGPURequestDeviceStatus status, WGPUDevice device,
                                WGPUStringView message, void* out, void*) {
                    if (status == WGPURequestDeviceStatus_Success)
                    {
                        *static_cast<WGPUDevice*>(out) = device;
                    }
                    else
                    {
                        DawnLog(LogLevel::Error, "RequestDevice failed: " + SvToStr(message));
                    }
                },
                .userdata1 = &g_state.device,
            };
            waitFuture(wgpuAdapterRequestDevice(g_state.adapter, &devDesc, deviceCb));
            if (!g_state.device)
            {
                return false;
            }
            g_state.queue = wgpuDeviceGetQueue(g_state.device);

            // Surface from the native window (Win32 HWND).
#if defined(_WIN32)
            WGPUSurfaceSourceWindowsHWND chained{
                .chain = {
                    .sType = WGPUSType_SurfaceSourceWindowsHWND,
                },
                .hinstance = ::GetModuleHandle(nullptr),
                .hwnd = window,
            };
            WGPUSurfaceDescriptor surfDesc{
                .nextInChain = &chained.chain,
                .label = EmptyStringView(),
            };
            g_state.surface = wgpuInstanceCreateSurface(g_state.instance, &surfDesc);
#endif
            if (!g_state.surface)
            {
                DawnLog(LogLevel::Error, "CreateSurface failed");
                return false;
            }

            // Pick the preferred format and configure.
            WGPUSurfaceCapabilities caps{
                .nextInChain = nullptr,
            };
            wgpuSurfaceGetCapabilities(g_state.surface, g_state.adapter, &caps);
            if (caps.formatCount > 0)
            {
                g_state.surfaceFormat = caps.formats[0];
            }
            wgpuSurfaceCapabilitiesFreeMembers(caps);

            WGPUSurfaceConfiguration cfg{
                .device = g_state.device,
                .format = g_state.surfaceFormat,
                .usage = WGPUTextureUsage_RenderAttachment,
                .width = width,
                .height = height,
                .alphaMode = WGPUCompositeAlphaMode_Auto,
                .presentMode = WGPUPresentMode_Fifo,
            };
            wgpuSurfaceConfigure(g_state.surface, &cfg);

            g_state.ready = true;
            DawnLogF(LogLevel::Log, "Dawn device + surface ready (%ux%u, format=%d)",
                width, height, static_cast<int>(g_state.surfaceFormat));
            return true;
        }

        // Milestone test: clear the surface to a solid color via Dawn, no bgfx.
        void ClearToColor(float r, float g, float b)
        {
            if (!g_state.ready)
            {
                return;
            }

            WGPUSurfaceTexture st{
                .nextInChain = nullptr,
            };
            wgpuSurfaceGetCurrentTexture(g_state.surface, &st);
            if (!st.texture)
            {
                DawnLog(LogLevel::Error, "GetCurrentTexture: null");
                return;
            }

            WGPUTextureView view = wgpuTextureCreateView(st.texture, nullptr);

            WGPURenderPassColorAttachment color{
                .view = view,
                .depthSlice = WGPU_DEPTH_SLICE_UNDEFINED,
                .loadOp = WGPULoadOp_Clear,
                .storeOp = WGPUStoreOp_Store,
                .clearValue = {
                    .r = r,
                    .g = g,
                    .b = b,
                    .a = 1.0,
                },
            };

            WGPURenderPassDescriptor passDesc{
                .label = EmptyStringView(),
                .colorAttachmentCount = 1,
                .colorAttachments = &color,
            };

            WGPUCommandEncoder encoder = wgpuDeviceCreateCommandEncoder(g_state.device, nullptr);
            WGPURenderPassEncoder pass = wgpuCommandEncoderBeginRenderPass(encoder, &passDesc);
            wgpuRenderPassEncoderEnd(pass);
            WGPUCommandBuffer commands = wgpuCommandEncoderFinish(encoder, nullptr);
            wgpuQueueSubmit(g_state.queue, 1, &commands);
            wgpuSurfacePresent(g_state.surface);

            wgpuCommandBufferRelease(commands);
            wgpuRenderPassEncoderRelease(pass);
            wgpuCommandEncoderRelease(encoder);
            wgpuTextureViewRelease(view);
            wgpuTextureRelease(st.texture);
        }
    }

    // =========================================================================
    // WebGPU (navigator.gpu) bindings implemented over Dawn (wgpu C++).
    //
    // Each GPU object is a plain Napi::Object carrying a hidden "_h" External<T>
    // wrapping the wgpu handle and a "_type" tag string. Methods are attached as
    // Napi::Functions; they capture the owning handle by value (all wgpu methods
    // are const) and read handles out of arguments via GetH<T>(). Promises
    // resolve synchronously since the Dawn adapter/device already exist.
    // =========================================================================
    namespace
    {
        // ---- small JS helpers ------------------------------------------------
        inline bool IsNullish(Napi::Value v) { return v.IsUndefined() || v.IsNull(); }

        template <typename Fn>
        void SetMethod(Napi::Object obj, const char* name, Fn&& fn)
        {
            obj.Set(name, Napi::Function::New(obj.Env(), std::forward<Fn>(fn), name));
        }

        Napi::Object NewGPUObject(Napi::Env env, const char* type)
        {
            Napi::Object o = Napi::Object::New(env);
            o.Set("_type", Napi::String::New(env, type));
            return o;
        }

        // Count of GPU wrappers created since process start. Used to gate how
        // often we force GC + drain finalizers (see PumpJsFinalizers): dropped
        // wrappers are only reclaimed when V8 collects and their N-API finalizer
        // runs, and this host never drains finalizers on its own during the
        // native render loop.
        uint64_t g_wrappersCreated = 0;

#if defined(NATIVEDAWN_V8_FINALIZER_DRAIN)
        uint64_t g_wrappersAtLastPump = 0;
        // Force a GC (so dead wrappers' finalizers get enqueued) then drain the
        // napi finalizer queue (which this host otherwise never drains at
        // runtime). Bounded-frequency: only forces GC once enough wrappers have
        // been created since the last pump, so steady-state rendering isn't
        // penalised by a full GC every frame.
        void PumpJsFinalizers(Napi::Env env, bool force)
        {
            constexpr uint64_t kWrappersPerPump = 1500;
            if (!force && g_wrappersCreated - g_wrappersAtLastPump < kWrappersPerPump)
            {
                return;
            }
            g_wrappersAtLastPump = g_wrappersCreated;

            auto* e = reinterpret_cast<napi_env__*>(static_cast<napi_env>(env));
            if (e->isolate != nullptr)
            {
                // Runs GC and invokes first-pass weak callbacks, which enqueue
                // the dead wrappers' finalizers into pending_finalizers.
                e->isolate->LowMemoryNotification();
            }
            while (!e->pending_finalizers.empty())
            {
                v8impl::RefTracker* rt = *e->pending_finalizers.begin();
                e->pending_finalizers.erase(rt);
                rt->Finalize();
            }
        }
#else
        void PumpJsFinalizers(Napi::Env, bool) {}
#endif

        // The C API has no RAII: handles are raw pointers with manual
        // refcounting. WgpuOps<T> maps each object type to its AddRef/Release
        // pair so the N-API wrapper below can hold a strong reference for
        // exactly as long as the JS object is alive -- the same lifetime the
        // C++ wrapper types used to provide implicitly.
        template <typename T>
        struct WgpuOps;

#define NATIVEDAWN_WGPU_OPS(Type)                                              \
    template <>                                                                \
    struct WgpuOps<WGPU##Type>                                                 \
    {                                                                          \
        static void AddRef(WGPU##Type h) { if (h) wgpu##Type##AddRef(h); }     \
        static void Release(WGPU##Type h) { if (h) wgpu##Type##Release(h); }   \
    };

        NATIVEDAWN_WGPU_OPS(Adapter)
        NATIVEDAWN_WGPU_OPS(BindGroup)
        NATIVEDAWN_WGPU_OPS(BindGroupLayout)
        NATIVEDAWN_WGPU_OPS(Buffer)
        NATIVEDAWN_WGPU_OPS(CommandBuffer)
        NATIVEDAWN_WGPU_OPS(CommandEncoder)
        NATIVEDAWN_WGPU_OPS(ComputePassEncoder)
        NATIVEDAWN_WGPU_OPS(ComputePipeline)
        NATIVEDAWN_WGPU_OPS(Device)
        NATIVEDAWN_WGPU_OPS(Instance)
        NATIVEDAWN_WGPU_OPS(PipelineLayout)
        NATIVEDAWN_WGPU_OPS(QuerySet)
        NATIVEDAWN_WGPU_OPS(Queue)
        NATIVEDAWN_WGPU_OPS(RenderBundle)
        NATIVEDAWN_WGPU_OPS(RenderBundleEncoder)
        NATIVEDAWN_WGPU_OPS(RenderPassEncoder)
        NATIVEDAWN_WGPU_OPS(RenderPipeline)
        NATIVEDAWN_WGPU_OPS(Sampler)
        NATIVEDAWN_WGPU_OPS(ShaderModule)
        NATIVEDAWN_WGPU_OPS(Surface)
        NATIVEDAWN_WGPU_OPS(Texture)
        NATIVEDAWN_WGPU_OPS(TextureView)

#undef NATIVEDAWN_WGPU_OPS

        // Takes ownership of one reference to `h` and releases it when the JS
        // wrapper is finalized. WebGPU C creation functions return a handle the
        // caller owns, so `MakeExt(env, wgpuDeviceCreateX(...))` transfers that
        // reference directly. Wrapping a *borrowed* handle (one this code did
        // not just create, e.g. g_state.device) requires an explicit
        // WgpuOps<T>::AddRef at the call site.
        template <typename T>
        Napi::External<T> MakeExt(Napi::Env env, const T& h)
        {
            ++g_wrappersCreated;
            return Napi::External<T>::New(env, new T(h), [](Napi::Env, T* p) {
                WgpuOps<T>::Release(*p);
                delete p;
            });
        }

        template <typename T>
        void SetHandle(Napi::Object o, const T& h)
        {
            o.Set("_h", MakeExt<T>(o.Env(), h));
        }

        // Wraps a handle this code does not own (e.g. a long-lived g_state
        // object). MakeExt consumes a reference, so take one first.
        template <typename T>
        void SetBorrowedHandle(Napi::Object o, const T& h)
        {
            WgpuOps<T>::AddRef(h);
            SetHandle(o, h);
        }

        template <typename T>
        T* GetH(Napi::Value v)
        {
            if (!v.IsObject()) return nullptr;
            Napi::Object o = v.As<Napi::Object>();
            if (!o.Has("_h")) return nullptr;
            Napi::Value h = o.Get("_h");
            if (!h.IsExternal()) return nullptr;
            return h.As<Napi::External<T>>().Data();
        }

        // The C API takes strings as an explicit (data, length) pair rather
        // than a NUL-terminated pointer. The referenced storage must outlive
        // the descriptor it is attached to.
        WGPUStringView StrView(const std::string& s)
        {
            return WGPUStringView{
                .data = s.data(),
                .length = s.size(),
            };
        }

        std::string TypeTag(Napi::Value v)
        {
            if (!v.IsObject()) return {};
            Napi::Object o = v.As<Napi::Object>();
            if (!o.Has("_type")) return {};
            Napi::Value t = o.Get("_type");
            return t.IsString() ? t.As<Napi::String>().Utf8Value() : std::string{};
        }

        // ---- property readers ------------------------------------------------
        std::string PropStr(Napi::Object o, const char* k)
        {
            if (!o.Has(k)) return {};
            Napi::Value v = o.Get(k);
            if (!v.IsString()) return {};
            return v.As<Napi::String>().Utf8Value();
        }
        // Same, but substitutes `def` when the property is absent, not a string,
        // or empty -- so the caller reads the property exactly once.
        std::string PropStrOr(Napi::Object o, const char* k, const char* def)
        {
            std::string s = PropStr(o, k);
            return s.empty() ? std::string{def} : s;
        }
        uint32_t PropU32(Napi::Object o, const char* k, uint32_t def)
        {
            if (!o.Has(k)) return def;
            Napi::Value v = o.Get(k);
            if (!v.IsNumber()) return def;
            return v.As<Napi::Number>().Uint32Value();
        }
        uint64_t PropU64(Napi::Object o, const char* k, uint64_t def)
        {
            if (!o.Has(k)) return def;
            Napi::Value v = o.Get(k);
            if (!v.IsNumber()) return def;
            return static_cast<uint64_t>(v.As<Napi::Number>().Int64Value());
        }
        int32_t PropI32(Napi::Object o, const char* k, int32_t def)
        {
            if (!o.Has(k)) return def;
            Napi::Value v = o.Get(k);
            if (!v.IsNumber()) return def;
            return v.As<Napi::Number>().Int32Value();
        }
        double PropF64(Napi::Object o, const char* k, double def)
        {
            if (!o.Has(k)) return def;
            Napi::Value v = o.Get(k);
            if (!v.IsNumber()) return def;
            return v.As<Napi::Number>().DoubleValue();
        }
        bool PropBool(Napi::Object o, const char* k, bool def)
        {
            if (!o.Has(k)) return def;
            Napi::Value v = o.Get(k);
            if (!v.IsBoolean()) return def;
            return v.As<Napi::Boolean>().Value();
        }
        bool PropPresent(Napi::Object o, const char* k)
        {
            return o.Has(k) && !IsNullish(o.Get(k));
        }

        // ---- argument readers ------------------------------------------------
        bool ArgIsUndef(const Napi::CallbackInfo& info, size_t i)
        {
            return i >= info.Length() || IsNullish(info[i]);
        }
        uint32_t ArgU32(const Napi::CallbackInfo& info, size_t i, uint32_t def)
        {
            if (i >= info.Length() || !info[i].IsNumber()) return def;
            return info[i].As<Napi::Number>().Uint32Value();
        }
        uint64_t ArgU64(const Napi::CallbackInfo& info, size_t i, uint64_t def)
        {
            if (i >= info.Length() || !info[i].IsNumber()) return def;
            return static_cast<uint64_t>(info[i].As<Napi::Number>().Int64Value());
        }
        int32_t ArgI32(const Napi::CallbackInfo& info, size_t i, int32_t def)
        {
            if (i >= info.Length() || !info[i].IsNumber()) return def;
            return info[i].As<Napi::Number>().Int32Value();
        }
        double ArgF64(const Napi::CallbackInfo& info, size_t i, double def)
        {
            if (i >= info.Length() || !info[i].IsNumber()) return def;
            return info[i].As<Napi::Number>().DoubleValue();
        }

        // ---- byte access for ArrayBuffer / TypedArray / DataView -------------
        struct Bytes { uint8_t* data; size_t size; };
        Bytes GetBytes(Napi::Value v)
        {
            if (v.IsTypedArray())
            {
                Napi::TypedArray ta = v.As<Napi::TypedArray>();
                uint8_t* base = static_cast<uint8_t*>(ta.ArrayBuffer().Data());
                return { base + ta.ByteOffset(), ta.ByteLength() };
            }
            if (v.IsArrayBuffer())
            {
                Napi::ArrayBuffer ab = v.As<Napi::ArrayBuffer>();
                return { static_cast<uint8_t*>(ab.Data()), ab.ByteLength() };
            }
            if (v.IsDataView())
            {
                Napi::DataView dv = v.As<Napi::DataView>();
                uint8_t* base = static_cast<uint8_t*>(dv.ArrayBuffer().Data());
                return { base + dv.ByteOffset(), dv.ByteLength() };
            }
            return { nullptr, 0 };
        }

        // ---- base64 (used to turn object-URL blobs into universally-resolvable
        // data: URLs; see createObjectURL below) ------------------------------
        std::string Base64Encode(const uint8_t* data, size_t size)
        {
            static const char* T = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
            std::string out;
            out.reserve(((size + 2) / 3) * 4);
            size_t i = 0;
            for (; i + 2 < size; i += 3)
            {
                uint32_t b = (uint32_t(data[i]) << 16) | (uint32_t(data[i + 1]) << 8) | uint32_t(data[i + 2]);
                out.push_back(T[(b >> 18) & 63]);
                out.push_back(T[(b >> 12) & 63]);
                out.push_back(T[(b >> 6) & 63]);
                out.push_back(T[b & 63]);
            }
            if (i < size)
            {
                uint32_t b = uint32_t(data[i]) << 16;
                bool two = (i + 1 < size);
                if (two) b |= uint32_t(data[i + 1]) << 8;
                out.push_back(T[(b >> 18) & 63]);
                out.push_back(T[(b >> 12) & 63]);
                out.push_back(two ? T[(b >> 6) & 63] : '=');
                out.push_back('=');
            }
            return out;
        }

        bool Base64Decode(const char* data, size_t size, std::vector<uint8_t>& out)
        {
            static int8_t rev[256];
            static bool init = false;
            if (!init)
            {
                for (int i = 0; i < 256; ++i) rev[i] = -1;
                const char* T = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
                for (int i = 0; i < 64; ++i) rev[(uint8_t)T[i]] = (int8_t)i;
                init = true;
            }
            out.clear();
            out.reserve((size / 4) * 3);
            uint32_t buf = 0;
            int bits = 0;
            for (size_t i = 0; i < size; ++i)
            {
                uint8_t c = (uint8_t)data[i];
                if (c == '=' || c == '\n' || c == '\r' || c == ' ' || c == '\t') continue;
                int8_t v = rev[c];
                if (v < 0) return false;
                buf = (buf << 6) | (uint32_t)v;
                bits += 6;
                if (bits >= 8)
                {
                    bits -= 8;
                    out.push_back((uint8_t)((buf >> bits) & 0xFF));
                }
            }
            return true;
        }

        // IEEE-754 float32 -> float16 (half). Adequate for the [0,1] image data
        // that copyExternalImageToTexture converts into half-float textures.
        uint16_t FloatToHalf(float value)
        {
            uint32_t x;
            std::memcpy(&x, &value, sizeof(x));
            const uint32_t sign = (x >> 16) & 0x8000u;
            int32_t exponent = static_cast<int32_t>((x >> 23) & 0xFFu) - 127 + 15;
            const uint32_t mantissa = x & 0x7FFFFFu;
            if (exponent <= 0)
            {
                return static_cast<uint16_t>(sign);
            }
            if (exponent >= 31)
            {
                return static_cast<uint16_t>(sign | 0x7C00u);
            }
            return static_cast<uint16_t>(sign | (static_cast<uint32_t>(exponent) << 10) | (mantissa >> 13));
        }

        // ---- GPUExtent3D / GPUOrigin3D / GPUColor parsing --------------------
        WGPUExtent3D ParseExtent3D(Napi::Value v)
        {
            WGPUExtent3D e{
                .width = 1,
                .height = 1,
                .depthOrArrayLayers = 1,
            };
            if (v.IsArray())
            {
                Napi::Array a = v.As<Napi::Array>();
                if (a.Length() > 0 && a.Get(0u).IsNumber()) e.width = a.Get(0u).As<Napi::Number>().Uint32Value();
                if (a.Length() > 1 && a.Get(1u).IsNumber()) e.height = a.Get(1u).As<Napi::Number>().Uint32Value();
                if (a.Length() > 2 && a.Get(2u).IsNumber()) e.depthOrArrayLayers = a.Get(2u).As<Napi::Number>().Uint32Value();
            }
            else if (v.IsObject())
            {
                Napi::Object o = v.As<Napi::Object>();
                e.width = PropU32(o, "width", 1);
                e.height = PropU32(o, "height", 1);
                e.depthOrArrayLayers = PropU32(o, "depthOrArrayLayers", 1);
            }
            if (e.height == 0) e.height = 1;
            if (e.depthOrArrayLayers == 0) e.depthOrArrayLayers = 1;
            return e;
        }
        WGPUOrigin3D ParseOrigin3D(Napi::Value v)
        {
            WGPUOrigin3D o3{
                .x = 0,
                .y = 0,
                .z = 0,
            };
            if (v.IsArray())
            {
                Napi::Array a = v.As<Napi::Array>();
                if (a.Length() > 0 && a.Get(0u).IsNumber()) o3.x = a.Get(0u).As<Napi::Number>().Uint32Value();
                if (a.Length() > 1 && a.Get(1u).IsNumber()) o3.y = a.Get(1u).As<Napi::Number>().Uint32Value();
                if (a.Length() > 2 && a.Get(2u).IsNumber()) o3.z = a.Get(2u).As<Napi::Number>().Uint32Value();
            }
            else if (v.IsObject())
            {
                Napi::Object o = v.As<Napi::Object>();
                o3.x = PropU32(o, "x", 0);
                o3.y = PropU32(o, "y", 0);
                o3.z = PropU32(o, "z", 0);
            }
            return o3;
        }
        WGPUColor ParseColor(Napi::Value v)
        {
            WGPUColor c{
                .r = 0.0,
                .g = 0.0,
                .b = 0.0,
                .a = 0.0,
            };
            if (v.IsArray())
            {
                Napi::Array a = v.As<Napi::Array>();
                if (a.Length() > 0 && a.Get(0u).IsNumber()) c.r = a.Get(0u).As<Napi::Number>().DoubleValue();
                if (a.Length() > 1 && a.Get(1u).IsNumber()) c.g = a.Get(1u).As<Napi::Number>().DoubleValue();
                if (a.Length() > 2 && a.Get(2u).IsNumber()) c.b = a.Get(2u).As<Napi::Number>().DoubleValue();
                if (a.Length() > 3 && a.Get(3u).IsNumber()) c.a = a.Get(3u).As<Napi::Number>().DoubleValue();
            }
            else if (v.IsObject())
            {
                Napi::Object o = v.As<Napi::Object>();
                c.r = PropF64(o, "r", 0);
                c.g = PropF64(o, "g", 0);
                c.b = PropF64(o, "b", 0);
                c.a = PropF64(o, "a", 0);
            }
            return c;
        }

        // ---- enum string <-> WebGPU value mapping ----------------------------
        //
        // WebGPU spells its enums as strings on the JS side. Each enum gets one
        // table sorted by name so a lookup is a binary search over a const array
        // rather than a chain of string compares, and the names live in one
        // contiguous block of .rdata instead of being scattered across dozens of
        // comparison sites. Sortedness is asserted at compile time, so a
        // misplaced entry is a build error and not a silent lookup miss.
        //
        // The table and the search are deliberately untyped (int32_t, which every
        // WGPU enum fits -- they are all 32-bit thanks to their _Force32 member).
        // A template over the enum type would instantiate the whole binary search
        // once per enum, which costs more code than the if-chains it replaces.
        struct EnumMapping
        {
            std::string_view name;
            int32_t value;
        };

        constexpr bool IsSortedByName(const EnumMapping* table, size_t count)
        {
            for (size_t i = 1; i < count; ++i)
            {
                if (!(table[i - 1].name < table[i].name))
                {
                    return false;
                }
            }
            return true;
        }

        int32_t LookupEnumValue(const EnumMapping* table, size_t count,
            std::string_view name, int32_t fallback)
        {
            const EnumMapping* const last = table + count;
            const EnumMapping* it = std::lower_bound(table, last, name,
                [](const EnumMapping& e, std::string_view n) { return e.name < n; });
            return (it != last && it->name == name) ? it->value : fallback;
        }

        // Defines `Enum fn(std::string_view)` backed by a sorted table.
        // The table is passed as the trailing arguments so its internal commas
        // survive macro expansion.
#define NATIVEDAWN_ENUM_MAP(fn, Enum, fallback, ...)                           \
    constexpr EnumMapping k##fn##Table[] = __VA_ARGS__;                        \
    static_assert(IsSortedByName(k##fn##Table, std::size(k##fn##Table)),       \
        #fn " mapping table must be sorted by name");                          \
    static_assert(sizeof(Enum) == sizeof(int32_t), #fn " enum is not 32-bit"); \
    Enum fn(std::string_view s)                                                \
    {                                                                          \
        return Enum(LookupEnumValue(k##fn##Table, std::size(k##fn##Table), s,  \
            int32_t(fallback)));                                               \
    }


        NATIVEDAWN_ENUM_MAP(textureFormat, WGPUTextureFormat, WGPUTextureFormat_Undefined,
        {
            {"bc1-rgba-unorm",        WGPUTextureFormat_BC1RGBAUnorm},
            {"bc1-rgba-unorm-srgb",   WGPUTextureFormat_BC1RGBAUnormSrgb},
            {"bc2-rgba-unorm",        WGPUTextureFormat_BC2RGBAUnorm},
            {"bc2-rgba-unorm-srgb",   WGPUTextureFormat_BC2RGBAUnormSrgb},
            {"bc3-rgba-unorm",        WGPUTextureFormat_BC3RGBAUnorm},
            {"bc3-rgba-unorm-srgb",   WGPUTextureFormat_BC3RGBAUnormSrgb},
            {"bc4-r-snorm",           WGPUTextureFormat_BC4RSnorm},
            {"bc4-r-unorm",           WGPUTextureFormat_BC4RUnorm},
            {"bc5-rg-snorm",          WGPUTextureFormat_BC5RGSnorm},
            {"bc5-rg-unorm",          WGPUTextureFormat_BC5RGUnorm},
            {"bc6h-rgb-float",        WGPUTextureFormat_BC6HRGBFloat},
            {"bc6h-rgb-ufloat",       WGPUTextureFormat_BC6HRGBUfloat},
            {"bc7-rgba-unorm",        WGPUTextureFormat_BC7RGBAUnorm},
            {"bc7-rgba-unorm-srgb",   WGPUTextureFormat_BC7RGBAUnormSrgb},
            {"bgra8unorm",            WGPUTextureFormat_BGRA8Unorm},
            {"bgra8unorm-srgb",       WGPUTextureFormat_BGRA8UnormSrgb},
            {"depth16unorm",          WGPUTextureFormat_Depth16Unorm},
            {"depth24plus",           WGPUTextureFormat_Depth24Plus},
            {"depth24plus-stencil8",  WGPUTextureFormat_Depth24PlusStencil8},
            {"depth32float",          WGPUTextureFormat_Depth32Float},
            {"depth32float-stencil8", WGPUTextureFormat_Depth32FloatStencil8},
            {"r16float",              WGPUTextureFormat_R16Float},
            {"r16sint",               WGPUTextureFormat_R16Sint},
            {"r16uint",               WGPUTextureFormat_R16Uint},
            {"r32float",              WGPUTextureFormat_R32Float},
            {"r32sint",               WGPUTextureFormat_R32Sint},
            {"r32uint",               WGPUTextureFormat_R32Uint},
            {"r8sint",                WGPUTextureFormat_R8Sint},
            {"r8snorm",               WGPUTextureFormat_R8Snorm},
            {"r8uint",                WGPUTextureFormat_R8Uint},
            {"r8unorm",               WGPUTextureFormat_R8Unorm},
            {"rg11b10ufloat",         WGPUTextureFormat_RG11B10Ufloat},
            {"rg16float",             WGPUTextureFormat_RG16Float},
            {"rg16sint",              WGPUTextureFormat_RG16Sint},
            {"rg16uint",              WGPUTextureFormat_RG16Uint},
            {"rg32float",             WGPUTextureFormat_RG32Float},
            {"rg32sint",              WGPUTextureFormat_RG32Sint},
            {"rg32uint",              WGPUTextureFormat_RG32Uint},
            {"rg8sint",               WGPUTextureFormat_RG8Sint},
            {"rg8snorm",              WGPUTextureFormat_RG8Snorm},
            {"rg8uint",               WGPUTextureFormat_RG8Uint},
            {"rg8unorm",              WGPUTextureFormat_RG8Unorm},
            {"rgb10a2uint",           WGPUTextureFormat_RGB10A2Uint},
            {"rgb10a2unorm",          WGPUTextureFormat_RGB10A2Unorm},
            {"rgb9e5ufloat",          WGPUTextureFormat_RGB9E5Ufloat},
            {"rgba16float",           WGPUTextureFormat_RGBA16Float},
            {"rgba16sint",            WGPUTextureFormat_RGBA16Sint},
            {"rgba16uint",            WGPUTextureFormat_RGBA16Uint},
            {"rgba32float",           WGPUTextureFormat_RGBA32Float},
            {"rgba32sint",            WGPUTextureFormat_RGBA32Sint},
            {"rgba32uint",            WGPUTextureFormat_RGBA32Uint},
            {"rgba8sint",             WGPUTextureFormat_RGBA8Sint},
            {"rgba8snorm",            WGPUTextureFormat_RGBA8Snorm},
            {"rgba8uint",             WGPUTextureFormat_RGBA8Uint},
            {"rgba8unorm",            WGPUTextureFormat_RGBA8Unorm},
            {"rgba8unorm-srgb",       WGPUTextureFormat_RGBA8UnormSrgb},
            {"stencil8",              WGPUTextureFormat_Stencil8}
        })

        NATIVEDAWN_ENUM_MAP(vertexFormat, WGPUVertexFormat, WGPUVertexFormat_Float32,
        {
            {"float16x2", WGPUVertexFormat_Float16x2},
            {"float16x4", WGPUVertexFormat_Float16x4},
            {"float32",   WGPUVertexFormat_Float32},
            {"float32x2", WGPUVertexFormat_Float32x2},
            {"float32x3", WGPUVertexFormat_Float32x3},
            {"float32x4", WGPUVertexFormat_Float32x4},
            {"sint16x2",  WGPUVertexFormat_Sint16x2},
            {"sint16x4",  WGPUVertexFormat_Sint16x4},
            {"sint32",    WGPUVertexFormat_Sint32},
            {"sint32x2",  WGPUVertexFormat_Sint32x2},
            {"sint32x3",  WGPUVertexFormat_Sint32x3},
            {"sint32x4",  WGPUVertexFormat_Sint32x4},
            {"sint8x2",   WGPUVertexFormat_Sint8x2},
            {"sint8x4",   WGPUVertexFormat_Sint8x4},
            {"snorm16x2", WGPUVertexFormat_Snorm16x2},
            {"snorm16x4", WGPUVertexFormat_Snorm16x4},
            {"snorm8x2",  WGPUVertexFormat_Snorm8x2},
            {"snorm8x4",  WGPUVertexFormat_Snorm8x4},
            {"uint16x2",  WGPUVertexFormat_Uint16x2},
            {"uint16x4",  WGPUVertexFormat_Uint16x4},
            {"uint32",    WGPUVertexFormat_Uint32},
            {"uint32x2",  WGPUVertexFormat_Uint32x2},
            {"uint32x3",  WGPUVertexFormat_Uint32x3},
            {"uint32x4",  WGPUVertexFormat_Uint32x4},
            {"uint8x2",   WGPUVertexFormat_Uint8x2},
            {"uint8x4",   WGPUVertexFormat_Uint8x4},
            {"unorm16x2", WGPUVertexFormat_Unorm16x2},
            {"unorm16x4", WGPUVertexFormat_Unorm16x4},
            {"unorm8x2",  WGPUVertexFormat_Unorm8x2},
            {"unorm8x4",  WGPUVertexFormat_Unorm8x4}
        })

        NATIVEDAWN_ENUM_MAP(indexFormat, WGPUIndexFormat, WGPUIndexFormat_Undefined,
        {
            {"uint16", WGPUIndexFormat_Uint16},
            {"uint32", WGPUIndexFormat_Uint32}
        })

        NATIVEDAWN_ENUM_MAP(primitiveTopology, WGPUPrimitiveTopology, WGPUPrimitiveTopology_TriangleList,
        {
            {"line-list",      WGPUPrimitiveTopology_LineList},
            {"line-strip",     WGPUPrimitiveTopology_LineStrip},
            {"point-list",     WGPUPrimitiveTopology_PointList},
            {"triangle-list",  WGPUPrimitiveTopology_TriangleList},
            {"triangle-strip", WGPUPrimitiveTopology_TriangleStrip}
        })

        NATIVEDAWN_ENUM_MAP(cullMode, WGPUCullMode, WGPUCullMode_None,
        {
            {"back",  WGPUCullMode_Back},
            {"front", WGPUCullMode_Front},
            {"none",  WGPUCullMode_None}
        })

        NATIVEDAWN_ENUM_MAP(frontFace, WGPUFrontFace, WGPUFrontFace_CCW,
        {
            {"ccw", WGPUFrontFace_CCW},
            {"cw",  WGPUFrontFace_CW}
        })

        NATIVEDAWN_ENUM_MAP(compareFunction, WGPUCompareFunction, WGPUCompareFunction_Undefined,
        {
            {"always",        WGPUCompareFunction_Always},
            {"equal",         WGPUCompareFunction_Equal},
            {"greater",       WGPUCompareFunction_Greater},
            {"greater-equal", WGPUCompareFunction_GreaterEqual},
            {"less",          WGPUCompareFunction_Less},
            {"less-equal",    WGPUCompareFunction_LessEqual},
            {"never",         WGPUCompareFunction_Never},
            {"not-equal",     WGPUCompareFunction_NotEqual}
        })

        NATIVEDAWN_ENUM_MAP(stencilOperation, WGPUStencilOperation, WGPUStencilOperation_Keep,
        {
            {"decrement-clamp", WGPUStencilOperation_DecrementClamp},
            {"decrement-wrap",  WGPUStencilOperation_DecrementWrap},
            {"increment-clamp", WGPUStencilOperation_IncrementClamp},
            {"increment-wrap",  WGPUStencilOperation_IncrementWrap},
            {"invert",          WGPUStencilOperation_Invert},
            {"keep",            WGPUStencilOperation_Keep},
            {"replace",         WGPUStencilOperation_Replace},
            {"zero",            WGPUStencilOperation_Zero}
        })

        NATIVEDAWN_ENUM_MAP(blendFactor, WGPUBlendFactor, WGPUBlendFactor_One,
        {
            {"constant",            WGPUBlendFactor_Constant},
            {"dst",                 WGPUBlendFactor_Dst},
            {"dst-alpha",           WGPUBlendFactor_DstAlpha},
            {"one",                 WGPUBlendFactor_One},
            {"one-minus-constant",  WGPUBlendFactor_OneMinusConstant},
            {"one-minus-dst",       WGPUBlendFactor_OneMinusDst},
            {"one-minus-dst-alpha", WGPUBlendFactor_OneMinusDstAlpha},
            {"one-minus-src",       WGPUBlendFactor_OneMinusSrc},
            {"one-minus-src-alpha", WGPUBlendFactor_OneMinusSrcAlpha},
            {"src",                 WGPUBlendFactor_Src},
            {"src-alpha",           WGPUBlendFactor_SrcAlpha},
            {"src-alpha-saturated", WGPUBlendFactor_SrcAlphaSaturated},
            {"zero",                WGPUBlendFactor_Zero}
        })

        NATIVEDAWN_ENUM_MAP(blendOperation, WGPUBlendOperation, WGPUBlendOperation_Add,
        {
            {"add",              WGPUBlendOperation_Add},
            {"max",              WGPUBlendOperation_Max},
            {"min",              WGPUBlendOperation_Min},
            {"reverse-subtract", WGPUBlendOperation_ReverseSubtract},
            {"subtract",         WGPUBlendOperation_Subtract}
        })

        NATIVEDAWN_ENUM_MAP(addressMode, WGPUAddressMode, WGPUAddressMode_ClampToEdge,
        {
            {"clamp-to-edge", WGPUAddressMode_ClampToEdge},
            {"mirror-repeat", WGPUAddressMode_MirrorRepeat},
            {"repeat",        WGPUAddressMode_Repeat}
        })

        NATIVEDAWN_ENUM_MAP(filterMode, WGPUFilterMode, WGPUFilterMode_Nearest,
        {
            {"linear",  WGPUFilterMode_Linear},
            {"nearest", WGPUFilterMode_Nearest}
        })

        NATIVEDAWN_ENUM_MAP(mipmapFilterMode, WGPUMipmapFilterMode, WGPUMipmapFilterMode_Nearest,
        {
            {"linear",  WGPUMipmapFilterMode_Linear},
            {"nearest", WGPUMipmapFilterMode_Nearest}
        })

        NATIVEDAWN_ENUM_MAP(textureViewDimension, WGPUTextureViewDimension, WGPUTextureViewDimension_Undefined,
        {
            {"1d",         WGPUTextureViewDimension_1D},
            {"2d",         WGPUTextureViewDimension_2D},
            {"2d-array",   WGPUTextureViewDimension_2DArray},
            {"3d",         WGPUTextureViewDimension_3D},
            {"cube",       WGPUTextureViewDimension_Cube},
            {"cube-array", WGPUTextureViewDimension_CubeArray}
        })

        NATIVEDAWN_ENUM_MAP(textureDimension, WGPUTextureDimension, WGPUTextureDimension_2D,
        {
            {"1d", WGPUTextureDimension_1D},
            {"2d", WGPUTextureDimension_2D},
            {"3d", WGPUTextureDimension_3D}
        })

        NATIVEDAWN_ENUM_MAP(textureSampleType, WGPUTextureSampleType, WGPUTextureSampleType_Float,
        {
            {"depth",              WGPUTextureSampleType_Depth},
            {"float",              WGPUTextureSampleType_Float},
            {"sint",               WGPUTextureSampleType_Sint},
            {"uint",               WGPUTextureSampleType_Uint},
            {"unfilterable-float", WGPUTextureSampleType_UnfilterableFloat}
        })

        NATIVEDAWN_ENUM_MAP(storageTextureAccess, WGPUStorageTextureAccess, WGPUStorageTextureAccess_Undefined,
        {
            {"read-only",  WGPUStorageTextureAccess_ReadOnly},
            {"read-write", WGPUStorageTextureAccess_ReadWrite},
            {"write-only", WGPUStorageTextureAccess_WriteOnly}
        })

        NATIVEDAWN_ENUM_MAP(samplerBindingType, WGPUSamplerBindingType, WGPUSamplerBindingType_Filtering,
        {
            {"comparison",    WGPUSamplerBindingType_Comparison},
            {"filtering",     WGPUSamplerBindingType_Filtering},
            {"non-filtering", WGPUSamplerBindingType_NonFiltering}
        })

        NATIVEDAWN_ENUM_MAP(bufferBindingType, WGPUBufferBindingType, WGPUBufferBindingType_Uniform,
        {
            {"read-only-storage", WGPUBufferBindingType_ReadOnlyStorage},
            {"storage",           WGPUBufferBindingType_Storage},
            {"uniform",           WGPUBufferBindingType_Uniform}
        })

        NATIVEDAWN_ENUM_MAP(loadOp, WGPULoadOp, WGPULoadOp_Load,
        {
            {"clear", WGPULoadOp_Clear},
            {"load",  WGPULoadOp_Load}
        })

        NATIVEDAWN_ENUM_MAP(storeOp, WGPUStoreOp, WGPUStoreOp_Store,
        {
            {"discard", WGPUStoreOp_Discard},
            {"store",   WGPUStoreOp_Store}
        })

        NATIVEDAWN_ENUM_MAP(vertexStepMode, WGPUVertexStepMode, WGPUVertexStepMode_Vertex,
        {
            {"instance", WGPUVertexStepMode_Instance},
            {"vertex",   WGPUVertexStepMode_Vertex}
        })

        NATIVEDAWN_ENUM_MAP(featureName, WGPUFeatureName, WGPUFeatureName(0),
        {
            {"bgra8unorm-storage",       WGPUFeatureName_BGRA8UnormStorage},
            {"depth-clip-control",       WGPUFeatureName_DepthClipControl},
            {"depth32float-stencil8",    WGPUFeatureName_Depth32FloatStencil8},
            {"dual-source-blending",     WGPUFeatureName_DualSourceBlending},
            {"float32-filterable",       WGPUFeatureName_Float32Filterable},
            {"indirect-first-instance",  WGPUFeatureName_IndirectFirstInstance},
            {"rg11b10ufloat-renderable", WGPUFeatureName_RG11B10UfloatRenderable},
            {"shader-f16",               WGPUFeatureName_ShaderF16},
            {"texture-compression-astc", WGPUFeatureName_TextureCompressionASTC},
            {"texture-compression-bc",   WGPUFeatureName_TextureCompressionBC},
            {"texture-compression-etc2", WGPUFeatureName_TextureCompressionETC2},
            {"timestamp-query",          WGPUFeatureName_TimestampQuery}
        })

        NATIVEDAWN_ENUM_MAP(queryType, WGPUQueryType, WGPUQueryType_Occlusion,
        {
            {"occlusion", WGPUQueryType_Occlusion},
            {"timestamp", WGPUQueryType_Timestamp}
        })

        NATIVEDAWN_ENUM_MAP(powerPreference, WGPUPowerPreference, WGPUPowerPreference_Undefined,
        {
            {"high-performance", WGPUPowerPreference_HighPerformance},
            {"low-power",        WGPUPowerPreference_LowPower}
        })


#undef NATIVEDAWN_ENUM_MAP

        // Reverse lookup. Rare (only when reporting a texture's format back to
        // JS), so a linear scan over the forward table keeps one source of truth
        // instead of duplicating 60+ names in a switch. Every table name is a
        // string literal, so data() is NUL-terminated.
        const char* textureFormatStr(WGPUTextureFormat f)
        {
            for (const auto& e : ktextureFormatTable)
            {
                if (e.value == int32_t(f))
                {
                    return e.name.data();
                }
            }
            return "bgra8unorm";
        }

        // ---- limits ----------------------------------------------------------
        constexpr WGPULimits DefaultLimits()
        {
            return {
                .nextInChain = nullptr,
                .maxTextureDimension1D = WGPU_LIMIT_U32_UNDEFINED,
                .maxTextureDimension2D = WGPU_LIMIT_U32_UNDEFINED,
                .maxTextureDimension3D = WGPU_LIMIT_U32_UNDEFINED,
                .maxTextureArrayLayers = WGPU_LIMIT_U32_UNDEFINED,
                .maxBindGroups = WGPU_LIMIT_U32_UNDEFINED,
                .maxBindGroupsPlusVertexBuffers = WGPU_LIMIT_U32_UNDEFINED,
                .maxBindingsPerBindGroup = WGPU_LIMIT_U32_UNDEFINED,
                .maxDynamicUniformBuffersPerPipelineLayout = WGPU_LIMIT_U32_UNDEFINED,
                .maxDynamicStorageBuffersPerPipelineLayout = WGPU_LIMIT_U32_UNDEFINED,
                .maxSampledTexturesPerShaderStage = WGPU_LIMIT_U32_UNDEFINED,
                .maxSamplersPerShaderStage = WGPU_LIMIT_U32_UNDEFINED,
                .maxStorageBuffersPerShaderStage = WGPU_LIMIT_U32_UNDEFINED,
                .maxStorageTexturesPerShaderStage = WGPU_LIMIT_U32_UNDEFINED,
                .maxUniformBuffersPerShaderStage = WGPU_LIMIT_U32_UNDEFINED,
                .maxUniformBufferBindingSize = WGPU_LIMIT_U64_UNDEFINED,
                .maxStorageBufferBindingSize = WGPU_LIMIT_U64_UNDEFINED,
                .minUniformBufferOffsetAlignment = WGPU_LIMIT_U32_UNDEFINED,
                .minStorageBufferOffsetAlignment = WGPU_LIMIT_U32_UNDEFINED,
                .maxVertexBuffers = WGPU_LIMIT_U32_UNDEFINED,
                .maxBufferSize = WGPU_LIMIT_U64_UNDEFINED,
                .maxVertexAttributes = WGPU_LIMIT_U32_UNDEFINED,
                .maxVertexBufferArrayStride = WGPU_LIMIT_U32_UNDEFINED,
                .maxInterStageShaderVariables = WGPU_LIMIT_U32_UNDEFINED,
                .maxColorAttachments = WGPU_LIMIT_U32_UNDEFINED,
                .maxColorAttachmentBytesPerSample = WGPU_LIMIT_U32_UNDEFINED,
                .maxComputeWorkgroupStorageSize = WGPU_LIMIT_U32_UNDEFINED,
                .maxComputeInvocationsPerWorkgroup = WGPU_LIMIT_U32_UNDEFINED,
                .maxComputeWorkgroupSizeX = WGPU_LIMIT_U32_UNDEFINED,
                .maxComputeWorkgroupSizeY = WGPU_LIMIT_U32_UNDEFINED,
                .maxComputeWorkgroupSizeZ = WGPU_LIMIT_U32_UNDEFINED,
                .maxComputeWorkgroupsPerDimension = WGPU_LIMIT_U32_UNDEFINED,
                .maxImmediateSize = WGPU_LIMIT_U32_UNDEFINED,
            };
        }

        void FillLimits(Napi::Object o, const WGPULimits& L)
        {
            Napi::Env env = o.Env();
            auto N = [&](const char* k, double v) { o.Set(k, Napi::Number::New(env, v)); };
            N("maxTextureDimension1D", L.maxTextureDimension1D);
            N("maxTextureDimension2D", L.maxTextureDimension2D);
            N("maxTextureDimension3D", L.maxTextureDimension3D);
            N("maxTextureArrayLayers", L.maxTextureArrayLayers);
            N("maxBindGroups", L.maxBindGroups);
            N("maxBindGroupsPlusVertexBuffers", L.maxBindGroupsPlusVertexBuffers);
            N("maxBindingsPerBindGroup", L.maxBindingsPerBindGroup);
            N("maxDynamicUniformBuffersPerPipelineLayout", L.maxDynamicUniformBuffersPerPipelineLayout);
            N("maxDynamicStorageBuffersPerPipelineLayout", L.maxDynamicStorageBuffersPerPipelineLayout);
            N("maxSampledTexturesPerShaderStage", L.maxSampledTexturesPerShaderStage);
            N("maxSamplersPerShaderStage", L.maxSamplersPerShaderStage);
            N("maxStorageBuffersPerShaderStage", L.maxStorageBuffersPerShaderStage);
            N("maxStorageTexturesPerShaderStage", L.maxStorageTexturesPerShaderStage);
            N("maxUniformBuffersPerShaderStage", L.maxUniformBuffersPerShaderStage);
            N("maxUniformBufferBindingSize", static_cast<double>(L.maxUniformBufferBindingSize));
            N("maxStorageBufferBindingSize", static_cast<double>(L.maxStorageBufferBindingSize));
            N("minUniformBufferOffsetAlignment", L.minUniformBufferOffsetAlignment);
            N("minStorageBufferOffsetAlignment", L.minStorageBufferOffsetAlignment);
            N("maxVertexBuffers", L.maxVertexBuffers);
            N("maxBufferSize", static_cast<double>(L.maxBufferSize));
            N("maxVertexAttributes", L.maxVertexAttributes);
            N("maxVertexBufferArrayStride", L.maxVertexBufferArrayStride);
            N("maxInterStageShaderVariables", L.maxInterStageShaderVariables);
            N("maxColorAttachments", L.maxColorAttachments);
            N("maxColorAttachmentBytesPerSample", L.maxColorAttachmentBytesPerSample);
            N("maxComputeWorkgroupStorageSize", L.maxComputeWorkgroupStorageSize);
            N("maxComputeInvocationsPerWorkgroup", L.maxComputeInvocationsPerWorkgroup);
            N("maxComputeWorkgroupSizeX", L.maxComputeWorkgroupSizeX);
            N("maxComputeWorkgroupSizeY", L.maxComputeWorkgroupSizeY);
            N("maxComputeWorkgroupSizeZ", L.maxComputeWorkgroupSizeZ);
            N("maxComputeWorkgroupsPerDimension", L.maxComputeWorkgroupsPerDimension);
        }

        // ---- surface state ---------------------------------------------------
        bool g_surfaceConfigured = false;
        bool g_currentTextureAcquired = false;
        // Babylon reuses its upload/render command encoders across
        // requestAnimationFrame callbacks, so a render pass recorded against the
        // surface texture in frame N can be submitted in frame N+1. Presenting
        // destroys the surface texture, so we must not present until the work
        // that targets it has actually been submitted.
        bool g_surfaceWorkPending = false;

        // Remembered from the last GPUCanvasContext.configure() so the surface can
        // be re-configured on a resize without losing the requested usage/alpha.
        WGPUTextureUsage g_surfaceUsage = WGPUTextureUsage(WGPUTextureUsage_RenderAttachment | WGPUTextureUsage_CopySrc);
        WGPUCompositeAlphaMode g_surfaceAlphaMode = WGPUCompositeAlphaMode_Opaque;

        // Size the canvas currently reports. The surface catches up with it at the
        // next safe point (see ApplyPendingSurfaceResize).
        uint32_t g_requestedWidth = 0;
        uint32_t g_requestedHeight = 0;

        // Reconfiguring a surface destroys every texture it has handed out, so it must not
        // happen while work targeting one has been recorded but not submitted - Dawn would
        // reject the submit with "Destroyed texture ... used in a submit". That is what
        // g_surfaceWorkPending tracks. Merely holding an acquired texture is not a reason to
        // wait; it is released here and reacquired at the new size. Waiting on it instead
        // starves the resize, and since Babylon reallocates its depth attachment
        // synchronously inside setSize, every frame until the surface catches up renders a
        // new-size depth against an old-size surface and fails validation.
        void ApplyPendingSurfaceResize()
        {
            if (g_requestedWidth == 0 || g_requestedHeight == 0) return;
            if (g_requestedWidth == g_state.width && g_requestedHeight == g_state.height) return;
            if (g_surfaceWorkPending) return;
            if (!g_surfaceConfigured || g_state.surface == nullptr || g_state.device == nullptr) return;

            g_state.width = g_requestedWidth;
            g_state.height = g_requestedHeight;

            if (g_state.currentSurfaceTexture != nullptr)
            {
                wgpuTextureRelease(g_state.currentSurfaceTexture);
                g_state.currentSurfaceTexture = nullptr;
            }
            // The reconfigure destroys the texture the surface handed out, so it can no
            // longer be presented; the next acquire replaces it.
            g_currentTextureAcquired = false;

            WGPUSurfaceConfiguration cfg{
                .device = g_state.device,
                .format = g_state.surfaceFormat,
                .usage = g_surfaceUsage,
                .width = g_state.width,
                .height = g_state.height,
                .alphaMode = g_surfaceAlphaMode,
                .presentMode = WGPUPresentMode_Fifo,
            };
            wgpuSurfaceConfigure(g_state.surface, &cfg);
        }

        // Resize the drawing buffer (the Dawn surface) without touching the OS
        // window. On the web, assigning canvas.width/height resizes the drawing
        // buffer and the swap chain follows; Babylon relies on that for
        // engine.setHardwareScalingLevel(), which calls setSize(clientWidth/level,
        // ...) and then allocates its depth attachment at the new size. Without
        // this the colour attachment stays at the old size and every render pass
        // fails validation with "does not match the size of the other attachments'
        // base plane", and the framebuffer readback comes back the wrong size.
        void ResizeDrawingBuffer(uint32_t width, uint32_t height)
        {
            if (width < 1) width = 1;
            if (height < 1) height = 1;
            g_requestedWidth = width;
            g_requestedHeight = height;

            if (!g_surfaceConfigured || g_state.surface == nullptr || g_state.device == nullptr)
            {
                // Nothing configured yet: the values are picked up at configure().
                g_state.width = width;
                g_state.height = height;
                return;
            }
            ApplyPendingSurfaceResize();
        }

        // ---- forward declarations of object builders -------------------------
        Napi::Object MakeAdapter(Napi::Env env);
        Napi::Object MakeDevice(Napi::Env env);
        Napi::Object MakeQueue(Napi::Env env);
        Napi::Object MakeBuffer(Napi::Env env, WGPUBuffer h, uint64_t size, uint32_t usage, bool mapped);
        Napi::Object MakeTexture(Napi::Env env, WGPUTexture h, uint32_t w, uint32_t ht, uint32_t depth,
            uint32_t mip, uint32_t sample, const std::string& fmt, uint32_t usage, const std::string& dim);
        Napi::Object MakeTextureView(Napi::Env env, WGPUTextureView h, uint32_t w, uint32_t ht, WGPUTexture src);
        Napi::Object MakeSampler(Napi::Env env, WGPUSampler h);
        Napi::Object MakeBindGroupLayout(Napi::Env env, WGPUBindGroupLayout h);
        Napi::Object MakeBindGroup(Napi::Env env, WGPUBindGroup h);
        Napi::Object MakePipelineLayout(Napi::Env env, WGPUPipelineLayout h);
        Napi::Object MakeShaderModule(Napi::Env env, WGPUShaderModule h);
        Napi::Object MakeRenderPipeline(Napi::Env env, WGPURenderPipeline h);
        Napi::Object MakeComputePipeline(Napi::Env env, WGPUComputePipeline h);
        Napi::Object MakeCommandEncoder(Napi::Env env, WGPUCommandEncoder h);
        Napi::Object MakeRenderPassEncoder(Napi::Env env, WGPURenderPassEncoder h, uint32_t rtWidth, uint32_t rtHeight);
        Napi::Object MakeComputePassEncoder(Napi::Env env, WGPUComputePassEncoder h);
        Napi::Object MakeRenderBundleEncoder(Napi::Env env, WGPURenderBundleEncoder h);
        Napi::Object MakeRenderBundle(Napi::Env env, WGPURenderBundle h);
        Napi::Object MakeCommandBuffer(Napi::Env env, WGPUCommandBuffer h);
        Napi::Object MakeQuerySet(Napi::Env env, WGPUQuerySet h, uint32_t count);
        Napi::Object MakeCanvasContext(Napi::Env env);
        Napi::Object DoCreateRenderPipeline(Napi::Env env, Napi::Value descVal);
        Napi::Object DoCreateComputePipeline(Napi::Env env, Napi::Value descVal);

        // ---- set-like helper for .features -----------------------------------        // Returns a real JS Set populated with the enabled WebGPU feature name
        // strings, so JS gets forEach/has/size/iteration for free (the WebGPU
        // spec exposes GPUSupportedFeatures as a setlike<DOMString>).
        template <typename HasFn>
        Napi::Object MakeFeatureSet(Napi::Env env, HasFn hasFn)
        {
            static const char* const kFeatureNames[] = {
                "depth-clip-control",
                "depth32float-stencil8",
                "texture-compression-bc",
                "texture-compression-bc-sliced-3d",
                "texture-compression-etc2",
                "texture-compression-astc",
                "texture-compression-astc-sliced-3d",
                "timestamp-query",
                "indirect-first-instance",
                "shader-f16",
                "rg11b10ufloat-renderable",
                "bgra8unorm-storage",
                "float32-filterable",
                "float32-blendable",
                "clip-distances",
                "dual-source-blending",
                "subgroups",
            };

            Napi::Function setCtor = env.Global().Get("Set").As<Napi::Function>();
            Napi::Object set = setCtor.New({}).As<Napi::Object>();
            Napi::Function add = set.Get("add").As<Napi::Function>();
            for (const char* name : kFeatureNames)
            {
                if (hasFn(std::string(name)))
                {
                    add.Call(set, {Napi::String::New(env, name)});
                }
            }
            return set;
        }

        // ---- GPUBuffer -------------------------------------------------------
        Napi::Object MakeBuffer(Napi::Env env, WGPUBuffer h, uint64_t size, uint32_t usage, bool mapped)
        {
            Napi::Object o = NewGPUObject(env, "GPUBuffer");
            SetHandle(o, h);
            o.Set("size", Napi::Number::New(env, static_cast<double>(size)));
            o.Set("usage", Napi::Number::New(env, usage));
            o.Set("mapState", Napi::String::New(env, mapped ? "mapped" : "unmapped"));

            // Outstanding getMappedRange() shadow buffers for this GPUBuffer.
            // getMappedRange must NOT hand V8 a raw pointer into Dawn's mapped
            // memory: that memory is freed/invalidated by unmap(), leaving a
            // dangling external ArrayBuffer that corrupts the V8 heap and crashes
            // a later GC. Instead each getMappedRange returns a V8-owned
            // ArrayBuffer (seeded from Dawn's current bytes) and unmap() copies
            // those bytes back into Dawn before unmapping.
            struct MappedRange
            {
                uint64_t offset;
                uint64_t size;
                Napi::Reference<Napi::ArrayBuffer> ab;
            };
            auto ranges = std::make_shared<std::vector<MappedRange>>();

            SetMethod(o, "mapAsync", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                Napi::Env env = info.Env();
                uint32_t mode = ArgU32(info, 0, 0);
                uint64_t offset = ArgU64(info, 1, 0);
                uint64_t size = ArgIsUndef(info, 2) ? WGPU_WHOLE_MAP_SIZE : ArgU64(info, 2, 0);
                auto d = Napi::Promise::Deferred::New(env);
                WGPUBufferMapCallbackInfo mapCb{
                    .mode = WGPUCallbackMode_WaitAnyOnly,
                    .callback = [](WGPUMapAsyncStatus, WGPUStringView, void*, void*) {},
                };
                WGPUFuture f = wgpuBufferMapAsync(h, WGPUMapMode(mode), static_cast<size_t>(offset),
                    static_cast<size_t>(size), mapCb);
                WaitFuture(f);
                d.Resolve(env.Undefined());
                return d.Promise();
            });
            SetMethod(o, "getMappedRange", [h, ranges](const Napi::CallbackInfo& info) -> Napi::Value {
                Napi::Env env = info.Env();
                uint64_t offset = ArgU64(info, 0, 0);
                uint64_t size = ArgIsUndef(info, 1)
                    ? (wgpuBufferGetSize(h) - offset) : ArgU64(info, 1, 0);
                Napi::ArrayBuffer ab = Napi::ArrayBuffer::New(env, static_cast<size_t>(size));
                // Seed from Dawn's current mapped bytes so read-maps work. For a
                // write/mappedAtCreation map the source is writable too.
                const void* src = wgpuBufferGetConstMappedRange(h, static_cast<size_t>(offset), static_cast<size_t>(size));
                if (src != nullptr && size > 0)
                {
                    std::memcpy(ab.Data(), src, static_cast<size_t>(size));
                }
                ranges->push_back({offset, size, Napi::Persistent(ab)});
                return ab;
            });
            SetMethod(o, "unmap", [h, ranges](const Napi::CallbackInfo& info) -> Napi::Value {
                // Copy each outstanding shadow buffer back into Dawn's mapped
                // memory before unmapping (write-through), then release the JS
                // references so the ArrayBuffers can be collected.
                for (auto& r : *ranges)
                {
                    void* dst = wgpuBufferGetMappedRange(h, static_cast<size_t>(r.offset), static_cast<size_t>(r.size));
                    if (dst != nullptr && !r.ab.IsEmpty())
                    {
                        Napi::ArrayBuffer ab = r.ab.Value();
                        if (ab.ByteLength() >= r.size && r.size > 0)
                        {
                            std::memcpy(dst, ab.Data(), static_cast<size_t>(r.size));
                        }
                    }
                    r.ab.Reset();
                }
                ranges->clear();
                wgpuBufferUnmap(h);
                return info.Env().Undefined();
            });
            SetMethod(o, "destroy", [h, ranges](const Napi::CallbackInfo& info) -> Napi::Value {
                for (auto& r : *ranges) { r.ab.Reset(); }
                ranges->clear();
                // The lambda holds a raw handle for a few frames; keep a strong
                // reference so the object cannot be released by JS finalization
                // before the deferred Destroy runs.
                wgpuBufferAddRef(h);
                DeferDestroy([h]() { wgpuBufferDestroy(h); wgpuBufferRelease(h); });
                return info.Env().Undefined();
            });
            return o;
        }

        // ---- GPUTextureView --------------------------------------------------
        Napi::Object MakeTextureView(Napi::Env env, WGPUTextureView h, uint32_t w, uint32_t ht, WGPUTexture src)
        {
            Napi::Object o = NewGPUObject(env, "GPUTextureView");
            SetHandle(o, h);
            // Size of the mip level this view targets. Needed by beginRenderPass so
            // the render pass can clamp scissor rects to the attachment.
            o.Set("__width", Napi::Number::New(env, w));
            o.Set("__height", Napi::Number::New(env, ht));
            o.Set("__tex", Napi::Number::New(env, static_cast<double>(reinterpret_cast<uintptr_t>(src))));
            return o;
        }

        // ---- GPUTexture ------------------------------------------------------
        Napi::Object MakeTexture(Napi::Env env, WGPUTexture h, uint32_t w, uint32_t ht, uint32_t depth,
            uint32_t mip, uint32_t sample, const std::string& fmt, uint32_t usage, const std::string& dim)
        {
            Napi::Object o = NewGPUObject(env, "GPUTexture");
            SetHandle(o, h);
            o.Set("width", Napi::Number::New(env, w));
            o.Set("height", Napi::Number::New(env, ht));
            o.Set("depthOrArrayLayers", Napi::Number::New(env, depth));
            o.Set("mipLevelCount", Napi::Number::New(env, mip));
            o.Set("sampleCount", Napi::Number::New(env, sample));
            o.Set("format", Napi::String::New(env, fmt));
            o.Set("dimension", Napi::String::New(env, dim.empty() ? "2d" : dim));
            o.Set("usage", Napi::Number::New(env, usage));

            SetMethod(o, "createView", [h, w, ht](const Napi::CallbackInfo& info) -> Napi::Value {
                Napi::Env env = info.Env();
                WGPUTextureViewDescriptor d{
                    .label = EmptyStringView(),
                    .format = WGPUTextureFormat_Undefined,
                    .dimension = WGPUTextureViewDimension_Undefined,
                    .mipLevelCount = WGPU_MIP_LEVEL_COUNT_UNDEFINED,
                    .arrayLayerCount = WGPU_ARRAY_LAYER_COUNT_UNDEFINED,
                    .aspect = WGPUTextureAspect_Undefined,
                    .usage = WGPUTextureUsage_None,
                };
                if (info.Length() > 0 && info[0].IsObject())
                {
                    Napi::Object desc = info[0].As<Napi::Object>();
                    if (PropPresent(desc, "format")) d.format = textureFormat(PropStr(desc, "format"));
                    if (PropPresent(desc, "dimension")) d.dimension = textureViewDimension(PropStr(desc, "dimension"));
                    d.baseMipLevel = PropU32(desc, "baseMipLevel", 0);
                    if (PropPresent(desc, "mipLevelCount")) d.mipLevelCount = PropU32(desc, "mipLevelCount", 0);
                    d.baseArrayLayer = PropU32(desc, "baseArrayLayer", 0);
                    if (PropPresent(desc, "arrayLayerCount")) d.arrayLayerCount = PropU32(desc, "arrayLayerCount", 0);
                    std::string aspect = PropStr(desc, "aspect");
                    if (aspect == "stencil-only") d.aspect = WGPUTextureAspect_StencilOnly;
                    else if (aspect == "depth-only") d.aspect = WGPUTextureAspect_DepthOnly;
                    else d.aspect = WGPUTextureAspect_All;
                    if (PropPresent(desc, "usage")) d.usage = WGPUTextureUsage(PropU32(desc, "usage", 0));
                }
                return MakeTextureView(env, wgpuTextureCreateView(h, &d),
                    std::max(1u, w >> d.baseMipLevel), std::max(1u, ht >> d.baseMipLevel), h);
            });
            SetMethod(o, "destroy", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                wgpuTextureAddRef(h);
                DeferDestroy([h]() { wgpuTextureDestroy(h); wgpuTextureRelease(h); });
                return info.Env().Undefined();
            });
            return o;
        }

        // ---- GPUSampler ------------------------------------------------------
        Napi::Object MakeSampler(Napi::Env env, WGPUSampler h)
        {
            Napi::Object o = NewGPUObject(env, "GPUSampler");
            SetHandle(o, h);
            return o;
        }

        // ---- GPUBindGroupLayout ----------------------------------------------
        Napi::Object MakeBindGroupLayout(Napi::Env env, WGPUBindGroupLayout h)
        {
            Napi::Object o = NewGPUObject(env, "GPUBindGroupLayout");
            SetHandle(o, h);
            return o;
        }

        // ---- GPUBindGroup ----------------------------------------------------
        Napi::Object MakeBindGroup(Napi::Env env, WGPUBindGroup h)
        {
            Napi::Object o = NewGPUObject(env, "GPUBindGroup");
            SetHandle(o, h);
            return o;
        }

        // ---- GPUPipelineLayout -----------------------------------------------
        Napi::Object MakePipelineLayout(Napi::Env env, WGPUPipelineLayout h)
        {
            Napi::Object o = NewGPUObject(env, "GPUPipelineLayout");
            SetHandle(o, h);
            return o;
        }

        // ---- GPUShaderModule -------------------------------------------------
        Napi::Object MakeShaderModule(Napi::Env env, WGPUShaderModule h)
        {
            Napi::Object o = NewGPUObject(env, "GPUShaderModule");
            SetHandle(o, h);
            SetMethod(o, "getCompilationInfo", [](const Napi::CallbackInfo& info) -> Napi::Value {
                Napi::Env env = info.Env();
                auto d = Napi::Promise::Deferred::New(env);
                Napi::Object res = Napi::Object::New(env);
                res.Set("messages", Napi::Array::New(env));
                d.Resolve(res);
                return d.Promise();
            });
            return o;
        }

        // ---- GPUQuerySet -----------------------------------------------------
        Napi::Object MakeQuerySet(Napi::Env env, WGPUQuerySet h, uint32_t count)
        {
            Napi::Object o = NewGPUObject(env, "GPUQuerySet");
            SetHandle(o, h);
            o.Set("count", Napi::Number::New(env, count));
            SetMethod(o, "destroy", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                wgpuQuerySetAddRef(h);
                DeferDestroy([h]() { wgpuQuerySetDestroy(h); wgpuQuerySetRelease(h); });
                return info.Env().Undefined();
            });
            return o;
        }

        // ---- GPUCommandBuffer ------------------------------------------------
        Napi::Object MakeCommandBuffer(Napi::Env env, WGPUCommandBuffer h)
        {
            Napi::Object o = NewGPUObject(env, "GPUCommandBuffer");
            SetHandle(o, h);
            return o;
        }

        // ---- GPURenderPipeline -----------------------------------------------
        Napi::Object MakeRenderPipeline(Napi::Env env, WGPURenderPipeline h)
        {
            Napi::Object o = NewGPUObject(env, "GPURenderPipeline");
            SetHandle(o, h);
            SetMethod(o, "getBindGroupLayout", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                Napi::Env env = info.Env();
                uint32_t index = ArgU32(info, 0, 0);
                return MakeBindGroupLayout(env, wgpuRenderPipelineGetBindGroupLayout(h, index));
            });
            return o;
        }

        // ---- GPUComputePipeline ----------------------------------------------
        Napi::Object MakeComputePipeline(Napi::Env env, WGPUComputePipeline h)
        {
            Napi::Object o = NewGPUObject(env, "GPUComputePipeline");
            SetHandle(o, h);
            SetMethod(o, "getBindGroupLayout", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                Napi::Env env = info.Env();
                uint32_t index = ArgU32(info, 0, 0);
                return MakeBindGroupLayout(env, wgpuComputePipelineGetBindGroupLayout(h, index));
            });
            return o;
        }

        // ---- createRenderPipeline (shared by sync + async) -------------------
        Napi::Object DoCreateRenderPipeline(Napi::Env env, Napi::Value descVal)
        {
            if (!descVal.IsObject())
            {
                throw Napi::Error::New(env, "NativeDawn: createRenderPipeline requires a descriptor");
            }
            Napi::Object desc = descVal.As<Napi::Object>();

            WGPURenderPipelineDescriptor rp{
                .label = EmptyStringView(),
                .vertex = {
                    .module = nullptr,
                    .entryPoint = EmptyStringView(),
                },
                .primitive = {
                    .topology = WGPUPrimitiveTopology_Undefined,
                    .stripIndexFormat = WGPUIndexFormat_Undefined,
                    .frontFace = WGPUFrontFace_Undefined,
                    .cullMode = WGPUCullMode_Undefined,
                    .unclippedDepth = false,
                },
                .multisample = {
                    .count = 1,
                    .mask = 0xffffffff,
                    .alphaToCoverageEnabled = false,
                },
            };
            std::string label = PropStr(desc, "label");
            if (!label.empty()) rp.label = StrView(label);

            // layout (object => explicit; "auto"/absent => auto layout)
            {
                Napi::Value layoutV = desc.Get("layout");
                WGPUPipelineLayout* pl = GetH<WGPUPipelineLayout>(layoutV);
                if (pl != nullptr) rp.layout = *pl;
            }

            // vertex
            std::string vEntry;
            std::vector<WGPUVertexBufferLayout> vBuffers;
            std::vector<std::vector<WGPUVertexAttribute>> vAttrs;
            {
                Napi::Value vtxV = desc.Get("vertex");
                if (!vtxV.IsObject())
                {
                    throw Napi::Error::New(env, "NativeDawn: createRenderPipeline requires vertex stage");
                }
                Napi::Object vtx = vtxV.As<Napi::Object>();
                WGPUShaderModule* mod = GetH<WGPUShaderModule>(vtx.Get("module"));
                if (mod != nullptr) rp.vertex.module = *mod;
                vEntry = PropStr(vtx, "entryPoint");
                if (!vEntry.empty()) rp.vertex.entryPoint = StrView(vEntry);

                Napi::Value buffersV = vtx.Get("buffers");
                if (buffersV.IsArray())
                {
                    Napi::Array buffers = buffersV.As<Napi::Array>();
                    size_t n = buffers.Length();
                    vBuffers.resize(n);
                    vAttrs.resize(n);
                    for (size_t i = 0; i < n; ++i)
                    {
                        Napi::Value be = buffers.Get(static_cast<uint32_t>(i));
                        if (!be.IsObject())
                        {
                            vBuffers[i].stepMode = WGPUVertexStepMode_Undefined;
                            vBuffers[i].arrayStride = 0;
                            vBuffers[i].attributeCount = 0;
                            continue;
                        }
                        Napi::Object bo = be.As<Napi::Object>();
                        vBuffers[i].arrayStride = PropU64(bo, "arrayStride", 0);
                        vBuffers[i].stepMode = PropPresent(bo, "stepMode")
                            ? vertexStepMode(PropStr(bo, "stepMode")) : WGPUVertexStepMode_Vertex;
                        Napi::Value attrsV = bo.Get("attributes");
                        if (attrsV.IsArray())
                        {
                            Napi::Array attrs = attrsV.As<Napi::Array>();
                            for (uint32_t j = 0; j < attrs.Length(); ++j)
                            {
                                Napi::Value av = attrs.Get(j);
                                if (!av.IsObject()) continue;
                                Napi::Object ao = av.As<Napi::Object>();
                                WGPUVertexAttribute va{
                                    .format = vertexFormat(PropStr(ao, "format")),
                                    .offset = PropU64(ao, "offset", 0),
                                    .shaderLocation = PropU32(ao, "shaderLocation", 0),
                                };
                                vAttrs[i].push_back(va);
                            }
                        }
                        vBuffers[i].attributeCount = vAttrs[i].size();
                        vBuffers[i].attributes = vAttrs[i].data();
                    }
                    rp.vertex.bufferCount = n;
                    rp.vertex.buffers = vBuffers.data();
                }
            }

            // primitive
            {
                Napi::Value prV = desc.Get("primitive");
                if (prV.IsObject())
                {
                    Napi::Object pr = prV.As<Napi::Object>();
                    rp.primitive.topology = PropPresent(pr, "topology")
                        ? primitiveTopology(PropStr(pr, "topology")) : WGPUPrimitiveTopology_TriangleList;
                    if (PropPresent(pr, "stripIndexFormat"))
                        rp.primitive.stripIndexFormat = indexFormat(PropStr(pr, "stripIndexFormat"));
                    rp.primitive.frontFace = frontFace(PropStr(pr, "frontFace"));
                    rp.primitive.cullMode = cullMode(PropStr(pr, "cullMode"));
                }
            }

            // depthStencil
            WGPUDepthStencilState dss{
                .format = WGPUTextureFormat_Undefined,
                .depthWriteEnabled = WGPUOptionalBool_Undefined,
                .depthCompare = WGPUCompareFunction_Undefined,
                .stencilFront = {
                    .compare = WGPUCompareFunction_Undefined,
                    .failOp = WGPUStencilOperation_Undefined,
                    .depthFailOp = WGPUStencilOperation_Undefined,
                    .passOp = WGPUStencilOperation_Undefined,
                },
                .stencilBack = {
                    .compare = WGPUCompareFunction_Undefined,
                    .failOp = WGPUStencilOperation_Undefined,
                    .depthFailOp = WGPUStencilOperation_Undefined,
                    .passOp = WGPUStencilOperation_Undefined,
                },
                .stencilReadMask = 0xffffffff,
                .stencilWriteMask = 0xffffffff,
                .depthBias = 0,
                .depthBiasSlopeScale = 0.0f,
                .depthBiasClamp = 0.0f,
            };
            {
                Napi::Value dsV = desc.Get("depthStencil");
                if (dsV.IsObject())
                {
                    Napi::Object ds = dsV.As<Napi::Object>();
                    dss.format = textureFormat(PropStr(ds, "format"));
                    dss.depthWriteEnabled = PropBool(ds, "depthWriteEnabled", false)
                        ? WGPUOptionalBool_True : WGPUOptionalBool_False;
                    if (PropPresent(ds, "depthCompare"))
                        dss.depthCompare = compareFunction(PropStr(ds, "depthCompare"));
                    dss.stencilReadMask = PropU32(ds, "stencilReadMask", 0xFFFFFFFF);
                    dss.stencilWriteMask = PropU32(ds, "stencilWriteMask", 0xFFFFFFFF);
                    dss.depthBias = PropI32(ds, "depthBias", 0);
                    dss.depthBiasSlopeScale = static_cast<float>(PropF64(ds, "depthBiasSlopeScale", 0));
                    dss.depthBiasClamp = static_cast<float>(PropF64(ds, "depthBiasClamp", 0));
                    auto parseFace = [](Napi::Object f, WGPUStencilFaceState& out) {
                        if (PropPresent(f, "compare")) out.compare = compareFunction(PropStr(f, "compare"));
                        if (PropPresent(f, "failOp")) out.failOp = stencilOperation(PropStr(f, "failOp"));
                        if (PropPresent(f, "depthFailOp")) out.depthFailOp = stencilOperation(PropStr(f, "depthFailOp"));
                        if (PropPresent(f, "passOp")) out.passOp = stencilOperation(PropStr(f, "passOp"));
                    };
                    if (ds.Get("stencilFront").IsObject()) parseFace(ds.Get("stencilFront").As<Napi::Object>(), dss.stencilFront);
                    if (ds.Get("stencilBack").IsObject()) parseFace(ds.Get("stencilBack").As<Napi::Object>(), dss.stencilBack);
                    rp.depthStencil = &dss;
                }
            }

            // multisample
            {
                Napi::Value msV = desc.Get("multisample");
                if (msV.IsObject())
                {
                    Napi::Object ms = msV.As<Napi::Object>();
                    rp.multisample.count = PropU32(ms, "count", 1);
                    rp.multisample.mask = PropU32(ms, "mask", 0xFFFFFFFF);
                    rp.multisample.alphaToCoverageEnabled = PropBool(ms, "alphaToCoverageEnabled", false);
                }
            }

            // fragment
            std::string fEntry;
            WGPUFragmentState fs{
                .module = nullptr,
                .entryPoint = EmptyStringView(),
            };
            std::vector<WGPUColorTargetState> fTargets;
            std::vector<WGPUBlendState> fBlends;
            {
                Napi::Value frV = desc.Get("fragment");
                if (frV.IsObject())
                {
                    Napi::Object fr = frV.As<Napi::Object>();
                    WGPUShaderModule* mod = GetH<WGPUShaderModule>(fr.Get("module"));
                    if (mod != nullptr) fs.module = *mod;
                    fEntry = PropStr(fr, "entryPoint");
                    if (!fEntry.empty()) fs.entryPoint = StrView(fEntry);

                    Napi::Value tV = fr.Get("targets");
                    if (tV.IsArray())
                    {
                        Napi::Array targets = tV.As<Napi::Array>();
                        size_t n = targets.Length();
                        fTargets.resize(n);
                        fBlends.resize(n);
                        for (size_t i = 0; i < n; ++i)
                        {
                            Napi::Value te = targets.Get(static_cast<uint32_t>(i));
                            if (!te.IsObject()) continue;
                            Napi::Object to = te.As<Napi::Object>();
                            fTargets[i].format = textureFormat(PropStr(to, "format"));
                            fTargets[i].writeMask = WGPUColorWriteMask(PropU32(to, "writeMask", 0xF));
                            Napi::Value blV = to.Get("blend");
                            if (blV.IsObject())
                            {
                                Napi::Object bl = blV.As<Napi::Object>();
                                WGPUBlendState& bs = fBlends[i];
                                Napi::Value cV = bl.Get("color");
                                if (cV.IsObject())
                                {
                                    Napi::Object c = cV.As<Napi::Object>();
                                    if (PropPresent(c, "operation")) bs.color.operation = blendOperation(PropStr(c, "operation"));
                                    if (PropPresent(c, "srcFactor")) bs.color.srcFactor = blendFactor(PropStr(c, "srcFactor"));
                                    if (PropPresent(c, "dstFactor")) bs.color.dstFactor = blendFactor(PropStr(c, "dstFactor"));
                                }
                                Napi::Value aV = bl.Get("alpha");
                                if (aV.IsObject())
                                {
                                    Napi::Object a = aV.As<Napi::Object>();
                                    if (PropPresent(a, "operation")) bs.alpha.operation = blendOperation(PropStr(a, "operation"));
                                    if (PropPresent(a, "srcFactor")) bs.alpha.srcFactor = blendFactor(PropStr(a, "srcFactor"));
                                    if (PropPresent(a, "dstFactor")) bs.alpha.dstFactor = blendFactor(PropStr(a, "dstFactor"));
                                }
                                fTargets[i].blend = &bs;
                            }
                        }
                        fs.targetCount = n;
                        fs.targets = fTargets.data();
                    }
                    rp.fragment = &fs;
                }
            }

            WGPURenderPipeline pipe = wgpuDeviceCreateRenderPipeline(g_state.device, &rp);
            return MakeRenderPipeline(env, pipe);
        }

        // ---- createComputePipeline -------------------------------------------
        Napi::Object DoCreateComputePipeline(Napi::Env env, Napi::Value descVal)
        {
            if (!descVal.IsObject())
            {
                throw Napi::Error::New(env, "NativeDawn: createComputePipeline requires a descriptor");
            }
            Napi::Object desc = descVal.As<Napi::Object>();

            WGPUComputePipelineDescriptor cp{
                .label = EmptyStringView(),
                .compute = {
                    .module = nullptr,
                    .entryPoint = EmptyStringView(),
                },
            };
            std::string label = PropStr(desc, "label");
            if (!label.empty()) cp.label = StrView(label);

            {
                Napi::Value layoutV = desc.Get("layout");
                WGPUPipelineLayout* pl = GetH<WGPUPipelineLayout>(layoutV);
                if (pl != nullptr) cp.layout = *pl;
            }

            std::string cEntry;
            Napi::Value coV = desc.Get("compute");
            if (!coV.IsObject())
            {
                throw Napi::Error::New(env, "NativeDawn: createComputePipeline requires compute stage");
            }
            Napi::Object co = coV.As<Napi::Object>();
            WGPUShaderModule* mod = GetH<WGPUShaderModule>(co.Get("module"));
            if (mod != nullptr) cp.compute.module = *mod;
            cEntry = PropStr(co, "entryPoint");
            if (!cEntry.empty()) cp.compute.entryPoint = StrView(cEntry);

            WGPUComputePipeline pipe = wgpuDeviceCreateComputePipeline(g_state.device, &cp);
            return MakeComputePipeline(env, pipe);
        }

        // ---- texel copy helpers ----------------------------------------------
        WGPUTexelCopyBufferInfo ParseTexelCopyBuffer(Napi::Object o)
        {
            WGPUTexelCopyBufferInfo info{
                .layout = {
                    .offset = 0,
                    .bytesPerRow = WGPU_COPY_STRIDE_UNDEFINED,
                    .rowsPerImage = WGPU_COPY_STRIDE_UNDEFINED,
                },
            };
            WGPUBuffer* b = GetH<WGPUBuffer>(o.Get("buffer"));
            if (b != nullptr) info.buffer = *b;
            info.layout.offset = PropU64(o, "offset", 0);
            info.layout.bytesPerRow = PropPresent(o, "bytesPerRow")
                ? PropU32(o, "bytesPerRow", 0) : WGPU_COPY_STRIDE_UNDEFINED;
            info.layout.rowsPerImage = PropPresent(o, "rowsPerImage")
                ? PropU32(o, "rowsPerImage", 0) : WGPU_COPY_STRIDE_UNDEFINED;
            return info;
        }
        WGPUTexelCopyTextureInfo ParseTexelCopyTexture(Napi::Object o)
        {
            WGPUTexelCopyTextureInfo info{
                .origin = {
                    .x = 0,
                    .y = 0,
                    .z = 0,
                },
                .aspect = WGPUTextureAspect_Undefined,
            };
            WGPUTexture* t = GetH<WGPUTexture>(o.Get("texture"));
            if (t != nullptr) info.texture = *t;
            if (t != nullptr && *t == g_state.currentSurfaceTexture)
            {
                g_surfaceWorkPending = true;
            }
            info.mipLevel = PropU32(o, "mipLevel", 0);
            if (PropPresent(o, "origin")) info.origin = ParseOrigin3D(o.Get("origin"));
            std::string aspect = PropStr(o, "aspect");
            if (aspect == "stencil-only") info.aspect = WGPUTextureAspect_StencilOnly;
            else if (aspect == "depth-only") info.aspect = WGPUTextureAspect_DepthOnly;
            else info.aspect = WGPUTextureAspect_All;
            return info;
        }

        std::vector<uint32_t> ReadU32Array(Napi::Value v)
        {
            std::vector<uint32_t> out;
            if (v.IsArray())
            {
                Napi::Array a = v.As<Napi::Array>();
                for (uint32_t i = 0; i < a.Length(); ++i)
                {
                    Napi::Value e = a.Get(i);
                    if (e.IsNumber()) out.push_back(e.As<Napi::Number>().Uint32Value());
                }
            }
            else if (v.IsTypedArray())
            {
                Napi::TypedArray ta = v.As<Napi::TypedArray>();
                if (ta.TypedArrayType() == napi_uint32_array)
                {
                    Napi::Uint32Array u = v.As<Napi::Uint32Array>();
                    for (size_t i = 0; i < u.ElementLength(); ++i) out.push_back(u[i]);
                }
            }
            return out;
        }

        // ---- GPURenderPassEncoder --------------------------------------------
        Napi::Object MakeRenderPassEncoder(Napi::Env env, WGPURenderPassEncoder h, uint32_t rtWidth, uint32_t rtHeight)
        {
            Napi::Object o = NewGPUObject(env, "GPURenderPassEncoder");
            SetHandle(o, h);
            SetMethod(o, "setPipeline", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                WGPURenderPipeline* p = GetH<WGPURenderPipeline>(info[0]);
                if (p != nullptr) wgpuRenderPassEncoderSetPipeline(h, *p);
                return info.Env().Undefined();
            });
            SetMethod(o, "setBindGroup", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                uint32_t index = ArgU32(info, 0, 0);
                WGPUBindGroup bg = nullptr;
                WGPUBindGroup* p = GetH<WGPUBindGroup>(info[1]);
                if (p != nullptr) bg = *p;
                std::vector<uint32_t> offsets;
                if (info.Length() > 2 && !IsNullish(info[2])) offsets = ReadU32Array(info[2]);
                wgpuRenderPassEncoderSetBindGroup(h, index, bg, offsets.size(), offsets.empty() ? nullptr : offsets.data());
                return info.Env().Undefined();
            });
            SetMethod(o, "setVertexBuffer", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                uint32_t slot = ArgU32(info, 0, 0);
                WGPUBuffer buf = nullptr;
                WGPUBuffer* p = GetH<WGPUBuffer>(info[1]);
                if (p != nullptr) buf = *p;
                uint64_t offset = ArgU64(info, 2, 0);
                uint64_t size = ArgIsUndef(info, 3) ? WGPU_WHOLE_SIZE : ArgU64(info, 3, 0);
                wgpuRenderPassEncoderSetVertexBuffer(h, slot, buf, offset, size);
                return info.Env().Undefined();
            });
            SetMethod(o, "setIndexBuffer", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                WGPUBuffer buf = nullptr;
                WGPUBuffer* p = GetH<WGPUBuffer>(info[0]);
                if (p != nullptr) buf = *p;
                std::string fmt = (info.Length() > 1 && info[1].IsString())
                    ? info[1].As<Napi::String>().Utf8Value() : std::string{};
                uint64_t offset = ArgU64(info, 2, 0);
                uint64_t size = ArgIsUndef(info, 3) ? WGPU_WHOLE_SIZE : ArgU64(info, 3, 0);
                wgpuRenderPassEncoderSetIndexBuffer(h, buf, indexFormat(fmt), offset, size);
                return info.Env().Undefined();
            });
            SetMethod(o, "draw", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                wgpuRenderPassEncoderDraw(h, ArgU32(info, 0, 0), ArgU32(info, 1, 1), ArgU32(info, 2, 0), ArgU32(info, 3, 0));
                return info.Env().Undefined();
            });
            SetMethod(o, "drawIndexed", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                wgpuRenderPassEncoderDrawIndexed(h, ArgU32(info, 0, 0), ArgU32(info, 1, 1), ArgU32(info, 2, 0),
                    ArgI32(info, 3, 0), ArgU32(info, 4, 0));
                return info.Env().Undefined();
            });
            SetMethod(o, "drawIndirect", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                WGPUBuffer* b = GetH<WGPUBuffer>(info[0]);
                if (b != nullptr) wgpuRenderPassEncoderDrawIndirect(h, *b, ArgU64(info, 1, 0));
                return info.Env().Undefined();
            });
            SetMethod(o, "drawIndexedIndirect", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                WGPUBuffer* b = GetH<WGPUBuffer>(info[0]);
                if (b != nullptr) wgpuRenderPassEncoderDrawIndexedIndirect(h, *b, ArgU64(info, 1, 0));
                return info.Env().Undefined();
            });
            SetMethod(o, "setViewport", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                wgpuRenderPassEncoderSetViewport(h, static_cast<float>(ArgF64(info, 0, 0)), static_cast<float>(ArgF64(info, 1, 0)),
                    static_cast<float>(ArgF64(info, 2, 0)), static_cast<float>(ArgF64(info, 3, 0)),
                    static_cast<float>(ArgF64(info, 4, 0)), static_cast<float>(ArgF64(info, 5, 1)));
                return info.Env().Undefined();
            });
            SetMethod(o, "setScissorRect", [h, rtWidth, rtHeight](const Napi::CallbackInfo& info) -> Napi::Value {
                // WebGPU rejects a scissor rect that is not fully contained in the
                // render target, and the rejection invalidates the whole command
                // buffer, so a single out-of-range rect blanks the entire frame.
                // D3D11/Metal clamp instead, and Babylon relies on that, so
                // intersect the rect with the attachment here. Read the arguments
                // as doubles: Babylon can pass negative offsets, which would wrap
                // to huge values if taken as uint32.
                const double xd = ArgF64(info, 0, 0);
                const double yd = ArgF64(info, 1, 0);
                int64_t x0 = static_cast<int64_t>(xd);
                int64_t y0 = static_cast<int64_t>(yd);
                int64_t x1 = x0 + static_cast<int64_t>(ArgF64(info, 2, 0));
                int64_t y1 = y0 + static_cast<int64_t>(ArgF64(info, 3, 0));

                if (rtWidth != 0 && rtHeight != 0)
                {
                    x0 = std::clamp<int64_t>(x0, 0, rtWidth);
                    y0 = std::clamp<int64_t>(y0, 0, rtHeight);
                    x1 = std::clamp<int64_t>(x1, x0, rtWidth);
                    y1 = std::clamp<int64_t>(y1, y0, rtHeight);
                }
                else
                {
                    // Attachment size unknown; at least keep the values in range.
                    x0 = std::max<int64_t>(x0, 0);
                    y0 = std::max<int64_t>(y0, 0);
                    x1 = std::max<int64_t>(x1, x0);
                    y1 = std::max<int64_t>(y1, y0);
                }

                wgpuRenderPassEncoderSetScissorRect(h, static_cast<uint32_t>(x0), static_cast<uint32_t>(y0),
                    static_cast<uint32_t>(x1 - x0), static_cast<uint32_t>(y1 - y0));
                return info.Env().Undefined();
            });
            SetMethod(o, "setBlendConstant", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                WGPUColor c = ParseColor(info[0]);
                wgpuRenderPassEncoderSetBlendConstant(h, &c);
                return info.Env().Undefined();
            });
            SetMethod(o, "setStencilReference", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                wgpuRenderPassEncoderSetStencilReference(h, ArgU32(info, 0, 0));
                return info.Env().Undefined();
            });
            SetMethod(o, "beginOcclusionQuery", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                wgpuRenderPassEncoderBeginOcclusionQuery(h, ArgU32(info, 0, 0));
                return info.Env().Undefined();
            });
            SetMethod(o, "endOcclusionQuery", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                wgpuRenderPassEncoderEndOcclusionQuery(h);
                return info.Env().Undefined();
            });
            SetMethod(o, "executeBundles", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                std::vector<WGPURenderBundle> bundles;
                if (info.Length() > 0 && info[0].IsArray())
                {
                    Napi::Array a = info[0].As<Napi::Array>();
                    for (uint32_t i = 0; i < a.Length(); ++i)
                    {
                        WGPURenderBundle* b = GetH<WGPURenderBundle>(a.Get(i));
                        if (b != nullptr) bundles.push_back(*b);
                    }
                }
                wgpuRenderPassEncoderExecuteBundles(h, bundles.size(), bundles.empty() ? nullptr : bundles.data());
                return info.Env().Undefined();
            });
            SetMethod(o, "pushDebugGroup", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                std::string s = (info.Length() > 0 && info[0].IsString()) ? info[0].As<Napi::String>().Utf8Value() : std::string{};
                wgpuRenderPassEncoderPushDebugGroup(h, StrView(s));
                return info.Env().Undefined();
            });
            SetMethod(o, "popDebugGroup", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                wgpuRenderPassEncoderPopDebugGroup(h);
                return info.Env().Undefined();
            });
            SetMethod(o, "insertDebugMarker", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                std::string s = (info.Length() > 0 && info[0].IsString()) ? info[0].As<Napi::String>().Utf8Value() : std::string{};
                wgpuRenderPassEncoderInsertDebugMarker(h, StrView(s));
                return info.Env().Undefined();
            });
            SetMethod(o, "end", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                wgpuRenderPassEncoderEnd(h);
                return info.Env().Undefined();
            });
            return o;
        }

        // ---- GPUComputePassEncoder -------------------------------------------
        Napi::Object MakeComputePassEncoder(Napi::Env env, WGPUComputePassEncoder h)
        {
            Napi::Object o = NewGPUObject(env, "GPUComputePassEncoder");
            SetHandle(o, h);
            SetMethod(o, "setPipeline", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                WGPUComputePipeline* p = GetH<WGPUComputePipeline>(info[0]);
                if (p != nullptr) wgpuComputePassEncoderSetPipeline(h, *p);
                return info.Env().Undefined();
            });
            SetMethod(o, "setBindGroup", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                uint32_t index = ArgU32(info, 0, 0);
                WGPUBindGroup bg = nullptr;
                WGPUBindGroup* p = GetH<WGPUBindGroup>(info[1]);
                if (p != nullptr) bg = *p;
                std::vector<uint32_t> offsets;
                if (info.Length() > 2 && !IsNullish(info[2])) offsets = ReadU32Array(info[2]);
                wgpuComputePassEncoderSetBindGroup(h, index, bg, offsets.size(), offsets.empty() ? nullptr : offsets.data());
                return info.Env().Undefined();
            });
            SetMethod(o, "dispatchWorkgroups", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                wgpuComputePassEncoderDispatchWorkgroups(h, ArgU32(info, 0, 1), ArgU32(info, 1, 1), ArgU32(info, 2, 1));
                return info.Env().Undefined();
            });
            SetMethod(o, "dispatchWorkgroupsIndirect", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                WGPUBuffer* b = GetH<WGPUBuffer>(info[0]);
                if (b != nullptr) wgpuComputePassEncoderDispatchWorkgroupsIndirect(h, *b, ArgU64(info, 1, 0));
                return info.Env().Undefined();
            });
            SetMethod(o, "pushDebugGroup", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                std::string s = (info.Length() > 0 && info[0].IsString()) ? info[0].As<Napi::String>().Utf8Value() : std::string{};
                wgpuComputePassEncoderPushDebugGroup(h, StrView(s));
                return info.Env().Undefined();
            });
            SetMethod(o, "popDebugGroup", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                wgpuComputePassEncoderPopDebugGroup(h);
                return info.Env().Undefined();
            });
            SetMethod(o, "insertDebugMarker", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                std::string s = (info.Length() > 0 && info[0].IsString()) ? info[0].As<Napi::String>().Utf8Value() : std::string{};
                wgpuComputePassEncoderInsertDebugMarker(h, StrView(s));
                return info.Env().Undefined();
            });
            SetMethod(o, "end", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                wgpuComputePassEncoderEnd(h);
                return info.Env().Undefined();
            });
            return o;
        }

        // ---- GPURenderBundle -------------------------------------------------
        Napi::Object MakeRenderBundle(Napi::Env env, WGPURenderBundle h)
        {
            Napi::Object o = NewGPUObject(env, "GPURenderBundle");
            SetHandle(o, h);
            return o;
        }

        // ---- GPURenderBundleEncoder ------------------------------------------
        Napi::Object MakeRenderBundleEncoder(Napi::Env env, WGPURenderBundleEncoder h)
        {
            Napi::Object o = NewGPUObject(env, "GPURenderBundleEncoder");
            SetHandle(o, h);
            SetMethod(o, "setPipeline", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                WGPURenderPipeline* p = GetH<WGPURenderPipeline>(info[0]);
                if (p != nullptr) wgpuRenderBundleEncoderSetPipeline(h, *p);
                return info.Env().Undefined();
            });
            SetMethod(o, "setBindGroup", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                uint32_t index = ArgU32(info, 0, 0);
                WGPUBindGroup bg = nullptr;
                WGPUBindGroup* p = GetH<WGPUBindGroup>(info[1]);
                if (p != nullptr) bg = *p;
                std::vector<uint32_t> offsets;
                if (info.Length() > 2 && !IsNullish(info[2])) offsets = ReadU32Array(info[2]);
                wgpuRenderBundleEncoderSetBindGroup(h, index, bg, offsets.size(), offsets.empty() ? nullptr : offsets.data());
                return info.Env().Undefined();
            });
            SetMethod(o, "setVertexBuffer", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                uint32_t slot = ArgU32(info, 0, 0);
                WGPUBuffer buf = nullptr;
                WGPUBuffer* p = GetH<WGPUBuffer>(info[1]);
                if (p != nullptr) buf = *p;
                uint64_t offset = ArgU64(info, 2, 0);
                uint64_t size = ArgIsUndef(info, 3) ? WGPU_WHOLE_SIZE : ArgU64(info, 3, 0);
                wgpuRenderBundleEncoderSetVertexBuffer(h, slot, buf, offset, size);
                return info.Env().Undefined();
            });
            SetMethod(o, "setIndexBuffer", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                WGPUBuffer buf = nullptr;
                WGPUBuffer* p = GetH<WGPUBuffer>(info[0]);
                if (p != nullptr) buf = *p;
                std::string fmt = (info.Length() > 1 && info[1].IsString())
                    ? info[1].As<Napi::String>().Utf8Value() : std::string{};
                uint64_t offset = ArgU64(info, 2, 0);
                uint64_t size = ArgIsUndef(info, 3) ? WGPU_WHOLE_SIZE : ArgU64(info, 3, 0);
                wgpuRenderBundleEncoderSetIndexBuffer(h, buf, indexFormat(fmt), offset, size);
                return info.Env().Undefined();
            });
            SetMethod(o, "draw", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                wgpuRenderBundleEncoderDraw(h, ArgU32(info, 0, 0), ArgU32(info, 1, 1), ArgU32(info, 2, 0), ArgU32(info, 3, 0));
                return info.Env().Undefined();
            });
            SetMethod(o, "drawIndexed", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                wgpuRenderBundleEncoderDrawIndexed(h, ArgU32(info, 0, 0), ArgU32(info, 1, 1), ArgU32(info, 2, 0),
                    ArgI32(info, 3, 0), ArgU32(info, 4, 0));
                return info.Env().Undefined();
            });
            SetMethod(o, "pushDebugGroup", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                std::string s = (info.Length() > 0 && info[0].IsString()) ? info[0].As<Napi::String>().Utf8Value() : std::string{};
                wgpuRenderBundleEncoderPushDebugGroup(h, StrView(s));
                return info.Env().Undefined();
            });
            SetMethod(o, "popDebugGroup", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                wgpuRenderBundleEncoderPopDebugGroup(h);
                return info.Env().Undefined();
            });
            SetMethod(o, "insertDebugMarker", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                std::string s = (info.Length() > 0 && info[0].IsString()) ? info[0].As<Napi::String>().Utf8Value() : std::string{};
                wgpuRenderBundleEncoderInsertDebugMarker(h, StrView(s));
                return info.Env().Undefined();
            });
            SetMethod(o, "finish", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                Napi::Env env = info.Env();
                WGPURenderBundleDescriptor d{
                    .nextInChain = nullptr,
                    .label = EmptyStringView(),
                };
                std::string label;
                if (info.Length() > 0 && info[0].IsObject())
                {
                    label = PropStr(info[0].As<Napi::Object>(), "label");
                    if (!label.empty()) d.label = StrView(label);
                }
                return MakeRenderBundle(env, wgpuRenderBundleEncoderFinish(h, &d));
            });
            return o;
        }

        // ---- GPUCommandEncoder -----------------------------------------------
        Napi::Object MakeCommandEncoder(Napi::Env env, WGPUCommandEncoder h)
        {
            Napi::Object o = NewGPUObject(env, "GPUCommandEncoder");
            SetHandle(o, h);

            SetMethod(o, "beginRenderPass", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                Napi::Env env = info.Env();
                if (info.Length() == 0 || !info[0].IsObject())
                {
                    throw Napi::Error::New(env, "NativeDawn: beginRenderPass requires a descriptor");
                }
                Napi::Object desc = info[0].As<Napi::Object>();
                WGPURenderPassDescriptor rpd{
                    .label = EmptyStringView(),
                    .colorAttachmentCount = 0,
                    .colorAttachments = nullptr,
                };
                std::string label = PropStr(desc, "label");
                if (!label.empty()) rpd.label = StrView(label);

                // WebGPU validates scissor rects against the attachment size, so
                // remember it and hand it to the render pass encoder.
                uint32_t rtWidth = 0;
                uint32_t rtHeight = 0;
                auto noteAttachmentSize = [&rtWidth, &rtHeight](Napi::Value viewV) {
                    if (!viewV.IsObject()) return;
                    Napi::Object v = viewV.As<Napi::Object>();
                    const auto tex = reinterpret_cast<WGPUTexture>(
                        static_cast<uintptr_t>(PropF64(v, "__tex", 0)));
                    if (tex != nullptr && tex == g_state.currentSurfaceTexture)
                    {
                        g_surfaceWorkPending = true;
                    }
                    if (rtWidth != 0) return;
                    rtWidth = PropU32(v, "__width", 0);
                    rtHeight = PropU32(v, "__height", 0);
                };

                std::vector<WGPURenderPassColorAttachment> colors;
                Napi::Value caV = desc.Get("colorAttachments");
                if (caV.IsArray())
                {
                    Napi::Array arr = caV.As<Napi::Array>();
                    for (uint32_t i = 0; i < arr.Length(); ++i)
                    {
                        WGPURenderPassColorAttachment c{
                            .depthSlice = WGPU_DEPTH_SLICE_UNDEFINED,
                            .loadOp = WGPULoadOp_Undefined,
                            .storeOp = WGPUStoreOp_Undefined,
                            .clearValue = {
                                .r = 0.0,
                                .g = 0.0,
                                .b = 0.0,
                                .a = 0.0,
                            },
                        };
                        Napi::Value ev = arr.Get(i);
                        if (!ev.IsObject()) { colors.push_back(c); continue; }
                        Napi::Object ca = ev.As<Napi::Object>();
                        Napi::Value viewV = ca.Get("view");
                        WGPUTextureView* view = GetH<WGPUTextureView>(viewV);
                        if (view != nullptr) c.view = *view;
                        noteAttachmentSize(viewV);
                        if (PropPresent(ca, "depthSlice")) c.depthSlice = PropU32(ca, "depthSlice", 0);
                        WGPUTextureView* resolve = GetH<WGPUTextureView>(ca.Get("resolveTarget"));
                        if (resolve != nullptr) c.resolveTarget = *resolve;
                        c.loadOp = loadOp(PropStr(ca, "loadOp"));
                        c.storeOp = storeOp(PropStr(ca, "storeOp"));
                        if (PropPresent(ca, "clearValue")) c.clearValue = ParseColor(ca.Get("clearValue"));
                        colors.push_back(c);
                    }
                }
                rpd.colorAttachmentCount = colors.size();
                rpd.colorAttachments = colors.empty() ? nullptr : colors.data();

                WGPURenderPassDepthStencilAttachment dsa{
                    .depthLoadOp = WGPULoadOp_Undefined,
                    .depthStoreOp = WGPUStoreOp_Undefined,
                    .depthClearValue = WGPU_DEPTH_CLEAR_VALUE_UNDEFINED,
                    .depthReadOnly = false,
                    .stencilLoadOp = WGPULoadOp_Undefined,
                    .stencilStoreOp = WGPUStoreOp_Undefined,
                    .stencilClearValue = 0,
                    .stencilReadOnly = false,
                };
                Napi::Value dsV = desc.Get("depthStencilAttachment");
                if (dsV.IsObject())
                {
                    Napi::Object ds = dsV.As<Napi::Object>();
                    Napi::Value dsViewV = ds.Get("view");
                    WGPUTextureView* view = GetH<WGPUTextureView>(dsViewV);
                    if (view != nullptr) dsa.view = *view;
                    noteAttachmentSize(dsViewV);
                    if (PropPresent(ds, "depthLoadOp")) dsa.depthLoadOp = loadOp(PropStr(ds, "depthLoadOp"));
                    if (PropPresent(ds, "depthStoreOp")) dsa.depthStoreOp = storeOp(PropStr(ds, "depthStoreOp"));
                    dsa.depthClearValue = static_cast<float>(PropF64(ds, "depthClearValue", 0));
                    dsa.depthReadOnly = PropBool(ds, "depthReadOnly", false);
                    if (PropPresent(ds, "stencilLoadOp")) dsa.stencilLoadOp = loadOp(PropStr(ds, "stencilLoadOp"));
                    if (PropPresent(ds, "stencilStoreOp")) dsa.stencilStoreOp = storeOp(PropStr(ds, "stencilStoreOp"));
                    dsa.stencilClearValue = PropU32(ds, "stencilClearValue", 0);
                    dsa.stencilReadOnly = PropBool(ds, "stencilReadOnly", false);
                    rpd.depthStencilAttachment = &dsa;
                }

                return MakeRenderPassEncoder(env, wgpuCommandEncoderBeginRenderPass(h, &rpd), rtWidth, rtHeight);
            });
            SetMethod(o, "beginComputePass", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                Napi::Env env = info.Env();
                WGPUComputePassDescriptor d{
                    .nextInChain = nullptr,
                    .label = EmptyStringView(),
                };
                std::string label;
                if (info.Length() > 0 && info[0].IsObject())
                {
                    label = PropStr(info[0].As<Napi::Object>(), "label");
                    if (!label.empty()) d.label = StrView(label);
                }
                return MakeComputePassEncoder(env, wgpuCommandEncoderBeginComputePass(h, &d));
            });
            SetMethod(o, "copyBufferToBuffer", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                WGPUBuffer* src = GetH<WGPUBuffer>(info[0]);
                WGPUBuffer* dst = GetH<WGPUBuffer>(info[2]);
                if (src != nullptr && dst != nullptr)
                    wgpuCommandEncoderCopyBufferToBuffer(h, *src, ArgU64(info, 1, 0), *dst, ArgU64(info, 3, 0), ArgU64(info, 4, 0));
                return info.Env().Undefined();
            });
            SetMethod(o, "copyBufferToTexture", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                WGPUTexelCopyBufferInfo src = ParseTexelCopyBuffer(info[0].As<Napi::Object>());
                WGPUTexelCopyTextureInfo dst = ParseTexelCopyTexture(info[1].As<Napi::Object>());
                WGPUExtent3D size = ParseExtent3D(info[2]);
                wgpuCommandEncoderCopyBufferToTexture(h, &src, &dst, &size);
                return info.Env().Undefined();
            });
            SetMethod(o, "copyTextureToBuffer", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                WGPUTexelCopyTextureInfo src = ParseTexelCopyTexture(info[0].As<Napi::Object>());
                WGPUTexelCopyBufferInfo dst = ParseTexelCopyBuffer(info[1].As<Napi::Object>());
                WGPUExtent3D size = ParseExtent3D(info[2]);
                wgpuCommandEncoderCopyTextureToBuffer(h, &src, &dst, &size);
                return info.Env().Undefined();
            });
            SetMethod(o, "copyTextureToTexture", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                WGPUTexelCopyTextureInfo src = ParseTexelCopyTexture(info[0].As<Napi::Object>());
                WGPUTexelCopyTextureInfo dst = ParseTexelCopyTexture(info[1].As<Napi::Object>());
                WGPUExtent3D size = ParseExtent3D(info[2]);
                wgpuCommandEncoderCopyTextureToTexture(h, &src, &dst, &size);
                return info.Env().Undefined();
            });
            SetMethod(o, "clearBuffer", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                WGPUBuffer* b = GetH<WGPUBuffer>(info[0]);
                if (b != nullptr)
                {
                    uint64_t offset = ArgU64(info, 1, 0);
                    uint64_t size = ArgIsUndef(info, 2) ? WGPU_WHOLE_SIZE : ArgU64(info, 2, 0);
                    wgpuCommandEncoderClearBuffer(h, *b, offset, size);
                }
                return info.Env().Undefined();
            });
            SetMethod(o, "resolveQuerySet", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                WGPUQuerySet* qs = GetH<WGPUQuerySet>(info[0]);
                WGPUBuffer* dst = GetH<WGPUBuffer>(info[3]);
                if (qs != nullptr && dst != nullptr)
                    wgpuCommandEncoderResolveQuerySet(h, *qs, ArgU32(info, 1, 0), ArgU32(info, 2, 0), *dst, ArgU64(info, 4, 0));
                return info.Env().Undefined();
            });
            SetMethod(o, "pushDebugGroup", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                std::string s = (info.Length() > 0 && info[0].IsString()) ? info[0].As<Napi::String>().Utf8Value() : std::string{};
                wgpuCommandEncoderPushDebugGroup(h, StrView(s));
                return info.Env().Undefined();
            });
            SetMethod(o, "popDebugGroup", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                wgpuCommandEncoderPopDebugGroup(h);
                return info.Env().Undefined();
            });
            SetMethod(o, "insertDebugMarker", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                std::string s = (info.Length() > 0 && info[0].IsString()) ? info[0].As<Napi::String>().Utf8Value() : std::string{};
                wgpuCommandEncoderInsertDebugMarker(h, StrView(s));
                return info.Env().Undefined();
            });
            SetMethod(o, "finish", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                Napi::Env env = info.Env();
                WGPUCommandBufferDescriptor d{
                    .nextInChain = nullptr,
                    .label = EmptyStringView(),
                };
                std::string label;
                if (info.Length() > 0 && info[0].IsObject())
                {
                    label = PropStr(info[0].As<Napi::Object>(), "label");
                    if (!label.empty()) d.label = StrView(label);
                }
                return MakeCommandBuffer(env, wgpuCommandEncoderFinish(h, &d));
            });
            return o;
        }

        // ---- GPUQueue --------------------------------------------------------
        Napi::Object MakeQueue(Napi::Env env)
        {
            Napi::Object o = NewGPUObject(env, "GPUQueue");
            SetBorrowedHandle(o, g_state.queue);
            SetMethod(o, "submit", [](const Napi::CallbackInfo& info) -> Napi::Value {
                std::vector<WGPUCommandBuffer> bufs;
                if (info.Length() > 0 && info[0].IsArray())
                {
                    Napi::Array arr = info[0].As<Napi::Array>();
                    for (uint32_t i = 0; i < arr.Length(); ++i)
                    {
                        WGPUCommandBuffer* cb = GetH<WGPUCommandBuffer>(arr.Get(i));
                        if (cb != nullptr) bufs.push_back(*cb);
                    }
                }
                if (!bufs.empty())
                {
                    wgpuQueueSubmit(g_state.queue, bufs.size(), bufs.data());
                    g_surfaceWorkPending = false;
                }
                return info.Env().Undefined();
            });
            SetMethod(o, "writeBuffer", [](const Napi::CallbackInfo& info) -> Napi::Value {
                WGPUBuffer* b = GetH<WGPUBuffer>(info[0]);
                if (b == nullptr) return info.Env().Undefined();
                uint64_t bufferOffset = ArgU64(info, 1, 0);
                Bytes bytes = GetBytes(info[2]);
                if (bytes.data == nullptr) return info.Env().Undefined();
                size_t dataOffset = static_cast<size_t>(ArgU64(info, 3, 0));
                size_t length = (!ArgIsUndef(info, 4))
                    ? static_cast<size_t>(ArgU64(info, 4, 0)) : (bytes.size - dataOffset);
                wgpuQueueWriteBuffer(g_state.queue, *b, bufferOffset, bytes.data + dataOffset, length);
                return info.Env().Undefined();
            });
            SetMethod(o, "writeTexture", [](const Napi::CallbackInfo& info) -> Napi::Value {
                Napi::Env env = info.Env();
                if (!info[0].IsObject()) return env.Undefined();
                WGPUTexelCopyTextureInfo tci = ParseTexelCopyTexture(info[0].As<Napi::Object>());
                Bytes bytes = GetBytes(info[1]);
                Napi::Object layout = info[2].As<Napi::Object>();
                WGPUTexelCopyBufferLayout tbl{
                    .offset = PropU64(layout, "offset", 0),
                    .bytesPerRow = PropPresent(layout, "bytesPerRow")
                        ? PropU32(layout, "bytesPerRow", 0) : WGPU_COPY_STRIDE_UNDEFINED,
                    .rowsPerImage = PropPresent(layout, "rowsPerImage")
                        ? PropU32(layout, "rowsPerImage", 0) : WGPU_COPY_STRIDE_UNDEFINED,
                };
                WGPUExtent3D ext = ParseExtent3D(info[3]);
                if (bytes.data != nullptr)
                    wgpuQueueWriteTexture(g_state.queue, &tci, bytes.data, bytes.size, &tbl, &ext);
                return env.Undefined();
            });
            SetMethod(o, "copyExternalImageToTexture", [](const Napi::CallbackInfo& info) -> Napi::Value {
                Napi::Env env = info.Env();
                if (!info[0].IsObject() || !info[1].IsObject())
                {
                    throw Napi::Error::New(env, "copyExternalImageToTexture: bad arguments");
                }
                Napi::Object srcDesc = info[0].As<Napi::Object>();
                Napi::Value bmpVal = srcDesc.Get("source");
                if (!bmpVal.IsObject())
                {
                    throw Napi::Error::New(env, "copyExternalImageToTexture: missing source");
                }
                Napi::Object bmp = bmpVal.As<Napi::Object>();
                // A NanoVG-backed 2D canvas keeps its content on the GPU; flush and
                // mirror it into __pixels so the upload path below sees it.
                Babylon::Plugins::Internal::SyncDawnCanvasPixels(env, bmp);
                Bytes px = GetBytes(bmp.Get("__pixels"));
                uint32_t w = PropU32(bmp, "width", 0);
                uint32_t h = PropU32(bmp, "height", 0);
                if (px.data == nullptr || w == 0 || h == 0)
                {
                    // Tolerant path: some sources arrive without pixels -- e.g. GUI /
                    // DynamicTexture canvases whose 2D text/gradient content we don't
                    // rasterize, or a texture whose async decode yielded nothing.
                    // Rather than throw an uncaught error that aborts the entire
                    // scene (failing otherwise-correct tests on one incidental
                    // texture), skip this upload and leave the destination texture
                    // unchanged so the rest of the frame still renders.
                    static bool warned = false;
                    if (!warned)
                    {
                        warned = true;
                        DawnLogF(LogLevel::Warn, "copyExternalImageToTexture: source has no decoded pixels; skipping (w=%u h=%u hasPixels=%d)",
                            w, h, px.data != nullptr ? 1 : 0);
                    }
                    return env.Undefined();
                }
                bool flipY = false;
                {
                    Napi::Value f = srcDesc.Get("flipY");
                    if (f.IsBoolean()) flipY = f.As<Napi::Boolean>().Value();
                }

                Napi::Object destDesc = info[1].As<Napi::Object>();
                WGPUTexelCopyTextureInfo tci = ParseTexelCopyTexture(destDesc);
                WGPUExtent3D ext = ParseExtent3D(info[2]);
                if (ext.width == 0) ext.width = w;
                if (ext.height == 0) ext.height = h;
                if (ext.depthOrArrayLayers == 0) ext.depthOrArrayLayers = 1;

                // Clamp the copy extent to the destination texture's mip bounds.
                // Source canvases/bitmaps can be a pixel larger than the texture
                // Babylon created from a fractional CSS/GUI size (e.g. 3380x103
                // into a 3379x102 texture); an out-of-bounds WriteTexture is a
                // Dawn validation error that loses the whole device and cascades
                // into every subsequent test failing.
                if (tci.texture)
                {
                    const uint32_t mip = tci.mipLevel;
                    uint32_t tw = wgpuTextureGetWidth(tci.texture) >> mip;  if (tw == 0) tw = 1;
                    uint32_t th = wgpuTextureGetHeight(tci.texture) >> mip; if (th == 0) th = 1;
                    const uint32_t availW = (tci.origin.x < tw) ? (tw - tci.origin.x) : 0u;
                    const uint32_t availH = (tci.origin.y < th) ? (th - tci.origin.y) : 0u;
                    if (ext.width > availW) ext.width = availW;
                    if (ext.height > availH) ext.height = availH;
                    if (ext.width == 0 || ext.height == 0) return env.Undefined();
                }

                // Destination texture format (the spec allows the source RGBA8 to
                // be converted to the destination format on copy).
                std::string fmt;
                {
                    Napi::Value texV = destDesc.Get("texture");
                    if (texV.IsObject())
                    {
                        Napi::Value fv = texV.As<Napi::Object>().Get("format");
                        if (fv.IsString()) fmt = fv.As<Napi::String>().Utf8Value();
                    }
                }

                // Access the source RGBA8 row, honoring flipY.
                const uint32_t srcRowBytes = w * 4u;
                auto srcRow = [&](uint32_t y) -> const uint8_t* {
                    uint32_t sy = flipY ? (h - 1 - y) : y;
                    return px.data + static_cast<size_t>(sy) * srcRowBytes;
                };

                WGPUTexelCopyBufferLayout tbl{
                    .offset = 0,
                    .bytesPerRow = WGPU_COPY_STRIDE_UNDEFINED,
                    .rowsPerImage = h,
                };

                if (fmt == "rgba16float")
                {
                    // Convert 8-bit [0,255] -> normalized [0,1] -> half float.
                    std::vector<uint16_t> half(static_cast<size_t>(w) * h * 4u);
                    for (uint32_t y = 0; y < h; ++y)
                    {
                        const uint8_t* row = srcRow(y);
                        uint16_t* dst = &half[static_cast<size_t>(y) * w * 4u];
                        for (uint32_t i = 0; i < w * 4u; ++i)
                        {
                            dst[i] = FloatToHalf(static_cast<float>(row[i]) / 255.0f);
                        }
                    }
                    tbl.bytesPerRow = w * 8u;
                    wgpuQueueWriteTexture(g_state.queue, &tci, half.data(),
                        static_cast<size_t>(w) * h * 8u, &tbl, &ext);
                }
                else if (fmt == "rgba32float")
                {
                    std::vector<float> f(static_cast<size_t>(w) * h * 4u);
                    for (uint32_t y = 0; y < h; ++y)
                    {
                        const uint8_t* row = srcRow(y);
                        float* dst = &f[static_cast<size_t>(y) * w * 4u];
                        for (uint32_t i = 0; i < w * 4u; ++i)
                        {
                            dst[i] = static_cast<float>(row[i]) / 255.0f;
                        }
                    }
                    tbl.bytesPerRow = w * 16u;
                    wgpuQueueWriteTexture(g_state.queue, &tci, f.data(),
                        static_cast<size_t>(w) * h * 16u, &tbl, &ext);
                }
                else if (fmt == "bgra8unorm" || fmt == "bgra8unorm-srgb")
                {
                    // Swap R/B from the RGBA8 source.
                    std::vector<uint8_t> bgra(static_cast<size_t>(w) * h * 4u);
                    for (uint32_t y = 0; y < h; ++y)
                    {
                        const uint8_t* row = srcRow(y);
                        uint8_t* dst = &bgra[static_cast<size_t>(y) * srcRowBytes];
                        for (uint32_t x = 0; x < w; ++x)
                        {
                            dst[x * 4 + 0] = row[x * 4 + 2];
                            dst[x * 4 + 1] = row[x * 4 + 1];
                            dst[x * 4 + 2] = row[x * 4 + 0];
                            dst[x * 4 + 3] = row[x * 4 + 3];
                        }
                    }
                    tbl.bytesPerRow = srcRowBytes;
                    wgpuQueueWriteTexture(g_state.queue, &tci, bgra.data(),
                        static_cast<size_t>(srcRowBytes) * h, &tbl, &ext);
                }
                else
                {
                    // rgba8unorm / rgba8unorm-srgb and default: copy as-is (with
                    // flipY applied per row if needed).
                    tbl.bytesPerRow = srcRowBytes;
                    if (flipY)
                    {
                        std::vector<uint8_t> flipped(static_cast<size_t>(srcRowBytes) * h);
                        for (uint32_t y = 0; y < h; ++y)
                        {
                            std::memcpy(&flipped[static_cast<size_t>(y) * srcRowBytes], srcRow(y), srcRowBytes);
                        }
                        wgpuQueueWriteTexture(g_state.queue, &tci, flipped.data(),
                            static_cast<size_t>(srcRowBytes) * h, &tbl, &ext);
                    }
                    else
                    {
                        wgpuQueueWriteTexture(g_state.queue, &tci, px.data,
                            static_cast<size_t>(srcRowBytes) * h, &tbl, &ext);
                    }
                }
                return env.Undefined();
            });
            SetMethod(o, "onSubmittedWorkDone", [](const Napi::CallbackInfo& info) -> Napi::Value {
                Napi::Env env = info.Env();
                auto d = Napi::Promise::Deferred::New(env);
                WGPUQueueWorkDoneCallbackInfo cb{
                    .mode = WGPUCallbackMode_WaitAnyOnly,
                    .callback = [](WGPUQueueWorkDoneStatus, WGPUStringView, void*, void*) {},
                };
                WaitFuture(wgpuQueueOnSubmittedWorkDone(g_state.queue, cb));
                d.Resolve(env.Undefined());
                return d.Promise();
            });
            o.Set("label", Napi::String::New(env, ""));
            return o;
        }

        // ---- GPUDevice -------------------------------------------------------
        Napi::Object MakeDevice(Napi::Env env)
        {
            Napi::Object o = NewGPUObject(env, "GPUDevice");
            SetBorrowedHandle(o, g_state.device);
            o.Set("queue", MakeQueue(env));
            o.Set("label", Napi::String::New(env, ""));

            o.Set("features", MakeFeatureSet(env, [](const std::string& name) {
                return name.empty() ? false : static_cast<bool>(wgpuDeviceHasFeature(g_state.device, featureName(name)));
            }));
            {
                WGPULimits L = DefaultLimits();
                wgpuDeviceGetLimits(g_state.device, &L);
                Napi::Object limits = Napi::Object::New(env);
                FillLimits(limits, L);
                o.Set("limits", limits);
            }
            // lost: a promise that never resolves.
            {
                auto lost = Napi::Promise::Deferred::New(env);
                o.Set("lost", lost.Promise());
            }

            SetMethod(o, "createBuffer", [](const Napi::CallbackInfo& info) -> Napi::Value {
                Napi::Env env = info.Env();
                Napi::Object desc = info[0].As<Napi::Object>();
                const uint64_t size = PropU64(desc, "size", 0);
                uint32_t usage = PropU32(desc, "usage", 0);
                bool mapped = PropBool(desc, "mappedAtCreation", false);
                WGPUBufferDescriptor bd{
                    .label = EmptyStringView(),
                    .usage = WGPUBufferUsage(usage),
                    .size = size,
                    .mappedAtCreation = mapped,
                };
                std::string label = PropStr(desc, "label");
                if (!label.empty()) bd.label = StrView(label);
                WGPUBuffer buf = wgpuDeviceCreateBuffer(g_state.device, &bd);
                if (!buf) throw Napi::Error::New(env, "NativeDawn: createBuffer failed");
                return MakeBuffer(env, buf, bd.size, usage, mapped);
            });
            SetMethod(o, "createTexture", [](const Napi::CallbackInfo& info) -> Napi::Value {
                Napi::Env env = info.Env();
                Napi::Object desc = info[0].As<Napi::Object>();
                WGPUTextureDescriptor td{
                    .label = EmptyStringView(),
                    .usage = WGPUTextureUsage_None,
                    .dimension = WGPUTextureDimension_Undefined,
                    .size = ParseExtent3D(desc.Get("size")),
                    .format = WGPUTextureFormat_Undefined,
                    .mipLevelCount = PropU32(desc, "mipLevelCount", 1),
                    .sampleCount = PropU32(desc, "sampleCount", 1),
                };
                std::string fmt = PropStr(desc, "format");
                td.format = textureFormat(fmt);
                uint32_t usage = PropU32(desc, "usage", 0);
                td.usage = WGPUTextureUsage(usage);
                std::string dim = PropStr(desc, "dimension");
                td.dimension = textureDimension(dim);
                std::vector<WGPUTextureFormat> viewFormats;
                Napi::Value vf = desc.Get("viewFormats");
                if (vf.IsArray())
                {
                    Napi::Array a = vf.As<Napi::Array>();
                    for (uint32_t i = 0; i < a.Length(); ++i)
                    {
                        Napi::Value e = a.Get(i);
                        if (e.IsString()) viewFormats.push_back(textureFormat(e.As<Napi::String>().Utf8Value()));
                    }
                    td.viewFormatCount = viewFormats.size();
                    td.viewFormats = viewFormats.data();
                }
                std::string label = PropStr(desc, "label");
                if (!label.empty()) td.label = StrView(label);
                WGPUTexture tex = wgpuDeviceCreateTexture(g_state.device, &td);
                if (!tex) throw Napi::Error::New(env, "NativeDawn: createTexture failed");
                return MakeTexture(env, tex, td.size.width, td.size.height, td.size.depthOrArrayLayers,
                    td.mipLevelCount, td.sampleCount, fmt, usage, dim);
            });
            SetMethod(o, "createSampler", [](const Napi::CallbackInfo& info) -> Napi::Value {
                Napi::Env env = info.Env();
                WGPUSamplerDescriptor sd{
                    .label = EmptyStringView(),
                    .addressModeU = WGPUAddressMode_Undefined,
                    .addressModeV = WGPUAddressMode_Undefined,
                    .addressModeW = WGPUAddressMode_Undefined,
                    .magFilter = WGPUFilterMode_Undefined,
                    .minFilter = WGPUFilterMode_Undefined,
                    .mipmapFilter = WGPUMipmapFilterMode_Undefined,
                    .lodMinClamp = 0.0f,
                    .lodMaxClamp = 32.0f,
                    .compare = WGPUCompareFunction_Undefined,
                    .maxAnisotropy = 1,
                };
                if (info.Length() > 0 && info[0].IsObject())
                {
                    Napi::Object desc = info[0].As<Napi::Object>();
                    sd.addressModeU = addressMode(PropStr(desc, "addressModeU"));
                    sd.addressModeV = addressMode(PropStr(desc, "addressModeV"));
                    sd.addressModeW = addressMode(PropStr(desc, "addressModeW"));
                    sd.magFilter = filterMode(PropStr(desc, "magFilter"));
                    sd.minFilter = filterMode(PropStr(desc, "minFilter"));
                    sd.mipmapFilter = mipmapFilterMode(PropStr(desc, "mipmapFilter"));
                    sd.lodMinClamp = static_cast<float>(PropF64(desc, "lodMinClamp", 0.0));
                    sd.lodMaxClamp = static_cast<float>(PropF64(desc, "lodMaxClamp", 32.0));
                    if (PropPresent(desc, "compare")) sd.compare = compareFunction(PropStr(desc, "compare"));
                    sd.maxAnisotropy = static_cast<uint16_t>(PropU32(desc, "maxAnisotropy", 1));
                }
                WGPUSampler s = wgpuDeviceCreateSampler(g_state.device, &sd);
                return MakeSampler(env, s);
            });
            SetMethod(o, "createShaderModule", [](const Napi::CallbackInfo& info) -> Napi::Value {
                Napi::Env env = info.Env();
                Napi::Object desc = info[0].As<Napi::Object>();
                std::string code = PropStr(desc, "code");
                WGPUShaderSourceWGSL wgsl{
                    .chain = {
                        .sType = WGPUSType_ShaderSourceWGSL,
                    },
                    .code = StrView(code),
                };
                WGPUShaderModuleDescriptor smd{
                    .nextInChain = &wgsl.chain,
                    .label = EmptyStringView(),
                };
                std::string label = PropStr(desc, "label");
                if (!label.empty()) smd.label = StrView(label);
                WGPUShaderModule m = wgpuDeviceCreateShaderModule(g_state.device, &smd);
                return MakeShaderModule(env, m);
            });
            SetMethod(o, "createBindGroupLayout", [](const Napi::CallbackInfo& info) -> Napi::Value {
                Napi::Env env = info.Env();
                Napi::Object desc = info[0].As<Napi::Object>();
                Napi::Value entriesV = desc.Get("entries");
                std::vector<WGPUBindGroupLayoutEntry> entries;
                if (entriesV.IsArray())
                {
                    Napi::Array arr = entriesV.As<Napi::Array>();
                    entries.resize(arr.Length());
                    for (uint32_t i = 0; i < arr.Length(); ++i)
                    {
                        Napi::Object e = arr.Get(i).As<Napi::Object>();
                        WGPUBindGroupLayoutEntry& dst = entries[i];
                        dst.binding = PropU32(e, "binding", 0);
                        dst.visibility = WGPUShaderStage(PropU32(e, "visibility", 0));
                        if (e.Get("buffer").IsObject())
                        {
                            Napi::Object b = e.Get("buffer").As<Napi::Object>();
                            dst.buffer.type = bufferBindingType(PropStrOr(b, "type", "uniform"));
                            dst.buffer.hasDynamicOffset = PropBool(b, "hasDynamicOffset", false);
                            dst.buffer.minBindingSize = PropU64(b, "minBindingSize", 0);
                        }
                        if (e.Get("sampler").IsObject())
                        {
                            Napi::Object s = e.Get("sampler").As<Napi::Object>();
                            dst.sampler.type = samplerBindingType(PropStrOr(s, "type", "filtering"));
                        }
                        if (e.Get("texture").IsObject())
                        {
                            Napi::Object t = e.Get("texture").As<Napi::Object>();
                            dst.texture.sampleType = textureSampleType(PropStrOr(t, "sampleType", "float"));
                            dst.texture.viewDimension = textureViewDimension(PropStrOr(t, "viewDimension", "2d"));
                            dst.texture.multisampled = PropBool(t, "multisampled", false);
                        }
                        if (e.Get("storageTexture").IsObject())
                        {
                            Napi::Object st = e.Get("storageTexture").As<Napi::Object>();
                            dst.storageTexture.access = storageTextureAccess(PropStrOr(st, "access", "write-only"));
                            dst.storageTexture.format = textureFormat(PropStr(st, "format"));
                            dst.storageTexture.viewDimension = textureViewDimension(PropStrOr(st, "viewDimension", "2d"));
                        }
                    }
                }
                WGPUBindGroupLayoutDescriptor bgld{
                    .label = EmptyStringView(),
                    .entryCount = entries.size(),
                    .entries = entries.empty() ? nullptr : entries.data(),
                };
                std::string label = PropStr(desc, "label");
                if (!label.empty()) bgld.label = StrView(label);
                WGPUBindGroupLayout bgl = wgpuDeviceCreateBindGroupLayout(g_state.device, &bgld);
                return MakeBindGroupLayout(env, bgl);
            });
            SetMethod(o, "createBindGroup", [](const Napi::CallbackInfo& info) -> Napi::Value {
                Napi::Env env = info.Env();
                Napi::Object desc = info[0].As<Napi::Object>();
                WGPUBindGroupLayout* layout = GetH<WGPUBindGroupLayout>(desc.Get("layout"));
                if (layout == nullptr) throw Napi::Error::New(env, "NativeDawn: createBindGroup missing layout");
                Napi::Value entriesV = desc.Get("entries");
                std::vector<WGPUBindGroupEntry> entries;
                if (entriesV.IsArray())
                {
                    Napi::Array arr = entriesV.As<Napi::Array>();
                    entries.resize(arr.Length());
                    for (uint32_t i = 0; i < arr.Length(); ++i)
                    {
                        Napi::Object e = arr.Get(i).As<Napi::Object>();
                        WGPUBindGroupEntry& dst = entries[i];
                        dst.binding = PropU32(e, "binding", 0);
                        Napi::Value resource = e.Get("resource");
                        if (!resource.IsObject()) continue;
                        Napi::Object ro = resource.As<Napi::Object>();
                        if (ro.Has("buffer") && ro.Get("buffer").IsObject())
                        {
                            WGPUBuffer* b = GetH<WGPUBuffer>(ro.Get("buffer"));
                            if (b != nullptr)
                            {
                                dst.buffer = *b;
                                dst.offset = PropU64(ro, "offset", 0);
                                dst.size = PropPresent(ro, "size") ? PropU64(ro, "size", 0) : WGPU_WHOLE_SIZE;
                            }
                        }
                        else
                        {
                            std::string ty = TypeTag(resource);
                            if (ty == "GPUSampler")
                            {
                                WGPUSampler* s = GetH<WGPUSampler>(resource);
                                if (s != nullptr) dst.sampler = *s;
                            }
                            else if (ty == "GPUTextureView")
                            {
                                WGPUTextureView* v = GetH<WGPUTextureView>(resource);
                                if (v != nullptr) dst.textureView = *v;
                            }
                        }
                    }
                }
                WGPUBindGroupDescriptor bgd{
                    .label = EmptyStringView(),
                    .layout = *layout,
                    .entryCount = entries.size(),
                    .entries = entries.empty() ? nullptr : entries.data(),
                };
                std::string label = PropStr(desc, "label");
                if (!label.empty()) bgd.label = StrView(label);
                WGPUBindGroup bg = wgpuDeviceCreateBindGroup(g_state.device, &bgd);
                return MakeBindGroup(env, bg);
            });
            SetMethod(o, "createPipelineLayout", [](const Napi::CallbackInfo& info) -> Napi::Value {
                Napi::Env env = info.Env();
                Napi::Object desc = info[0].As<Napi::Object>();
                std::vector<WGPUBindGroupLayout> layouts;
                Napi::Value bglsV = desc.Get("bindGroupLayouts");
                if (bglsV.IsArray())
                {
                    Napi::Array arr = bglsV.As<Napi::Array>();
                    for (uint32_t i = 0; i < arr.Length(); ++i)
                    {
                        WGPUBindGroupLayout* l = GetH<WGPUBindGroupLayout>(arr.Get(i));
                        if (l != nullptr) layouts.push_back(*l);
                    }
                }
                WGPUPipelineLayoutDescriptor pld{
                    .label = EmptyStringView(),
                    .bindGroupLayoutCount = layouts.size(),
                    .bindGroupLayouts = layouts.empty() ? nullptr : layouts.data(),
                };
                std::string label = PropStr(desc, "label");
                if (!label.empty()) pld.label = StrView(label);
                WGPUPipelineLayout pl = wgpuDeviceCreatePipelineLayout(g_state.device, &pld);
                return MakePipelineLayout(env, pl);
            });
            SetMethod(o, "createRenderPipeline", [](const Napi::CallbackInfo& info) -> Napi::Value {
                return DoCreateRenderPipeline(info.Env(), info[0]);
            });
            SetMethod(o, "createRenderPipelineAsync", [](const Napi::CallbackInfo& info) -> Napi::Value {
                Napi::Env env = info.Env();
                auto d = Napi::Promise::Deferred::New(env);
                d.Resolve(DoCreateRenderPipeline(env, info[0]));
                return d.Promise();
            });
            SetMethod(o, "createComputePipeline", [](const Napi::CallbackInfo& info) -> Napi::Value {
                return DoCreateComputePipeline(info.Env(), info[0]);
            });
            SetMethod(o, "createComputePipelineAsync", [](const Napi::CallbackInfo& info) -> Napi::Value {
                Napi::Env env = info.Env();
                auto d = Napi::Promise::Deferred::New(env);
                d.Resolve(DoCreateComputePipeline(env, info[0]));
                return d.Promise();
            });
            SetMethod(o, "createCommandEncoder", [](const Napi::CallbackInfo& info) -> Napi::Value {
                Napi::Env env = info.Env();
                WGPUCommandEncoderDescriptor ced{
                    .nextInChain = nullptr,
                    .label = EmptyStringView(),
                };
                std::string label;
                if (info.Length() > 0 && info[0].IsObject())
                {
                    label = PropStr(info[0].As<Napi::Object>(), "label");
                    if (!label.empty()) ced.label = StrView(label);
                }
                return MakeCommandEncoder(env, wgpuDeviceCreateCommandEncoder(g_state.device, &ced));
            });
            SetMethod(o, "createRenderBundleEncoder", [](const Napi::CallbackInfo& info) -> Napi::Value {
                Napi::Env env = info.Env();
                Napi::Object desc = info[0].As<Napi::Object>();
                WGPURenderBundleEncoderDescriptor rbed{
                    .label = EmptyStringView(),
                    .colorFormatCount = 0,
                    .colorFormats = nullptr,
                    .depthStencilFormat = WGPUTextureFormat_Undefined,
                    .sampleCount = 1,
                    .depthReadOnly = false,
                    .stencilReadOnly = false,
                };
                std::vector<WGPUTextureFormat> cf;
                Napi::Value cfV = desc.Get("colorFormats");
                if (cfV.IsArray())
                {
                    Napi::Array a = cfV.As<Napi::Array>();
                    for (uint32_t i = 0; i < a.Length(); ++i)
                    {
                        Napi::Value e = a.Get(i);
                        cf.push_back(e.IsString() ? textureFormat(e.As<Napi::String>().Utf8Value())
                                                  : WGPUTextureFormat_Undefined);
                    }
                }
                rbed.colorFormatCount = cf.size();
                rbed.colorFormats = cf.empty() ? nullptr : cf.data();
                std::string dsFmt = PropStr(desc, "depthStencilFormat");
                if (!dsFmt.empty()) rbed.depthStencilFormat = textureFormat(dsFmt);
                rbed.sampleCount = PropU32(desc, "sampleCount", 1);
                rbed.depthReadOnly = PropBool(desc, "depthReadOnly", false);
                rbed.stencilReadOnly = PropBool(desc, "stencilReadOnly", false);
                std::string label = PropStr(desc, "label");
                if (!label.empty()) rbed.label = StrView(label);
                return MakeRenderBundleEncoder(env, wgpuDeviceCreateRenderBundleEncoder(g_state.device, &rbed));
            });
            SetMethod(o, "createQuerySet", [](const Napi::CallbackInfo& info) -> Napi::Value {
                Napi::Env env = info.Env();
                Napi::Object desc = info[0].As<Napi::Object>();
                uint32_t count = PropU32(desc, "count", 0);
                WGPUQuerySetDescriptor qsd{
                    .label = EmptyStringView(),
                    .type = queryType(PropStr(desc, "type")),
                    .count = count,
                };
                return MakeQuerySet(env, wgpuDeviceCreateQuerySet(g_state.device, &qsd), count);
            });
            SetMethod(o, "pushErrorScope", [](const Napi::CallbackInfo& info) -> Napi::Value {
                return info.Env().Undefined();
            });
            SetMethod(o, "popErrorScope", [](const Napi::CallbackInfo& info) -> Napi::Value {
                Napi::Env env = info.Env();
                auto d = Napi::Promise::Deferred::New(env);
                d.Resolve(env.Null());
                return d.Promise();
            });
            SetMethod(o, "destroy", [](const Napi::CallbackInfo& info) -> Napi::Value {
                // No-op: the shared g_state.device must stay alive.
                return info.Env().Undefined();
            });
            SetMethod(o, "addEventListener", [](const Napi::CallbackInfo& info) -> Napi::Value {
                return info.Env().Undefined();
            });
            SetMethod(o, "removeEventListener", [](const Napi::CallbackInfo& info) -> Napi::Value {
                return info.Env().Undefined();
            });
            return o;
        }

        // ---- GPUAdapter ------------------------------------------------------
        Napi::Object MakeAdapterInfo(Napi::Env env)
        {
            Napi::Object i = Napi::Object::New(env);
            WGPUAdapterInfo info{
                .nextInChain = nullptr,
                .vendor = EmptyStringView(),
                .architecture = EmptyStringView(),
                .device = EmptyStringView(),
                .description = EmptyStringView(),
                .backendType = WGPUBackendType_Undefined,
            };
            wgpuAdapterGetInfo(g_state.adapter, &info);
            i.Set("vendor", Napi::String::New(env, SvToStr(info.vendor)));
            i.Set("architecture", Napi::String::New(env, SvToStr(info.architecture)));
            i.Set("device", Napi::String::New(env, SvToStr(info.device)));
            i.Set("description", Napi::String::New(env, SvToStr(info.description)));
            wgpuAdapterInfoFreeMembers(info);
            return i;
        }

        Napi::Object MakeAdapter(Napi::Env env)
        {
            Napi::Object o = NewGPUObject(env, "GPUAdapter");
            SetBorrowedHandle(o, g_state.adapter);
            o.Set("isFallbackAdapter", Napi::Boolean::New(env, false));

            o.Set("features", MakeFeatureSet(env, [](const std::string& name) {
                return name.empty() ? false : static_cast<bool>(wgpuAdapterHasFeature(g_state.adapter, featureName(name)));
            }));
            {
                WGPULimits L = DefaultLimits();
                wgpuAdapterGetLimits(g_state.adapter, &L);
                Napi::Object limits = Napi::Object::New(env);
                FillLimits(limits, L);
                o.Set("limits", limits);
            }
            o.Set("info", MakeAdapterInfo(env));
            SetMethod(o, "requestAdapterInfo", [](const Napi::CallbackInfo& info) -> Napi::Value {
                Napi::Env env = info.Env();
                auto d = Napi::Promise::Deferred::New(env);
                d.Resolve(MakeAdapterInfo(env));
                return d.Promise();
            });
            SetMethod(o, "requestDevice", [](const Napi::CallbackInfo& info) -> Napi::Value {
                Napi::Env env = info.Env();
                auto d = Napi::Promise::Deferred::New(env);
                d.Resolve(MakeDevice(env));
                return d.Promise();
            });
            return o;
        }

        // ---- GPUCanvasContext ------------------------------------------------
        Napi::Object MakeCanvasContext(Napi::Env env)
        {
            Napi::Object o = NewGPUObject(env, "GPUCanvasContext");
            SetMethod(o, "configure", [](const Napi::CallbackInfo& info) -> Napi::Value {
                Napi::Env env = info.Env();
                if (info.Length() == 0 || !info[0].IsObject()) return env.Undefined();
                Napi::Object desc = info[0].As<Napi::Object>();
                std::string fmt = PropStr(desc, "format");
                WGPUTextureFormat f = fmt.empty() ? g_state.surfaceFormat : textureFormat(fmt);
                g_state.surfaceFormat = f;
                uint32_t usage = PropU32(desc, "usage", static_cast<uint32_t>(WGPUTextureUsage_RenderAttachment));
                std::string am = PropStr(desc, "alphaMode");
                g_surfaceUsage = WGPUTextureUsage(usage | static_cast<uint32_t>(WGPUTextureUsage_CopySrc));
                g_surfaceAlphaMode = (am == "premultiplied")
                    ? WGPUCompositeAlphaMode_Premultiplied : WGPUCompositeAlphaMode_Opaque;
                WGPUSurfaceConfiguration cfg{
                    .device = g_state.device,
                    .format = f,
                    .usage = g_surfaceUsage,
                    .width = g_state.width > 1 ? g_state.width : 1,
                    .height = g_state.height > 1 ? g_state.height : 1,
                    .alphaMode = g_surfaceAlphaMode,
                    .presentMode = WGPUPresentMode_Fifo,
                };
                wgpuSurfaceConfigure(g_state.surface, &cfg);
                g_surfaceConfigured = true;
                return env.Undefined();
            });
            SetMethod(o, "unconfigure", [](const Napi::CallbackInfo& info) -> Napi::Value {
                if (g_surfaceConfigured)
                {
                    wgpuSurfaceUnconfigure(g_state.surface);
                    g_surfaceConfigured = false;
                }
                return info.Env().Undefined();
            });
            SetMethod(o, "getCurrentTexture", [](const Napi::CallbackInfo& info) -> Napi::Value {
                Napi::Env env = info.Env();
                // Safe point: no texture is outstanding yet this frame, so a
                // resize requested mid-render-loop can be applied now.
                ApplyPendingSurfaceResize();
                WGPUSurfaceTexture st{
                    .nextInChain = nullptr,
                };
                wgpuSurfaceGetCurrentTexture(g_state.surface, &st);
                if (!st.texture) throw Napi::Error::New(env, "NativeDawn: getCurrentTexture returned null");
                g_currentTextureAcquired = true;
                if (g_state.currentSurfaceTexture) wgpuTextureRelease(g_state.currentSurfaceTexture);
                wgpuTextureAddRef(st.texture);
                g_state.currentSurfaceTexture = st.texture;
                return MakeTexture(env, st.texture, g_state.width, g_state.height, 1, 1, 1,
                    textureFormatStr(g_state.surfaceFormat),
                    static_cast<uint32_t>(WGPUTextureUsage_RenderAttachment), "2d");
            });
            SetMethod(o, "getConfiguration", [](const Napi::CallbackInfo& info) -> Napi::Value {
                Napi::Env env = info.Env();
                Napi::Object c = Napi::Object::New(env);
                c.Set("format", Napi::String::New(env, textureFormatStr(g_state.surfaceFormat)));
                return c;
            });
            return o;
        }

        // __WGPU_BUILDERS__

        // ---- top-level installation -----------------------------------------
        void InstallWebGPU(Napi::Env env)
        {
            Napi::Object global = env.Global();

            Napi::Object navigator;
            Napi::Value navV = global.Get("navigator");
            if (navV.IsObject())
            {
                navigator = navV.As<Napi::Object>();
            }
            else
            {
                navigator = Napi::Object::New(env);
                global.Set("navigator", navigator);
            }

            Napi::Object gpu = Napi::Object::New(env);
            SetMethod(gpu, "requestAdapter", [](const Napi::CallbackInfo& info) -> Napi::Value {
                Napi::Env env = info.Env();
                auto d = Napi::Promise::Deferred::New(env);
                d.Resolve(MakeAdapter(env));
                return d.Promise();
            });
            SetMethod(gpu, "getPreferredCanvasFormat", [](const Napi::CallbackInfo& info) -> Napi::Value {
                return Napi::String::New(info.Env(), textureFormatStr(g_state.surfaceFormat));
            });
            Napi::Object wlf = Napi::Object::New(env);
            SetMethod(wlf, "has", [](const Napi::CallbackInfo& info) -> Napi::Value {
                return Napi::Boolean::New(info.Env(), false);
            });
            wlf.Set("size", Napi::Number::New(env, 0));
            gpu.Set("wgslLanguageFeatures", wlf);
            navigator.Set("gpu", gpu);

            SetMethod(global, "_nativeDawnGetContext", [](const Napi::CallbackInfo& info) -> Napi::Value {
                return MakeCanvasContext(info.Env());
            });
            SetMethod(global, "_nativeDawnPresent", [](const Napi::CallbackInfo& info) -> Napi::Value {
                if (g_surfaceConfigured && g_currentTextureAcquired && !g_surfaceWorkPending)
                {
                    wgpuSurfacePresent(g_state.surface);
                    g_currentTextureAcquired = false;
                }
                if (g_state.instance)
                {
                    wgpuInstanceProcessEvents(g_state.instance);
                }
                return info.Env().Undefined();
            });

            // Expose the real Dawn surface (drawing buffer) size so the JS canvas
            // can match it exactly (avoids color/depth attachment size mismatch).
            SetMethod(global, "_nativeDawnSurfaceSize", [](const Napi::CallbackInfo& info) -> Napi::Value {
                Napi::Object size = Napi::Object::New(info.Env());
                size.Set("width", Napi::Number::New(info.Env(), g_state.width));
                size.Set("height", Napi::Number::New(info.Env(), g_state.height));
                return size;
            });

            // ---- Validation-harness support --------------------------------------
            // Read back the most recently acquired surface texture as tightly-packed
            // top-down RGBA8. Returns {width,height,data(ArrayBuffer)}. Used by the
            // Dawn test shim's TestUtils.getFrameBufferData for pixel comparison.
            SetMethod(global, "_nativeDawnReadPixels", [](const Napi::CallbackInfo& info) -> Napi::Value {
                Napi::Env env = info.Env();
                if (!g_state.currentSurfaceTexture)
                {
                    throw Napi::Error::New(env, "_nativeDawnReadPixels: no surface texture acquired");
                }
                const uint32_t w = g_state.width;
                const uint32_t h = g_state.height;
                const uint32_t unpadded = w * 4u;
                const uint32_t padded = (unpadded + 255u) & ~255u;
                const uint64_t bufSize = static_cast<uint64_t>(padded) * h;

                WGPUBufferDescriptor bd{
                    .label = EmptyStringView(),
                    .usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_MapRead,
                    .size = bufSize,
                };
                WGPUBuffer buf = wgpuDeviceCreateBuffer(g_state.device, &bd);

                WGPUTexelCopyTextureInfo src{
                    .texture = g_state.currentSurfaceTexture,
                    .mipLevel = 0,
                    .origin = {
                        .x = 0,
                        .y = 0,
                        .z = 0,
                    },
                    .aspect = WGPUTextureAspect_All,
                };
                WGPUTexelCopyBufferInfo dst{
                    .layout = {
                        .offset = 0,
                        .bytesPerRow = padded,
                        .rowsPerImage = h,
                    },
                    .buffer = buf,
                };
                WGPUExtent3D ext{
                    .width = w,
                    .height = h,
                    .depthOrArrayLayers = 1,
                };

                WGPUCommandEncoder enc = wgpuDeviceCreateCommandEncoder(g_state.device, nullptr);
                wgpuCommandEncoderCopyTextureToBuffer(enc, &src, &dst, &ext);
                WGPUCommandBuffer cmd = wgpuCommandEncoderFinish(enc, nullptr);
                wgpuQueueSubmit(g_state.queue, 1, &cmd);

                WGPUBufferMapCallbackInfo mapCb{
                    .mode = WGPUCallbackMode_WaitAnyOnly,
                    .callback = [](WGPUMapAsyncStatus, WGPUStringView, void*, void*) {},
                };
                WaitFuture(wgpuBufferMapAsync(buf, WGPUMapMode_Read, 0, bufSize, mapCb));

                const uint8_t* mapped = static_cast<const uint8_t*>(wgpuBufferGetConstMappedRange(buf, 0, bufSize));
                const size_t outSize = static_cast<size_t>(unpadded) * h;
                Napi::ArrayBuffer ab = Napi::ArrayBuffer::New(env, outSize);
                uint8_t* out = static_cast<uint8_t*>(ab.Data());
                const bool bgra = (g_state.surfaceFormat == WGPUTextureFormat_BGRA8Unorm ||
                                   g_state.surfaceFormat == WGPUTextureFormat_BGRA8UnormSrgb);
                if (mapped != nullptr)
                {
                    for (uint32_t y = 0; y < h; ++y)
                    {
                        const uint8_t* srcRow = mapped + static_cast<size_t>(y) * padded;
                        uint8_t* dstRow = out + static_cast<size_t>(y) * unpadded;
                        for (uint32_t x = 0; x < w; ++x)
                        {
                            const uint8_t c0 = srcRow[x * 4 + 0];
                            const uint8_t c1 = srcRow[x * 4 + 1];
                            const uint8_t c2 = srcRow[x * 4 + 2];
                            const uint8_t c3 = srcRow[x * 4 + 3];
                            if (bgra)
                            {
                                dstRow[x * 4 + 0] = c2;
                                dstRow[x * 4 + 1] = c1;
                                dstRow[x * 4 + 2] = c0;
                                dstRow[x * 4 + 3] = c3;
                            }
                            else
                            {
                                dstRow[x * 4 + 0] = c0;
                                dstRow[x * 4 + 1] = c1;
                                dstRow[x * 4 + 2] = c2;
                                dstRow[x * 4 + 3] = c3;
                            }
                        }
                    }
                }
                wgpuBufferUnmap(buf);
                wgpuCommandBufferRelease(cmd);
                wgpuCommandEncoderRelease(enc);
                wgpuBufferRelease(buf);

                Napi::Object res = Napi::Object::New(env);
                res.Set("width", Napi::Number::New(env, w));
                res.Set("height", Napi::Number::New(env, h));
                res.Set("data", ab);
                return res;
            });

            // Resize the window client area + Dawn surface (used by
            // TestUtils.updateSize so the framebuffer matches reference-image size).
            SetMethod(global, "_nativeDawnResize", [](const Napi::CallbackInfo& info) -> Napi::Value {
                Napi::Env env = info.Env();
                uint32_t w = info.Length() > 0 && info[0].IsNumber() ? info[0].As<Napi::Number>().Uint32Value() : g_state.width;
                uint32_t h = info.Length() > 1 && info[1].IsNumber() ? info[1].As<Napi::Number>().Uint32Value() : g_state.height;
                if (w < 1) w = 1;
                if (h < 1) h = 1;
#if defined(_WIN32)
                if (g_state.hwnd != nullptr)
                {
                    HWND hwnd = static_cast<HWND>(g_state.hwnd);
                    RECT rc{0, 0, static_cast<LONG>(w), static_cast<LONG>(h)};
                    const DWORD style = static_cast<DWORD>(::GetWindowLongPtrW(hwnd, GWL_STYLE));
                    const DWORD exStyle = static_cast<DWORD>(::GetWindowLongPtrW(hwnd, GWL_EXSTYLE));
                    ::AdjustWindowRectEx(&rc, style, FALSE, exStyle);
                    ::SetWindowPos(hwnd, nullptr, 0, 0, rc.right - rc.left, rc.bottom - rc.top,
                        SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
                }
#endif
                ResizeDrawingBuffer(w, h);
                return env.Undefined();
            });

            // Set the window title (TestUtils.setTitle).
            SetMethod(global, "_nativeDawnSetTitle", [](const Napi::CallbackInfo& info) -> Napi::Value {
                Napi::Env env = info.Env();
#if defined(_WIN32)
                if (g_state.hwnd != nullptr && info.Length() > 0 && info[0].IsString())
                {
                    ::SetWindowTextA(static_cast<HWND>(g_state.hwnd), info[0].As<Napi::String>().Utf8Value().c_str());
                }
#endif
                return env.Undefined();
            });

            // Terminate the process with the given exit code (TestUtils.exit).
            SetMethod(global, "_nativeDawnExit", [](const Napi::CallbackInfo& info) -> Napi::Value {
                const int code = info.Length() > 0 && info[0].IsNumber() ? info[0].As<Napi::Number>().Int32Value() : 0;
                std::fflush(stdout);
                std::fflush(stderr);
                std::quick_exit(code);
                return info.Env().Undefined();
            });

            // Read a local file as an ArrayBuffer. Argument is a filesystem path
            // (forward or back slashes). Returns the bytes, or null if not found.
            // Backs the Dawn test shim's XMLHttpRequest replacement, whose local
            // file loads cannot use UrlLib/WinRT (file:// throws there in this app).
            SetMethod(global, "_nativeDawnReadFileBytes", [](const Napi::CallbackInfo& info) -> Napi::Value {
                Napi::Env env = info.Env();
                if (info.Length() < 1 || !info[0].IsString())
                {
                    return env.Null();
                }
                const std::string path = info[0].As<Napi::String>().Utf8Value();
                std::FILE* f = std::fopen(path.c_str(), "rb");
                if (f == nullptr)
                {
                    return env.Null();
                }
                std::fseek(f, 0, SEEK_END);
                const long size = std::ftell(f);
                std::fseek(f, 0, SEEK_SET);
                if (size < 0)
                {
                    std::fclose(f);
                    return env.Null();
                }
                Napi::ArrayBuffer ab = Napi::ArrayBuffer::New(env, static_cast<size_t>(size));
                if (size > 0)
                {
                    const size_t read = std::fread(ab.Data(), 1, static_cast<size_t>(size), f);
                    (void)read;
                }
                std::fclose(f);
                return ab;
            });

            // Decode an encoded image (PNG/JPEG/...) ArrayBuffer/TypedArray to an
            // ImageBitmap-like object {width,height,__pixels(ArrayBuffer RGBA8)}.
            // Backs the JS createImageBitmap / Image shims so glTF textures work
            // in this no-DOM environment.
            SetMethod(global, "_nativeDawnDecodeImage", [](const Napi::CallbackInfo& info) -> Napi::Value {
                Napi::Env env = info.Env();
                Bytes in = GetBytes(info.Length() > 0 ? info[0] : env.Undefined());
                if (in.data == nullptr || in.size == 0)
                {
                    throw Napi::Error::New(env, "_nativeDawnDecodeImage: no input bytes");
                }
                int w = 0;
                int h = 0;
                std::vector<uint8_t> rgba;
                if (!DecodeRGBA(in.data, in.size, rgba, w, h))
                {
                    throw Napi::Error::New(env, "_nativeDawnDecodeImage: decode failed");
                }
                Napi::ArrayBuffer ab = Napi::ArrayBuffer::New(env, rgba.size());
                std::memcpy(ab.Data(), rgba.data(), rgba.size());
                Napi::Object out = Napi::Object::New(env);
                out.Set("width", Napi::Number::New(env, w));
                out.Set("height", Napi::Number::New(env, h));
                out.Set("__pixels", ab);
                return out;
            });

            DawnLog(LogLevel::Log, "WebGPU (navigator.gpu) installed");
        }
    } // namespace (webgpu)

    namespace
    {
        // ---- Dawn bootstrap glue, implemented in C++ (Napi) -------------------
        // Formerly dawn_bootstrap.js. Installs, at global scope and before
        // babylon.max.js loads, everything WebGPUEngine needs to run the standard
        // (bgfx-oriented) Playground scene scripts unmodified on Dawn/WebGPU:
        //   * no-DOM canvas / document / window / location shims,
        //   * image decoding shims (createImageBitmap / Image / URL.createObjectURL)
        //     backed by the native bimg decoder,
        //   * a requestAnimationFrame pump driven by globalThis.frame()
        //     (called each host frame by the Embedding View::RenderFrame),
        //   * a __dawnResize hook (called by View::Resize),
        //   * a deferred WebGPUEngine creation that aliases BABYLON.NativeEngine
        //     once babylon.max.js defines BABYLON, so the scene scripts'
        //     synchronous `new BABYLON.NativeEngine()` returns the ready engine.
        // Input is NOT handled here (it flows through NativeInput, like bgfx).
        //
        // All state is JS-thread-only (the plugin only ever runs on the JS thread).

        std::vector<Napi::FunctionReference> g_rafQueue;
        Napi::ObjectReference g_blobRegistry;
        Napi::ObjectReference g_bootstrapCanvas;
        Napi::Reference<Napi::Value> g_babylon;
        bool g_engineStarted = false;
        uint32_t g_blobSeq = 0;

        // Deferred framebuffer readback for the validation harness. The harness
        // calls TestUtils.getFrameBufferData() from inside the WebGPU render
        // callback, BEFORE the engine's endFrame() submits the GPU commands. We
        // stash the callback and perform the readback in frame(), after the rAF
        // flush (which runs endFrame) but before present, so the surface texture
        // holds the freshly-submitted render.
        bool g_readbackPending = false;
        Napi::FunctionReference g_readbackCallback;

        Napi::Value Noop(const Napi::CallbackInfo& info) { return info.Env().Undefined(); }

        // ---- minimal 2D canvas raster (enough for WebGPU texture upload) -----
        // Babylon's WebGPU texture path sometimes draws a decoded image onto a 2D
        // canvas (e.g. for invert-Y or resize) and uses that canvas as the
        // copyExternalImageToTexture source. We back the 2D context with the
        // canvas's `__pixels` RGBA8 buffer so the canvas is a valid image source.
        struct Ctm { float sx{1}, sy{1}, tx{0}, ty{0}; std::vector<std::array<float, 4>> stack; };

        Napi::ArrayBuffer EnsureCanvasBuffer(Napi::Env env, Napi::Object canvas)
        {
            uint32_t w = canvas.Get("width").ToNumber().Uint32Value();
            uint32_t h = canvas.Get("height").ToNumber().Uint32Value();
            if (w == 0) w = 1;
            if (h == 0) h = 1;
            const size_t need = static_cast<size_t>(w) * h * 4u;
            Napi::Value pv = canvas.Get("__pixels");
            if (pv.IsArrayBuffer() && pv.As<Napi::ArrayBuffer>().ByteLength() == need)
            {
                return pv.As<Napi::ArrayBuffer>();
            }
            Napi::ArrayBuffer ab = Napi::ArrayBuffer::New(env, need);
            std::memset(ab.Data(), 0, need);
            canvas.Set("__pixels", ab);
            return ab;
        }

        // 2D context methods read their canvas back off `this` rather than
        // capturing it: getContext caches the context on the canvas, so a
        // captured strong reference would form a JS->native->JS cycle that V8
        // cannot collect, pinning the canvas and its pixel buffer forever.
        Napi::Object CtxCanvas(const Napi::CallbackInfo& info)
        {
            return info.This().As<Napi::Object>().Get("canvas").As<Napi::Object>();
        }

        Napi::Object Make2DContext(Napi::Env env, Napi::Object canvas)
        {
            auto ctm = std::make_shared<Ctm>();
            Napi::Object ctx = Napi::Object::New(env);
            ctx.Set("canvas", canvas);
            ctx.Set("fillStyle", Napi::String::New(env, "#000000"));
            ctx.Set("strokeStyle", Napi::String::New(env, "#000000"));
            ctx.Set("globalAlpha", Napi::Number::New(env, 1));
            ctx.Set("imageSmoothingEnabled", Napi::Boolean::New(env, true));

            SetMethod(ctx, "save", [ctm](const Napi::CallbackInfo& info) -> Napi::Value {
                ctm->stack.push_back({ctm->sx, ctm->sy, ctm->tx, ctm->ty});
                return info.Env().Undefined();
            });
            SetMethod(ctx, "restore", [ctm](const Napi::CallbackInfo& info) -> Napi::Value {
                if (!ctm->stack.empty()) { auto a = ctm->stack.back(); ctm->stack.pop_back(); ctm->sx = a[0]; ctm->sy = a[1]; ctm->tx = a[2]; ctm->ty = a[3]; }
                return info.Env().Undefined();
            });
            SetMethod(ctx, "translate", [ctm](const Napi::CallbackInfo& info) -> Napi::Value {
                ctm->tx += ctm->sx * info[0].ToNumber().FloatValue();
                ctm->ty += ctm->sy * info[1].ToNumber().FloatValue();
                return info.Env().Undefined();
            });
            SetMethod(ctx, "scale", [ctm](const Napi::CallbackInfo& info) -> Napi::Value {
                ctm->sx *= info[0].ToNumber().FloatValue();
                ctm->sy *= info[1].ToNumber().FloatValue();
                return info.Env().Undefined();
            });
            SetMethod(ctx, "setTransform", [ctm](const Napi::CallbackInfo& info) -> Napi::Value {
                if (info.Length() >= 6)
                {
                    ctm->sx = info[0].ToNumber().FloatValue();
                    ctm->sy = info[3].ToNumber().FloatValue();
                    ctm->tx = info[4].ToNumber().FloatValue();
                    ctm->ty = info[5].ToNumber().FloatValue();
                }
                return info.Env().Undefined();
            });
            SetMethod(ctx, "resetTransform", [ctm](const Napi::CallbackInfo& info) -> Napi::Value {
                ctm->sx = 1; ctm->sy = 1; ctm->tx = 0; ctm->ty = 0;
                return info.Env().Undefined();
            });
            SetMethod(ctx, "transform", Noop);
            SetMethod(ctx, "rotate", Noop);
            SetMethod(ctx, "beginPath", Noop);
            SetMethod(ctx, "closePath", Noop);
            SetMethod(ctx, "fill", Noop);
            SetMethod(ctx, "stroke", Noop);
            SetMethod(ctx, "moveTo", Noop);
            SetMethod(ctx, "lineTo", Noop);
            SetMethod(ctx, "rect", Noop);
            SetMethod(ctx, "clip", Noop);
            SetMethod(ctx, "fillText", [](const Napi::CallbackInfo& info) -> Napi::Value {
                // Text rasterization is not supported on the WebGPU backend (the
                // bgfx/nanovg Canvas polyfill is unavailable). Ensure the backing
                // buffer exists so the canvas is still a valid texture source.
                EnsureCanvasBuffer(info.Env(), CtxCanvas(info));
                return info.Env().Undefined();
            });
            SetMethod(ctx, "strokeText", Noop);
            SetMethod(ctx, "setLineDash", Noop);
            // Path / shape ops we don't rasterize (GUI backgrounds, rounded rects,
            // arcs). No-ops keep the canvas a valid texture source; strokeRect just
            // ensures the backing buffer exists like fillRect.
            SetMethod(ctx, "strokeRect", [](const Napi::CallbackInfo& info) -> Napi::Value {
                EnsureCanvasBuffer(info.Env(), CtxCanvas(info));
                return info.Env().Undefined();
            });
            SetMethod(ctx, "arc", Noop);
            SetMethod(ctx, "arcTo", Noop);
            SetMethod(ctx, "ellipse", Noop);
            SetMethod(ctx, "quadraticCurveTo", Noop);
            SetMethod(ctx, "bezierCurveTo", Noop);
            SetMethod(ctx, "roundRect", Noop);
            SetMethod(ctx, "clearHitCanvas", Noop);
            // Gradients / patterns: return a stub carrying addColorStop so GUI code
            // that builds a gradient fillStyle doesn't throw. We don't rasterize
            // the gradient, but the object shape is honored.
            auto makeGradient = [](const Napi::CallbackInfo& info) -> Napi::Value {
                Napi::Object g = Napi::Object::New(info.Env());
                SetMethod(g, "addColorStop", Noop);
                return g;
            };
            SetMethod(ctx, "createLinearGradient", makeGradient);
            SetMethod(ctx, "createRadialGradient", makeGradient);
            SetMethod(ctx, "createConicGradient", makeGradient);
            SetMethod(ctx, "createPattern", [](const Napi::CallbackInfo& info) -> Napi::Value {
                return info.Env().Null();
            });
            SetMethod(ctx, "measureText", [](const Napi::CallbackInfo& info) -> Napi::Value {
                Napi::Object o = Napi::Object::New(info.Env());
                o.Set("width", Napi::Number::New(info.Env(), 8));
                return o;
            });
            SetMethod(ctx, "getContextAttributes", [](const Napi::CallbackInfo& info) -> Napi::Value {
                return Napi::Object::New(info.Env());
            });
            SetMethod(ctx, "fillRect", [](const Napi::CallbackInfo& info) -> Napi::Value {
                EnsureCanvasBuffer(info.Env(), CtxCanvas(info));
                return info.Env().Undefined();
            });

            SetMethod(ctx, "clearRect", [](const Napi::CallbackInfo& info) -> Napi::Value {
                Napi::Env env = info.Env();
                Napi::Object canvas = CtxCanvas(info);
                Napi::ArrayBuffer ab = EnsureCanvasBuffer(env, canvas);
                const int cw = static_cast<int>(canvas.Get("width").ToNumber().Uint32Value());
                const int ch = static_cast<int>(canvas.Get("height").ToNumber().Uint32Value());
                int x = info[0].ToNumber().Int32Value();
                int y = info[1].ToNumber().Int32Value();
                int w = info[2].ToNumber().Int32Value();
                int h = info[3].ToNumber().Int32Value();
                uint8_t* buf = static_cast<uint8_t*>(ab.Data());
                for (int yy = y; yy < y + h && yy < ch; ++yy)
                {
                    if (yy < 0) continue;
                    for (int xx = x; xx < x + w && xx < cw; ++xx)
                    {
                        if (xx < 0) continue;
                        uint8_t* p = buf + (static_cast<size_t>(yy) * cw + xx) * 4;
                        p[0] = p[1] = p[2] = p[3] = 0;
                    }
                }
                return env.Undefined();
            });

            SetMethod(ctx, "drawImage", [ctm](const Napi::CallbackInfo& info) -> Napi::Value {
                Napi::Env env = info.Env();
                if (info.Length() < 3 || !info[0].IsObject()) return env.Undefined();
                Napi::Object img = info[0].As<Napi::Object>();
                Bytes src = GetBytes(img.Get("__pixels"));
                uint32_t iw = PropU32(img, "width", 0);
                uint32_t ih = PropU32(img, "height", 0);
                if (iw == 0) iw = PropU32(img, "naturalWidth", 0);
                if (ih == 0) ih = PropU32(img, "naturalHeight", 0);
                if (src.data == nullptr || iw == 0 || ih == 0)
                {
                    // Source not yet decoded (e.g. an unrendered DynamicTexture
                    // label). Leave the destination buffer as-is.
                    return env.Undefined();
                }

                double sx = 0, sy = 0, sw = iw, sh = ih, dx, dy, dw, dh;
                if (info.Length() >= 9)
                {
                    sx = info[1].ToNumber().DoubleValue(); sy = info[2].ToNumber().DoubleValue();
                    sw = info[3].ToNumber().DoubleValue(); sh = info[4].ToNumber().DoubleValue();
                    dx = info[5].ToNumber().DoubleValue(); dy = info[6].ToNumber().DoubleValue();
                    dw = info[7].ToNumber().DoubleValue(); dh = info[8].ToNumber().DoubleValue();
                }
                else if (info.Length() >= 5)
                {
                    dx = info[1].ToNumber().DoubleValue(); dy = info[2].ToNumber().DoubleValue();
                    dw = info[3].ToNumber().DoubleValue(); dh = info[4].ToNumber().DoubleValue();
                }
                else
                {
                    dx = info[1].ToNumber().DoubleValue(); dy = info[2].ToNumber().DoubleValue();
                    dw = iw; dh = ih;
                }

                Napi::Object canvas = CtxCanvas(info);
                Napi::ArrayBuffer ab = EnsureCanvasBuffer(env, canvas);
                const int cw = static_cast<int>(canvas.Get("width").ToNumber().Uint32Value());
                const int ch = static_cast<int>(canvas.Get("height").ToNumber().Uint32Value());
                uint8_t* dst = static_cast<uint8_t*>(ab.Data());
                const int idw = static_cast<int>(std::lround(dw));
                const int idh = static_cast<int>(std::lround(dh));
                for (int ddy = 0; ddy < idh; ++ddy)
                {
                    int syi = static_cast<int>(sy + ((ddy + 0.5) / dh) * sh);
                    if (syi < 0) syi = 0;
                    if (syi >= static_cast<int>(ih)) syi = ih - 1;
                    for (int ddx = 0; ddx < idw; ++ddx)
                    {
                        int sxi = static_cast<int>(sx + ((ddx + 0.5) / dw) * sw);
                        if (sxi < 0) sxi = 0;
                        if (sxi >= static_cast<int>(iw)) sxi = iw - 1;
                        const uint8_t* sp = src.data + (static_cast<size_t>(syi) * iw + sxi) * 4;
                        const double px = dx + ddx + 0.5;
                        const double py = dy + ddy + 0.5;
                        const int bx = static_cast<int>(std::floor(ctm->sx * px + ctm->tx));
                        const int by = static_cast<int>(std::floor(ctm->sy * py + ctm->ty));
                        if (bx < 0 || by < 0 || bx >= cw || by >= ch) continue;
                        uint8_t* dp = dst + (static_cast<size_t>(by) * cw + bx) * 4;
                        dp[0] = sp[0]; dp[1] = sp[1]; dp[2] = sp[2]; dp[3] = sp[3];
                    }
                }
                return env.Undefined();
            });

            SetMethod(ctx, "getImageData", [](const Napi::CallbackInfo& info) -> Napi::Value {
                Napi::Env env = info.Env();
                Napi::Object canvas = CtxCanvas(info);
                Napi::ArrayBuffer ab = EnsureCanvasBuffer(env, canvas);
                const int cw = static_cast<int>(canvas.Get("width").ToNumber().Uint32Value());
                const int ch = static_cast<int>(canvas.Get("height").ToNumber().Uint32Value());
                int x = info[0].ToNumber().Int32Value();
                int y = info[1].ToNumber().Int32Value();
                int w = info[2].ToNumber().Int32Value();
                int h = info[3].ToNumber().Int32Value();
                if (w <= 0 || h <= 0) { w = cw; h = ch; x = 0; y = 0; }
                Napi::ArrayBuffer out = Napi::ArrayBuffer::New(env, static_cast<size_t>(w) * h * 4u);
                uint8_t* od = static_cast<uint8_t*>(out.Data());
                std::memset(od, 0, static_cast<size_t>(w) * h * 4u);
                const uint8_t* sd = static_cast<const uint8_t*>(ab.Data());
                for (int yy = 0; yy < h; ++yy)
                {
                    const int syy = y + yy;
                    if (syy < 0 || syy >= ch) continue;
                    for (int xx = 0; xx < w; ++xx)
                    {
                        const int sxx = x + xx;
                        if (sxx < 0 || sxx >= cw) continue;
                        std::memcpy(od + (static_cast<size_t>(yy) * w + xx) * 4, sd + (static_cast<size_t>(syy) * cw + sxx) * 4, 4);
                    }
                }
                Napi::Function u8c = env.Global().Get("Uint8ClampedArray").As<Napi::Function>();
                Napi::Object dataArr = u8c.New({out, Napi::Number::New(env, 0), Napi::Number::New(env, static_cast<double>(w) * h * 4)}).As<Napi::Object>();
                Napi::Object res = Napi::Object::New(env);
                res.Set("data", dataArr);
                res.Set("width", Napi::Number::New(env, w));
                res.Set("height", Napi::Number::New(env, h));
                return res;
            });

            SetMethod(ctx, "putImageData", [](const Napi::CallbackInfo& info) -> Napi::Value {
                Napi::Env env = info.Env();
                if (!info[0].IsObject()) return env.Undefined();
                Napi::Object imgData = info[0].As<Napi::Object>();
                const int dx = info.Length() > 1 ? info[1].ToNumber().Int32Value() : 0;
                const int dy = info.Length() > 2 ? info[2].ToNumber().Int32Value() : 0;
                const uint32_t iw = PropU32(imgData, "width", 0);
                const uint32_t ih = PropU32(imgData, "height", 0);
                Bytes src = GetBytes(imgData.Get("data"));
                if (src.data == nullptr || iw == 0 || ih == 0) return env.Undefined();
                Napi::Object canvas = CtxCanvas(info);
                Napi::ArrayBuffer ab = EnsureCanvasBuffer(env, canvas);
                const int cw = static_cast<int>(canvas.Get("width").ToNumber().Uint32Value());
                const int ch = static_cast<int>(canvas.Get("height").ToNumber().Uint32Value());
                uint8_t* dst = static_cast<uint8_t*>(ab.Data());
                for (uint32_t yy = 0; yy < ih; ++yy)
                {
                    const int by = dy + static_cast<int>(yy);
                    if (by < 0 || by >= ch) continue;
                    for (uint32_t xx = 0; xx < iw; ++xx)
                    {
                        const int bx = dx + static_cast<int>(xx);
                        if (bx < 0 || bx >= cw) continue;
                        std::memcpy(dst + (static_cast<size_t>(by) * cw + bx) * 4, src.data + (static_cast<size_t>(yy) * iw + xx) * 4, 4);
                    }
                }
                return env.Undefined();
            });

            SetMethod(ctx, "createImageData", [](const Napi::CallbackInfo& info) -> Napi::Value {
                Napi::Env env = info.Env();
                uint32_t w = 1;
                uint32_t h = 1;
                if (info.Length() >= 2 && info[0].IsNumber())
                {
                    w = info[0].ToNumber().Uint32Value();
                    h = info[1].ToNumber().Uint32Value();
                }
                else if (info.Length() >= 1 && info[0].IsObject())
                {
                    Napi::Object o = info[0].As<Napi::Object>();
                    w = PropU32(o, "width", 1);
                    h = PropU32(o, "height", 1);
                }
                Napi::ArrayBuffer ab = Napi::ArrayBuffer::New(env, static_cast<size_t>(w) * h * 4u);
                std::memset(ab.Data(), 0, static_cast<size_t>(w) * h * 4u);
                Napi::Function u8c = env.Global().Get("Uint8ClampedArray").As<Napi::Function>();
                Napi::Object dataArr = u8c.New({ab, Napi::Number::New(env, 0), Napi::Number::New(env, static_cast<double>(w) * h * 4)}).As<Napi::Object>();
                Napi::Object res = Napi::Object::New(env);
                res.Set("data", dataArr);
                res.Set("width", Napi::Number::New(env, w));
                res.Set("height", Napi::Number::New(env, h));
                return res;
            });

            return ctx;
        }

        // Build a no-DOM canvas whose getContext("webgpu") returns the Dawn context
        // and getContext("2d") returns the raster context above.
        Napi::Object MakeCanvas(Napi::Env env, uint32_t width, uint32_t height)
        {
            Napi::Object canvas = Napi::Object::New(env);
            canvas.Set("width", Napi::Number::New(env, width));
            canvas.Set("height", Napi::Number::New(env, height));
            canvas.Set("clientWidth", Napi::Number::New(env, width));
            canvas.Set("clientHeight", Napi::Number::New(env, height));
            canvas.Set("style", Napi::Object::New(env));
            // Some scenes stash metadata on canvas.dataset (e.g. Babylon-Lite sets
            // canvas.dataset.ready = "true"); provide a plain object so those
            // assignments don't throw.
            canvas.Set("dataset", Napi::Object::New(env));
            SetMethod(canvas, "getContext", [](const Napi::CallbackInfo& info) -> Napi::Value {
                Napi::Env env = info.Env();
                const std::string type = info.Length() > 0 && info[0].IsString() ? info[0].As<Napi::String>().Utf8Value() : "";
                if (type == "webgpu")
                {
                    return MakeCanvasContext(env);
                }
                if (type == "2d")
                {
                    Napi::Object self = info.This().As<Napi::Object>();
                    Napi::Value existing = self.Get("__ctx2d");
                    if (existing.IsObject())
                    {
                        return existing;
                    }
                    // Prefer the real NanoVG-on-WebGPU context so text, paths and
                    // gradients actually rasterize (GUI, DynamicTexture). Fall back
                    // to the blit-only stub if the canvas backend isn't up yet.
                    Napi::Value c;
                    try
                    {
                        c = Babylon::Plugins::Internal::AttachDawn2DContext(env, self);
                    }
                    catch (const std::exception& e)
                    {
                        DawnLogF(LogLevel::Warn, "getContext('2d'): NanoVG canvas unavailable (%s); using blit-only stub", e.what());
                        c = Napi::Value{};
                    }
                    if (!c.IsObject())
                    {
                        c = Make2DContext(env, self);
                    }
                    self.Set("__ctx2d", c);
                    return c;
                }
                return env.Null();
            });
            SetMethod(canvas, "getBoundingClientRect", [](const Napi::CallbackInfo& info) -> Napi::Value {
                Napi::Env env = info.Env();
                Napi::Object self = info.This().As<Napi::Object>();
                double w = self.Get("width").ToNumber().DoubleValue();
                double h = self.Get("height").ToNumber().DoubleValue();
                Napi::Object r = Napi::Object::New(env);
                r.Set("x", Napi::Number::New(env, 0));
                r.Set("y", Napi::Number::New(env, 0));
                r.Set("left", Napi::Number::New(env, 0));
                r.Set("top", Napi::Number::New(env, 0));
                r.Set("right", Napi::Number::New(env, w));
                r.Set("bottom", Napi::Number::New(env, h));
                r.Set("width", Napi::Number::New(env, w));
                r.Set("height", Napi::Number::New(env, h));
                return r;
            });
            SetMethod(canvas, "setAttribute", Noop);
            SetMethod(canvas, "removeAttribute", Noop);
            // toDataURL: we don't PNG-encode here; return a 1x1 transparent PNG so
            // callers (screenshot/serialization helpers) get a valid data: URL
            // instead of throwing "toDataURL is not a function".
            SetMethod(canvas, "toDataURL", [](const Napi::CallbackInfo& info) -> Napi::Value {
                return Napi::String::New(info.Env(),
                    "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAAEAAAABCAQAAAC1HAwCAAAAC0lEQVR42mNk+M9QDwADhgGAWjR9awAAAABJRU5ErkJggg==");
            });
            SetMethod(canvas, "addEventListener", Noop);
            SetMethod(canvas, "removeEventListener", Noop);
            SetMethod(canvas, "dispatchEvent", [](const Napi::CallbackInfo& info) -> Napi::Value {
                return Napi::Boolean::New(info.Env(), true);
            });
            SetMethod(canvas, "setPointerCapture", Noop);
            SetMethod(canvas, "releasePointerCapture", Noop);
            SetMethod(canvas, "hasPointerCapture", [](const Napi::CallbackInfo& info) -> Napi::Value {
                return Napi::Boolean::New(info.Env(), false);
            });
            SetMethod(canvas, "focus", Noop);
            SetMethod(canvas, "getRootNode", [](const Napi::CallbackInfo& info) -> Napi::Value {
                return info.This();
            });
            // The default Babylon loading screen appends its overlay div to
            // `renderingCanvas.parentNode`; provide a DOM-node-like parent so
            // displayLoadingUI()/hideLoadingUI() don't dereference undefined.
            {
                Napi::Object parent = Napi::Object::New(env);
                SetMethod(parent, "appendChild", Noop);
                SetMethod(parent, "removeChild", Noop);
                SetMethod(parent, "insertBefore", Noop);
                parent.Set("style", Napi::Object::New(env));
                canvas.Set("parentNode", parent);
                canvas.Set("parentElement", parent);
            }
            return canvas;
        }

        // Decode an encoded-image ArrayBuffer/TypedArray into an ImageBitmap-like
        // object {width,height,__pixels(RGBA8 ArrayBuffer),close} via bimg.
        Napi::Object DecodeToBitmap(Napi::Env env, Napi::Value abVal)
        {
            Napi::Object bmp = Napi::Object::New(env);
            SetMethod(bmp, "close", Noop);

            Bytes in = GetBytes(abVal);
            int w = 0;
            int h = 0;
            std::vector<uint8_t> rgba;
            if (in.data == nullptr || in.size == 0 || !DecodeRGBA(in.data, in.size, rgba, w, h))
            {
                bmp.Set("width", Napi::Number::New(env, 1));
                bmp.Set("height", Napi::Number::New(env, 1));
                bmp.Set("__pixels", Napi::ArrayBuffer::New(env, 4));
                return bmp;
            }
            Napi::ArrayBuffer ab = Napi::ArrayBuffer::New(env, rgba.size());
            std::memcpy(ab.Data(), rgba.data(), rgba.size());
            bmp.Set("width", Napi::Number::New(env, w));
            bmp.Set("height", Napi::Number::New(env, h));
            bmp.Set("__pixels", ab);
            return bmp;
        }

        // Resolve `src` (ArrayBuffer / TypedArray view / {arrayBuffer()} / blob URL
        // string / fetchable URL string) to an ArrayBuffer, returning a Promise.
        Napi::Value ToArrayBuffer(Napi::Env env, Napi::Value src)
        {
            if (src.IsArrayBuffer())
            {
                auto d = Napi::Promise::Deferred::New(env);
                d.Resolve(src);
                return d.Promise();
            }
            if (src.IsTypedArray() || src.IsDataView())
            {
                Napi::Object o = src.As<Napi::Object>();
                Napi::Value buffer = o.Get("buffer");
                double offset = o.Get("byteOffset").ToNumber().DoubleValue();
                double length = o.Get("byteLength").ToNumber().DoubleValue();
                Napi::Value sliced = buffer.As<Napi::Object>().Get("slice").As<Napi::Function>().Call(
                    buffer, {Napi::Number::New(env, offset), Napi::Number::New(env, offset + length)});
                auto d = Napi::Promise::Deferred::New(env);
                d.Resolve(sliced);
                return d.Promise();
            }
            if (src.IsObject() && src.As<Napi::Object>().Get("arrayBuffer").IsFunction())
            {
                Napi::Object o = src.As<Napi::Object>();
                return o.Get("arrayBuffer").As<Napi::Function>().Call(o, {});
            }
            if (src.IsString())
            {
                const std::string id = src.As<Napi::String>().Utf8Value();
                if (id.rfind("data:", 0) == 0)
                {
                    auto d = Napi::Promise::Deferred::New(env);
                    size_t comma = id.find(',');
                    size_t semi = id.find(";base64");
                    if (comma != std::string::npos && semi != std::string::npos && semi < comma)
                    {
                        std::vector<uint8_t> bytes;
                        const char* b64 = id.c_str() + comma + 1;
                        size_t b64len = id.size() - comma - 1;
                        if (Base64Decode(b64, b64len, bytes))
                        {
                            Napi::ArrayBuffer ab = Napi::ArrayBuffer::New(env, bytes.size());
                            std::memcpy(ab.Data(), bytes.data(), bytes.size());
                            d.Resolve(ab);
                            return d.Promise();
                        }
                    }
                    d.Resolve(Napi::ArrayBuffer::New(env, 0));
                    return d.Promise();
                }
                Napi::Value blob = g_blobRegistry.Value().Get(id);
                if (!blob.IsUndefined())
                {
                    return ToArrayBuffer(env, blob);
                }
                Napi::Value fetchVal = env.Global().Get("fetch");
                if (fetchVal.IsFunction())
                {
                    Napi::Value p = fetchVal.As<Napi::Function>().Call({src});
                    Napi::Function toAb = Napi::Function::New(env, [](const Napi::CallbackInfo& info) -> Napi::Value {
                        Napi::Object r = info[0].As<Napi::Object>();
                        return r.Get("arrayBuffer").As<Napi::Function>().Call(r, {});
                    });
                    return p.As<Napi::Object>().Get("then").As<Napi::Function>().Call(p, {toAb});
                }
            }
            auto d = Napi::Promise::Deferred::New(env);
            d.Reject(Napi::Error::New(env, "toArrayBuffer: unsupported source").Value());
            return d.Promise();
        }

        // Create + initialize the WebGPUEngine once babylon.max.js has defined
        // BABYLON, then alias BABYLON.NativeEngine to return it.
        void OnBabylonReady(Napi::Env env, Napi::Value babylonVal)
        {
            if (g_engineStarted || !babylonVal.IsObject())
            {
                return;
            }
            Napi::Object babylon = babylonVal.As<Napi::Object>();
            Napi::Value wgpuCtor = babylon.Get("WebGPUEngine");
            if (!wgpuCtor.IsFunction())
            {
                return;
            }
            g_engineStarted = true;

            // Babylon assigns Tools.LoadScript = _LoadScriptWeb when the `_native`
            // global is absent -- which it is in the Dawn build (NativeEngine is
            // disabled). _LoadScriptWeb injects a <script> DOM node and waits for
            // its onload event, which never fires in this headless host. As a
            // result Babylon's glslang + twgsl WASM helpers never load, so shaders
            // authored in GLSL (NodeMaterial GLSL mode, ShaderMaterial,
            // ProceduralTexture, and various custom particle/post-process effects)
            // can never be transpiled GLSL -> SPIR-V -> WGSL and their scenes hang
            // forever on executeWhenReady. Route script loading through the working
            // file/XHR path instead (mirrors Babylon's own _LoadScriptNative).
            // WGSL-authored shaders (Standard/PBR/most post-processes) bypass this
            // path entirely and are unaffected.
            static const char* kLoadScriptPatch =
                "(function(){var T=BABYLON&&BABYLON.Tools;"
                "if(!T||T.__dawnLoadScriptPatched)return;"
                "T.__dawnLoadScriptPatched=true;"
                "T.LoadScript=function(url,onSuccess,onError){"
                "T.LoadFile(url,function(data){"
                "try{Function(data).apply(null);onSuccess&&onSuccess();}"
                "catch(e){onError&&onError('NativeDawn LoadScript eval error for '+url,e);}},"
                "undefined,undefined,false,function(req,ex){"
                "onError&&onError('NativeDawn LoadScript load error for '+url,ex);});};"
                "})();";
            env.RunScript(kLoadScriptPatch);

            // Reuse a canvas already created via document.getElementById (e.g. a
            // Babylon-Lite scene that grabbed "renderCanvas" before this ran) so
            // both engines share the single Dawn-surface-wired canvas.
            Napi::Object canvas;
            Napi::Value existingCanvas = env.Global().Get("__dawnCanvas");
            if (existingCanvas.IsObject())
            {
                canvas = existingCanvas.As<Napi::Object>();
            }
            else
            {
                canvas = MakeCanvas(env, g_state.width, g_state.height);
                env.Global().Set("__dawnCanvas", canvas);
            }
            g_bootstrapCanvas = Napi::Persistent(canvas);

            // On the web, assigning canvas.width/height resizes the drawing buffer.
            // Babylon's setSize() (and therefore setHardwareScalingLevel) does
            // exactly that, so bind those properties to the Dawn surface instead of
            // leaving them as inert data. Only the canvas that actually backs the
            // surface gets this treatment; offscreen 2D canvases keep plain values.
            canvas.DefineProperties({
                Napi::PropertyDescriptor::Accessor(env, canvas, "width",
                    [](const Napi::CallbackInfo& info) -> Napi::Value {
                        return Napi::Number::New(info.Env(), g_requestedWidth != 0 ? g_requestedWidth : g_state.width);
                    },
                    [](const Napi::CallbackInfo& info) {
                        if (info.Length() > 0 && info[0].IsNumber())
                        {
                            ResizeDrawingBuffer(info[0].As<Napi::Number>().Uint32Value(),
                                g_requestedHeight != 0 ? g_requestedHeight : g_state.height);
                        }
                    }),
                Napi::PropertyDescriptor::Accessor(env, canvas, "height",
                    [](const Napi::CallbackInfo& info) -> Napi::Value {
                        return Napi::Number::New(info.Env(), g_requestedHeight != 0 ? g_requestedHeight : g_state.height);
                    },
                    [](const Napi::CallbackInfo& info) {
                        if (info.Length() > 0 && info[0].IsNumber())
                        {
                            ResizeDrawingBuffer(g_requestedWidth != 0 ? g_requestedWidth : g_state.width,
                                info[0].As<Napi::Number>().Uint32Value());
                        }
                    }),
            });

            Napi::Object opts = Napi::Object::New(env);
            opts.Set("antialias", Napi::Boolean::New(env, false));
            opts.Set("stencil", Napi::Boolean::New(env, true));
            opts.Set("premultipliedAlpha", Napi::Boolean::New(env, false));
            opts.Set("enableAllFeatures", Napi::Boolean::New(env, false));

            Napi::Object engine = wgpuCtor.As<Napi::Function>().New({canvas, opts});
            engine.Set("enableOfflineSupport", Napi::Boolean::New(env, false));
            engine.Set("disableManifestCheck", Napi::Boolean::New(env, true));
            // Stash the engine so the async init callback can promote it without
            // capturing handles; the alias is only installed once ready.
            env.Global().Set("__dawnPendingEngine", engine);

            Napi::Value initPromise = engine.Get("initAsync").As<Napi::Function>().Call(engine, {});
            Napi::Function onReady = Napi::Function::New(env, [](const Napi::CallbackInfo& info) -> Napi::Value {
                Napi::Env env = info.Env();
                Napi::Object global = env.Global();
                global.Set("__dawnEngine", global.Get("__dawnPendingEngine"));
                Napi::Value babylon = global.Get("BABYLON");
                if (babylon.IsObject())
                {
                    babylon.As<Napi::Object>().Set("NativeEngine",
                        Napi::Function::New(env, [](const Napi::CallbackInfo& info) -> Napi::Value {
                            return info.Env().Global().Get("__dawnEngine");
                        }));
                }
                DawnLog(LogLevel::Log, "WebGPUEngine ready");
                return env.Undefined();
            });
            Napi::Function onErr = Napi::Function::New(env, [](const Napi::CallbackInfo& info) -> Napi::Value {
                std::string msg = info.Length() > 0 ? info[0].ToString().Utf8Value() : "?";
                DawnLog(LogLevel::Error, "engine init failed: " + msg);
                return info.Env().Undefined();
            });
            initPromise.As<Napi::Object>().Get("then").As<Napi::Function>().Call(initPromise, {onReady, onErr});
        }

        // Install all of the above onto the global object. Replaces the eval of
        // dawn_bootstrap.js.
        void InstallBootstrap(Napi::Env env)
        {
            Napi::Object global = env.Global();

            g_rafQueue.clear();
            g_engineStarted = false;
            g_blobSeq = 0;
            g_blobRegistry = Napi::Persistent(Napi::Object::New(env));

            // Babylon's WebGPU shader path (and the emscripten glslang/twgsl glue
            // it loads) references the `self` global, which browsers/web-workers
            // define but this host does not. Without it `_initGlslangAsync` throws
            // `ReferenceError: self is not defined`, GLSL shaders never transpile,
            // and their scenes hang forever. Alias `self` (and `window`) to the
            // global object.
            if (global.Get("self").IsUndefined())
            {
                global.Set("self", global);
            }
            if (global.Get("globalThis").IsUndefined())
            {
                global.Set("globalThis", global);
            }

            // WebGPU exposes its bitflag namespaces (GPUBufferUsage, GPUTextureUsage,
            // GPUShaderStage, GPUColorWrite, GPUMapMode) as globals in browsers.
            // Babylon.js carries internal fallbacks, but Babylon-Lite reads them
            // straight off globalThis (e.g. globalThis.GPUShaderStage.VERTEX), so
            // define the standard constant values here if absent.
            {
                static const char* kGpuConstPatch =
                    "(function(){var g=globalThis;"
                    "if(g.GPUBufferUsage&&g.GPUShaderStage)return;"
                    "g.GPUBufferUsage=g.GPUBufferUsage||{MAP_READ:1,MAP_WRITE:2,COPY_SRC:4,COPY_DST:8,INDEX:16,VERTEX:32,UNIFORM:64,STORAGE:128,INDIRECT:256,QUERY_RESOLVE:512};"
                    "g.GPUTextureUsage=g.GPUTextureUsage||{COPY_SRC:1,COPY_DST:2,TEXTURE_BINDING:4,STORAGE_BINDING:8,RENDER_ATTACHMENT:16};"
                    "g.GPUShaderStage=g.GPUShaderStage||{VERTEX:1,FRAGMENT:2,COMPUTE:4};"
                    "g.GPUColorWrite=g.GPUColorWrite||{RED:1,GREEN:2,BLUE:4,ALPHA:8,ALL:15};"
                    "g.GPUMapMode=g.GPUMapMode||{READ:1,WRITE:2};"
                    "})();";
                env.RunScript(kGpuConstPatch);
            }

            // The host's native TextDecoder polyfill only supports UTF-8, but the
            // emscripten glue for Babylon's glslang/twgsl WASM modules constructs
            // `new TextDecoder('utf-16le')` (UTF16ToString). That throws, rejecting
            // glslang init and hanging every GLSL-shader scene. Install a JS
            // TextDecoder that adds UTF-16 (LE/BE) support and delegates all other
            // encodings to the native decoder (preserving correct UTF-8 handling
            // used by e.g. the glTF/Draco loaders).
            {
                static const char* kTextDecoderPatch =
                    "(function(){var N=globalThis.TextDecoder;"
                    "if(N&&N.__dawnUtf16)return;"
                    "function norm(l){return String(l==null?'utf-8':l).trim().toLowerCase();}"
                    "function TD(label,opts){var l=norm(label);this._label=l;"
                    "this._le=(l==='utf-16le'||l==='utf-16'||l==='ucs-2'||l==='ucs2'||l==='unicode'||l==='csunicode'||l==='iso-10646-ucs-2');"
                    "this._be=(l==='utf-16be');"
                    "if(!this._le&&!this._be&&N){try{this._n=new N(label,opts);}catch(e){this._n=null;}}}"
                    "TD.__dawnUtf16=true;"
                    "TD.prototype.decode=function(input,o){if(input==null)return '';var b;"
                    "if(input instanceof ArrayBuffer)b=new Uint8Array(input);"
                    "else if(ArrayBuffer.isView(input))b=new Uint8Array(input.buffer,input.byteOffset,input.byteLength);"
                    "else return '';"
                    "if(this._le||this._be){var s='',le=this._le;for(var i=0;i+1<b.length;i+=2){var c=le?(b[i]|(b[i+1]<<8)):((b[i]<<8)|b[i+1]);s+=String.fromCharCode(c);}return s;}"
                    "if(this._n)return this._n.decode(input,o);"
                    "var out='',i2=0,n=b.length;while(i2<n){var x=b[i2++];if(x<128){out+=String.fromCharCode(x);}"
                    "else if(x>=192&&x<224){out+=String.fromCharCode(((x&31)<<6)|(b[i2++]&63));}"
                    "else if(x>=224&&x<240){out+=String.fromCharCode(((x&15)<<12)|((b[i2++]&63)<<6)|(b[i2++]&63));}"
                    "else{var cp=((x&7)<<18)|((b[i2++]&63)<<12)|((b[i2++]&63)<<6)|(b[i2++]&63);cp-=0x10000;out+=String.fromCharCode(0xD800+(cp>>10),0xDC00+(cp&1023));}}return out;};"
                    "Object.defineProperty(TD.prototype,'encoding',{get:function(){return this._label;}});"
                    "globalThis.TextDecoder=TD;})();";
                env.RunScript(kTextDecoderPatch);
            }

            // V8's async WebAssembly.instantiate posts compilation completion to a
            // foreground task runner this host does not pump, so the returned
            // promise never settles and the glslang/twgsl modules never finish
            // loading (their scenes hang). Regular Promise microtasks DO run here,
            // so override instantiate to compile synchronously (via the sync
            // Module/Instance constructors) and hand back an already-resolved
            // promise. The glslang/twgsl WASM blobs are small, so the brief
            // synchronous compile is acceptable.
            {
                static const char* kWasmSyncPatch =
                    "(function(){if(typeof WebAssembly==='undefined'||WebAssembly.__dawnSyncInstantiate)return;"
                    "WebAssembly.__dawnSyncInstantiate=true;"
                    "WebAssembly.instantiate=function(bytes,imports){try{"
                    "if(bytes instanceof WebAssembly.Module){return Promise.resolve(new WebAssembly.Instance(bytes,imports));}"
                    "var m=new WebAssembly.Module(bytes);var inst=new WebAssembly.Instance(m,imports);"
                    "return Promise.resolve({module:m,instance:inst});}"
                    "catch(e){return Promise.reject(e);}};})();";
                env.RunScript(kWasmSyncPatch);
            }

            // ---- canvas / document / window / location shims -----------------
            if (global.Get("document").IsUndefined())
            {
                Napi::Object document = Napi::Object::New(env);
                SetMethod(document, "createElement", [](const Napi::CallbackInfo& info) -> Napi::Value {
                    Napi::Env env = info.Env();
                    std::string tag = info.Length() > 0 && info[0].IsString() ? info[0].As<Napi::String>().Utf8Value() : "";
                    if (tag == "canvas")
                    {
                        return MakeCanvas(env, 1280, 720);
                    }
                    if (tag == "img")
                    {
                        return env.Global().Get("Image").As<Napi::Function>().New({});
                    }
                    Napi::Object el = Napi::Object::New(env);
                    el.Set("style", Napi::Object::New(env));
                    el.Set("clientWidth", Napi::Number::New(env, 0));
                    el.Set("clientHeight", Napi::Number::New(env, 0));
                    SetMethod(el, "setAttribute", Noop);
                    SetMethod(el, "appendChild", Noop);
                    SetMethod(el, "removeChild", Noop);
                    SetMethod(el, "insertBefore", Noop);
                    SetMethod(el, "addEventListener", Noop);
                    SetMethod(el, "removeEventListener", Noop);
                    SetMethod(el, "focus", Noop);
                    SetMethod(el, "blur", Noop);
                    SetMethod(el, "getContext", [](const Napi::CallbackInfo& info) -> Napi::Value {
                        return info.Env().Null();
                    });
                    // GUI controls query getInputElement().getBoundingClientRect();
                    // provide a zero-rect so those paths don't throw.
                    SetMethod(el, "getBoundingClientRect", [](const Napi::CallbackInfo& info) -> Napi::Value {
                        Napi::Env env = info.Env();
                        Napi::Object r = Napi::Object::New(env);
                        for (const char* k : {"x", "y", "left", "top", "right", "bottom", "width", "height"})
                        {
                            r.Set(k, Napi::Number::New(env, 0));
                        }
                        return r;
                    });
                    return el;
                });
                SetMethod(document, "addEventListener", Noop);
                SetMethod(document, "removeEventListener", Noop);
                SetMethod(document, "getElementById", [](const Napi::CallbackInfo& info) -> Napi::Value {
                    // Hand back the single Dawn-surface-wired canvas when a scene
                    // asks for its render canvas by id (e.g. Babylon-Lite fetches
                    // document.getElementById("renderCanvas")). Other ids return
                    // null, matching prior behavior so the validation harness (which
                    // never requests a canvas by id) is unaffected. The canvas is
                    // created lazily and cached as __dawnCanvas so the Babylon.js
                    // bootstrap and Lite share the same canvas.
                    Napi::Env env = info.Env();
                    const std::string id = info.Length() > 0 && info[0].IsString()
                        ? info[0].As<Napi::String>().Utf8Value() : "";
                    if (id != "renderCanvas" && id != "canvas" && id != "babylon-canvas")
                    {
                        return env.Null();
                    }
                    Napi::Object global = env.Global();
                    Napi::Value existing = global.Get("__dawnCanvas");
                    if (existing.IsObject())
                    {
                        return existing;
                    }
                    Napi::Object canvas = MakeCanvas(env, g_state.width, g_state.height);
                    global.Set("__dawnCanvas", canvas);
                    return canvas;
                });
                // Return an empty array-like for the query methods some Babylon
                // paths call (e.g. glTF loaders probing for <script>/<link> tags).
                SetMethod(document, "getElementsByTagName", [](const Napi::CallbackInfo& info) -> Napi::Value {
                    Napi::Env env = info.Env();
                    const std::string tag = info.Length() > 0 && info[0].IsString()
                        ? info[0].As<Napi::String>().Utf8Value() : "";
                    Napi::Array arr = Napi::Array::New(env);
                    // Some Babylon paths do getElementsByTagName("head")[0].appendChild(...)
                    // (e.g. the default loading screen injecting a <style>). Return a
                    // single DOM-node-like stub for head/body so [0] is valid.
                    if (tag == "head" || tag == "body")
                    {
                        Napi::Object node = Napi::Object::New(env);
                        SetMethod(node, "appendChild", Noop);
                        SetMethod(node, "removeChild", Noop);
                        SetMethod(node, "insertBefore", Noop);
                        node.Set("style", Napi::Object::New(env));
                        arr.Set(uint32_t(0), node);
                    }
                    return arr;
                });
                SetMethod(document, "getElementsByClassName", [](const Napi::CallbackInfo& info) -> Napi::Value {
                    return Napi::Array::New(info.Env());
                });
                SetMethod(document, "querySelectorAll", [](const Napi::CallbackInfo& info) -> Napi::Value {
                    return Napi::Array::New(info.Env());
                });
                SetMethod(document, "querySelector", [](const Napi::CallbackInfo& info) -> Napi::Value {
                    return info.Env().Null();
                });
                Napi::Object body = Napi::Object::New(env);
                SetMethod(body, "appendChild", Noop);
                SetMethod(body, "removeChild", Noop);
                SetMethod(body, "insertBefore", Noop);
                body.Set("style", Napi::Object::New(env));
                document.Set("body", body);
                Napi::Object head = Napi::Object::New(env);
                SetMethod(head, "appendChild", Noop);
                SetMethod(head, "removeChild", Noop);
                SetMethod(head, "insertBefore", Noop);
                head.Set("style", Napi::Object::New(env));
                document.Set("head", head);
                document.Set("documentElement", Napi::Object::New(env));
                global.Set("document", document);
            }
            if (global.Get("OffscreenCanvas").IsUndefined())
            {
                global.Set("OffscreenCanvas", Napi::Function::New(env, [](const Napi::CallbackInfo& info) -> Napi::Value {
                    const uint32_t w = info.Length() > 0 && info[0].IsNumber() ? info[0].As<Napi::Number>().Uint32Value() : 1;
                    const uint32_t h = info.Length() > 1 && info[1].IsNumber() ? info[1].As<Napi::Number>().Uint32Value() : 1;
                    return MakeCanvas(info.Env(), w, h);
                }));
            }
            SetMethod(global, "addEventListener", Noop);
            SetMethod(global, "removeEventListener", Noop);

            if (global.Get("location").IsUndefined())
            {
                Napi::Object location = Napi::Object::New(env);
                location.Set("href", Napi::String::New(env, "file:///"));
                location.Set("origin", Napi::String::New(env, "file://"));
                location.Set("protocol", Napi::String::New(env, "file:"));
                location.Set("host", Napi::String::New(env, ""));
                location.Set("hostname", Napi::String::New(env, ""));
                location.Set("pathname", Napi::String::New(env, "/"));
                location.Set("search", Napi::String::New(env, ""));
                location.Set("hash", Napi::String::New(env, ""));
                global.Set("location", location);
            }

            // Babylon's WebGPU engine reloads glTF external-texture object URLs
            // through its internal XHR/data-URL loader (forceBitmapOverHTML-
            // ImageElement). That path cannot resolve our `blob:nativedawn/N`
            // ids (only createImageBitmap's ToArrayBuffer can), so such textures
            // used to load as 0 bytes -> 1x1 -> black. Stash the source bytes on
            // each Blob instance (as `__dawnU8`) so createObjectURL below can
            // return a universally-resolvable `data:` URL instead.
            {
                static const char* kBlobStashPatch =
                    "(function(){var R=globalThis.Blob;"
                    "if(typeof R!=='function'||R.__dawnStash)return;"
                    "function W(parts,opts){var b=new R(parts,opts);try{"
                    "var p=parts&&parts[0],u8=null;"
                    "if(p instanceof ArrayBuffer)u8=new Uint8Array(p);"
                    "else if(p&&p.buffer instanceof ArrayBuffer)u8=new Uint8Array(p.buffer,p.byteOffset||0,p.byteLength);"
                    "if(u8){b.__dawnU8=u8;b.__dawnType=(opts&&opts.type)||'';}"
                    "}catch(e){}return b;}"
                    "W.prototype=R.prototype;W.__dawnStash=true;globalThis.Blob=W;})();";
                env.RunScript(kBlobStashPatch);
            }

            // ---- image decoding shims ----------------------------------------
            // The host's native URL polyfill requires its arguments to be strings
            // and throws "A string was expected" when passed a URL object as the
            // base (e.g. `new URL(".", new URL(src, base))`, which Babylon-Lite's
            // glTF loader does). Browsers stringify such arguments. Wrap URL to
            // coerce non-string url/base args to strings while preserving the
            // static helpers (createObjectURL/revokeObjectURL) added below.
            {
                static const char* kUrlCoercePatch =
                    "(function(){var N=globalThis.URL;"
                    "if(!N||N.__dawnCoerce)return;"
                    "function W(u,b){var us=(u===undefined||u===null)?u:String(u);"
                    "return (b===undefined||b===null)?new N(us):new N(us,String(b));}"
                    "W.prototype=N.prototype;W.__dawnCoerce=true;"
                    "for(var k in N){try{W[k]=N[k];}catch(e){}}"
                    "if(N.createObjectURL)W.createObjectURL=function(){return N.createObjectURL.apply(N,arguments);};"
                    "if(N.revokeObjectURL)W.revokeObjectURL=function(){return N.revokeObjectURL.apply(N,arguments);};"
                    "globalThis.URL=W;})();";
                env.RunScript(kUrlCoercePatch);
            }

            // Root-relative asset fetches (e.g. Babylon-Lite scene1's
            // `fetch("/brdf-lut.png")`, a lab-dev-server convention) resolve to a
            // local file that doesn't exist in this host, and a failed local fetch
            // currently crashes the process. Redirect root-relative "/x" fetch URLs
            // to the app's Scripts directory (app:///Scripts/x) where such assets
            // can be placed, so self-contained lite bundles that expect their
            // sibling assets at the site root load correctly.
            {
                static const char* kFetchRewritePatch =
                    "(function(){var f=globalThis.fetch;"
                    "if(typeof f!=='function'||f.__dawnRewrite)return;"
                    "function W(u,o){try{"
                    "if(typeof u==='string'&&u.charAt(0)==='/'&&u.charAt(1)!=='/'){u='app:///Scripts'+u;}"
                    "else if(u&&typeof u==='object'&&typeof u.url==='string'&&u.url.charAt(0)==='/'&&u.url.charAt(1)!=='/'){u='app:///Scripts'+u.url;}"
                    "}catch(e){}return f.call(this,u,o);}"
                    "W.__dawnRewrite=true;globalThis.fetch=W;})();";
                env.RunScript(kFetchRewritePatch);
            }

            Napi::Value urlVal = global.Get("URL");
            if (urlVal.IsObject() || urlVal.IsFunction())
            {
                Napi::Object url = urlVal.As<Napi::Object>();
                if (!url.Get("createObjectURL").IsFunction())
                {
                    SetMethod(url, "createObjectURL", [](const Napi::CallbackInfo& info) -> Napi::Value {
                        Napi::Env env = info.Env();
                        // If the blob carries stashed source bytes (see Blob patch),
                        // return a data: URL so Babylon's internal XHR/data-URL
                        // texture loader can resolve it (blob:nativedawn ids can't
                        // be resolved by that path). Falls back to the registry.
                        if (info.Length() > 0 && info[0].IsObject())
                        {
                            Napi::Object blob = info[0].As<Napi::Object>();
                            Napi::Value u8 = blob.Get("__dawnU8");
                            if (u8.IsTypedArray() || u8.IsArrayBuffer())
                            {
                                Bytes b = GetBytes(u8);
                                if (b.data != nullptr && b.size != 0)
                                {
                                    std::string type;
                                    Napi::Value ty = blob.Get("__dawnType");
                                    if (ty.IsString()) type = ty.As<Napi::String>().Utf8Value();
                                    if (type.empty()) type = "application/octet-stream";
                                    std::string dataUrl = "data:" + type + ";base64," + Base64Encode(b.data, b.size);
                                    return Napi::String::New(env, dataUrl);
                                }
                            }
                        }
                        std::string id = "blob:nativedawn/" + std::to_string(++g_blobSeq);
                        g_blobRegistry.Value().Set(id, info.Length() > 0 ? info[0] : env.Undefined());
                        return Napi::String::New(env, id);
                    });
                }
                if (!url.Get("revokeObjectURL").IsFunction())
                {
                    SetMethod(url, "revokeObjectURL", [](const Napi::CallbackInfo& info) -> Napi::Value {
                        if (info.Length() > 0 && info[0].IsString())
                        {
                            g_blobRegistry.Value().Delete(info[0].As<Napi::String>().Utf8Value());
                        }
                        return info.Env().Undefined();
                    });
                }
            }

            if (global.Get("createImageBitmap").IsUndefined())
            {
                SetMethod(global, "createImageBitmap", [](const Napi::CallbackInfo& info) -> Napi::Value {
                    Napi::Env env = info.Env();
                    Napi::Value src = info.Length() > 0 ? info[0] : env.Undefined();
                    auto deferred = Napi::Promise::Deferred::New(env);
                    if (src.IsObject() && src.As<Napi::Object>().Get("__pixels").IsArrayBuffer())
                    {
                        deferred.Resolve(src);
                        return deferred.Promise();
                    }
                    Napi::Value p = ToArrayBuffer(env, src);
                    Napi::Function onAb = Napi::Function::New(env, [deferred](const Napi::CallbackInfo& info) -> Napi::Value {
                        Napi::Env env = info.Env();
                        deferred.Resolve(DecodeToBitmap(env, info.Length() > 0 ? info[0] : env.Undefined()));
                        return env.Undefined();
                    });
                    Napi::Function onErr = Napi::Function::New(env, [deferred](const Napi::CallbackInfo& info) -> Napi::Value {
                        deferred.Reject(info.Length() > 0 ? info[0] : info.Env().Undefined());
                        return info.Env().Undefined();
                    });
                    p.As<Napi::Object>().Get("then").As<Napi::Function>().Call(p, {onAb, onErr});
                    return deferred.Promise();
                });
            }

            if (global.Get("Image").IsUndefined())
            {
                Napi::Function imageCtor = Napi::Function::New(env, [](const Napi::CallbackInfo& info) -> Napi::Value {
                    Napi::Env env = info.Env();
                    Napi::Object img = Napi::Object::New(env);
                    img.Set("width", Napi::Number::New(env, 0));
                    img.Set("height", Napi::Number::New(env, 0));
                    img.Set("naturalWidth", Napi::Number::New(env, 0));
                    img.Set("naturalHeight", Napi::Number::New(env, 0));
                    img.Set("__pixels", env.Null());
                    img.Set("onload", env.Null());
                    img.Set("onerror", env.Null());
                    img.Set("crossOrigin", env.Null());
                    img.Set("complete", Napi::Boolean::New(env, false));
                    img.Set("_src", Napi::String::New(env, ""));
                    SetMethod(img, "decode", [](const Napi::CallbackInfo& info) -> Napi::Value {
                        // Decoding already happened in the `src` setter. Returning a
                        // never-settling promise here strands Babylon's
                        // _createImageBitmapFromSource, which awaits decode().
                        auto d = Napi::Promise::Deferred::New(info.Env());
                        d.Resolve(info.Env().Undefined());
                        return d.Promise();
                    });
                    SetMethod(img, "addEventListener", [](const Napi::CallbackInfo& info) -> Napi::Value {
                        Napi::Object self = info.This().As<Napi::Object>();
                        std::string name = info.Length() > 0 && info[0].IsString() ? info[0].As<Napi::String>().Utf8Value() : "";
                        if (name == "load") self.Set("onload", info[1]);
                        else if (name == "error") self.Set("onerror", info[1]);
                        return info.Env().Undefined();
                    });
                    SetMethod(img, "removeEventListener", Noop);

                    // Define the async-decoding `src` accessor. The setter must not
                    // capture a strong reference to `img`: the accessor is installed
                    // on `img` itself, so a captured Napi::Persistent forms a
                    // JS->native->JS cycle that V8's GC cannot break, pinning every
                    // Image (and its decoded pixel ArrayBuffer) for the whole
                    // process. Take the reference per assignment instead and drop it
                    // once the decode settles.
                    Napi::Object desc = Napi::Object::New(env);
                    desc.Set("configurable", Napi::Boolean::New(env, true));
                    desc.Set("get", Napi::Function::New(env, [](const Napi::CallbackInfo& info) -> Napi::Value {
                        return info.This().As<Napi::Object>().Get("_src");
                    }));
                    desc.Set("set", Napi::Function::New(env, [](const Napi::CallbackInfo& info) -> Napi::Value {
                        Napi::Env env = info.Env();
                        Napi::Value v = info.Length() > 0 ? info[0] : env.Undefined();
                        auto imgRef = std::make_shared<Napi::ObjectReference>(Napi::Persistent(info.This().As<Napi::Object>()));
                        imgRef->Value().Set("_src", v);
                        Napi::Value p = ToArrayBuffer(env, v);
                        Napi::Function onAb = Napi::Function::New(env, [imgRef](const Napi::CallbackInfo& info) -> Napi::Value {
                            Napi::Env env = info.Env();
                            Napi::Object img = imgRef->Value();
                            imgRef->Reset();
                            Napi::Object bmp = DecodeToBitmap(env, info.Length() > 0 ? info[0] : env.Undefined());
                            img.Set("width", bmp.Get("width"));
                            img.Set("naturalWidth", bmp.Get("width"));
                            img.Set("height", bmp.Get("height"));
                            img.Set("naturalHeight", bmp.Get("height"));
                            img.Set("__pixels", bmp.Get("__pixels"));
                            img.Set("complete", Napi::Boolean::New(env, true));
                            Napi::Value onload = img.Get("onload");
                            if (onload.IsFunction())
                            {
                                Napi::Object ev = Napi::Object::New(env);
                                ev.Set("target", img);
                                onload.As<Napi::Function>().Call(img, {ev});
                            }
                            return env.Undefined();
                        });
                        Napi::Function onErr = Napi::Function::New(env, [imgRef](const Napi::CallbackInfo& info) -> Napi::Value {
                            Napi::Env env = info.Env();
                            Napi::Object img = imgRef->Value();
                            imgRef->Reset();
                            Napi::Value onerror = img.Get("onerror");
                            if (onerror.IsFunction())
                            {
                                onerror.As<Napi::Function>().Call(img, {info.Length() > 0 ? info[0] : env.Undefined()});
                            }
                            return env.Undefined();
                        });
                        p.As<Napi::Object>().Get("then").As<Napi::Function>().Call(p, {onAb, onErr});
                        return env.Undefined();
                    }));
                    Napi::Object objectCtor = env.Global().Get("Object").As<Napi::Object>();
                    objectCtor.Get("defineProperty").As<Napi::Function>().Call(objectCtor, {img, Napi::String::New(env, "src"), desc});
                    return img;
                });
                global.Set("Image", imageCtor);
                if (global.Get("HTMLImageElement").IsUndefined())
                {
                    global.Set("HTMLImageElement", imageCtor);
                }
            }

            // ---- requestAnimationFrame pump ----------------------------------
            SetMethod(global, "requestAnimationFrame", [](const Napi::CallbackInfo& info) -> Napi::Value {
                if (info.Length() > 0 && info[0].IsFunction())
                {
                    g_rafQueue.push_back(Napi::Persistent(info[0].As<Napi::Function>()));
                }
                return Napi::Number::New(info.Env(), static_cast<double>(g_rafQueue.size()));
            });
            SetMethod(global, "cancelAnimationFrame", Noop);
            SetMethod(global, "frame", [](const Napi::CallbackInfo& info) -> Napi::Value {
                Napi::Env env = info.Env();
                // Present the PREVIOUS frame before running this frame's work.
                // Babylon reuses its upload/render command encoders across
                // requestAnimationFrame callbacks, so a render pass recorded
                // against the surface texture in one frame is often only
                // submitted during the next one. Presenting destroys the surface
                // texture, so presenting at the end of the frame that recorded
                // the work loses it ("Destroyed texture [Texture of [Surface]]
                // used in a submit") and blanks the frame. Present at the top of
                // the next frame, and only once the recorded work has actually
                // been submitted (g_surfaceWorkPending).
                if (g_surfaceConfigured && g_currentTextureAcquired && !g_surfaceWorkPending)
                {
                    wgpuSurfacePresent(g_state.surface);
                    g_currentTextureAcquired = false;
                    ApplyPendingSurfaceResize();
                }
                if (g_state.instance)
                {
                    wgpuInstanceProcessEvents(g_state.instance);
                }
                double now = 0.0;
                Napi::Value perf = env.Global().Get("performance");
                if (perf.IsObject())
                {
                    Napi::Value nowFn = perf.As<Napi::Object>().Get("now");
                    if (nowFn.IsFunction())
                    {
                        now = nowFn.As<Napi::Function>().Call(perf, {}).ToNumber().DoubleValue();
                    }
                }
                std::vector<Napi::FunctionReference> queue;
                queue.swap(g_rafQueue);
                for (auto& cb : queue)
                {
                    cb.Value().Call({Napi::Number::New(env, now)});
                }
                // Deferred framebuffer readback (validation harness): the render
                // callbacks above ran the engine's beginFrame/render/endFrame, so
                // the surface texture now holds the submitted frame. Read it back
                // and invoke the stashed callback BEFORE presenting (present
                // releases the texture).
                if (g_readbackPending)
                {
                    g_readbackPending = false;
                    Napi::Value rp = env.Global().Get("_nativeDawnReadPixels");
                    if (rp.IsFunction() && !g_readbackCallback.IsEmpty())
                    {
                        Napi::Object res = rp.As<Napi::Function>().Call({}).As<Napi::Object>();
                        Napi::ArrayBuffer ab = res.Get("data").As<Napi::ArrayBuffer>();
                        Napi::Uint8Array u8 = Napi::Uint8Array::New(env, ab.ByteLength(), ab, 0);
                        g_readbackCallback.Value().Call({u8});
                    }
                    g_readbackCallback.Reset();
                    // Reclaim this frame's short-lived GPU wrappers now: the
                    // validation harness renders only a frame or two per scene,
                    // so the gated per-frame pump below may not fire before the
                    // next scene loads and starts allocating again.
                    PumpJsFinalizers(env, true);
                }
                // The present for this frame is deferred to the top of the next
                // frame (see the comment there), so nothing is presented here.
                if (g_state.instance)
                {
                    wgpuInstanceProcessEvents(g_state.instance);
                }
                // Retire GPU resources whose deferred-destroy delay has elapsed
                // (see DeferDestroy). Runs after ProcessEvents so completed
                // submissions have released their references.
                FlushPendingDestroy();
                // Bounded-frequency GC + finalizer drain so dropped GPU wrappers
                // (and the Dawn allocations they hold) don't accumulate across a
                // long-running session. No-op on non-V8 engines.
                PumpJsFinalizers(env, false);
                return env.Undefined();
            });

            // Forward timer/animation methods onto `window` (Window polyfill
            // provides `window` but not these).
            Napi::Value windowVal = global.Get("window");
            if (windowVal.IsObject())
            {
                Napi::Object w = windowVal.As<Napi::Object>();
                for (const char* name : {"setTimeout", "clearTimeout", "setInterval", "clearInterval",
                                         "requestAnimationFrame", "cancelAnimationFrame"})
                {
                    Napi::Value fn = global.Get(name);
                    if (!fn.IsUndefined())
                    {
                        w.Set(name, fn);
                    }
                }
                SetMethod(w, "addEventListener", Noop);
                SetMethod(w, "removeEventListener", Noop);
                SetMethod(w, "dispatchEvent", [](const Napi::CallbackInfo& info) -> Napi::Value {
                    return Napi::Boolean::New(info.Env(), true);
                });
            }

            // ---- inert event constructors ------------------------------------
            auto makeEventClass = [](Napi::Env env) {
                return Napi::Function::New(env, [](const Napi::CallbackInfo& info) -> Napi::Value {
                    Napi::Env env = info.Env();
                    Napi::Object self = info.This().As<Napi::Object>();
                    self.Set("type", info.Length() > 0 ? info[0] : env.Undefined());
                    if (info.Length() > 1 && info[1].IsObject())
                    {
                        Napi::Object init = info[1].As<Napi::Object>();
                        Napi::Array keys = init.GetPropertyNames();
                        for (uint32_t i = 0; i < keys.Length(); ++i)
                        {
                            Napi::Value k = keys.Get(i);
                            self.Set(k, init.Get(k));
                        }
                    }
                    SetMethod(self, "preventDefault", Noop);
                    SetMethod(self, "stopPropagation", Noop);
                    SetMethod(self, "stopImmediatePropagation", Noop);
                    return env.Undefined();
                });
            };
            for (const char* name : {"PointerEvent", "MouseEvent", "WheelEvent", "Event"})
            {
                if (global.Get(name).IsUndefined())
                {
                    global.Set(name, makeEventClass(env));
                }
            }
            if (windowVal.IsObject())
            {
                Napi::Object w = windowVal.As<Napi::Object>();
                for (const char* name : {"PointerEvent", "MouseEvent", "WheelEvent"})
                {
                    w.Set(name, global.Get(name));
                }
            }

            // ---- resize bridge (called by Embedding View::Resize) ------------
            SetMethod(global, "__dawnResize", [](const Napi::CallbackInfo& info) -> Napi::Value {
                Napi::Env env = info.Env();
                double width = info.Length() > 0 ? info[0].ToNumber().DoubleValue() : 0.0;
                double height = info.Length() > 1 ? info[1].ToNumber().DoubleValue() : 0.0;
                if (!g_bootstrapCanvas.IsEmpty())
                {
                    Napi::Object canvas = g_bootstrapCanvas.Value();
                    canvas.Set("width", Napi::Number::New(env, width));
                    canvas.Set("height", Napi::Number::New(env, height));
                    canvas.Set("clientWidth", Napi::Number::New(env, width));
                    canvas.Set("clientHeight", Napi::Number::New(env, height));
                }
                Napi::Value engine = env.Global().Get("__dawnEngine");
                if (engine.IsObject())
                {
                    Napi::Value setSize = engine.As<Napi::Object>().Get("setSize");
                    if (setSize.IsFunction())
                    {
                        setSize.As<Napi::Function>().Call(engine, {
                            Napi::Number::New(env, width),
                            Napi::Number::New(env, height),
                            Napi::Boolean::New(env, true)});
                    }
                }
                return env.Undefined();
            });

            // ---- deferred WebGPUEngine creation via the BABYLON global hook --
            g_babylon.Reset();
            Napi::Value existing = global.Get("BABYLON");
            if (existing.IsObject() && existing.As<Napi::Object>().Get("WebGPUEngine").IsFunction())
            {
                g_babylon = Napi::Persistent(existing);
                OnBabylonReady(env, existing);
            }
            else
            {
                Napi::Object desc = Napi::Object::New(env);
                desc.Set("configurable", Napi::Boolean::New(env, true));
                desc.Set("get", Napi::Function::New(env, [](const Napi::CallbackInfo& info) -> Napi::Value {
                    return g_babylon.IsEmpty() ? info.Env().Undefined() : g_babylon.Value();
                }));
                desc.Set("set", Napi::Function::New(env, [](const Napi::CallbackInfo& info) -> Napi::Value {
                    Napi::Env env = info.Env();
                    Napi::Value v = info.Length() > 0 ? info[0] : env.Undefined();
                    g_babylon = Napi::Persistent(v);
                    OnBabylonReady(env, v);
                    return env.Undefined();
                }));
                Napi::Object objectCtor = global.Get("Object").As<Napi::Object>();
                objectCtor.Get("defineProperty").As<Napi::Function>().Call(objectCtor,
                    {global, Napi::String::New(env, "BABYLON"), desc});
            }
        }
    } // namespace (bootstrap)

    namespace
    {
        // Directory containing the running executable (for TestUtils output dir).
        std::filesystem::path ExeDir()
        {
#if defined(_WIN32)
            wchar_t buf[MAX_PATH]{};
            ::GetModuleFileNameW(nullptr, buf, MAX_PATH);
            return std::filesystem::path(buf).parent_path();
#else
            return std::filesystem::current_path();
#endif
        }

        // Install a Dawn-compatible `TestUtils` global so validation_native.js runs
        // on the WebGPU backend. Mirrors the bgfx TestUtils plugin's JS surface but
        // is backed by the NativeDawn native functions (surface readback, image
        // decode) instead of the bgfx Graphics::DeviceContext.
        void InstallTestUtils(Napi::Env env)
        {
            Napi::Object global = env.Global();
            Napi::Object tu = Napi::Object::New(env);

            SetMethod(tu, "getGraphicsApiName", [](const Napi::CallbackInfo& info) -> Napi::Value {
                return Napi::String::New(info.Env(), "WebGPU");
            });
            SetMethod(tu, "getOutputDirectory", [](const Napi::CallbackInfo& info) -> Napi::Value {
                return Napi::String::New(info.Env(), ExeDir().string());
            });
            SetMethod(tu, "setTitle", [](const Napi::CallbackInfo& info) -> Napi::Value {
                Napi::Value fn = info.Env().Global().Get("_nativeDawnSetTitle");
                if (fn.IsFunction())
                {
                    fn.As<Napi::Function>().Call({info.Length() > 0 ? info[0] : info.Env().Undefined()});
                }
                return info.Env().Undefined();
            });
            SetMethod(tu, "updateSize", [](const Napi::CallbackInfo& info) -> Napi::Value {
                Napi::Env env = info.Env();
                Napi::Object global = env.Global();
                const uint32_t w = info.Length() > 0 && info[0].IsNumber() ? info[0].As<Napi::Number>().Uint32Value() : g_state.width;
                const uint32_t h = info.Length() > 1 && info[1].IsNumber() ? info[1].As<Napi::Number>().Uint32Value() : g_state.height;
                // Resize the Dawn surface (drawing buffer) to EXACTLY w x h so the
                // readback matches the reference-image size. Surface-only: don't
                // tie it to the window client area (which is smaller by the title
                // bar / menu and would give e.g. 600x380 instead of 600x400).
                ResizeSurface(w, h);
                Napi::Value dr = global.Get("__dawnResize");
                if (dr.IsFunction())
                {
                    dr.As<Napi::Function>().Call({Napi::Number::New(env, w), Napi::Number::New(env, h)});
                }
                return env.Undefined();
            });
            SetMethod(tu, "exit", [](const Napi::CallbackInfo& info) -> Napi::Value {
                const int code = info.Length() > 0 && info[0].IsNumber() ? info[0].As<Napi::Number>().Int32Value() : 0;
                std::fflush(stdout);
                std::fflush(stderr);
                std::quick_exit(code);
                return info.Env().Undefined();
            });
            SetMethod(tu, "captureNextFrame", Noop);

            // Reference-image decode: returns {width,height,__pixels(ArrayBuffer RGBA8)}.
            SetMethod(tu, "decodeImage", [](const Napi::CallbackInfo& info) -> Napi::Value {
                Napi::Value fn = info.Env().Global().Get("_nativeDawnDecodeImage");
                return fn.As<Napi::Function>().Call({info.Length() > 0 ? info[0] : info.Env().Undefined()});
            });
            SetMethod(tu, "getImageData", [](const Napi::CallbackInfo& info) -> Napi::Value {
                Napi::Env env = info.Env();
                Napi::Object img = info[0].As<Napi::Object>();
                Napi::ArrayBuffer ab = img.Get("__pixels").As<Napi::ArrayBuffer>();
                return Napi::Uint8Array::New(env, ab.ByteLength(), ab, 0);
            });

            // Framebuffer readback: the harness calls this inside the render loop
            // (after scene.render() but before the engine's endFrame submits). We
            // defer the actual readback to frame(), which runs after endFrame and
            // before present, so the surface texture holds the submitted render.
            SetMethod(tu, "getFrameBufferData", [](const Napi::CallbackInfo& info) -> Napi::Value {
                g_readbackCallback = Napi::Persistent(info[0].As<Napi::Function>());
                g_readbackPending = true;
                return info.Env().Undefined();
            });

            SetMethod(tu, "writePNG", [](const Napi::CallbackInfo& info) -> Napi::Value {
                Napi::Env env = info.Env();
                Napi::Uint8Array buffer = info[0].As<Napi::Uint8Array>();
                const uint32_t width = info[1].As<Napi::Number>().Uint32Value();
                const uint32_t height = info[2].As<Napi::Number>().Uint32Value();
                const std::string filename = info[3].As<Napi::String>().Utf8Value();
                if (buffer.ByteLength() < static_cast<size_t>(width) * height * 4)
                {
                    return env.Undefined();
                }
                bx::FileWriter writer;
                bx::FilePath filepath(filename.c_str());
                bx::makeAll(bx::FilePath(filepath.getPath()));
                bx::Error err;
                if (writer.open(filepath, false, &err))
                {
                    bimg::imageWritePng(&writer, width, height, width * 4, buffer.Data(),
                        bimg::TextureFormat::RGBA8, false);
                    writer.close();
                }
                return env.Undefined();
            });

            global.Set("TestUtils", tu);
        }
    } // namespace (testutils)

    void Initialize(Napi::Env env, void* window, uint32_t width, uint32_t height)
    {
        // Bind diagnostics to this environment first, so adapter/device creation
        // messages (including any Dawn validation errors raised during startup)
        // already reach the JS console rather than the stderr fallback.
        g_logEnv = env;
        g_jsThreadId = std::this_thread::get_id();

        if (!CreateDeviceAndSurface(window, width, height))
        {
            DawnLog(LogLevel::Error, "initialization failed");
            return;
        }

        // Milestone hook: a global to prove Dawn renders from JS without bgfx.
        // navigator.gpu and the full WebGPU surface are added incrementally.
        Napi::Object global = env.Global();
        global.Set("_nativeDawnClear", Napi::Function::New(env, [](const Napi::CallbackInfo& info) {
            float r = info.Length() > 0 ? info[0].ToNumber().FloatValue() : 0.0f;
            float g = info.Length() > 1 ? info[1].ToNumber().FloatValue() : 0.0f;
            float b = info.Length() > 2 ? info[2].ToNumber().FloatValue() : 0.0f;
            ClearToColor(r, g, b);
            return info.Env().Undefined();
        }, "_nativeDawnClear"));

        if (g_state.ready)
        {
            InstallWebGPU(env);
            InstallBootstrap(env);
            InstallTestUtils(env);

            // Replace the Canvas polyfill's bgfx-backed NativeCanvas (registered
            // earlier in Runtime.cpp) with the Dawn one. The bgfx version's
            // constructor throws on this path anyway, since it acquires the
            // Graphics::DeviceContext that only NativeEngine creates. This gives
            // 2D canvases — and therefore Babylon GUI, dynamic textures and text —
            // real rendering instead of the blit-only stub.
            Babylon::Plugins::Internal::NativeCanvasDawn::Initialize(env, g_state.device, g_state.instance);
        }
    }

    void Deinitialize(Napi::Env env)
    {
        // These references must be reset before Napi::Detach. Leaving them for
        // static destruction makes napi_delete_reference dereference a dead
        // napi_env during the CRT on-exit pass.
        g_readbackPending = false;
        g_readbackCallback.Reset();
        g_rafQueue.clear();
        g_blobRegistry.Reset();
        g_bootstrapCanvas.Reset();
        g_babylon.Reset();
        g_engineStarted = false;
        g_blobSeq = 0;

        // Drain deferred wrapper finalizers while both the N-API environment and
        // the Dawn device are valid.
        PumpJsFinalizers(env, true);

        // Deferred destroy callbacks own an extra resource reference. Run them
        // before releasing the plugin's device and instance references.
        for (auto& pending : g_pendingDestroy)
        {
            pending.second();
        }
        g_pendingDestroy.clear();

        if (g_state.currentSurfaceTexture)
        {
            wgpuTextureRelease(g_state.currentSurfaceTexture);
            g_state.currentSurfaceTexture = nullptr;
        }
        g_currentTextureAcquired = false;

        if (g_surfaceConfigured && g_state.surface)
        {
            wgpuSurfaceUnconfigure(g_state.surface);
        }
        g_surfaceConfigured = false;

        if (g_state.surface) wgpuSurfaceRelease(g_state.surface);
        if (g_state.queue) wgpuQueueRelease(g_state.queue);
        if (g_state.device) wgpuDeviceRelease(g_state.device);
        if (g_state.adapter) wgpuAdapterRelease(g_state.adapter);
        if (g_state.instance) wgpuInstanceRelease(g_state.instance);

        g_state = {};
        g_wrappersCreated = 0;
#if defined(NATIVEDAWN_V8_FINALIZER_DRAIN)
        g_wrappersAtLastPump = 0;
#endif

        // The environment is about to go away; anything logged after this point
        // has to use the stderr fallback.
        g_logEnv = nullptr;
        g_jsThreadId = {};
        g_requestedWidth = 0;
        g_requestedHeight = 0;
    }

    void Tick(Napi::Env)
    {
        if (g_state.ready)
        {
            wgpuInstanceProcessEvents(g_state.instance);
        }
    }

    void ResizeSurface(uint32_t width, uint32_t height)
    {
        if (width < 1) width = 1;
        if (height < 1) height = 1;
        if (!g_state.ready)
        {
            return;
        }
        // Goes through the same deferral as canvas.width/height so a resize
        // arriving mid-frame cannot destroy the texture the in-flight command
        // buffer is still referencing.
        ResizeDrawingBuffer(width, height);
    }
}
