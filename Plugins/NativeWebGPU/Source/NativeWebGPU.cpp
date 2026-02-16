#include <Babylon/Plugins/NativeWebGPU.h>
#include <Babylon/JsRuntime.h>
#include <Babylon/Graphics/WgpuInterop.h>

#include <napi/napi.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <cstdint>
#ifdef BABYLON_NATIVE_WEBGPU_TEST_HOOKS
#include <future>
#endif
#include <limits>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

namespace Babylon::Plugins::NativeWebGPU
{
    namespace
    {
        std::atomic_bool g_sawWebGpuDrawCall{false};
        std::atomic_uint64_t g_renderPipelineCreateCount{0};
        std::atomic_uint64_t g_commandEncoderCreateCount{0};
        std::atomic_uint64_t g_renderPassBeginCount{0};
        std::atomic_uint64_t g_queueSubmitCount{0};
        std::atomic_uint64_t g_drawCallCount{0};
        std::atomic_uint64_t g_textureCreateCount{0};
        std::atomic_uint64_t g_textureViewCreateCount{0};
        std::atomic_uint64_t g_bindGroupCreateCount{0};
        std::atomic_uint64_t g_bufferCreateCount{0};
        std::atomic_uint64_t g_bufferRequestedBytes{0};
        constexpr auto kBackendMode = "interop-shim-babylonjs-webgpu";
        constexpr auto kWebGpuDeveloperFeaturesMode = "webgpu-developer-features";
        constexpr auto kUnsafeWebGpuMode = "unsafe-webgpu";

        constexpr auto JS_NAVIGATOR_NAME = "navigator";
        constexpr auto JS_GPU_NAME = "gpu";

        struct TextureDescriptorData final
        {
            std::string Label{};
            std::string Format{"bgra8unorm"};
            std::string Dimension{"2d"};
            uint32_t Width{1};
            uint32_t Height{1};
            uint32_t DepthOrArrayLayers{1};
            uint32_t MipLevelCount{1};
            uint32_t SampleCount{1};
            uint32_t Usage{16};
        };

        struct CanvasContextState final
        {
            std::string Format{"bgra8unorm"};
            uint32_t Width{1280};
            uint32_t Height{720};
            uint32_t Usage{16};
            bool Configured{};
            bool Destroyed{};
            Napi::ObjectReference CachedTexture{};
        };

        struct ComputePassState final
        {
            std::string ShaderCode{};
            std::string EntryPoint{"main"};
        };

        struct RenderBundleState final
        {
            uint64_t DrawCallCount{};
        };

        struct CommandEncoderState final
        {
            Napi::ObjectReference RenderPass{};
            Napi::ObjectReference ComputePass{};
            Napi::ObjectReference CommandBuffer{};
        };

        uint32_t ToUint32(const Napi::Value& value, uint32_t fallback)
        {
            if (!value.IsNumber())
            {
                return fallback;
            }

            const auto raw = value.As<Napi::Number>().Int64Value();
            if (raw <= 0)
            {
                return fallback;
            }

            return static_cast<uint32_t>(std::min<int64_t>(raw, std::numeric_limits<uint32_t>::max()));
        }

        uint32_t GetUint32(const Napi::Object& object, const char* key, uint32_t fallback)
        {
            return object.Has(key) ? ToUint32(object.Get(key), fallback) : fallback;
        }

        std::string GetString(const Napi::Object& object, const char* key, std::string_view fallback = "")
        {
            if (!object.Has(key))
            {
                return std::string{fallback};
            }

            const auto value = object.Get(key);
            if (!value.IsString())
            {
                return std::string{fallback};
            }

            return value.As<Napi::String>().Utf8Value();
        }

        bool ImportCanvasTexturePayload(const Napi::Object& payload)
        {
            if (!payload.Has("nativeTexture"))
            {
                return false;
            }

            const auto nativeTextureValue = payload.Get("nativeTexture");
            if (!nativeTextureValue.IsExternal())
            {
                return false;
            }

            const void* nativeTexture = nativeTextureValue.As<Napi::External<void>>().Data();
            if (nativeTexture == nullptr)
            {
                return false;
            }

            const uint32_t width = payload.Has("width") ? ToUint32(payload.Get("width"), 1) : 1;
            const uint32_t height = payload.Has("height") ? ToUint32(payload.Get("height"), 1) : 1;
            return babylon_wgpu_import_canvas_texture_from_native(nativeTexture, width, height);
        }

        std::optional<Napi::Object> ExtractCanvasTexturePayload(const Napi::Value& sourceDescriptor)
        {
            if (!sourceDescriptor.IsObject())
            {
                return std::nullopt;
            }

            auto descriptor = sourceDescriptor.As<Napi::Object>();
            if (descriptor.Has("nativeTexture") && descriptor.Get("nativeTexture").IsExternal())
            {
                return descriptor;
            }

            if (!descriptor.Has("source"))
            {
                return std::nullopt;
            }

            auto sourceValue = descriptor.Get("source");
            if (!sourceValue.IsObject())
            {
                return std::nullopt;
            }

            auto sourceObject = sourceValue.As<Napi::Object>();
            if (sourceObject.Has("nativeTexture") && sourceObject.Get("nativeTexture").IsExternal())
            {
                return sourceObject;
            }

            if (!sourceObject.Has("getCanvasTexture"))
            {
                return std::nullopt;
            }

            auto getCanvasTextureValue = sourceObject.Get("getCanvasTexture");
            if (!getCanvasTextureValue.IsFunction())
            {
                return std::nullopt;
            }

            auto payloadValue = getCanvasTextureValue.As<Napi::Function>().Call(sourceObject, {});
            if (!payloadValue.IsObject())
            {
                return std::nullopt;
            }

            auto payloadObject = payloadValue.As<Napi::Object>();
            if (!payloadObject.Has("nativeTexture") || !payloadObject.Get("nativeTexture").IsExternal())
            {
                return std::nullopt;
            }

            return payloadObject;
        }

        void NoOpCallback(const Napi::CallbackInfo& info)
        {
            (void)info;
        }

        void MarkDrawRequestedCallback(const Napi::CallbackInfo& info)
        {
            (void)info;
            g_sawWebGpuDrawCall.store(true, std::memory_order_release);
            babylon_wgpu_mark_webgpu_draw_requested();
        }

        void MarkDrawCallCallback(const Napi::CallbackInfo& info)
        {
            (void)info;
            g_sawWebGpuDrawCall.store(true, std::memory_order_release);
            g_drawCallCount.fetch_add(1, std::memory_order_relaxed);
            babylon_wgpu_mark_webgpu_draw_requested();
        }

        Napi::Function GetCachedFunction(Napi::Env env, const char* key, void (*callback)(const Napi::CallbackInfo&))
        {
            auto nativeObject = JsRuntime::NativeObject::GetFromJavaScript(env);
            if (nativeObject.Has(key))
            {
                auto cached = nativeObject.Get(key);
                if (cached.IsFunction())
                {
                    return cached.As<Napi::Function>();
                }
            }

            auto function = Napi::Function::New(env, callback);
            nativeObject.Set(key, function);
            return function;
        }

        Napi::Function GetNoOpFunction(Napi::Env env)
        {
            return GetCachedFunction(env, "__nativeWebGpuNoOp", &NoOpCallback);
        }

        Napi::Function GetMarkDrawRequestedFunction(Napi::Env env)
        {
            return GetCachedFunction(env, "__nativeWebGpuMarkDrawRequested", &MarkDrawRequestedCallback);
        }

        Napi::Function GetMarkDrawCallFunction(Napi::Env env)
        {
            return GetCachedFunction(env, "__nativeWebGpuMarkDrawCall", &MarkDrawCallCallback);
        }

        constexpr bool kBuildEnableWebGpuDeveloperFeatures =
#if defined(BABYLON_NATIVE_ENABLE_WEBGPU_DEVELOPER_FEATURES) || defined(BABYLON_NATIVE_WEBGPU_TEST_HOOKS)
            true;
#else
            false;
#endif

        constexpr bool kBuildEnableUnsafeWebGpu =
#if defined(BABYLON_NATIVE_ENABLE_UNSAFE_WEBGPU) || defined(BABYLON_NATIVE_WEBGPU_TEST_HOOKS)
            true;
#else
            false;
#endif

