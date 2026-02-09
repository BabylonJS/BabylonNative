#include <Babylon/Plugins/NativeWebGPU.h>
#include <Babylon/JsRuntime.h>

#include <napi/napi.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstdint>
#ifdef BABYLON_NATIVE_WEBGPU_TEST_HOOKS
#include <future>
#endif
#include <limits>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

namespace Babylon::Plugins::NativeWebGPU
{
    namespace
    {
        extern "C"
        {
            bool babylon_wgpu_dispatch_compute_global(const char* shaderSource, const char* entryPoint, uint32_t x, uint32_t y, uint32_t z);
            void babylon_wgpu_mark_webgpu_draw_requested();
            bool babylon_wgpu_is_webgpu_draw_enabled();
            uint64_t babylon_wgpu_get_render_frame_count();
            uint64_t babylon_wgpu_get_debug_texture_hash();
            uint32_t babylon_wgpu_get_debug_texture_width();
            uint32_t babylon_wgpu_get_debug_texture_height();
            void babylon_wgpu_reset_webgpu_draw_requested();
            bool babylon_wgpu_set_debug_texture_from_native(const void* nativeTexture, uint32_t width, uint32_t height);
        }

        std::atomic_bool g_sawWebGpuDrawCall{false};
        std::atomic_uint64_t g_renderPipelineCreateCount{0};
        std::atomic_uint64_t g_commandEncoderCreateCount{0};
        std::atomic_uint64_t g_renderPassBeginCount{0};
        std::atomic_uint64_t g_queueSubmitCount{0};
        std::atomic_uint64_t g_drawCallCount{0};

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

        std::string GetString(const Napi::Object& object, const char* key, std::string fallback)
        {
            if (!object.Has(key))
            {
                return fallback;
            }

            const auto value = object.Get(key);
            if (!value.IsString())
            {
                return fallback;
            }

            return value.As<Napi::String>().Utf8Value();
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

                auto view = Napi::Object::New(viewInfo.Env());
                auto viewFormat = descriptor.Format;
                auto viewDimension = descriptor.Dimension;
                auto mipLevelCount = descriptor.MipLevelCount;
                auto arrayLayerCount = descriptor.DepthOrArrayLayers;

                if (hasDescriptor)
                {
                    const auto viewDescriptor = viewInfo[0].As<Napi::Object>();
                    viewFormat = GetString(viewDescriptor, "format", viewFormat);
                    viewDimension = GetString(viewDescriptor, "dimension", viewDimension);
                    mipLevelCount = GetUint32(viewDescriptor, "mipLevelCount", mipLevelCount);
                    arrayLayerCount = GetUint32(viewDescriptor, "arrayLayerCount", arrayLayerCount);
                    view.Set("label", Napi::String::New(viewInfo.Env(), GetString(viewDescriptor, "label", "")));
                }
                else
                {
                    view.Set("label", Napi::String::New(viewInfo.Env(), ""));
                }

                view.Set("format", Napi::String::New(viewInfo.Env(), viewFormat));
                view.Set("dimension", Napi::String::New(viewInfo.Env(), viewDimension));
                view.Set("mipLevelCount", Napi::Number::From(viewInfo.Env(), mipLevelCount));
                view.Set("arrayLayerCount", Napi::Number::From(viewInfo.Env(), arrayLayerCount));
                view.Set("texture", textureObject);

                if (!hasDescriptor)
                {
                    textureObject.Set("__defaultView", view);
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

                return Napi::ArrayBuffer::New(info.Env(), byteLength);
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
            queue.Set("copyExternalImageToTexture", noOp);
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
            device.Set("lost", CreateNeverPromise(env));

            device.Set("addEventListener", noOp);
            device.Set("removeEventListener", noOp);
            device.Set("destroy", noOp);
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
                return ResolvePromiseAsync(info.Env(), [](Napi::Env callbackEnv) {
                    return CreateGpuDevice(callbackEnv);
                }, "GPUAdapter.requestDevice");
            }));