        bool ReadBooleanFlag(const Napi::Object& object, const char* key)
        {
            if (!object.Has(key))
            {
                return false;
            }

            auto value = object.Get(key);
            if (value.IsBoolean())
            {
                return value.As<Napi::Boolean>().Value();
            }

            if (value.IsNumber())
            {
                return value.As<Napi::Number>().Int64Value() != 0;
            }

            if (value.IsString())
            {
                const auto text = value.As<Napi::String>().Utf8Value();
                return text == "1" || text == "true" || text == "on";
            }

            return false;
        }

        bool IsWebGpuDeveloperFeaturesEnabled(Napi::Env env)
        {
            if (kBuildEnableWebGpuDeveloperFeatures)
            {
                return true;
            }

            auto global = env.Global();
            // Chromium/WebKit-aligned naming for non-standard developer surfaces.
            static constexpr std::array<const char*, 4> kFlagNames{
                "__enableWebGPUDeveloperFeatures",
                "__webgpuDeveloperFeatures",
                "__webkitWebGPUDeveloperModeEnabled",
                "__webkitWebGPUDeveloperExtrasEnabled",
            };
            return std::any_of(kFlagNames.begin(), kFlagNames.end(), [&global](const char* flagName) {
                return ReadBooleanFlag(global, flagName);
            });
        }

        bool IsUnsafeWebGpuEnabled(Napi::Env env)
        {
            if (kBuildEnableUnsafeWebGpu)
            {
                return true;
            }

            auto global = env.Global();
            // Chromium-aligned "unsafe webgpu" naming for host-only interop hooks.
            static constexpr std::array<const char*, 3> kFlagNames{
                "__enableUnsafeWebGPU",
                "__unsafeWebGPU",
                "__allowUnsafeWebGPU",
            };
            return std::any_of(kFlagNames.begin(), kFlagNames.end(), [&global](const char* flagName) {
                return ReadBooleanFlag(global, flagName);
            });
        }

        using PromiseResolveFactory = std::function<Napi::Value(Napi::Env)>;

#ifdef BABYLON_NATIVE_WEBGPU_TEST_HOOKS
        struct FuturePromiseState final
        {
            std::future<std::string> Future{};
            std::shared_ptr<Napi::Promise::Deferred> Deferred{};
            PromiseResolveFactory ResolveFactory{};
            std::string OperationName{};
            std::string CallSiteStack{};
        };
#endif

        std::string CaptureCallSiteStack(Napi::Env env, const std::string& operationName)
        {
            auto errorValue = Napi::Error::New(env, operationName).Value();
            if (!errorValue.IsObject())
            {
                return {};
            }

            auto errorObject = errorValue.As<Napi::Object>();
            if (!errorObject.Has("stack"))
            {
                return {};
            }

            auto stackValue = errorObject.Get("stack");
            if (!stackValue.IsString())
            {
                return {};
            }

            return stackValue.As<Napi::String>().Utf8Value();
        }

        std::string MergeCallSiteStack(const std::string& errorMessage, const std::string& callSiteStack, const std::string& operationName)
        {
            std::string stack{"Error: " + errorMessage};

            if (!callSiteStack.empty())
            {
                const auto newlineIndex = callSiteStack.find('\n');
                if (newlineIndex != std::string::npos)
                {
                    stack += callSiteStack.substr(newlineIndex);
                }
            }

            if (!operationName.empty())
            {
                stack += "\n    at [native async] ";
                stack += operationName;
            }

            return stack;
        }

        Napi::Value CreateRejectedErrorValue(Napi::Env env, const std::string& errorMessage, const std::string& operationName, const std::string& callSiteStack)
        {
            auto rejectValue = Napi::Error::New(env, errorMessage).Value();
            if (!rejectValue.IsObject())
            {
                return rejectValue;
            }

            auto rejectObject = rejectValue.As<Napi::Object>();
            rejectObject.Set("nativeOperation", Napi::String::New(env, operationName));

            const auto mergedStack = MergeCallSiteStack(errorMessage, callSiteStack, operationName);
            if (!mergedStack.empty())
            {
                rejectObject.Set("stack", Napi::String::New(env, mergedStack));
            }

            return rejectValue;
        }

#ifdef BABYLON_NATIVE_WEBGPU_TEST_HOOKS
        void ScheduleFuturePromiseSettlement(
            Babylon::JsRuntime& runtime,
            std::shared_ptr<FuturePromiseState> state)
        {
            runtime.Dispatch([&runtime, state = std::move(state)](Napi::Env callbackEnv) {
                if (state->Future.wait_for(std::chrono::milliseconds{0}) != std::future_status::ready)
                {
                    ScheduleFuturePromiseSettlement(runtime, state);
                    return;
                }

                std::string errorMessage{};
                try
                {
                    errorMessage = state->Future.get();
                }
                catch (const std::exception& exception)
                {
                    errorMessage = exception.what();
                }
                catch (...)
                {
                    errorMessage = "Unknown asynchronous failure.";
                }

                Napi::HandleScope scope{callbackEnv};
                if (errorMessage.empty())
                {
                    try
                    {
                        state->Deferred->Resolve(state->ResolveFactory(callbackEnv));
                    }
                    catch (const Napi::Error& error)
                    {
                        errorMessage = error.Message();
                    }
                    catch (const std::exception& exception)
                    {
                        errorMessage = exception.what();
                    }
                    catch (...)
                    {
                        errorMessage = "Unknown JavaScript conversion failure.";
                    }
                }

                if (errorMessage.empty())
                {
                    return;
                }

                state->Deferred->Reject(CreateRejectedErrorValue(callbackEnv, errorMessage, state->OperationName, state->CallSiteStack));
            });
        }

        Napi::Promise ResolvePromiseFromFuture(
            Napi::Env env,
            std::future<std::string>&& future,
            PromiseResolveFactory resolveFactory,
            std::string operationName)
        {
            auto state = std::make_shared<FuturePromiseState>();
            state->Future = std::move(future);
            state->Deferred = std::make_shared<Napi::Promise::Deferred>(Napi::Promise::Deferred::New(env));
            state->ResolveFactory = std::move(resolveFactory);
            state->OperationName = std::move(operationName);
            state->CallSiteStack = CaptureCallSiteStack(env, state->OperationName);

            auto promise = state->Deferred->Promise();
            auto& runtime = Babylon::JsRuntime::GetFromJavaScript(env);
            ScheduleFuturePromiseSettlement(runtime, state);

            return promise;
        }
#endif

        Napi::Promise ResolvePromiseAsync(
            Napi::Env env,
            PromiseResolveFactory resolveFactory,
            std::string operationName)
        {
            auto deferred = std::make_shared<Napi::Promise::Deferred>(Napi::Promise::Deferred::New(env));
            auto callSiteStack = CaptureCallSiteStack(env, operationName);
            auto promise = deferred->Promise();
            auto& runtime = Babylon::JsRuntime::GetFromJavaScript(env);

            runtime.Dispatch([deferred = std::move(deferred),
                                 resolveFactory = std::move(resolveFactory),
                                 operationName = std::move(operationName),
                                 callSiteStack = std::move(callSiteStack)](Napi::Env callbackEnv) mutable {
                Napi::HandleScope scope{callbackEnv};

                try
                {
                    deferred->Resolve(resolveFactory(callbackEnv));
                    return;
                }
                catch (const Napi::Error& error)
                {
                    deferred->Reject(CreateRejectedErrorValue(callbackEnv, error.Message(), operationName, callSiteStack));
                    return;
                }
                catch (const std::exception& exception)
                {
                    deferred->Reject(CreateRejectedErrorValue(callbackEnv, exception.what(), operationName, callSiteStack));
                    return;
                }
                catch (...)
                {
                    deferred->Reject(CreateRejectedErrorValue(callbackEnv, "Unknown asynchronous failure.", operationName, callSiteStack));
                    return;
                }
            });

            return promise;
        }

        Napi::Value GetCachedResolvedUndefinedPromise(Napi::Env env)
        {
            constexpr auto CACHE_KEY = "__nativeWebGpuResolvedUndefinedPromise";
            auto global = env.Global();
            if (global.Has(CACHE_KEY))
            {
                auto cached = global.Get(CACHE_KEY);
                if (cached.IsObject())
                {
                    return cached;
                }
            }

            auto deferred = Napi::Promise::Deferred::New(env);
            deferred.Resolve(env.Undefined());
            auto promise = deferred.Promise();
            // Hot-path APIs (mapAsync/onSubmittedWorkDone/popErrorScope) can be
            // called every frame; reusing a settled Promise avoids per-frame churn.
            // wgpu-native currently reports NULL_FUTURE for these async C-ABI calls
            // on our target matrix, so completion is callback/immediate-driven and
            // we intentionally do not model per-call future identity here.
            // Non-CTS note: this is intentionally not per-call Promise identity.
            global.Set(CACHE_KEY, promise);
            return promise;
        }

        Napi::Value CreateNeverPromise(Napi::Env env)
        {
            auto promiseCtorValue = env.Global().Get("Promise");
            if (!promiseCtorValue.IsFunction())
            {
                return env.Undefined();
            }

            auto executor = Napi::Function::New(env, [](const Napi::CallbackInfo& info) {
                (void)info;
            });

            return promiseCtorValue.As<Napi::Function>().New({executor});
        }

        Napi::Object CreateSet(Napi::Env env)
        {
            auto setCtorValue = env.Global().Get("Set");
            if (!setCtorValue.IsFunction())
            {
                return Napi::Array::New(env);
            }

            return setCtorValue.As<Napi::Function>().New({});
        }

        // TODO(spec-compliance): These limits are hardcoded conservative defaults rather
        // than queried from the actual GPU adapter via the Rust backend. They should be
        // forwarded from the adapter's real limits once the FFI surface supports it.
        Napi::Object CreateLimits(Napi::Env env)
        {
            auto limits = Napi::Object::New(env);

            limits.Set("maxTextureDimension1D", Napi::Number::From(env, 8192));
            limits.Set("maxTextureDimension2D", Napi::Number::From(env, 8192));
            limits.Set("maxTextureDimension3D", Napi::Number::From(env, 2048));
            limits.Set("maxTextureArrayLayers", Napi::Number::From(env, 256));
            limits.Set("maxBindGroups", Napi::Number::From(env, 4));
            limits.Set("maxBindingsPerBindGroup", Napi::Number::From(env, 1000));
            limits.Set("maxDynamicUniformBuffersPerPipelineLayout", Napi::Number::From(env, 8));
            limits.Set("maxDynamicStorageBuffersPerPipelineLayout", Napi::Number::From(env, 4));
            limits.Set("maxSampledTexturesPerShaderStage", Napi::Number::From(env, 16));
            limits.Set("maxSamplersPerShaderStage", Napi::Number::From(env, 16));
            limits.Set("maxStorageBuffersPerShaderStage", Napi::Number::From(env, 8));
            limits.Set("maxStorageTexturesPerShaderStage", Napi::Number::From(env, 4));
            limits.Set("maxUniformBuffersPerShaderStage", Napi::Number::From(env, 12));
            limits.Set("maxUniformBufferBindingSize", Napi::Number::From(env, 65536));
            limits.Set("maxStorageBufferBindingSize", Napi::Number::From(env, 134217728));
            limits.Set("maxVertexBuffers", Napi::Number::From(env, 8));
            limits.Set("maxBufferSize", Napi::Number::From(env, 268435456));
            limits.Set("maxVertexAttributes", Napi::Number::From(env, 16));
            limits.Set("maxVertexBufferArrayStride", Napi::Number::From(env, 2048));
            limits.Set("maxInterStageShaderComponents", Napi::Number::From(env, 124));
            limits.Set("maxInterStageShaderVariables", Napi::Number::From(env, 31));
            limits.Set("maxColorAttachments", Napi::Number::From(env, 8));
            limits.Set("maxColorAttachmentBytesPerSample", Napi::Number::From(env, 32));
            limits.Set("maxComputeWorkgroupStorageSize", Napi::Number::From(env, 16384));
            limits.Set("maxComputeInvocationsPerWorkgroup", Napi::Number::From(env, 256));
            limits.Set("maxComputeWorkgroupSizeX", Napi::Number::From(env, 256));
            limits.Set("maxComputeWorkgroupSizeY", Napi::Number::From(env, 256));
            limits.Set("maxComputeWorkgroupSizeZ", Napi::Number::From(env, 64));
            limits.Set("maxComputeWorkgroupsPerDimension", Napi::Number::From(env, 65535));

            return limits;
        }

        TextureDescriptorData ParseTextureDescriptor(const Napi::CallbackInfo& info, uint32_t fallbackWidth = 1, uint32_t fallbackHeight = 1, const std::string& fallbackFormat = "bgra8unorm")
        {
            TextureDescriptorData descriptor{};
            descriptor.Width = fallbackWidth;
            descriptor.Height = fallbackHeight;
            descriptor.Format = fallbackFormat;

            if (info.Length() == 0 || !info[0].IsObject())
            {
                return descriptor;
            }

            const auto jsDescriptor = info[0].As<Napi::Object>();
            descriptor.Label = GetString(jsDescriptor, "label", "");
            descriptor.Format = GetString(jsDescriptor, "format", descriptor.Format);
            descriptor.Dimension = GetString(jsDescriptor, "dimension", descriptor.Dimension);
            descriptor.MipLevelCount = GetUint32(jsDescriptor, "mipLevelCount", descriptor.MipLevelCount);
            descriptor.SampleCount = GetUint32(jsDescriptor, "sampleCount", descriptor.SampleCount);
            descriptor.Usage = GetUint32(jsDescriptor, "usage", descriptor.Usage);

            if (jsDescriptor.Has("size"))
            {
                auto size = jsDescriptor.Get("size");
                if (size.IsArray())
                {
                    auto array = size.As<Napi::Array>();
                    descriptor.Width = array.Length() > 0 ? ToUint32(array.Get(static_cast<uint32_t>(0)), descriptor.Width) : descriptor.Width;
                    descriptor.Height = array.Length() > 1 ? ToUint32(array.Get(static_cast<uint32_t>(1)), descriptor.Height) : descriptor.Height;
                    descriptor.DepthOrArrayLayers = array.Length() > 2 ? ToUint32(array.Get(static_cast<uint32_t>(2)), descriptor.DepthOrArrayLayers) : descriptor.DepthOrArrayLayers;
                }
                else if (size.IsObject())
                {
                    auto sizeObject = size.As<Napi::Object>();
                    descriptor.Width = GetUint32(sizeObject, "width", descriptor.Width);
                    descriptor.Height = GetUint32(sizeObject, "height", descriptor.Height);
                    descriptor.DepthOrArrayLayers = GetUint32(sizeObject, "depthOrArrayLayers", descriptor.DepthOrArrayLayers);
                }
            }

            return descriptor;
        }