            return adapter;
        }

        Napi::Object CreateGpu(Napi::Env env)
        {
            auto gpu = Napi::Object::New(env);

            gpu.Set("wgslLanguageFeatures", CreateSet(env));
            gpu.Set("requestAdapter", Napi::Function::New(env, [](const Napi::CallbackInfo& info) -> Napi::Value {
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

            // Non-standard helper used by Playground validation to import a
            // CanvasWgpu native interop handle into the debug cube renderer path.
            gpu.Set("_setDebugTextureFromNative", Napi::Function::New(env, [](const Napi::CallbackInfo& info) -> Napi::Value {
                if (info.Length() == 0 || !info[0].IsObject())
                {
                    return Napi::Boolean::New(info.Env(), false);
                }

                const auto payload = info[0].As<Napi::Object>();
                if (!payload.Has("nativeTexture"))
                {
                    return Napi::Boolean::New(info.Env(), false);
                }

                const auto nativeTextureValue = payload.Get("nativeTexture");
                if (!nativeTextureValue.IsExternal())
                {
                    return Napi::Boolean::New(info.Env(), false);
                }

                const void* nativeTexture = nativeTextureValue.As<Napi::External<void>>().Data();
                if (nativeTexture == nullptr)
                {
                    return Napi::Boolean::New(info.Env(), false);
                }

                const uint32_t width = payload.Has("width") ? ToUint32(payload.Get("width"), 1) : 1;
                const uint32_t height = payload.Has("height") ? ToUint32(payload.Get("height"), 1) : 1;
                const bool ok = babylon_wgpu_set_debug_texture_from_native(nativeTexture, width, height);
                return Napi::Boolean::New(info.Env(), ok);
            }));

            gpu.Set("_isDrawPathActive", Napi::Function::New(env, [](const Napi::CallbackInfo& info) -> Napi::Value {
                (void)info;
                return Napi::Boolean::New(info.Env(), babylon_wgpu_is_webgpu_draw_enabled());
            }));

            gpu.Set("_debugStats", Napi::Function::New(env, [](const Napi::CallbackInfo& info) -> Napi::Value {
                auto env = info.Env();
                auto stats = Napi::Object::New(env);
                stats.Set("renderPipelineCreateCount", Napi::Number::From(env, static_cast<double>(g_renderPipelineCreateCount.load(std::memory_order_relaxed))));
                stats.Set("commandEncoderCreateCount", Napi::Number::From(env, static_cast<double>(g_commandEncoderCreateCount.load(std::memory_order_relaxed))));
                stats.Set("renderPassBeginCount", Napi::Number::From(env, static_cast<double>(g_renderPassBeginCount.load(std::memory_order_relaxed))));
                stats.Set("queueSubmitCount", Napi::Number::From(env, static_cast<double>(g_queueSubmitCount.load(std::memory_order_relaxed))));
                stats.Set("drawCallCount", Napi::Number::From(env, static_cast<double>(g_drawCallCount.load(std::memory_order_relaxed))));
                stats.Set("drawPathActive", Napi::Boolean::New(env, babylon_wgpu_is_webgpu_draw_enabled()));
                stats.Set("nativeRenderFrameCount", Napi::Number::From(env, static_cast<double>(babylon_wgpu_get_render_frame_count())));
                stats.Set("debugTextureHash", Napi::Number::From(env, static_cast<double>(babylon_wgpu_get_debug_texture_hash())));
                stats.Set("debugTextureWidth", Napi::Number::From(env, static_cast<double>(babylon_wgpu_get_debug_texture_width())));
                stats.Set("debugTextureHeight", Napi::Number::From(env, static_cast<double>(babylon_wgpu_get_debug_texture_height())));
                return stats;
            }));

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
                return;
            }
        }

        navigator.Set(JS_GPU_NAME, CreateGpu(env));
    }
}