        Napi::Object CreateGpuTexture(const Napi::CallbackInfo& info, const TextureDescriptorData& descriptor)
        {
            auto env = info.Env();
            auto texture = Napi::Object::New(env);
            g_textureCreateCount.fetch_add(1, std::memory_order_relaxed);

            texture.Set("label", Napi::String::New(env, descriptor.Label));
            texture.Set("format", Napi::String::New(env, descriptor.Format));
            texture.Set("dimension", Napi::String::New(env, descriptor.Dimension));
            texture.Set("width", Napi::Number::From(env, descriptor.Width));
            texture.Set("height", Napi::Number::From(env, descriptor.Height));
            texture.Set("depthOrArrayLayers", Napi::Number::From(env, descriptor.DepthOrArrayLayers));
            texture.Set("mipLevelCount", Napi::Number::From(env, descriptor.MipLevelCount));
            texture.Set("sampleCount", Napi::Number::From(env, descriptor.SampleCount));
            texture.Set("usage", Napi::Number::From(env, descriptor.Usage));
            // The Babylon render loop requests a default texture view every frame.
            // Cache the descriptor-less view to avoid transient JS allocations.
            texture.Set("__defaultView", env.Undefined());

            texture.Set("createView", Napi::Function::New(env, [descriptor](const Napi::CallbackInfo& viewInfo) -> Napi::Value {
                const bool hasDescriptor = viewInfo.Length() > 0 && viewInfo[0].IsObject();
                auto textureObject = viewInfo.This().As<Napi::Object>();
                if (!hasDescriptor && textureObject.Has("__defaultView"))
                {
                    auto cachedView = textureObject.Get("__defaultView");
                    if (cachedView.IsObject())
                    {
                        return cachedView;
                    }
                }

                auto viewFormat = descriptor.Format;
                auto viewDimension = descriptor.Dimension;
                auto mipLevelCount = descriptor.MipLevelCount;
                auto arrayLayerCount = descriptor.DepthOrArrayLayers;
                double descriptorCacheHash{0.0};
                std::string viewLabel{};

                if (hasDescriptor)
                {
                    const auto viewDescriptor = viewInfo[0].As<Napi::Object>();
                    viewFormat = GetString(viewDescriptor, "format", viewFormat);
                    viewDimension = GetString(viewDescriptor, "dimension", viewDimension);
                    mipLevelCount = GetUint32(viewDescriptor, "mipLevelCount", mipLevelCount);
                    arrayLayerCount = GetUint32(viewDescriptor, "arrayLayerCount", arrayLayerCount);
                    viewLabel = GetString(viewDescriptor, "label", "");

                    // Hash the descriptor components to avoid per-frame string
                    // allocation for cache key comparison. FNV-1a on the concatenated
                    // format|dimension|mipLevel|arrayLayer values.
                    auto fnvHash = [](std::string_view s, uint64_t h = 14695981039346656037ULL) -> uint64_t {
                        for (auto c : s) { h ^= static_cast<uint64_t>(c); h *= 1099511628211ULL; }
                        return h;
                    };
                    auto h = fnvHash(viewFormat);
                    h = fnvHash(viewDimension, h ^ 0xff);
                    h ^= static_cast<uint64_t>(mipLevelCount) * 2654435761ULL;
                    h ^= static_cast<uint64_t>(arrayLayerCount) * 2246822519ULL;
                    const double descriptorCacheHash = static_cast<double>(h);

                    if (textureObject.Has("__descriptorViewKey"))
                    {
                        auto cachedKey = textureObject.Get("__descriptorViewKey");
                        if (cachedKey.IsNumber() &&
                            cachedKey.As<Napi::Number>().DoubleValue() == descriptorCacheHash &&
                            textureObject.Has("__descriptorView"))
                        {
                            auto cachedView = textureObject.Get("__descriptorView");
                            if (cachedView.IsObject())
                            {
                                return cachedView;
                            }
                        }
                    }

                }

                auto view = Napi::Object::New(viewInfo.Env());
                g_textureViewCreateCount.fetch_add(1, std::memory_order_relaxed);
                view.Set("label", Napi::String::New(viewInfo.Env(), viewLabel));

                view.Set("format", Napi::String::New(viewInfo.Env(), viewFormat));
                view.Set("dimension", Napi::String::New(viewInfo.Env(), viewDimension));
                view.Set("mipLevelCount", Napi::Number::From(viewInfo.Env(), mipLevelCount));
                view.Set("arrayLayerCount", Napi::Number::From(viewInfo.Env(), arrayLayerCount));
                view.Set("texture", textureObject);

                if (!hasDescriptor)
                {
                    textureObject.Set("__defaultView", view);
                }
                else
                {
                    textureObject.Set("__descriptorViewKey", Napi::Number::New(viewInfo.Env(), descriptorCacheHash));
                    textureObject.Set("__descriptorView", view);
                }

                return view;
            }));

            texture.Set("destroy", Napi::Function::New(env, [](const Napi::CallbackInfo& destroyInfo) {
                destroyInfo.This().As<Napi::Object>().Set("__destroyed", Napi::Boolean::New(destroyInfo.Env(), true));
            }));

            return texture;
        }

        Napi::Object CreateGpuRenderPassEncoder(Napi::Env env)
        {
            auto pass = Napi::Object::New(env);
            auto noOp = GetNoOpFunction(env);

            pass.Set("setPipeline", GetMarkDrawRequestedFunction(env));
            pass.Set("setBindGroup", noOp);
            pass.Set("setVertexBuffer", noOp);
            pass.Set("setIndexBuffer", noOp);
            pass.Set("setViewport", noOp);
            pass.Set("setScissorRect", noOp);
            pass.Set("setStencilReference", noOp);
            pass.Set("setBlendConstant", noOp);
            auto markDrawCall = GetMarkDrawCallFunction(env);
            pass.Set("draw", markDrawCall);
            pass.Set("drawIndexed", markDrawCall);
            pass.Set("drawIndirect", markDrawCall);
            pass.Set("drawIndexedIndirect", markDrawCall);
            pass.Set("executeBundles", Napi::Function::New(env, [](const Napi::CallbackInfo& info) {
                if (info.Length() == 0 || !info[0].IsArray())
                {
                    return;
                }

                const auto bundles = info[0].As<Napi::Array>();
                uint64_t bundledDrawCalls{};

                for (uint32_t i = 0; i < bundles.Length(); ++i)
                {
                    const auto bundleValue = bundles.Get(i);
                    if (!bundleValue.IsObject())
                    {
                        continue;
                    }

                    const auto bundle = bundleValue.As<Napi::Object>();
                    if (!bundle.Has("__drawCallCount") || !bundle.Get("__drawCallCount").IsNumber())
                    {
                        continue;
                    }

                    const auto drawCount = bundle.Get("__drawCallCount").As<Napi::Number>().Int64Value();
                    if (drawCount > 0)
                    {
                        bundledDrawCalls += static_cast<uint64_t>(drawCount);
                    }
                }

                if (bundledDrawCalls > 0)
                {
                    g_sawWebGpuDrawCall.store(true, std::memory_order_release);
                    g_drawCallCount.fetch_add(bundledDrawCalls, std::memory_order_relaxed);
                    babylon_wgpu_mark_webgpu_draw_requested();
                }
            }));
            pass.Set("end", noOp);

            return pass;
        }

        Napi::Object CreateGpuRenderBundleEncoder(Napi::Env env)
        {
            auto encoder = Napi::Object::New(env);
            auto state = std::make_shared<RenderBundleState>();
            auto noOp = GetNoOpFunction(env);

            encoder.Set("setPipeline", noOp);
            encoder.Set("setBindGroup", noOp);
            encoder.Set("setVertexBuffer", noOp);
            encoder.Set("setIndexBuffer", noOp);
            encoder.Set("setViewport", noOp);
            encoder.Set("setScissorRect", noOp);
            encoder.Set("setStencilReference", noOp);
            encoder.Set("setBlendConstant", noOp);
            encoder.Set("pushDebugGroup", noOp);
            encoder.Set("popDebugGroup", noOp);
            encoder.Set("insertDebugMarker", noOp);

            encoder.Set("draw", Napi::Function::New(env, [state](const Napi::CallbackInfo& info) {
                (void)info;
                state->DrawCallCount += 1;
            }));
            encoder.Set("drawIndexed", Napi::Function::New(env, [state](const Napi::CallbackInfo& info) {
                (void)info;
                state->DrawCallCount += 1;
            }));
            encoder.Set("drawIndirect", Napi::Function::New(env, [state](const Napi::CallbackInfo& info) {
                (void)info;
                state->DrawCallCount += 1;
            }));
            encoder.Set("drawIndexedIndirect", Napi::Function::New(env, [state](const Napi::CallbackInfo& info) {
                (void)info;
                state->DrawCallCount += 1;
            }));

            encoder.Set("finish", Napi::Function::New(env, [state](const Napi::CallbackInfo& info) -> Napi::Value {
                auto bundle = Napi::Object::New(info.Env());
                bundle.Set("__drawCallCount", Napi::Number::From(info.Env(), static_cast<double>(state->DrawCallCount)));
                return bundle;
            }));

            return encoder;
        }

        Napi::Object CreateGpuComputePassEncoder(Napi::Env env)
        {
            auto pass = Napi::Object::New(env);
            auto noOp = GetNoOpFunction(env);
            auto state = std::make_shared<ComputePassState>();

            pass.Set("setPipeline", Napi::Function::New(env, [state](const Napi::CallbackInfo& info) {
                if (info.Length() == 0 || !info[0].IsObject())
                {
                    return;
                }

                const auto pipeline = info[0].As<Napi::Object>();
                state->ShaderCode = GetString(pipeline, "__wgslCode", "");
                state->EntryPoint = GetString(pipeline, "__entryPoint", "main");
            }));
            pass.Set("setBindGroup", noOp);
            pass.Set("dispatchWorkgroups", Napi::Function::New(env, [state](const Napi::CallbackInfo& info) {
                if (state->ShaderCode.empty())
                {
                    return;
                }

                const auto x = info.Length() > 0 ? ToUint32(info[0], 1) : 1;
                const auto y = info.Length() > 1 ? ToUint32(info[1], 1) : 1;
                const auto z = info.Length() > 2 ? ToUint32(info[2], 1) : 1;

                if (!babylon_wgpu_dispatch_compute_global(state->ShaderCode.c_str(), state->EntryPoint.c_str(), x, y, z))
                {
                    Napi::Error::New(info.Env(), "NativeWebGPU compute dispatch failed in wgpu backend.")
                        .ThrowAsJavaScriptException();
                }
            }));
            pass.Set("dispatchWorkgroupsIndirect", noOp);
            pass.Set("end", noOp);

            return pass;
        }

        Napi::Object CreateGpuCommandEncoder(Napi::Env env)
        {
            auto encoder = Napi::Object::New(env);
            auto noOp = GetNoOpFunction(env);
            auto state = std::make_shared<CommandEncoderState>();

            encoder.Set("beginRenderPass", Napi::Function::New(env, [state](const Napi::CallbackInfo& info) -> Napi::Value {
                g_renderPassBeginCount.fetch_add(1, std::memory_order_relaxed);
                if (state->RenderPass.IsEmpty())
                {
                    state->RenderPass = Napi::Persistent(CreateGpuRenderPassEncoder(info.Env()));
                }
                return state->RenderPass.Value();
            }));
            encoder.Set("beginComputePass", Napi::Function::New(env, [state](const Napi::CallbackInfo& info) -> Napi::Value {
                if (state->ComputePass.IsEmpty())
                {
                    state->ComputePass = Napi::Persistent(CreateGpuComputePassEncoder(info.Env()));
                }
                return state->ComputePass.Value();
            }));

            encoder.Set("copyBufferToBuffer", noOp);
            encoder.Set("copyTextureToTexture", noOp);
            encoder.Set("copyTextureToBuffer", noOp);
            encoder.Set("copyBufferToTexture", noOp);
            encoder.Set("clearBuffer", noOp);
            encoder.Set("pushDebugGroup", noOp);
            encoder.Set("popDebugGroup", noOp);
            encoder.Set("insertDebugMarker", noOp);

            encoder.Set("finish", Napi::Function::New(env, [state](const Napi::CallbackInfo& info) -> Napi::Value {
                if (state->CommandBuffer.IsEmpty())
                {
                    state->CommandBuffer = Napi::Persistent(Napi::Object::New(info.Env()));
                }
                return state->CommandBuffer.Value();
            }));

            return encoder;
        }

        Napi::Object CreateGpuShaderModule(Napi::Env env, std::string code)
        {
            auto shaderModule = Napi::Object::New(env);
            shaderModule.Set("__wgslCode", Napi::String::New(env, code));
            shaderModule.Set("getCompilationInfo", Napi::Function::New(env, [](const Napi::CallbackInfo& info) -> Napi::Value {
                return ResolvePromiseAsync(info.Env(), [](Napi::Env callbackEnv) -> Napi::Value {
                    auto result = Napi::Object::New(callbackEnv);
                    result.Set("messages", Napi::Array::New(callbackEnv, 0));
                    return result;
                }, "GPUShaderModule.getCompilationInfo");
            }));
            return shaderModule;
        }

        Napi::Object CreateGpuRenderPipeline(Napi::Env env)
        {
            auto pipeline = Napi::Object::New(env);
            pipeline.Set("getBindGroupLayout", Napi::Function::New(env, [](const Napi::CallbackInfo& info) -> Napi::Value {
                return Napi::Object::New(info.Env());
            }));
            return pipeline;
        }

        Napi::Object CreateGpuComputePipeline(Napi::Env env, std::string shaderCode, std::string entryPoint)
        {
            auto pipeline = Napi::Object::New(env);
            pipeline.Set("__wgslCode", Napi::String::New(env, shaderCode));
            pipeline.Set("__entryPoint", Napi::String::New(env, entryPoint));
            pipeline.Set("getBindGroupLayout", Napi::Function::New(env, [](const Napi::CallbackInfo& info) -> Napi::Value {
                return Napi::Object::New(info.Env());
            }));
            return pipeline;
        }

        Napi::Object CreateGpuBuffer(Napi::Env env, size_t size)
        {
            auto buffer = Napi::Object::New(env);
            buffer.Set("size", Napi::Number::From(env, size));
            g_bufferCreateCount.fetch_add(1, std::memory_order_relaxed);
            g_bufferRequestedBytes.fetch_add(static_cast<uint64_t>(size), std::memory_order_relaxed);

            buffer.Set("mapAsync", Napi::Function::New(env, [](const Napi::CallbackInfo& info) -> Napi::Value {
                return GetCachedResolvedUndefinedPromise(info.Env());
            }));

            buffer.Set("getMappedRange", Napi::Function::New(env, [size](const Napi::CallbackInfo& info) -> Napi::Value {
                size_t offset{};
                size_t byteLength{size};

                if (info.Length() > 0 && info[0].IsNumber())
                {
                    offset = static_cast<size_t>(std::max<int64_t>(0, info[0].As<Napi::Number>().Int64Value()));
                }
                if (info.Length() > 1 && info[1].IsNumber())
                {
                    byteLength = static_cast<size_t>(std::max<int64_t>(0, info[1].As<Napi::Number>().Int64Value()));
                }
                else if (offset < size)
                {
                    byteLength = size - offset;
                }
                else
                {
                    byteLength = 0;
                }

                auto bufferObject = info.This().As<Napi::Object>();
                if (bufferObject.Has("__cachedMappedRange") &&
                    bufferObject.Has("__cachedMappedRangeOffset") &&
                    bufferObject.Has("__cachedMappedRangeLength"))
                {
                    const auto cachedOffsetValue = bufferObject.Get("__cachedMappedRangeOffset");
                    const auto cachedLengthValue = bufferObject.Get("__cachedMappedRangeLength");
                    const auto cachedRangeValue = bufferObject.Get("__cachedMappedRange");

                    if (cachedOffsetValue.IsNumber() &&
                        cachedLengthValue.IsNumber() &&
                        cachedRangeValue.IsArrayBuffer())
                    {
                        const auto cachedOffset = static_cast<size_t>(
                            std::max<int64_t>(0, cachedOffsetValue.As<Napi::Number>().Int64Value()));
                        const auto cachedLength = static_cast<size_t>(
                            std::max<int64_t>(0, cachedLengthValue.As<Napi::Number>().Int64Value()));

                        if (cachedOffset == offset && cachedLength == byteLength)
                        {
                            return cachedRangeValue;
                        }
                    }
                }

                auto mappedRange = Napi::ArrayBuffer::New(info.Env(), byteLength);
                // Hot-path optimization: Babylon can query mapped ranges every frame.
                // Reusing the same backing ArrayBuffer for identical range requests
                // avoids transient JS heap churn in simulator/device loops.
                // Non-CTS note: this intentionally keeps stable object identity.
                bufferObject.Set("__cachedMappedRange", mappedRange);
                bufferObject.Set("__cachedMappedRangeOffset",
                    Napi::Number::From(info.Env(), static_cast<double>(offset)));
                bufferObject.Set("__cachedMappedRangeLength",
                    Napi::Number::From(info.Env(), static_cast<double>(byteLength)));
                return mappedRange;
            }));

            auto noOp = GetNoOpFunction(env);
            buffer.Set("unmap", noOp);
            buffer.Set("destroy", noOp);

            return buffer;
        }

        Napi::Object CreateGpuQueue(Napi::Env env)
        {
            auto queue = Napi::Object::New(env);
            auto noOp = GetNoOpFunction(env);

            queue.Set("submit", Napi::Function::New(env, [](const Napi::CallbackInfo& info) {
                (void)info;
                g_queueSubmitCount.fetch_add(1, std::memory_order_relaxed);
                if (g_sawWebGpuDrawCall.load(std::memory_order_acquire))
                {
                    babylon_wgpu_mark_webgpu_draw_requested();
                }
            }));
            queue.Set("writeBuffer", noOp);
            queue.Set("writeTexture", noOp);
            queue.Set("copyExternalImageToTexture", Napi::Function::New(env, [](const Napi::CallbackInfo& info) {
                if (info.Length() == 0)
                {
                    return;
                }

                // Standards-aligned bridge:
                // copyExternalImageToTexture({ source: canvasLike }, dst, size)
                // where `canvasLike` can expose getCanvasTexture() in this host.
                auto payload = ExtractCanvasTexturePayload(info[0]);
                if (!payload.has_value())
                {
                    return;
                }

                if (ImportCanvasTexturePayload(*payload))
                {
                    babylon_wgpu_mark_webgpu_draw_requested();
                }
            }));
            queue.Set("onSubmittedWorkDone", Napi::Function::New(env, [](const Napi::CallbackInfo& info) -> Napi::Value {
                return GetCachedResolvedUndefinedPromise(info.Env());
            }));

            return queue;
        }

        Napi::Object CreateGpuDevice(Napi::Env env)
        {
            auto device = Napi::Object::New(env);
            auto noOp = GetNoOpFunction(env);

            device.Set("features", CreateSet(env));
            device.Set("limits", CreateLimits(env));
            device.Set("queue", CreateGpuQueue(env));
            // TODO(spec-compliance): device.lost is a never-resolving promise. The shim
            // does not model device loss. When the Rust backend detects device loss (e.g.
            // adapter removal), this should resolve with a GPUDeviceLostInfo.
            device.Set("lost", CreateNeverPromise(env));

            device.Set("addEventListener", noOp);
            device.Set("removeEventListener", noOp);
            device.Set("destroy", noOp);
            // TODO(spec-compliance): Error scopes are completely opaque -- pushErrorScope
            // is a no-op and popErrorScope always resolves with undefined. GPU validation
            // errors from the Rust backend are never surfaced to JS. This should forward
            // to the wgpu device's error callback once the FFI supports it.
            device.Set("pushErrorScope", noOp);

            device.Set("popErrorScope", Napi::Function::New(env, [](const Napi::CallbackInfo& info) -> Napi::Value {
                return GetCachedResolvedUndefinedPromise(info.Env());
            }));

            device.Set("createBuffer", Napi::Function::New(env, [](const Napi::CallbackInfo& info) -> Napi::Value {
                size_t size{};
                if (info.Length() > 0 && info[0].IsObject())
                {
                    auto descriptor = info[0].As<Napi::Object>();
                    if (descriptor.Has("size") && descriptor.Get("size").IsNumber())
                    {
                        size = static_cast<size_t>(std::max<int64_t>(0, descriptor.Get("size").As<Napi::Number>().Int64Value()));
                    }
                }
                return CreateGpuBuffer(info.Env(), size);
            }));

            device.Set("createTexture", Napi::Function::New(env, [](const Napi::CallbackInfo& info) -> Napi::Value {
                auto descriptor = ParseTextureDescriptor(info);
                return CreateGpuTexture(info, descriptor);
            }));

            device.Set("createSampler", Napi::Function::New(env, [](const Napi::CallbackInfo& info) -> Napi::Value {
                return Napi::Object::New(info.Env());
            }));

            device.Set("createShaderModule", Napi::Function::New(env, [](const Napi::CallbackInfo& info) -> Napi::Value {
                std::string code{};
                if (info.Length() > 0 && info[0].IsObject())
                {
                    const auto descriptor = info[0].As<Napi::Object>();
                    code = GetString(descriptor, "code", "");
                }

                return CreateGpuShaderModule(info.Env(), std::move(code));
            }));

            device.Set("createCommandEncoder", Napi::Function::New(env, [](const Napi::CallbackInfo& info) -> Napi::Value {
                g_commandEncoderCreateCount.fetch_add(1, std::memory_order_relaxed);
                auto deviceObject = info.This().As<Napi::Object>();
                if (deviceObject.Has("__cachedCommandEncoder"))
                {
                    auto cachedEncoder = deviceObject.Get("__cachedCommandEncoder");
                    if (cachedEncoder.IsObject())
                    {
                        return cachedEncoder;
                    }
                }

                auto encoder = CreateGpuCommandEncoder(info.Env());
                // Reuse a single command encoder wrapper object to keep the
                // render-loop path allocation-free on the JS side.
                // Non-CTS note: command-encoder object identity is stable here.
                deviceObject.Set("__cachedCommandEncoder", encoder);
                return encoder;
            }));

            device.Set("createBindGroupLayout", Napi::Function::New(env, [](const Napi::CallbackInfo& info) -> Napi::Value {
                return Napi::Object::New(info.Env());
            }));

            device.Set("createPipelineLayout", Napi::Function::New(env, [](const Napi::CallbackInfo& info) -> Napi::Value {
                return Napi::Object::New(info.Env());
            }));

            device.Set("createBindGroup", Napi::Function::New(env, [](const Napi::CallbackInfo& info) -> Napi::Value {
                g_bindGroupCreateCount.fetch_add(1, std::memory_order_relaxed);
                return Napi::Object::New(info.Env());
            }));

            device.Set("createRenderPipeline", Napi::Function::New(env, [](const Napi::CallbackInfo& info) -> Napi::Value {
                g_renderPipelineCreateCount.fetch_add(1, std::memory_order_relaxed);
                return CreateGpuRenderPipeline(info.Env());
            }));

            device.Set("createRenderPipelineAsync", Napi::Function::New(env, [](const Napi::CallbackInfo& info) -> Napi::Value {
                g_renderPipelineCreateCount.fetch_add(1, std::memory_order_relaxed);
                const bool hasDescriptor = info.Length() > 0 && info[0].IsObject();
                return ResolvePromiseAsync(info.Env(), [hasDescriptor](Napi::Env callbackEnv) -> Napi::Value {
                    if (!hasDescriptor)
                    {
                        throw std::runtime_error{"createRenderPipelineAsync requires a descriptor object."};
                    }
                    return CreateGpuRenderPipeline(callbackEnv);
                }, "GPUDevice.createRenderPipelineAsync");
            }));

            device.Set("createRenderBundleEncoder", Napi::Function::New(env, [](const Napi::CallbackInfo& info) -> Napi::Value {
                (void)info;
                return CreateGpuRenderBundleEncoder(info.Env());
            }));

            device.Set("createComputePipeline", Napi::Function::New(env, [](const Napi::CallbackInfo& info) -> Napi::Value {
                std::string shaderCode{};
                std::string entryPoint{"main"};

                if (info.Length() > 0 && info[0].IsObject())
                {
                    const auto descriptor = info[0].As<Napi::Object>();
                    if (descriptor.Has("compute") && descriptor.Get("compute").IsObject())
                    {
                        const auto compute = descriptor.Get("compute").As<Napi::Object>();
                        entryPoint = GetString(compute, "entryPoint", entryPoint);
                        if (compute.Has("module") && compute.Get("module").IsObject())
                        {
                            shaderCode = GetString(compute.Get("module").As<Napi::Object>(), "__wgslCode", "");
                        }
                    }
                }

                return CreateGpuComputePipeline(info.Env(), std::move(shaderCode), std::move(entryPoint));
            }));

            device.Set("createComputePipelineAsync", Napi::Function::New(env, [](const Napi::CallbackInfo& info) -> Napi::Value {
                std::string shaderCode{};
                std::string entryPoint{"main"};

                if (info.Length() > 0 && info[0].IsObject())
                {
                    const auto descriptor = info[0].As<Napi::Object>();
                    if (descriptor.Has("compute") && descriptor.Get("compute").IsObject())
                    {
                        const auto compute = descriptor.Get("compute").As<Napi::Object>();
                        entryPoint = GetString(compute, "entryPoint", entryPoint);
                        if (compute.Has("module") && compute.Get("module").IsObject())
                        {
                            shaderCode = GetString(compute.Get("module").As<Napi::Object>(), "__wgslCode", "");
                        }
                    }
                }

                return ResolvePromiseAsync(info.Env(), [shaderCode = std::move(shaderCode), entryPoint = std::move(entryPoint)](Napi::Env callbackEnv) mutable -> Napi::Value {
                    if (shaderCode.empty())
                    {
                        throw std::runtime_error{"createComputePipelineAsync requires a WGSL shader module."};
                    }
                    return CreateGpuComputePipeline(callbackEnv, std::move(shaderCode), std::move(entryPoint));
                }, "GPUDevice.createComputePipelineAsync");
            }));

            device.Set("createQuerySet", Napi::Function::New(env, [](const Napi::CallbackInfo& info) -> Napi::Value {
                auto querySet = Napi::Object::New(info.Env());
                querySet.Set("destroy", Napi::Function::New(info.Env(), [](const Napi::CallbackInfo& nestedInfo) {
                    (void)nestedInfo;
                }));
                return querySet;
            }));

            return device;
        }

        Napi::Object CreateGpuCanvasContext(Napi::Env env)
        {
            auto context = Napi::Object::New(env);
            auto state = std::make_shared<CanvasContextState>();

            context.Set("configure", Napi::Function::New(env, [state](const Napi::CallbackInfo& info) {
                if (state->Destroyed)
                {
                    return;
                }

                if (info.Length() > 0 && info[0].IsObject())
                {
                    auto descriptor = info[0].As<Napi::Object>();
                    state->Format = GetString(descriptor, "format", state->Format);
                    state->Usage = GetUint32(descriptor, "usage", state->Usage);
                    if (descriptor.Has("size"))
                    {
                        auto sizeValue = descriptor.Get("size");
                        if (sizeValue.IsObject())
                        {
                            auto sizeObject = sizeValue.As<Napi::Object>();
                            state->Width = GetUint32(sizeObject, "width", state->Width);
                            state->Height = GetUint32(sizeObject, "height", state->Height);
                        }
                    }
                }

                state->Configured = true;
                state->CachedTexture.Reset();
            }));

            context.Set("unconfigure", Napi::Function::New(env, [state](const Napi::CallbackInfo& info) {
                (void)info;
                if (state->Destroyed)
                {
                    return;
                }

                state->Configured = false;
                state->CachedTexture.Reset();
            }));

            context.Set("getCurrentTexture", Napi::Function::New(env, [state](const Napi::CallbackInfo& info) -> Napi::Value {
                if (state->Destroyed)
                {
                    return info.Env().Undefined();
                }

                if (!state->CachedTexture.IsEmpty())
                {
                    return state->CachedTexture.Value();
                }

                TextureDescriptorData descriptor{};
                descriptor.Format = state->Format;
                descriptor.Width = state->Width;
                descriptor.Height = state->Height;
                descriptor.Usage = state->Usage;
                descriptor.Label = state->Configured ? "swapchain.current" : "swapchain.unconfigured";

                auto texture = CreateGpuTexture(info, descriptor);
                state->CachedTexture = Napi::Persistent(texture);
                return texture;
            }));

            context.Set("label", Napi::String::New(env, ""));
            context.Set("canvas", env.Undefined());
            context.Set("destroy", Napi::Function::New(env, [state](const Napi::CallbackInfo& info) {
                (void)info;
                if (state->Destroyed)
                {
                    return;
                }

                state->Destroyed = true;
                state->Configured = false;
                state->CachedTexture.Reset();
                state->Format = "bgra8unorm";
                state->Width = 1;
                state->Height = 1;
                state->Usage = 16;
            }));

            return context;
        }

        Napi::Object CreateGpuAdapter(Napi::Env env)
        {
            auto adapter = Napi::Object::New(env);

            adapter.Set("features", CreateSet(env));
            adapter.Set("limits", CreateLimits(env));
            adapter.Set("isFallbackAdapter", Napi::Boolean::New(env, false));

            auto info = Napi::Object::New(env);
            info.Set("vendor", Napi::String::New(env, "BabylonNative"));
            info.Set("architecture", Napi::String::New(env, "wgpu"));
            info.Set("description", Napi::String::New(env, "BabylonNative WGPU adapter"));
            info.Set("device", Napi::String::New(env, "0"));
            adapter.Set("info", info);

            adapter.Set("requestAdapterInfo", Napi::Function::New(env, [](const Napi::CallbackInfo& callbackInfo) -> Napi::Value {
                return ResolvePromiseAsync(callbackInfo.Env(), [](Napi::Env callbackEnv) {
                    auto adapterInfo = Napi::Object::New(callbackEnv);
                    adapterInfo.Set("vendor", Napi::String::New(callbackEnv, "BabylonNative"));
                    adapterInfo.Set("architecture", Napi::String::New(callbackEnv, "wgpu"));
                    adapterInfo.Set("description", Napi::String::New(callbackEnv, "BabylonNative WGPU adapter"));
                    adapterInfo.Set("device", Napi::String::New(callbackEnv, "0"));
                    return adapterInfo;
                }, "GPUAdapter.requestAdapterInfo");
            }));

            adapter.Set("requestDevice", Napi::Function::New(env, [](const Napi::CallbackInfo& info) -> Napi::Value {
                // Keep JS-observable async timing semantics without assuming
                // wgpu-native future IDs are available (NULL_FUTURE paths).
                return ResolvePromiseAsync(info.Env(), [](Napi::Env callbackEnv) {
                    return CreateGpuDevice(callbackEnv);
                }, "GPUAdapter.requestDevice");
            }));

            return adapter;
        }

        Napi::Value ImportCanvasTextureFromNative(const Napi::CallbackInfo& info)
        {
            if (info.Length() == 0)
            {
                return Napi::Boolean::New(info.Env(), false);
            }

            auto payload = ExtractCanvasTexturePayload(info[0]);
            if (!payload.has_value())
            {
                return Napi::Boolean::New(info.Env(), false);
            }

            return Napi::Boolean::New(info.Env(), ImportCanvasTexturePayload(*payload));
        }

        Napi::Object CreateGpu(Napi::Env env, bool developerFeaturesEnabled, bool unsafeWebGpuEnabled)
        {
            auto gpu = Napi::Object::New(env);

            gpu.Set("wgslLanguageFeatures", CreateSet(env));
            gpu.Set("requestAdapter", Napi::Function::New(env, [](const Napi::CallbackInfo& info) -> Napi::Value {
                // Keep JS-observable async timing semantics without assuming
                // wgpu-native future IDs are available (NULL_FUTURE paths).
                return ResolvePromiseAsync(info.Env(), [](Napi::Env callbackEnv) {
                    return CreateGpuAdapter(callbackEnv);
                }, "GPU.requestAdapter");
            }));

            gpu.Set("getPreferredCanvasFormat", Napi::Function::New(env, [](const Napi::CallbackInfo& info) -> Napi::Value {
                return Napi::String::New(info.Env(), "bgra8unorm");
            }));

            // Non-standard helper for native-hosted canvases until HTMLCanvasElement
            // integration is implemented for WGPU mode.
            gpu.Set("_createCanvasContext", Napi::Function::New(env, [](const Napi::CallbackInfo& info) -> Napi::Value {
                return CreateGpuCanvasContext(info.Env());
            }));

            if (developerFeaturesEnabled)
            {
                // Non-standard helper for native validation to execute a WGSL compute shader
                // through the native Rust wgpu backend.
                gpu.Set("_dispatchCompute", Napi::Function::New(env, [](const Napi::CallbackInfo& info) -> Napi::Value {
                    if (info.Length() == 0 || !info[0].IsString())
                    {
                        return Napi::Boolean::New(info.Env(), false);
                    }

                    const auto shaderCode = info[0].As<Napi::String>().Utf8Value();
                    const auto entryPoint = info.Length() > 1 && info[1].IsString() ? info[1].As<Napi::String>().Utf8Value() : std::string{"main"};
                    const auto x = info.Length() > 2 ? ToUint32(info[2], 1) : 1;
                    const auto y = info.Length() > 3 ? ToUint32(info[3], 1) : 1;
                    const auto z = info.Length() > 4 ? ToUint32(info[4], 1) : 1;

                    const bool ok = babylon_wgpu_dispatch_compute_global(shaderCode.c_str(), entryPoint.c_str(), x, y, z);
                    return Napi::Boolean::New(info.Env(), ok);
                }));

                gpu.Set("_markWebGpuDrawRequested", Napi::Function::New(env, [](const Napi::CallbackInfo& info) -> Napi::Value {
                    (void)info;
                    babylon_wgpu_mark_webgpu_draw_requested();
                    return info.Env().Undefined();
                }));

                gpu.Set("_isDrawPathActive", Napi::Function::New(env, [](const Napi::CallbackInfo& info) -> Napi::Value {
                    (void)info;
                    return Napi::Boolean::New(info.Env(), babylon_wgpu_is_webgpu_draw_enabled());
                }));

                auto backendStats = Napi::Function::New(env, [](const Napi::CallbackInfo& info) -> Napi::Value {
                    auto env = info.Env();
                    auto stats = Napi::Object::New(env);
                    stats.Set("renderPipelineCreateCount", Napi::Number::From(env, static_cast<double>(g_renderPipelineCreateCount.load(std::memory_order_relaxed))));
                    stats.Set("commandEncoderCreateCount", Napi::Number::From(env, static_cast<double>(g_commandEncoderCreateCount.load(std::memory_order_relaxed))));
                    stats.Set("renderPassBeginCount", Napi::Number::From(env, static_cast<double>(g_renderPassBeginCount.load(std::memory_order_relaxed))));
                    stats.Set("queueSubmitCount", Napi::Number::From(env, static_cast<double>(g_queueSubmitCount.load(std::memory_order_relaxed))));
                    stats.Set("drawCallCount", Napi::Number::From(env, static_cast<double>(g_drawCallCount.load(std::memory_order_relaxed))));
                    stats.Set("textureCreateCount", Napi::Number::From(env, static_cast<double>(g_textureCreateCount.load(std::memory_order_relaxed))));
                    stats.Set("textureViewCreateCount", Napi::Number::From(env, static_cast<double>(g_textureViewCreateCount.load(std::memory_order_relaxed))));
                    stats.Set("bindGroupCreateCount", Napi::Number::From(env, static_cast<double>(g_bindGroupCreateCount.load(std::memory_order_relaxed))));
                    stats.Set("bufferCreateCount", Napi::Number::From(env, static_cast<double>(g_bufferCreateCount.load(std::memory_order_relaxed))));
                    stats.Set("bufferRequestedBytes", Napi::Number::From(env, static_cast<double>(g_bufferRequestedBytes.load(std::memory_order_relaxed))));
                    stats.Set("drawPathActive", Napi::Boolean::New(env, babylon_wgpu_is_webgpu_draw_enabled()));
                    stats.Set("nativeRenderFrameCount", Napi::Number::From(env, static_cast<double>(babylon_wgpu_get_render_frame_count())));
                    stats.Set("canvasTextureHash", Napi::Number::From(env, static_cast<double>(babylon_wgpu_get_canvas_texture_hash())));
                    stats.Set("canvasTextureWidth", Napi::Number::From(env, static_cast<double>(babylon_wgpu_get_canvas_texture_width())));
                    stats.Set("canvasTextureHeight", Napi::Number::From(env, static_cast<double>(babylon_wgpu_get_canvas_texture_height())));
                    // Legacy stat keys kept for compatibility with older scripts.
                    stats.Set("debugTextureHash", Napi::Number::From(env, static_cast<double>(babylon_wgpu_get_canvas_texture_hash())));
                    stats.Set("debugTextureWidth", Napi::Number::From(env, static_cast<double>(babylon_wgpu_get_canvas_texture_width())));
                    stats.Set("debugTextureHeight", Napi::Number::From(env, static_cast<double>(babylon_wgpu_get_canvas_texture_height())));
                    stats.Set("estimatedGpuMemoryBytes", Napi::Number::From(env, static_cast<double>(babylon_wgpu_get_estimated_gpu_memory_bytes())));
                    stats.Set("canvasTextureImportSkipCount", Napi::Number::From(env, static_cast<double>(babylon_wgpu_get_canvas_texture_import_skip_count())));
                    stats.Set("debugTextureImportSkipCount", Napi::Number::From(env, static_cast<double>(babylon_wgpu_get_canvas_texture_import_skip_count())));
                    stats.Set("backendMode", Napi::String::New(env, kBackendMode));
                    stats.Set("developerFeaturesMode", Napi::String::New(env, kWebGpuDeveloperFeaturesMode));
                    return stats;
                });
                gpu.Set("_backendStats", backendStats);
                // Back-compat alias kept for existing tests/scripts.
                gpu.Set("_debugStats", backendStats);
            }

            if (unsafeWebGpuEnabled)
            {
                // Non-standard helper used to import a CanvasWgpu native interop
                // handle into the GraphicsWgpu-presented texture path.
                gpu.Set("_importCanvasTextureFromNative", Napi::Function::New(env, &ImportCanvasTextureFromNative));
                // Back-compat alias kept for existing Playground/test scripts.
                gpu.Set("_setDebugTextureFromNative", Napi::Function::New(env, &ImportCanvasTextureFromNative));
                gpu.Set("_unsafeMode", Napi::String::New(env, kUnsafeWebGpuMode));
            }

#ifdef BABYLON_NATIVE_WEBGPU_TEST_HOOKS
            // Non-standard helpers used only by native unit tests to validate
            // async std::future -> JS Promise semantics and rejection propagation.
            gpu.Set("_testResetDebugStats", Napi::Function::New(env, [](const Napi::CallbackInfo& info) -> Napi::Value {
                (void)info;
                g_sawWebGpuDrawCall.store(false, std::memory_order_release);
                g_renderPipelineCreateCount.store(0, std::memory_order_relaxed);
                g_commandEncoderCreateCount.store(0, std::memory_order_relaxed);
                g_renderPassBeginCount.store(0, std::memory_order_relaxed);
                g_queueSubmitCount.store(0, std::memory_order_relaxed);
                g_drawCallCount.store(0, std::memory_order_relaxed);
                g_textureCreateCount.store(0, std::memory_order_relaxed);
                g_textureViewCreateCount.store(0, std::memory_order_relaxed);
                g_bindGroupCreateCount.store(0, std::memory_order_relaxed);
                g_bufferCreateCount.store(0, std::memory_order_relaxed);
                g_bufferRequestedBytes.store(0, std::memory_order_relaxed);
                babylon_wgpu_reset_webgpu_draw_requested();
                return info.Env().Undefined();
            }));

            gpu.Set("_testAsyncResolve", Napi::Function::New(env, [](const Napi::CallbackInfo& info) -> Napi::Value {
                const auto value = info.Length() > 0 && info[0].IsString() ? info[0].As<Napi::String>().Utf8Value() : std::string{"ok"};
                auto future = std::async(std::launch::async, []() -> std::string {
                    return {};
                });

                return ResolvePromiseFromFuture(info.Env(), std::move(future), [value = std::move(value)](Napi::Env callbackEnv) {
                    return Napi::String::New(callbackEnv, value);
                }, "NativeWebGPU._testAsyncResolve");
            }));

            gpu.Set("_testAsyncReject", Napi::Function::New(env, [](const Napi::CallbackInfo& info) -> Napi::Value {
                const auto message = info.Length() > 0 && info[0].IsString() ? info[0].As<Napi::String>().Utf8Value() : std::string{"NativeWebGPU async rejection"};
                auto future = std::async(std::launch::async, [message]() -> std::string {
                    throw std::runtime_error{message};
                });

                return ResolvePromiseFromFuture(info.Env(), std::move(future), [](Napi::Env callbackEnv) {
                    return callbackEnv.Undefined();
                }, "NativeWebGPU._testAsyncReject");
            }));

            gpu.Set("_testAsyncResolveFactoryThrows", Napi::Function::New(env, [](const Napi::CallbackInfo& info) -> Napi::Value {
                const auto message = info.Length() > 0 && info[0].IsString() ? info[0].As<Napi::String>().Utf8Value() : std::string{"NativeWebGPU resolve factory rejection"};
                auto future = std::async(std::launch::async, []() -> std::string {
                    return {};
                });

                return ResolvePromiseFromFuture(info.Env(), std::move(future), [message](Napi::Env callbackEnv) -> Napi::Value {
                    (void)callbackEnv;
                    throw std::runtime_error{message};
                }, "NativeWebGPU._testAsyncResolveFactoryThrows");
            }));
#endif

            return gpu;
        }
    }

    // Initialization contract: this function must be called from an
    // AppRuntime::Dispatch callback BEFORE any user JavaScript executes.
    // The AppRuntime WorkQueue is FIFO, and ScriptLoader dispatches through
    // the same queue, so navigator.gpu is guaranteed to be synchronously
    // available by the time any script runs. Embedders do NOT need to poll
    // for navigator.gpu or use a readiness promise — just call Initialize()
    // in the Dispatch callback and load scripts via ScriptLoader afterward.
    //
    // This matches the W3C WebGPU spec where navigator.gpu is a synchronous
    // [SameObject] attribute, always present when WebGPU is enabled.
    void Initialize(Napi::Env env)
    {
        Napi::HandleScope scope{env};

        auto global = env.Global();
        Napi::Object navigator;

        if (global.Has(JS_NAVIGATOR_NAME) && global.Get(JS_NAVIGATOR_NAME).IsObject())
        {
            navigator = global.Get(JS_NAVIGATOR_NAME).As<Napi::Object>();
        }
        else
        {
            navigator = Napi::Object::New(env);
            global.Set(JS_NAVIGATOR_NAME, navigator);
        }

        if (navigator.Has(JS_GPU_NAME))
        {
            auto existingGpu = navigator.Get(JS_GPU_NAME);
            if (existingGpu.IsObject())
            {
                // navigator.gpu already exists (e.g. re-initialization after
                // Android surface recreation). Per W3C spec, navigator.gpu is
                // a [SameObject] attribute — nothing else to do.
                return;
            }
        }

        const bool developerFeaturesEnabled = IsWebGpuDeveloperFeaturesEnabled(env);
        const bool unsafeWebGpuEnabled = developerFeaturesEnabled || IsUnsafeWebGpuEnabled(env);
        auto gpu = CreateGpu(env, developerFeaturesEnabled, unsafeWebGpuEnabled);
        navigator.Set(JS_GPU_NAME, gpu);
    }
}
