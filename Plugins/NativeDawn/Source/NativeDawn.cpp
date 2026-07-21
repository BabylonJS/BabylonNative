#include <Babylon/Plugins/NativeDawn.h>

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

#include <webgpu/webgpu_cpp.h>

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
#include <filesystem>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace Babylon::Plugins::NativeDawn
{
    // Defined in ImageDecode.cpp (bimg): encoded image bytes -> RGBA8.
    bool DecodeRGBA(const uint8_t* data, size_t size, std::vector<uint8_t>& out, int& width, int& height);

    namespace
    {
        // Global Dawn state for the experiment. Single window / single device.
        struct State
        {
            wgpu::Instance instance;
            wgpu::Adapter adapter;
            wgpu::Device device;
            wgpu::Queue queue;
            wgpu::Surface surface;
            wgpu::TextureFormat surfaceFormat = wgpu::TextureFormat::BGRA8Unorm;
            wgpu::Texture currentSurfaceTexture;
            void* hwnd = nullptr;
            uint32_t width = 0;
            uint32_t height = 0;
            bool ready = false;
        };

        State g_state;

        void LogDeviceError(const wgpu::Device&, wgpu::ErrorType type, wgpu::StringView message)
        {
            std::fprintf(stderr, "[NativeDawn] device error (%d): %.*s\n",
                static_cast<int>(type), static_cast<int>(message.length), message.data);
        }

        bool CreateDeviceAndSurface(void* window, uint32_t width, uint32_t height)
        {
            g_state.hwnd = window;
            g_state.width = width;
            g_state.height = height;

            // Instance with synchronous WaitAny support.
            static const auto kTimedWaitAny = wgpu::InstanceFeatureName::TimedWaitAny;
            wgpu::InstanceDescriptor instDesc{};
            instDesc.requiredFeatureCount = 1;
            instDesc.requiredFeatures = &kTimedWaitAny;
            g_state.instance = wgpu::CreateInstance(&instDesc);
            if (!g_state.instance)
            {
                std::fprintf(stderr, "[NativeDawn] CreateInstance failed\n");
                return false;
            }

            // Adapter (high performance).
            wgpu::RequestAdapterOptions adapterOpts{};
            adapterOpts.powerPreference = wgpu::PowerPreference::HighPerformance;
            wgpu::Future af = g_state.instance.RequestAdapter(&adapterOpts, wgpu::CallbackMode::WaitAnyOnly,
                [](wgpu::RequestAdapterStatus status, wgpu::Adapter adapter, wgpu::StringView message, wgpu::Adapter* out) {
                    if (status == wgpu::RequestAdapterStatus::Success)
                    {
                        *out = std::move(adapter);
                    }
                    else
                    {
                        std::fprintf(stderr, "[NativeDawn] RequestAdapter failed: %.*s\n",
                            static_cast<int>(message.length), message.data);
                    }
                },
                &g_state.adapter);
            g_state.instance.WaitAny(af, UINT64_MAX);
            if (!g_state.adapter)
            {
                return false;
            }

            // Device.
            wgpu::DeviceDescriptor devDesc{};
            devDesc.SetUncapturedErrorCallback(&LogDeviceError);
            wgpu::Future df = g_state.adapter.RequestDevice(&devDesc, wgpu::CallbackMode::WaitAnyOnly,
                [](wgpu::RequestDeviceStatus status, wgpu::Device device, wgpu::StringView message, wgpu::Device* out) {
                    if (status == wgpu::RequestDeviceStatus::Success)
                    {
                        *out = std::move(device);
                    }
                    else
                    {
                        std::fprintf(stderr, "[NativeDawn] RequestDevice failed: %.*s\n",
                            static_cast<int>(message.length), message.data);
                    }
                },
                &g_state.device);
            g_state.instance.WaitAny(df, UINT64_MAX);
            if (!g_state.device)
            {
                return false;
            }
            g_state.queue = g_state.device.GetQueue();

            // Surface from the native window (Win32 HWND).
#if defined(_WIN32)
            wgpu::SurfaceSourceWindowsHWND chained{};
            chained.hwnd = window;
            chained.hinstance = ::GetModuleHandle(nullptr);
            wgpu::SurfaceDescriptor surfDesc{};
            surfDesc.nextInChain = &chained;
            g_state.surface = g_state.instance.CreateSurface(&surfDesc);
#endif
            if (!g_state.surface)
            {
                std::fprintf(stderr, "[NativeDawn] CreateSurface failed\n");
                return false;
            }

            // Pick the preferred format and configure.
            wgpu::SurfaceCapabilities caps{};
            g_state.surface.GetCapabilities(g_state.adapter, &caps);
            if (caps.formatCount > 0)
            {
                g_state.surfaceFormat = caps.formats[0];
            }

            wgpu::SurfaceConfiguration cfg{};
            cfg.device = g_state.device;
            cfg.format = g_state.surfaceFormat;
            cfg.usage = wgpu::TextureUsage::RenderAttachment;
            cfg.width = width;
            cfg.height = height;
            cfg.presentMode = wgpu::PresentMode::Fifo;
            g_state.surface.Configure(&cfg);

            g_state.ready = true;
            std::fprintf(stderr, "[NativeDawn] Dawn device + surface ready (%ux%u, format=%d)\n",
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

            wgpu::SurfaceTexture st{};
            g_state.surface.GetCurrentTexture(&st);
            if (!st.texture)
            {
                std::fprintf(stderr, "[NativeDawn] GetCurrentTexture: null\n");
                return;
            }

            wgpu::TextureView view = st.texture.CreateView();

            wgpu::RenderPassColorAttachment color{};
            color.view = view;
            color.loadOp = wgpu::LoadOp::Clear;
            color.storeOp = wgpu::StoreOp::Store;
            color.clearValue = {r, g, b, 1.0f};

            wgpu::RenderPassDescriptor passDesc{};
            passDesc.colorAttachmentCount = 1;
            passDesc.colorAttachments = &color;

            wgpu::CommandEncoder encoder = g_state.device.CreateCommandEncoder();
            wgpu::RenderPassEncoder pass = encoder.BeginRenderPass(&passDesc);
            pass.End();
            wgpu::CommandBuffer commands = encoder.Finish();
            g_state.queue.Submit(1, &commands);
            g_state.surface.Present();
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

        std::string SvToStr(const wgpu::StringView& sv)
        {
            if (sv.data == nullptr) return {};
            if (sv.length == static_cast<size_t>(-1)) return std::string(sv.data);
            return std::string(sv.data, sv.length);
        }

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

        template <typename T>
        Napi::External<T> MakeExt(Napi::Env env, const T& h)
        {
            return Napi::External<T>::New(env, new T(h), [](Napi::Env, T* p) { delete p; });
        }

        template <typename T>
        void SetHandle(Napi::Object o, const T& h)
        {
            o.Set("_h", MakeExt<T>(o.Env(), h));
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
        wgpu::Extent3D ParseExtent3D(Napi::Value v)
        {
            wgpu::Extent3D e{};
            e.width = 1; e.height = 1; e.depthOrArrayLayers = 1;
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
        wgpu::Origin3D ParseOrigin3D(Napi::Value v)
        {
            wgpu::Origin3D o3{};
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
        wgpu::Color ParseColor(Napi::Value v)
        {
            wgpu::Color c{};
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

        // ---- enum string -> wgpu mappings ------------------------------------
        wgpu::TextureFormat textureFormat(const std::string& s)
        {
            using F = wgpu::TextureFormat;
            if (s == "r8unorm") return F::R8Unorm;
            if (s == "r8snorm") return F::R8Snorm;
            if (s == "r8uint") return F::R8Uint;
            if (s == "r8sint") return F::R8Sint;
            if (s == "r16uint") return F::R16Uint;
            if (s == "r16sint") return F::R16Sint;
            if (s == "r16float") return F::R16Float;
            if (s == "rg8unorm") return F::RG8Unorm;
            if (s == "rg8snorm") return F::RG8Snorm;
            if (s == "rg8uint") return F::RG8Uint;
            if (s == "rg8sint") return F::RG8Sint;
            if (s == "r32uint") return F::R32Uint;
            if (s == "r32sint") return F::R32Sint;
            if (s == "r32float") return F::R32Float;
            if (s == "rg16uint") return F::RG16Uint;
            if (s == "rg16sint") return F::RG16Sint;
            if (s == "rg16float") return F::RG16Float;
            if (s == "rgba8unorm") return F::RGBA8Unorm;
            if (s == "rgba8unorm-srgb") return F::RGBA8UnormSrgb;
            if (s == "rgba8snorm") return F::RGBA8Snorm;
            if (s == "rgba8uint") return F::RGBA8Uint;
            if (s == "rgba8sint") return F::RGBA8Sint;
            if (s == "bgra8unorm") return F::BGRA8Unorm;
            if (s == "bgra8unorm-srgb") return F::BGRA8UnormSrgb;
            if (s == "rgb9e5ufloat") return F::RGB9E5Ufloat;
            if (s == "rgb10a2uint") return F::RGB10A2Uint;
            if (s == "rgb10a2unorm") return F::RGB10A2Unorm;
            if (s == "rg11b10ufloat") return F::RG11B10Ufloat;
            if (s == "rg32uint") return F::RG32Uint;
            if (s == "rg32sint") return F::RG32Sint;
            if (s == "rg32float") return F::RG32Float;
            if (s == "rgba16uint") return F::RGBA16Uint;
            if (s == "rgba16sint") return F::RGBA16Sint;
            if (s == "rgba16float") return F::RGBA16Float;
            if (s == "rgba32uint") return F::RGBA32Uint;
            if (s == "rgba32sint") return F::RGBA32Sint;
            if (s == "rgba32float") return F::RGBA32Float;
            if (s == "stencil8") return F::Stencil8;
            if (s == "depth16unorm") return F::Depth16Unorm;
            if (s == "depth24plus") return F::Depth24Plus;
            if (s == "depth24plus-stencil8") return F::Depth24PlusStencil8;
            if (s == "depth32float") return F::Depth32Float;
            if (s == "depth32float-stencil8") return F::Depth32FloatStencil8;
            if (s == "bc1-rgba-unorm") return F::BC1RGBAUnorm;
            if (s == "bc1-rgba-unorm-srgb") return F::BC1RGBAUnormSrgb;
            if (s == "bc2-rgba-unorm") return F::BC2RGBAUnorm;
            if (s == "bc2-rgba-unorm-srgb") return F::BC2RGBAUnormSrgb;
            if (s == "bc3-rgba-unorm") return F::BC3RGBAUnorm;
            if (s == "bc3-rgba-unorm-srgb") return F::BC3RGBAUnormSrgb;
            if (s == "bc4-r-unorm") return F::BC4RUnorm;
            if (s == "bc4-r-snorm") return F::BC4RSnorm;
            if (s == "bc5-rg-unorm") return F::BC5RGUnorm;
            if (s == "bc5-rg-snorm") return F::BC5RGSnorm;
            if (s == "bc6h-rgb-ufloat") return F::BC6HRGBUfloat;
            if (s == "bc6h-rgb-float") return F::BC6HRGBFloat;
            if (s == "bc7-rgba-unorm") return F::BC7RGBAUnorm;
            if (s == "bc7-rgba-unorm-srgb") return F::BC7RGBAUnormSrgb;
            return F::Undefined;
        }
        const char* textureFormatStr(wgpu::TextureFormat f)
        {
            using F = wgpu::TextureFormat;
            switch (f)
            {
            case F::R8Unorm: return "r8unorm";
            case F::R8Snorm: return "r8snorm";
            case F::R8Uint: return "r8uint";
            case F::R8Sint: return "r8sint";
            case F::R16Uint: return "r16uint";
            case F::R16Sint: return "r16sint";
            case F::R16Float: return "r16float";
            case F::RG8Unorm: return "rg8unorm";
            case F::RG8Snorm: return "rg8snorm";
            case F::RG8Uint: return "rg8uint";
            case F::RG8Sint: return "rg8sint";
            case F::R32Uint: return "r32uint";
            case F::R32Sint: return "r32sint";
            case F::R32Float: return "r32float";
            case F::RG16Uint: return "rg16uint";
            case F::RG16Sint: return "rg16sint";
            case F::RG16Float: return "rg16float";
            case F::RGBA8Unorm: return "rgba8unorm";
            case F::RGBA8UnormSrgb: return "rgba8unorm-srgb";
            case F::RGBA8Snorm: return "rgba8snorm";
            case F::RGBA8Uint: return "rgba8uint";
            case F::RGBA8Sint: return "rgba8sint";
            case F::BGRA8Unorm: return "bgra8unorm";
            case F::BGRA8UnormSrgb: return "bgra8unorm-srgb";
            case F::RGB9E5Ufloat: return "rgb9e5ufloat";
            case F::RGB10A2Uint: return "rgb10a2uint";
            case F::RGB10A2Unorm: return "rgb10a2unorm";
            case F::RG11B10Ufloat: return "rg11b10ufloat";
            case F::RG32Uint: return "rg32uint";
            case F::RG32Sint: return "rg32sint";
            case F::RG32Float: return "rg32float";
            case F::RGBA16Uint: return "rgba16uint";
            case F::RGBA16Sint: return "rgba16sint";
            case F::RGBA16Float: return "rgba16float";
            case F::RGBA32Uint: return "rgba32uint";
            case F::RGBA32Sint: return "rgba32sint";
            case F::RGBA32Float: return "rgba32float";
            case F::Stencil8: return "stencil8";
            case F::Depth16Unorm: return "depth16unorm";
            case F::Depth24Plus: return "depth24plus";
            case F::Depth24PlusStencil8: return "depth24plus-stencil8";
            case F::Depth32Float: return "depth32float";
            case F::Depth32FloatStencil8: return "depth32float-stencil8";
            default: return "bgra8unorm";
            }
        }
        wgpu::VertexFormat vertexFormat(const std::string& s)
        {
            using F = wgpu::VertexFormat;
            if (s == "uint8x2") return F::Uint8x2;
            if (s == "uint8x4") return F::Uint8x4;
            if (s == "sint8x2") return F::Sint8x2;
            if (s == "sint8x4") return F::Sint8x4;
            if (s == "unorm8x2") return F::Unorm8x2;
            if (s == "unorm8x4") return F::Unorm8x4;
            if (s == "snorm8x2") return F::Snorm8x2;
            if (s == "snorm8x4") return F::Snorm8x4;
            if (s == "uint16x2") return F::Uint16x2;
            if (s == "uint16x4") return F::Uint16x4;
            if (s == "sint16x2") return F::Sint16x2;
            if (s == "sint16x4") return F::Sint16x4;
            if (s == "unorm16x2") return F::Unorm16x2;
            if (s == "unorm16x4") return F::Unorm16x4;
            if (s == "snorm16x2") return F::Snorm16x2;
            if (s == "snorm16x4") return F::Snorm16x4;
            if (s == "float16x2") return F::Float16x2;
            if (s == "float16x4") return F::Float16x4;
            if (s == "float32") return F::Float32;
            if (s == "float32x2") return F::Float32x2;
            if (s == "float32x3") return F::Float32x3;
            if (s == "float32x4") return F::Float32x4;
            if (s == "uint32") return F::Uint32;
            if (s == "uint32x2") return F::Uint32x2;
            if (s == "uint32x3") return F::Uint32x3;
            if (s == "uint32x4") return F::Uint32x4;
            if (s == "sint32") return F::Sint32;
            if (s == "sint32x2") return F::Sint32x2;
            if (s == "sint32x3") return F::Sint32x3;
            if (s == "sint32x4") return F::Sint32x4;
            return F::Float32;
        }
        wgpu::IndexFormat indexFormat(const std::string& s)
        {
            if (s == "uint16") return wgpu::IndexFormat::Uint16;
            if (s == "uint32") return wgpu::IndexFormat::Uint32;
            return wgpu::IndexFormat::Undefined;
        }
        wgpu::PrimitiveTopology primitiveTopology(const std::string& s)
        {
            using T = wgpu::PrimitiveTopology;
            if (s == "point-list") return T::PointList;
            if (s == "line-list") return T::LineList;
            if (s == "line-strip") return T::LineStrip;
            if (s == "triangle-list") return T::TriangleList;
            if (s == "triangle-strip") return T::TriangleStrip;
            return T::TriangleList;
        }
        wgpu::CullMode cullMode(const std::string& s)
        {
            if (s == "none") return wgpu::CullMode::None;
            if (s == "front") return wgpu::CullMode::Front;
            if (s == "back") return wgpu::CullMode::Back;
            return wgpu::CullMode::None;
        }
        wgpu::FrontFace frontFace(const std::string& s)
        {
            if (s == "cw") return wgpu::FrontFace::CW;
            if (s == "ccw") return wgpu::FrontFace::CCW;
            return wgpu::FrontFace::CCW;
        }
        wgpu::CompareFunction compareFunction(const std::string& s)
        {
            using C = wgpu::CompareFunction;
            if (s == "never") return C::Never;
            if (s == "less") return C::Less;
            if (s == "equal") return C::Equal;
            if (s == "less-equal") return C::LessEqual;
            if (s == "greater") return C::Greater;
            if (s == "not-equal") return C::NotEqual;
            if (s == "greater-equal") return C::GreaterEqual;
            if (s == "always") return C::Always;
            return C::Undefined;
        }
        wgpu::StencilOperation stencilOperation(const std::string& s)
        {
            using O = wgpu::StencilOperation;
            if (s == "keep") return O::Keep;
            if (s == "zero") return O::Zero;
            if (s == "replace") return O::Replace;
            if (s == "invert") return O::Invert;
            if (s == "increment-clamp") return O::IncrementClamp;
            if (s == "decrement-clamp") return O::DecrementClamp;
            if (s == "increment-wrap") return O::IncrementWrap;
            if (s == "decrement-wrap") return O::DecrementWrap;
            return O::Keep;
        }
        wgpu::BlendFactor blendFactor(const std::string& s)
        {
            using B = wgpu::BlendFactor;
            if (s == "zero") return B::Zero;
            if (s == "one") return B::One;
            if (s == "src") return B::Src;
            if (s == "one-minus-src") return B::OneMinusSrc;
            if (s == "src-alpha") return B::SrcAlpha;
            if (s == "one-minus-src-alpha") return B::OneMinusSrcAlpha;
            if (s == "dst") return B::Dst;
            if (s == "one-minus-dst") return B::OneMinusDst;
            if (s == "dst-alpha") return B::DstAlpha;
            if (s == "one-minus-dst-alpha") return B::OneMinusDstAlpha;
            if (s == "src-alpha-saturated") return B::SrcAlphaSaturated;
            if (s == "constant") return B::Constant;
            if (s == "one-minus-constant") return B::OneMinusConstant;
            return B::One;
        }
        wgpu::BlendOperation blendOperation(const std::string& s)
        {
            using O = wgpu::BlendOperation;
            if (s == "add") return O::Add;
            if (s == "subtract") return O::Subtract;
            if (s == "reverse-subtract") return O::ReverseSubtract;
            if (s == "min") return O::Min;
            if (s == "max") return O::Max;
            return O::Add;
        }
        wgpu::AddressMode addressMode(const std::string& s)
        {
            if (s == "clamp-to-edge") return wgpu::AddressMode::ClampToEdge;
            if (s == "repeat") return wgpu::AddressMode::Repeat;
            if (s == "mirror-repeat") return wgpu::AddressMode::MirrorRepeat;
            return wgpu::AddressMode::ClampToEdge;
        }
        wgpu::FilterMode filterMode(const std::string& s)
        {
            if (s == "nearest") return wgpu::FilterMode::Nearest;
            if (s == "linear") return wgpu::FilterMode::Linear;
            return wgpu::FilterMode::Nearest;
        }
        wgpu::MipmapFilterMode mipmapFilterMode(const std::string& s)
        {
            if (s == "nearest") return wgpu::MipmapFilterMode::Nearest;
            if (s == "linear") return wgpu::MipmapFilterMode::Linear;
            return wgpu::MipmapFilterMode::Nearest;
        }
        wgpu::TextureViewDimension textureViewDimension(const std::string& s)
        {
            using D = wgpu::TextureViewDimension;
            if (s == "1d") return D::e1D;
            if (s == "2d") return D::e2D;
            if (s == "2d-array") return D::e2DArray;
            if (s == "cube") return D::Cube;
            if (s == "cube-array") return D::CubeArray;
            if (s == "3d") return D::e3D;
            return D::Undefined;
        }
        wgpu::TextureDimension textureDimension(const std::string& s)
        {
            using D = wgpu::TextureDimension;
            if (s == "1d") return D::e1D;
            if (s == "2d") return D::e2D;
            if (s == "3d") return D::e3D;
            return D::e2D;
        }
        wgpu::TextureSampleType textureSampleType(const std::string& s)
        {
            using T = wgpu::TextureSampleType;
            if (s == "float") return T::Float;
            if (s == "unfilterable-float") return T::UnfilterableFloat;
            if (s == "depth") return T::Depth;
            if (s == "sint") return T::Sint;
            if (s == "uint") return T::Uint;
            return T::Float;
        }
        wgpu::StorageTextureAccess storageTextureAccess(const std::string& s)
        {
            using A = wgpu::StorageTextureAccess;
            if (s == "write-only") return A::WriteOnly;
            if (s == "read-only") return A::ReadOnly;
            if (s == "read-write") return A::ReadWrite;
            return A::Undefined;
        }
        wgpu::SamplerBindingType samplerBindingType(const std::string& s)
        {
            using S = wgpu::SamplerBindingType;
            if (s == "filtering") return S::Filtering;
            if (s == "non-filtering") return S::NonFiltering;
            if (s == "comparison") return S::Comparison;
            return S::Filtering;
        }
        wgpu::BufferBindingType bufferBindingType(const std::string& s)
        {
            using B = wgpu::BufferBindingType;
            if (s == "uniform") return B::Uniform;
            if (s == "storage") return B::Storage;
            if (s == "read-only-storage") return B::ReadOnlyStorage;
            return B::Uniform;
        }
        wgpu::LoadOp loadOp(const std::string& s)
        {
            if (s == "load") return wgpu::LoadOp::Load;
            if (s == "clear") return wgpu::LoadOp::Clear;
            return wgpu::LoadOp::Load;
        }
        wgpu::StoreOp storeOp(const std::string& s)
        {
            if (s == "store") return wgpu::StoreOp::Store;
            if (s == "discard") return wgpu::StoreOp::Discard;
            return wgpu::StoreOp::Store;
        }
        wgpu::VertexStepMode vertexStepMode(const std::string& s)
        {
            if (s == "vertex") return wgpu::VertexStepMode::Vertex;
            if (s == "instance") return wgpu::VertexStepMode::Instance;
            return wgpu::VertexStepMode::Vertex;
        }
        wgpu::FeatureName featureName(const std::string& s)
        {
            using F = wgpu::FeatureName;
            if (s == "depth-clip-control") return F::DepthClipControl;
            if (s == "depth32float-stencil8") return F::Depth32FloatStencil8;
            if (s == "texture-compression-bc") return F::TextureCompressionBC;
            if (s == "texture-compression-etc2") return F::TextureCompressionETC2;
            if (s == "texture-compression-astc") return F::TextureCompressionASTC;
            if (s == "timestamp-query") return F::TimestampQuery;
            if (s == "indirect-first-instance") return F::IndirectFirstInstance;
            if (s == "shader-f16") return F::ShaderF16;
            if (s == "rg11b10ufloat-renderable") return F::RG11B10UfloatRenderable;
            if (s == "bgra8unorm-storage") return F::BGRA8UnormStorage;
            if (s == "float32-filterable") return F::Float32Filterable;
            if (s == "dual-source-blending") return F::DualSourceBlending;
            return F(0);
        }
        wgpu::QueryType queryType(const std::string& s)
        {
            if (s == "occlusion") return wgpu::QueryType::Occlusion;
            if (s == "timestamp") return wgpu::QueryType::Timestamp;
            return wgpu::QueryType::Occlusion;
        }
        wgpu::PowerPreference powerPreference(const std::string& s)
        {
            if (s == "low-power") return wgpu::PowerPreference::LowPower;
            if (s == "high-performance") return wgpu::PowerPreference::HighPerformance;
            return wgpu::PowerPreference::Undefined;
        }

        // ---- limits ----------------------------------------------------------
        void FillLimits(Napi::Object o, const wgpu::Limits& L)
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

        // ---- forward declarations of object builders -------------------------
        Napi::Object MakeAdapter(Napi::Env env);
        Napi::Object MakeDevice(Napi::Env env);
        Napi::Object MakeQueue(Napi::Env env);
        Napi::Object MakeBuffer(Napi::Env env, wgpu::Buffer h, uint64_t size, uint32_t usage, bool mapped);
        Napi::Object MakeTexture(Napi::Env env, wgpu::Texture h, uint32_t w, uint32_t ht, uint32_t depth,
            uint32_t mip, uint32_t sample, const std::string& fmt, uint32_t usage, const std::string& dim);
        Napi::Object MakeTextureView(Napi::Env env, wgpu::TextureView h);
        Napi::Object MakeSampler(Napi::Env env, wgpu::Sampler h);
        Napi::Object MakeBindGroupLayout(Napi::Env env, wgpu::BindGroupLayout h);
        Napi::Object MakeBindGroup(Napi::Env env, wgpu::BindGroup h);
        Napi::Object MakePipelineLayout(Napi::Env env, wgpu::PipelineLayout h);
        Napi::Object MakeShaderModule(Napi::Env env, wgpu::ShaderModule h);
        Napi::Object MakeRenderPipeline(Napi::Env env, wgpu::RenderPipeline h);
        Napi::Object MakeComputePipeline(Napi::Env env, wgpu::ComputePipeline h);
        Napi::Object MakeCommandEncoder(Napi::Env env, wgpu::CommandEncoder h);
        Napi::Object MakeRenderPassEncoder(Napi::Env env, wgpu::RenderPassEncoder h);
        Napi::Object MakeComputePassEncoder(Napi::Env env, wgpu::ComputePassEncoder h);
        Napi::Object MakeRenderBundleEncoder(Napi::Env env, wgpu::RenderBundleEncoder h);
        Napi::Object MakeRenderBundle(Napi::Env env, wgpu::RenderBundle h);
        Napi::Object MakeCommandBuffer(Napi::Env env, wgpu::CommandBuffer h);
        Napi::Object MakeQuerySet(Napi::Env env, wgpu::QuerySet h, uint32_t count);
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
        Napi::Object MakeBuffer(Napi::Env env, wgpu::Buffer h, uint64_t size, uint32_t usage, bool mapped)
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
                uint64_t size = ArgIsUndef(info, 2) ? wgpu::kWholeMapSize : ArgU64(info, 2, 0);
                auto d = Napi::Promise::Deferred::New(env);
                wgpu::Future f = h.MapAsync(wgpu::MapMode(mode), static_cast<size_t>(offset),
                    static_cast<size_t>(size), wgpu::CallbackMode::WaitAnyOnly,
                    [](wgpu::MapAsyncStatus, wgpu::StringView) {});
                g_state.instance.WaitAny(f, UINT64_MAX);
                d.Resolve(env.Undefined());
                return d.Promise();
            });
            SetMethod(o, "getMappedRange", [h, ranges](const Napi::CallbackInfo& info) -> Napi::Value {
                Napi::Env env = info.Env();
                uint64_t offset = ArgU64(info, 0, 0);
                uint64_t size = ArgIsUndef(info, 1)
                    ? (h.GetSize() - offset) : ArgU64(info, 1, 0);
                Napi::ArrayBuffer ab = Napi::ArrayBuffer::New(env, static_cast<size_t>(size));
                // Seed from Dawn's current mapped bytes so read-maps work. For a
                // write/mappedAtCreation map the source is writable too.
                const void* src = h.GetConstMappedRange(static_cast<size_t>(offset), static_cast<size_t>(size));
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
                    void* dst = h.GetMappedRange(static_cast<size_t>(r.offset), static_cast<size_t>(r.size));
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
                h.Unmap();
                return info.Env().Undefined();
            });
            SetMethod(o, "destroy", [h, ranges](const Napi::CallbackInfo& info) -> Napi::Value {
                for (auto& r : *ranges) { r.ab.Reset(); }
                ranges->clear();
                h.Destroy();
                return info.Env().Undefined();
            });
            return o;
        }

        // ---- GPUTextureView --------------------------------------------------
        Napi::Object MakeTextureView(Napi::Env env, wgpu::TextureView h)
        {
            Napi::Object o = NewGPUObject(env, "GPUTextureView");
            SetHandle(o, h);
            return o;
        }

        // ---- GPUTexture ------------------------------------------------------
        Napi::Object MakeTexture(Napi::Env env, wgpu::Texture h, uint32_t w, uint32_t ht, uint32_t depth,
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

            SetMethod(o, "createView", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                Napi::Env env = info.Env();
                wgpu::TextureViewDescriptor d{};
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
                    if (aspect == "stencil-only") d.aspect = wgpu::TextureAspect::StencilOnly;
                    else if (aspect == "depth-only") d.aspect = wgpu::TextureAspect::DepthOnly;
                    else d.aspect = wgpu::TextureAspect::All;
                    if (PropPresent(desc, "usage")) d.usage = wgpu::TextureUsage(PropU32(desc, "usage", 0));
                }
                return MakeTextureView(env, h.CreateView(&d));
            });
            SetMethod(o, "destroy", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                h.Destroy();
                return info.Env().Undefined();
            });
            return o;
        }

        // ---- GPUSampler ------------------------------------------------------
        Napi::Object MakeSampler(Napi::Env env, wgpu::Sampler h)
        {
            Napi::Object o = NewGPUObject(env, "GPUSampler");
            SetHandle(o, h);
            return o;
        }

        // ---- GPUBindGroupLayout ----------------------------------------------
        Napi::Object MakeBindGroupLayout(Napi::Env env, wgpu::BindGroupLayout h)
        {
            Napi::Object o = NewGPUObject(env, "GPUBindGroupLayout");
            SetHandle(o, h);
            return o;
        }

        // ---- GPUBindGroup ----------------------------------------------------
        Napi::Object MakeBindGroup(Napi::Env env, wgpu::BindGroup h)
        {
            Napi::Object o = NewGPUObject(env, "GPUBindGroup");
            SetHandle(o, h);
            return o;
        }

        // ---- GPUPipelineLayout -----------------------------------------------
        Napi::Object MakePipelineLayout(Napi::Env env, wgpu::PipelineLayout h)
        {
            Napi::Object o = NewGPUObject(env, "GPUPipelineLayout");
            SetHandle(o, h);
            return o;
        }

        // ---- GPUShaderModule -------------------------------------------------
        Napi::Object MakeShaderModule(Napi::Env env, wgpu::ShaderModule h)
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
        Napi::Object MakeQuerySet(Napi::Env env, wgpu::QuerySet h, uint32_t count)
        {
            Napi::Object o = NewGPUObject(env, "GPUQuerySet");
            SetHandle(o, h);
            o.Set("count", Napi::Number::New(env, count));
            SetMethod(o, "destroy", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                h.Destroy();
                return info.Env().Undefined();
            });
            return o;
        }

        // ---- GPUCommandBuffer ------------------------------------------------
        Napi::Object MakeCommandBuffer(Napi::Env env, wgpu::CommandBuffer h)
        {
            Napi::Object o = NewGPUObject(env, "GPUCommandBuffer");
            SetHandle(o, h);
            return o;
        }

        // ---- GPURenderPipeline -----------------------------------------------
        Napi::Object MakeRenderPipeline(Napi::Env env, wgpu::RenderPipeline h)
        {
            Napi::Object o = NewGPUObject(env, "GPURenderPipeline");
            SetHandle(o, h);
            SetMethod(o, "getBindGroupLayout", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                Napi::Env env = info.Env();
                uint32_t index = ArgU32(info, 0, 0);
                return MakeBindGroupLayout(env, h.GetBindGroupLayout(index));
            });
            return o;
        }

        // ---- GPUComputePipeline ----------------------------------------------
        Napi::Object MakeComputePipeline(Napi::Env env, wgpu::ComputePipeline h)
        {
            Napi::Object o = NewGPUObject(env, "GPUComputePipeline");
            SetHandle(o, h);
            SetMethod(o, "getBindGroupLayout", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                Napi::Env env = info.Env();
                uint32_t index = ArgU32(info, 0, 0);
                return MakeBindGroupLayout(env, h.GetBindGroupLayout(index));
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

            wgpu::RenderPipelineDescriptor rp{};
            std::string label = PropStr(desc, "label");
            if (!label.empty()) rp.label = label.c_str();

            // layout (object => explicit; "auto"/absent => auto layout)
            {
                Napi::Value layoutV = desc.Get("layout");
                wgpu::PipelineLayout* pl = GetH<wgpu::PipelineLayout>(layoutV);
                if (pl != nullptr) rp.layout = *pl;
            }

            // vertex
            std::string vEntry;
            std::vector<wgpu::VertexBufferLayout> vBuffers;
            std::vector<std::vector<wgpu::VertexAttribute>> vAttrs;
            {
                Napi::Value vtxV = desc.Get("vertex");
                if (!vtxV.IsObject())
                {
                    throw Napi::Error::New(env, "NativeDawn: createRenderPipeline requires vertex stage");
                }
                Napi::Object vtx = vtxV.As<Napi::Object>();
                wgpu::ShaderModule* mod = GetH<wgpu::ShaderModule>(vtx.Get("module"));
                if (mod != nullptr) rp.vertex.module = *mod;
                vEntry = PropStr(vtx, "entryPoint");
                if (!vEntry.empty()) rp.vertex.entryPoint = vEntry.c_str();

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
                            vBuffers[i].stepMode = wgpu::VertexStepMode::Undefined;
                            vBuffers[i].arrayStride = 0;
                            vBuffers[i].attributeCount = 0;
                            continue;
                        }
                        Napi::Object bo = be.As<Napi::Object>();
                        vBuffers[i].arrayStride = PropU64(bo, "arrayStride", 0);
                        vBuffers[i].stepMode = PropPresent(bo, "stepMode")
                            ? vertexStepMode(PropStr(bo, "stepMode")) : wgpu::VertexStepMode::Vertex;
                        Napi::Value attrsV = bo.Get("attributes");
                        if (attrsV.IsArray())
                        {
                            Napi::Array attrs = attrsV.As<Napi::Array>();
                            for (uint32_t j = 0; j < attrs.Length(); ++j)
                            {
                                Napi::Value av = attrs.Get(j);
                                if (!av.IsObject()) continue;
                                Napi::Object ao = av.As<Napi::Object>();
                                wgpu::VertexAttribute va{};
                                va.format = vertexFormat(PropStr(ao, "format"));
                                va.offset = PropU64(ao, "offset", 0);
                                va.shaderLocation = PropU32(ao, "shaderLocation", 0);
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
                        ? primitiveTopology(PropStr(pr, "topology")) : wgpu::PrimitiveTopology::TriangleList;
                    if (PropPresent(pr, "stripIndexFormat"))
                        rp.primitive.stripIndexFormat = indexFormat(PropStr(pr, "stripIndexFormat"));
                    rp.primitive.frontFace = frontFace(PropStr(pr, "frontFace"));
                    rp.primitive.cullMode = cullMode(PropStr(pr, "cullMode"));
                }
            }

            // depthStencil
            wgpu::DepthStencilState dss{};
            {
                Napi::Value dsV = desc.Get("depthStencil");
                if (dsV.IsObject())
                {
                    Napi::Object ds = dsV.As<Napi::Object>();
                    dss.format = textureFormat(PropStr(ds, "format"));
                    dss.depthWriteEnabled = PropBool(ds, "depthWriteEnabled", false)
                        ? wgpu::OptionalBool::True : wgpu::OptionalBool::False;
                    if (PropPresent(ds, "depthCompare"))
                        dss.depthCompare = compareFunction(PropStr(ds, "depthCompare"));
                    dss.stencilReadMask = PropU32(ds, "stencilReadMask", 0xFFFFFFFF);
                    dss.stencilWriteMask = PropU32(ds, "stencilWriteMask", 0xFFFFFFFF);
                    dss.depthBias = PropI32(ds, "depthBias", 0);
                    dss.depthBiasSlopeScale = static_cast<float>(PropF64(ds, "depthBiasSlopeScale", 0));
                    dss.depthBiasClamp = static_cast<float>(PropF64(ds, "depthBiasClamp", 0));
                    auto parseFace = [](Napi::Object f, wgpu::StencilFaceState& out) {
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
            wgpu::FragmentState fs{};
            std::vector<wgpu::ColorTargetState> fTargets;
            std::vector<wgpu::BlendState> fBlends;
            {
                Napi::Value frV = desc.Get("fragment");
                if (frV.IsObject())
                {
                    Napi::Object fr = frV.As<Napi::Object>();
                    wgpu::ShaderModule* mod = GetH<wgpu::ShaderModule>(fr.Get("module"));
                    if (mod != nullptr) fs.module = *mod;
                    fEntry = PropStr(fr, "entryPoint");
                    if (!fEntry.empty()) fs.entryPoint = fEntry.c_str();

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
                            fTargets[i].writeMask = wgpu::ColorWriteMask(PropU32(to, "writeMask", 0xF));
                            Napi::Value blV = to.Get("blend");
                            if (blV.IsObject())
                            {
                                Napi::Object bl = blV.As<Napi::Object>();
                                wgpu::BlendState& bs = fBlends[i];
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

            wgpu::RenderPipeline pipe = g_state.device.CreateRenderPipeline(&rp);
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

            wgpu::ComputePipelineDescriptor cp{};
            std::string label = PropStr(desc, "label");
            if (!label.empty()) cp.label = label.c_str();

            {
                Napi::Value layoutV = desc.Get("layout");
                wgpu::PipelineLayout* pl = GetH<wgpu::PipelineLayout>(layoutV);
                if (pl != nullptr) cp.layout = *pl;
            }

            std::string cEntry;
            Napi::Value coV = desc.Get("compute");
            if (!coV.IsObject())
            {
                throw Napi::Error::New(env, "NativeDawn: createComputePipeline requires compute stage");
            }
            Napi::Object co = coV.As<Napi::Object>();
            wgpu::ShaderModule* mod = GetH<wgpu::ShaderModule>(co.Get("module"));
            if (mod != nullptr) cp.compute.module = *mod;
            cEntry = PropStr(co, "entryPoint");
            if (!cEntry.empty()) cp.compute.entryPoint = cEntry.c_str();

            wgpu::ComputePipeline pipe = g_state.device.CreateComputePipeline(&cp);
            return MakeComputePipeline(env, pipe);
        }

        // ---- texel copy helpers ----------------------------------------------
        wgpu::TexelCopyBufferInfo ParseTexelCopyBuffer(Napi::Object o)
        {
            wgpu::TexelCopyBufferInfo info{};
            wgpu::Buffer* b = GetH<wgpu::Buffer>(o.Get("buffer"));
            if (b != nullptr) info.buffer = *b;
            info.layout.offset = PropU64(o, "offset", 0);
            info.layout.bytesPerRow = PropPresent(o, "bytesPerRow")
                ? PropU32(o, "bytesPerRow", 0) : wgpu::kCopyStrideUndefined;
            info.layout.rowsPerImage = PropPresent(o, "rowsPerImage")
                ? PropU32(o, "rowsPerImage", 0) : wgpu::kCopyStrideUndefined;
            return info;
        }
        wgpu::TexelCopyTextureInfo ParseTexelCopyTexture(Napi::Object o)
        {
            wgpu::TexelCopyTextureInfo info{};
            wgpu::Texture* t = GetH<wgpu::Texture>(o.Get("texture"));
            if (t != nullptr) info.texture = *t;
            info.mipLevel = PropU32(o, "mipLevel", 0);
            if (PropPresent(o, "origin")) info.origin = ParseOrigin3D(o.Get("origin"));
            std::string aspect = PropStr(o, "aspect");
            if (aspect == "stencil-only") info.aspect = wgpu::TextureAspect::StencilOnly;
            else if (aspect == "depth-only") info.aspect = wgpu::TextureAspect::DepthOnly;
            else info.aspect = wgpu::TextureAspect::All;
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
        Napi::Object MakeRenderPassEncoder(Napi::Env env, wgpu::RenderPassEncoder h)
        {
            Napi::Object o = NewGPUObject(env, "GPURenderPassEncoder");
            SetHandle(o, h);
            SetMethod(o, "setPipeline", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                wgpu::RenderPipeline* p = GetH<wgpu::RenderPipeline>(info[0]);
                if (p != nullptr) h.SetPipeline(*p);
                return info.Env().Undefined();
            });
            SetMethod(o, "setBindGroup", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                uint32_t index = ArgU32(info, 0, 0);
                wgpu::BindGroup bg{};
                wgpu::BindGroup* p = GetH<wgpu::BindGroup>(info[1]);
                if (p != nullptr) bg = *p;
                std::vector<uint32_t> offsets;
                if (info.Length() > 2 && !IsNullish(info[2])) offsets = ReadU32Array(info[2]);
                h.SetBindGroup(index, bg, offsets.size(), offsets.empty() ? nullptr : offsets.data());
                return info.Env().Undefined();
            });
            SetMethod(o, "setVertexBuffer", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                uint32_t slot = ArgU32(info, 0, 0);
                wgpu::Buffer buf{};
                wgpu::Buffer* p = GetH<wgpu::Buffer>(info[1]);
                if (p != nullptr) buf = *p;
                uint64_t offset = ArgU64(info, 2, 0);
                uint64_t size = ArgIsUndef(info, 3) ? wgpu::kWholeSize : ArgU64(info, 3, 0);
                h.SetVertexBuffer(slot, buf, offset, size);
                return info.Env().Undefined();
            });
            SetMethod(o, "setIndexBuffer", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                wgpu::Buffer buf{};
                wgpu::Buffer* p = GetH<wgpu::Buffer>(info[0]);
                if (p != nullptr) buf = *p;
                std::string fmt = (info.Length() > 1 && info[1].IsString())
                    ? info[1].As<Napi::String>().Utf8Value() : std::string{};
                uint64_t offset = ArgU64(info, 2, 0);
                uint64_t size = ArgIsUndef(info, 3) ? wgpu::kWholeSize : ArgU64(info, 3, 0);
                h.SetIndexBuffer(buf, indexFormat(fmt), offset, size);
                return info.Env().Undefined();
            });
            SetMethod(o, "draw", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                h.Draw(ArgU32(info, 0, 0), ArgU32(info, 1, 1), ArgU32(info, 2, 0), ArgU32(info, 3, 0));
                return info.Env().Undefined();
            });
            SetMethod(o, "drawIndexed", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                h.DrawIndexed(ArgU32(info, 0, 0), ArgU32(info, 1, 1), ArgU32(info, 2, 0),
                    ArgI32(info, 3, 0), ArgU32(info, 4, 0));
                return info.Env().Undefined();
            });
            SetMethod(o, "drawIndirect", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                wgpu::Buffer* b = GetH<wgpu::Buffer>(info[0]);
                if (b != nullptr) h.DrawIndirect(*b, ArgU64(info, 1, 0));
                return info.Env().Undefined();
            });
            SetMethod(o, "drawIndexedIndirect", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                wgpu::Buffer* b = GetH<wgpu::Buffer>(info[0]);
                if (b != nullptr) h.DrawIndexedIndirect(*b, ArgU64(info, 1, 0));
                return info.Env().Undefined();
            });
            SetMethod(o, "setViewport", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                h.SetViewport(static_cast<float>(ArgF64(info, 0, 0)), static_cast<float>(ArgF64(info, 1, 0)),
                    static_cast<float>(ArgF64(info, 2, 0)), static_cast<float>(ArgF64(info, 3, 0)),
                    static_cast<float>(ArgF64(info, 4, 0)), static_cast<float>(ArgF64(info, 5, 1)));
                return info.Env().Undefined();
            });
            SetMethod(o, "setScissorRect", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                h.SetScissorRect(ArgU32(info, 0, 0), ArgU32(info, 1, 0), ArgU32(info, 2, 0), ArgU32(info, 3, 0));
                return info.Env().Undefined();
            });
            SetMethod(o, "setBlendConstant", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                wgpu::Color c = ParseColor(info[0]);
                h.SetBlendConstant(&c);
                return info.Env().Undefined();
            });
            SetMethod(o, "setStencilReference", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                h.SetStencilReference(ArgU32(info, 0, 0));
                return info.Env().Undefined();
            });
            SetMethod(o, "beginOcclusionQuery", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                h.BeginOcclusionQuery(ArgU32(info, 0, 0));
                return info.Env().Undefined();
            });
            SetMethod(o, "endOcclusionQuery", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                h.EndOcclusionQuery();
                return info.Env().Undefined();
            });
            SetMethod(o, "executeBundles", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                std::vector<wgpu::RenderBundle> bundles;
                if (info.Length() > 0 && info[0].IsArray())
                {
                    Napi::Array a = info[0].As<Napi::Array>();
                    for (uint32_t i = 0; i < a.Length(); ++i)
                    {
                        wgpu::RenderBundle* b = GetH<wgpu::RenderBundle>(a.Get(i));
                        if (b != nullptr) bundles.push_back(*b);
                    }
                }
                h.ExecuteBundles(bundles.size(), bundles.empty() ? nullptr : bundles.data());
                return info.Env().Undefined();
            });
            SetMethod(o, "pushDebugGroup", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                std::string s = (info.Length() > 0 && info[0].IsString()) ? info[0].As<Napi::String>().Utf8Value() : std::string{};
                h.PushDebugGroup(s.c_str());
                return info.Env().Undefined();
            });
            SetMethod(o, "popDebugGroup", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                h.PopDebugGroup();
                return info.Env().Undefined();
            });
            SetMethod(o, "insertDebugMarker", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                std::string s = (info.Length() > 0 && info[0].IsString()) ? info[0].As<Napi::String>().Utf8Value() : std::string{};
                h.InsertDebugMarker(s.c_str());
                return info.Env().Undefined();
            });
            SetMethod(o, "end", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                h.End();
                return info.Env().Undefined();
            });
            return o;
        }

        // ---- GPUComputePassEncoder -------------------------------------------
        Napi::Object MakeComputePassEncoder(Napi::Env env, wgpu::ComputePassEncoder h)
        {
            Napi::Object o = NewGPUObject(env, "GPUComputePassEncoder");
            SetHandle(o, h);
            SetMethod(o, "setPipeline", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                wgpu::ComputePipeline* p = GetH<wgpu::ComputePipeline>(info[0]);
                if (p != nullptr) h.SetPipeline(*p);
                return info.Env().Undefined();
            });
            SetMethod(o, "setBindGroup", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                uint32_t index = ArgU32(info, 0, 0);
                wgpu::BindGroup bg{};
                wgpu::BindGroup* p = GetH<wgpu::BindGroup>(info[1]);
                if (p != nullptr) bg = *p;
                std::vector<uint32_t> offsets;
                if (info.Length() > 2 && !IsNullish(info[2])) offsets = ReadU32Array(info[2]);
                h.SetBindGroup(index, bg, offsets.size(), offsets.empty() ? nullptr : offsets.data());
                return info.Env().Undefined();
            });
            SetMethod(o, "dispatchWorkgroups", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                h.DispatchWorkgroups(ArgU32(info, 0, 1), ArgU32(info, 1, 1), ArgU32(info, 2, 1));
                return info.Env().Undefined();
            });
            SetMethod(o, "dispatchWorkgroupsIndirect", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                wgpu::Buffer* b = GetH<wgpu::Buffer>(info[0]);
                if (b != nullptr) h.DispatchWorkgroupsIndirect(*b, ArgU64(info, 1, 0));
                return info.Env().Undefined();
            });
            SetMethod(o, "pushDebugGroup", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                std::string s = (info.Length() > 0 && info[0].IsString()) ? info[0].As<Napi::String>().Utf8Value() : std::string{};
                h.PushDebugGroup(s.c_str());
                return info.Env().Undefined();
            });
            SetMethod(o, "popDebugGroup", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                h.PopDebugGroup();
                return info.Env().Undefined();
            });
            SetMethod(o, "insertDebugMarker", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                std::string s = (info.Length() > 0 && info[0].IsString()) ? info[0].As<Napi::String>().Utf8Value() : std::string{};
                h.InsertDebugMarker(s.c_str());
                return info.Env().Undefined();
            });
            SetMethod(o, "end", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                h.End();
                return info.Env().Undefined();
            });
            return o;
        }

        // ---- GPURenderBundle -------------------------------------------------
        Napi::Object MakeRenderBundle(Napi::Env env, wgpu::RenderBundle h)
        {
            Napi::Object o = NewGPUObject(env, "GPURenderBundle");
            SetHandle(o, h);
            return o;
        }

        // ---- GPURenderBundleEncoder ------------------------------------------
        Napi::Object MakeRenderBundleEncoder(Napi::Env env, wgpu::RenderBundleEncoder h)
        {
            Napi::Object o = NewGPUObject(env, "GPURenderBundleEncoder");
            SetHandle(o, h);
            SetMethod(o, "setPipeline", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                wgpu::RenderPipeline* p = GetH<wgpu::RenderPipeline>(info[0]);
                if (p != nullptr) h.SetPipeline(*p);
                return info.Env().Undefined();
            });
            SetMethod(o, "setBindGroup", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                uint32_t index = ArgU32(info, 0, 0);
                wgpu::BindGroup bg{};
                wgpu::BindGroup* p = GetH<wgpu::BindGroup>(info[1]);
                if (p != nullptr) bg = *p;
                std::vector<uint32_t> offsets;
                if (info.Length() > 2 && !IsNullish(info[2])) offsets = ReadU32Array(info[2]);
                h.SetBindGroup(index, bg, offsets.size(), offsets.empty() ? nullptr : offsets.data());
                return info.Env().Undefined();
            });
            SetMethod(o, "setVertexBuffer", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                uint32_t slot = ArgU32(info, 0, 0);
                wgpu::Buffer buf{};
                wgpu::Buffer* p = GetH<wgpu::Buffer>(info[1]);
                if (p != nullptr) buf = *p;
                uint64_t offset = ArgU64(info, 2, 0);
                uint64_t size = ArgIsUndef(info, 3) ? wgpu::kWholeSize : ArgU64(info, 3, 0);
                h.SetVertexBuffer(slot, buf, offset, size);
                return info.Env().Undefined();
            });
            SetMethod(o, "setIndexBuffer", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                wgpu::Buffer buf{};
                wgpu::Buffer* p = GetH<wgpu::Buffer>(info[0]);
                if (p != nullptr) buf = *p;
                std::string fmt = (info.Length() > 1 && info[1].IsString())
                    ? info[1].As<Napi::String>().Utf8Value() : std::string{};
                uint64_t offset = ArgU64(info, 2, 0);
                uint64_t size = ArgIsUndef(info, 3) ? wgpu::kWholeSize : ArgU64(info, 3, 0);
                h.SetIndexBuffer(buf, indexFormat(fmt), offset, size);
                return info.Env().Undefined();
            });
            SetMethod(o, "draw", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                h.Draw(ArgU32(info, 0, 0), ArgU32(info, 1, 1), ArgU32(info, 2, 0), ArgU32(info, 3, 0));
                return info.Env().Undefined();
            });
            SetMethod(o, "drawIndexed", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                h.DrawIndexed(ArgU32(info, 0, 0), ArgU32(info, 1, 1), ArgU32(info, 2, 0),
                    ArgI32(info, 3, 0), ArgU32(info, 4, 0));
                return info.Env().Undefined();
            });
            SetMethod(o, "pushDebugGroup", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                std::string s = (info.Length() > 0 && info[0].IsString()) ? info[0].As<Napi::String>().Utf8Value() : std::string{};
                h.PushDebugGroup(s.c_str());
                return info.Env().Undefined();
            });
            SetMethod(o, "popDebugGroup", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                h.PopDebugGroup();
                return info.Env().Undefined();
            });
            SetMethod(o, "insertDebugMarker", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                std::string s = (info.Length() > 0 && info[0].IsString()) ? info[0].As<Napi::String>().Utf8Value() : std::string{};
                h.InsertDebugMarker(s.c_str());
                return info.Env().Undefined();
            });
            SetMethod(o, "finish", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                Napi::Env env = info.Env();
                wgpu::RenderBundleDescriptor d{};
                std::string label;
                if (info.Length() > 0 && info[0].IsObject())
                {
                    label = PropStr(info[0].As<Napi::Object>(), "label");
                    if (!label.empty()) d.label = label.c_str();
                }
                return MakeRenderBundle(env, h.Finish(&d));
            });
            return o;
        }

        // ---- GPUCommandEncoder -----------------------------------------------
        Napi::Object MakeCommandEncoder(Napi::Env env, wgpu::CommandEncoder h)
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
                wgpu::RenderPassDescriptor rpd{};
                std::string label = PropStr(desc, "label");
                if (!label.empty()) rpd.label = label.c_str();

                std::vector<wgpu::RenderPassColorAttachment> colors;
                Napi::Value caV = desc.Get("colorAttachments");
                if (caV.IsArray())
                {
                    Napi::Array arr = caV.As<Napi::Array>();
                    for (uint32_t i = 0; i < arr.Length(); ++i)
                    {
                        wgpu::RenderPassColorAttachment c{};
                        Napi::Value ev = arr.Get(i);
                        if (!ev.IsObject()) { colors.push_back(c); continue; }
                        Napi::Object ca = ev.As<Napi::Object>();
                        wgpu::TextureView* view = GetH<wgpu::TextureView>(ca.Get("view"));
                        if (view != nullptr) c.view = *view;
                        if (PropPresent(ca, "depthSlice")) c.depthSlice = PropU32(ca, "depthSlice", 0);
                        wgpu::TextureView* resolve = GetH<wgpu::TextureView>(ca.Get("resolveTarget"));
                        if (resolve != nullptr) c.resolveTarget = *resolve;
                        c.loadOp = loadOp(PropStr(ca, "loadOp"));
                        c.storeOp = storeOp(PropStr(ca, "storeOp"));
                        if (PropPresent(ca, "clearValue")) c.clearValue = ParseColor(ca.Get("clearValue"));
                        colors.push_back(c);
                    }
                }
                rpd.colorAttachmentCount = colors.size();
                rpd.colorAttachments = colors.empty() ? nullptr : colors.data();

                wgpu::RenderPassDepthStencilAttachment dsa{};
                Napi::Value dsV = desc.Get("depthStencilAttachment");
                if (dsV.IsObject())
                {
                    Napi::Object ds = dsV.As<Napi::Object>();
                    wgpu::TextureView* view = GetH<wgpu::TextureView>(ds.Get("view"));
                    if (view != nullptr) dsa.view = *view;
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

                return MakeRenderPassEncoder(env, h.BeginRenderPass(&rpd));
            });
            SetMethod(o, "beginComputePass", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                Napi::Env env = info.Env();
                wgpu::ComputePassDescriptor d{};
                std::string label;
                if (info.Length() > 0 && info[0].IsObject())
                {
                    label = PropStr(info[0].As<Napi::Object>(), "label");
                    if (!label.empty()) d.label = label.c_str();
                }
                return MakeComputePassEncoder(env, h.BeginComputePass(&d));
            });
            SetMethod(o, "copyBufferToBuffer", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                wgpu::Buffer* src = GetH<wgpu::Buffer>(info[0]);
                wgpu::Buffer* dst = GetH<wgpu::Buffer>(info[2]);
                if (src != nullptr && dst != nullptr)
                    h.CopyBufferToBuffer(*src, ArgU64(info, 1, 0), *dst, ArgU64(info, 3, 0), ArgU64(info, 4, 0));
                return info.Env().Undefined();
            });
            SetMethod(o, "copyBufferToTexture", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                wgpu::TexelCopyBufferInfo src = ParseTexelCopyBuffer(info[0].As<Napi::Object>());
                wgpu::TexelCopyTextureInfo dst = ParseTexelCopyTexture(info[1].As<Napi::Object>());
                wgpu::Extent3D size = ParseExtent3D(info[2]);
                h.CopyBufferToTexture(&src, &dst, &size);
                return info.Env().Undefined();
            });
            SetMethod(o, "copyTextureToBuffer", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                wgpu::TexelCopyTextureInfo src = ParseTexelCopyTexture(info[0].As<Napi::Object>());
                wgpu::TexelCopyBufferInfo dst = ParseTexelCopyBuffer(info[1].As<Napi::Object>());
                wgpu::Extent3D size = ParseExtent3D(info[2]);
                h.CopyTextureToBuffer(&src, &dst, &size);
                return info.Env().Undefined();
            });
            SetMethod(o, "copyTextureToTexture", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                wgpu::TexelCopyTextureInfo src = ParseTexelCopyTexture(info[0].As<Napi::Object>());
                wgpu::TexelCopyTextureInfo dst = ParseTexelCopyTexture(info[1].As<Napi::Object>());
                wgpu::Extent3D size = ParseExtent3D(info[2]);
                h.CopyTextureToTexture(&src, &dst, &size);
                return info.Env().Undefined();
            });
            SetMethod(o, "clearBuffer", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                wgpu::Buffer* b = GetH<wgpu::Buffer>(info[0]);
                if (b != nullptr)
                {
                    uint64_t offset = ArgU64(info, 1, 0);
                    uint64_t size = ArgIsUndef(info, 2) ? wgpu::kWholeSize : ArgU64(info, 2, 0);
                    h.ClearBuffer(*b, offset, size);
                }
                return info.Env().Undefined();
            });
            SetMethod(o, "resolveQuerySet", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                wgpu::QuerySet* qs = GetH<wgpu::QuerySet>(info[0]);
                wgpu::Buffer* dst = GetH<wgpu::Buffer>(info[3]);
                if (qs != nullptr && dst != nullptr)
                    h.ResolveQuerySet(*qs, ArgU32(info, 1, 0), ArgU32(info, 2, 0), *dst, ArgU64(info, 4, 0));
                return info.Env().Undefined();
            });
            SetMethod(o, "pushDebugGroup", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                std::string s = (info.Length() > 0 && info[0].IsString()) ? info[0].As<Napi::String>().Utf8Value() : std::string{};
                h.PushDebugGroup(s.c_str());
                return info.Env().Undefined();
            });
            SetMethod(o, "popDebugGroup", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                h.PopDebugGroup();
                return info.Env().Undefined();
            });
            SetMethod(o, "insertDebugMarker", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                std::string s = (info.Length() > 0 && info[0].IsString()) ? info[0].As<Napi::String>().Utf8Value() : std::string{};
                h.InsertDebugMarker(s.c_str());
                return info.Env().Undefined();
            });
            SetMethod(o, "finish", [h](const Napi::CallbackInfo& info) -> Napi::Value {
                Napi::Env env = info.Env();
                wgpu::CommandBufferDescriptor d{};
                std::string label;
                if (info.Length() > 0 && info[0].IsObject())
                {
                    label = PropStr(info[0].As<Napi::Object>(), "label");
                    if (!label.empty()) d.label = label.c_str();
                }
                return MakeCommandBuffer(env, h.Finish(&d));
            });
            return o;
        }

        // ---- GPUQueue --------------------------------------------------------
        Napi::Object MakeQueue(Napi::Env env)
        {
            Napi::Object o = NewGPUObject(env, "GPUQueue");
            SetHandle(o, g_state.queue);
            SetMethod(o, "submit", [](const Napi::CallbackInfo& info) -> Napi::Value {
                std::vector<wgpu::CommandBuffer> bufs;
                if (info.Length() > 0 && info[0].IsArray())
                {
                    Napi::Array arr = info[0].As<Napi::Array>();
                    for (uint32_t i = 0; i < arr.Length(); ++i)
                    {
                        wgpu::CommandBuffer* cb = GetH<wgpu::CommandBuffer>(arr.Get(i));
                        if (cb != nullptr) bufs.push_back(*cb);
                    }
                }
                if (!bufs.empty()) g_state.queue.Submit(bufs.size(), bufs.data());
                return info.Env().Undefined();
            });
            SetMethod(o, "writeBuffer", [](const Napi::CallbackInfo& info) -> Napi::Value {
                wgpu::Buffer* b = GetH<wgpu::Buffer>(info[0]);
                if (b == nullptr) return info.Env().Undefined();
                uint64_t bufferOffset = ArgU64(info, 1, 0);
                Bytes bytes = GetBytes(info[2]);
                if (bytes.data == nullptr) return info.Env().Undefined();
                size_t dataOffset = static_cast<size_t>(ArgU64(info, 3, 0));
                size_t length = (!ArgIsUndef(info, 4))
                    ? static_cast<size_t>(ArgU64(info, 4, 0)) : (bytes.size - dataOffset);
                g_state.queue.WriteBuffer(*b, bufferOffset, bytes.data + dataOffset, length);
                return info.Env().Undefined();
            });
            SetMethod(o, "writeTexture", [](const Napi::CallbackInfo& info) -> Napi::Value {
                Napi::Env env = info.Env();
                if (!info[0].IsObject()) return env.Undefined();
                wgpu::TexelCopyTextureInfo tci = ParseTexelCopyTexture(info[0].As<Napi::Object>());
                Bytes bytes = GetBytes(info[1]);
                Napi::Object layout = info[2].As<Napi::Object>();
                wgpu::TexelCopyBufferLayout tbl{};
                tbl.offset = PropU64(layout, "offset", 0);
                tbl.bytesPerRow = PropPresent(layout, "bytesPerRow")
                    ? PropU32(layout, "bytesPerRow", 0) : wgpu::kCopyStrideUndefined;
                tbl.rowsPerImage = PropPresent(layout, "rowsPerImage")
                    ? PropU32(layout, "rowsPerImage", 0) : wgpu::kCopyStrideUndefined;
                wgpu::Extent3D ext = ParseExtent3D(info[3]);
                if (bytes.data != nullptr)
                    g_state.queue.WriteTexture(&tci, bytes.data, bytes.size, &tbl, &ext);
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
                Bytes px = GetBytes(bmp.Get("__pixels"));
                uint32_t w = PropU32(bmp, "width", 0);
                uint32_t h = PropU32(bmp, "height", 0);
                if (px.data == nullptr || w == 0 || h == 0)
                {
                    throw Napi::Error::New(env, "copyExternalImageToTexture: source has no decoded pixels");
                }
                bool flipY = false;
                {
                    Napi::Value f = srcDesc.Get("flipY");
                    if (f.IsBoolean()) flipY = f.As<Napi::Boolean>().Value();
                }

                Napi::Object destDesc = info[1].As<Napi::Object>();
                wgpu::TexelCopyTextureInfo tci = ParseTexelCopyTexture(destDesc);
                wgpu::Extent3D ext = ParseExtent3D(info[2]);
                if (ext.width == 0) ext.width = w;
                if (ext.height == 0) ext.height = h;
                if (ext.depthOrArrayLayers == 0) ext.depthOrArrayLayers = 1;

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

                wgpu::TexelCopyBufferLayout tbl{};
                tbl.offset = 0;
                tbl.rowsPerImage = h;

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
                    g_state.queue.WriteTexture(&tci, half.data(),
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
                    g_state.queue.WriteTexture(&tci, f.data(),
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
                    g_state.queue.WriteTexture(&tci, bgra.data(),
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
                        g_state.queue.WriteTexture(&tci, flipped.data(),
                            static_cast<size_t>(srcRowBytes) * h, &tbl, &ext);
                    }
                    else
                    {
                        g_state.queue.WriteTexture(&tci, px.data,
                            static_cast<size_t>(srcRowBytes) * h, &tbl, &ext);
                    }
                }
                return env.Undefined();
            });
            SetMethod(o, "onSubmittedWorkDone", [](const Napi::CallbackInfo& info) -> Napi::Value {
                Napi::Env env = info.Env();
                auto d = Napi::Promise::Deferred::New(env);
                wgpu::Future f = g_state.queue.OnSubmittedWorkDone(wgpu::CallbackMode::WaitAnyOnly,
                    [](wgpu::QueueWorkDoneStatus, wgpu::StringView) {});
                g_state.instance.WaitAny(f, UINT64_MAX);
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
            SetHandle(o, g_state.device);
            o.Set("queue", MakeQueue(env));
            o.Set("label", Napi::String::New(env, ""));

            o.Set("features", MakeFeatureSet(env, [](const std::string& name) {
                return name.empty() ? false : static_cast<bool>(g_state.device.HasFeature(featureName(name)));
            }));
            {
                wgpu::Limits L{};
                g_state.device.GetLimits(&L);
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
                wgpu::BufferDescriptor bd{};
                bd.size = PropU64(desc, "size", 0);
                uint32_t usage = PropU32(desc, "usage", 0);
                bd.usage = wgpu::BufferUsage(usage);
                bool mapped = PropBool(desc, "mappedAtCreation", false);
                bd.mappedAtCreation = mapped;
                std::string label = PropStr(desc, "label");
                if (!label.empty()) bd.label = label.c_str();
                wgpu::Buffer buf = g_state.device.CreateBuffer(&bd);
                if (!buf) throw Napi::Error::New(env, "NativeDawn: createBuffer failed");
                return MakeBuffer(env, buf, bd.size, usage, mapped);
            });
            SetMethod(o, "createTexture", [](const Napi::CallbackInfo& info) -> Napi::Value {
                Napi::Env env = info.Env();
                Napi::Object desc = info[0].As<Napi::Object>();
                wgpu::TextureDescriptor td{};
                td.size = ParseExtent3D(desc.Get("size"));
                td.mipLevelCount = PropU32(desc, "mipLevelCount", 1);
                td.sampleCount = PropU32(desc, "sampleCount", 1);
                std::string fmt = PropStr(desc, "format");
                td.format = textureFormat(fmt);
                uint32_t usage = PropU32(desc, "usage", 0);
                td.usage = wgpu::TextureUsage(usage);
                std::string dim = PropStr(desc, "dimension");
                td.dimension = textureDimension(dim);
                std::vector<wgpu::TextureFormat> viewFormats;
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
                if (!label.empty()) td.label = label.c_str();
                wgpu::Texture tex = g_state.device.CreateTexture(&td);
                if (!tex) throw Napi::Error::New(env, "NativeDawn: createTexture failed");
                return MakeTexture(env, tex, td.size.width, td.size.height, td.size.depthOrArrayLayers,
                    td.mipLevelCount, td.sampleCount, fmt, usage, dim);
            });
            SetMethod(o, "createSampler", [](const Napi::CallbackInfo& info) -> Napi::Value {
                Napi::Env env = info.Env();
                wgpu::SamplerDescriptor sd{};
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
                wgpu::Sampler s = g_state.device.CreateSampler(&sd);
                return MakeSampler(env, s);
            });
            SetMethod(o, "createShaderModule", [](const Napi::CallbackInfo& info) -> Napi::Value {
                Napi::Env env = info.Env();
                Napi::Object desc = info[0].As<Napi::Object>();
                std::string code = PropStr(desc, "code");
                wgpu::ShaderSourceWGSL wgsl{};
                wgsl.code = code.c_str();
                wgpu::ShaderModuleDescriptor smd{};
                smd.nextInChain = &wgsl;
                std::string label = PropStr(desc, "label");
                if (!label.empty()) smd.label = label.c_str();
                wgpu::ShaderModule m = g_state.device.CreateShaderModule(&smd);
                return MakeShaderModule(env, m);
            });
            SetMethod(o, "createBindGroupLayout", [](const Napi::CallbackInfo& info) -> Napi::Value {
                Napi::Env env = info.Env();
                Napi::Object desc = info[0].As<Napi::Object>();
                Napi::Value entriesV = desc.Get("entries");
                std::vector<wgpu::BindGroupLayoutEntry> entries;
                if (entriesV.IsArray())
                {
                    Napi::Array arr = entriesV.As<Napi::Array>();
                    entries.resize(arr.Length());
                    for (uint32_t i = 0; i < arr.Length(); ++i)
                    {
                        Napi::Object e = arr.Get(i).As<Napi::Object>();
                        wgpu::BindGroupLayoutEntry& dst = entries[i];
                        dst.binding = PropU32(e, "binding", 0);
                        dst.visibility = wgpu::ShaderStage(PropU32(e, "visibility", 0));
                        if (e.Get("buffer").IsObject())
                        {
                            Napi::Object b = e.Get("buffer").As<Napi::Object>();
                            dst.buffer.type = bufferBindingType(PropStr(b, "type").empty() ? "uniform" : PropStr(b, "type"));
                            dst.buffer.hasDynamicOffset = PropBool(b, "hasDynamicOffset", false);
                            dst.buffer.minBindingSize = PropU64(b, "minBindingSize", 0);
                        }
                        if (e.Get("sampler").IsObject())
                        {
                            Napi::Object s = e.Get("sampler").As<Napi::Object>();
                            dst.sampler.type = samplerBindingType(PropStr(s, "type").empty() ? "filtering" : PropStr(s, "type"));
                        }
                        if (e.Get("texture").IsObject())
                        {
                            Napi::Object t = e.Get("texture").As<Napi::Object>();
                            dst.texture.sampleType = textureSampleType(PropStr(t, "sampleType").empty() ? "float" : PropStr(t, "sampleType"));
                            dst.texture.viewDimension = textureViewDimension(PropStr(t, "viewDimension").empty() ? "2d" : PropStr(t, "viewDimension"));
                            dst.texture.multisampled = PropBool(t, "multisampled", false);
                        }
                        if (e.Get("storageTexture").IsObject())
                        {
                            Napi::Object st = e.Get("storageTexture").As<Napi::Object>();
                            dst.storageTexture.access = storageTextureAccess(PropStr(st, "access").empty() ? "write-only" : PropStr(st, "access"));
                            dst.storageTexture.format = textureFormat(PropStr(st, "format"));
                            dst.storageTexture.viewDimension = textureViewDimension(PropStr(st, "viewDimension").empty() ? "2d" : PropStr(st, "viewDimension"));
                        }
                    }
                }
                wgpu::BindGroupLayoutDescriptor bgld{};
                bgld.entryCount = entries.size();
                bgld.entries = entries.empty() ? nullptr : entries.data();
                std::string label = PropStr(desc, "label");
                if (!label.empty()) bgld.label = label.c_str();
                wgpu::BindGroupLayout bgl = g_state.device.CreateBindGroupLayout(&bgld);
                return MakeBindGroupLayout(env, bgl);
            });
            SetMethod(o, "createBindGroup", [](const Napi::CallbackInfo& info) -> Napi::Value {
                Napi::Env env = info.Env();
                Napi::Object desc = info[0].As<Napi::Object>();
                wgpu::BindGroupLayout* layout = GetH<wgpu::BindGroupLayout>(desc.Get("layout"));
                if (layout == nullptr) throw Napi::Error::New(env, "NativeDawn: createBindGroup missing layout");
                Napi::Value entriesV = desc.Get("entries");
                std::vector<wgpu::BindGroupEntry> entries;
                if (entriesV.IsArray())
                {
                    Napi::Array arr = entriesV.As<Napi::Array>();
                    entries.resize(arr.Length());
                    for (uint32_t i = 0; i < arr.Length(); ++i)
                    {
                        Napi::Object e = arr.Get(i).As<Napi::Object>();
                        wgpu::BindGroupEntry& dst = entries[i];
                        dst.binding = PropU32(e, "binding", 0);
                        Napi::Value resource = e.Get("resource");
                        if (!resource.IsObject()) continue;
                        Napi::Object ro = resource.As<Napi::Object>();
                        if (ro.Has("buffer") && ro.Get("buffer").IsObject())
                        {
                            wgpu::Buffer* b = GetH<wgpu::Buffer>(ro.Get("buffer"));
                            if (b != nullptr)
                            {
                                dst.buffer = *b;
                                dst.offset = PropU64(ro, "offset", 0);
                                dst.size = PropPresent(ro, "size") ? PropU64(ro, "size", 0) : wgpu::kWholeSize;
                            }
                        }
                        else
                        {
                            std::string ty = TypeTag(resource);
                            if (ty == "GPUSampler")
                            {
                                wgpu::Sampler* s = GetH<wgpu::Sampler>(resource);
                                if (s != nullptr) dst.sampler = *s;
                            }
                            else if (ty == "GPUTextureView")
                            {
                                wgpu::TextureView* v = GetH<wgpu::TextureView>(resource);
                                if (v != nullptr) dst.textureView = *v;
                            }
                        }
                    }
                }
                wgpu::BindGroupDescriptor bgd{};
                bgd.layout = *layout;
                bgd.entryCount = entries.size();
                bgd.entries = entries.empty() ? nullptr : entries.data();
                std::string label = PropStr(desc, "label");
                if (!label.empty()) bgd.label = label.c_str();
                wgpu::BindGroup bg = g_state.device.CreateBindGroup(&bgd);
                return MakeBindGroup(env, bg);
            });
            SetMethod(o, "createPipelineLayout", [](const Napi::CallbackInfo& info) -> Napi::Value {
                Napi::Env env = info.Env();
                Napi::Object desc = info[0].As<Napi::Object>();
                std::vector<wgpu::BindGroupLayout> layouts;
                Napi::Value bglsV = desc.Get("bindGroupLayouts");
                if (bglsV.IsArray())
                {
                    Napi::Array arr = bglsV.As<Napi::Array>();
                    for (uint32_t i = 0; i < arr.Length(); ++i)
                    {
                        wgpu::BindGroupLayout* l = GetH<wgpu::BindGroupLayout>(arr.Get(i));
                        if (l != nullptr) layouts.push_back(*l);
                    }
                }
                wgpu::PipelineLayoutDescriptor pld{};
                pld.bindGroupLayoutCount = layouts.size();
                pld.bindGroupLayouts = layouts.empty() ? nullptr : layouts.data();
                std::string label = PropStr(desc, "label");
                if (!label.empty()) pld.label = label.c_str();
                wgpu::PipelineLayout pl = g_state.device.CreatePipelineLayout(&pld);
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
                wgpu::CommandEncoderDescriptor ced{};
                std::string label;
                if (info.Length() > 0 && info[0].IsObject())
                {
                    label = PropStr(info[0].As<Napi::Object>(), "label");
                    if (!label.empty()) ced.label = label.c_str();
                }
                return MakeCommandEncoder(env, g_state.device.CreateCommandEncoder(&ced));
            });
            SetMethod(o, "createRenderBundleEncoder", [](const Napi::CallbackInfo& info) -> Napi::Value {
                Napi::Env env = info.Env();
                Napi::Object desc = info[0].As<Napi::Object>();
                wgpu::RenderBundleEncoderDescriptor rbed{};
                std::vector<wgpu::TextureFormat> cf;
                Napi::Value cfV = desc.Get("colorFormats");
                if (cfV.IsArray())
                {
                    Napi::Array a = cfV.As<Napi::Array>();
                    for (uint32_t i = 0; i < a.Length(); ++i)
                    {
                        Napi::Value e = a.Get(i);
                        cf.push_back(e.IsString() ? textureFormat(e.As<Napi::String>().Utf8Value())
                                                  : wgpu::TextureFormat::Undefined);
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
                if (!label.empty()) rbed.label = label.c_str();
                return MakeRenderBundleEncoder(env, g_state.device.CreateRenderBundleEncoder(&rbed));
            });
            SetMethod(o, "createQuerySet", [](const Napi::CallbackInfo& info) -> Napi::Value {
                Napi::Env env = info.Env();
                Napi::Object desc = info[0].As<Napi::Object>();
                wgpu::QuerySetDescriptor qsd{};
                qsd.type = queryType(PropStr(desc, "type"));
                uint32_t count = PropU32(desc, "count", 0);
                qsd.count = count;
                return MakeQuerySet(env, g_state.device.CreateQuerySet(&qsd), count);
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
            wgpu::AdapterInfo info{};
            g_state.adapter.GetInfo(&info);
            i.Set("vendor", Napi::String::New(env, SvToStr(info.vendor)));
            i.Set("architecture", Napi::String::New(env, SvToStr(info.architecture)));
            i.Set("device", Napi::String::New(env, SvToStr(info.device)));
            i.Set("description", Napi::String::New(env, SvToStr(info.description)));
            return i;
        }

        Napi::Object MakeAdapter(Napi::Env env)
        {
            Napi::Object o = NewGPUObject(env, "GPUAdapter");
            SetHandle(o, g_state.adapter);
            o.Set("isFallbackAdapter", Napi::Boolean::New(env, false));

            o.Set("features", MakeFeatureSet(env, [](const std::string& name) {
                return name.empty() ? false : static_cast<bool>(g_state.adapter.HasFeature(featureName(name)));
            }));
            {
                wgpu::Limits L{};
                g_state.adapter.GetLimits(&L);
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
                wgpu::TextureFormat f = fmt.empty() ? g_state.surfaceFormat : textureFormat(fmt);
                g_state.surfaceFormat = f;
                wgpu::SurfaceConfiguration cfg{};
                cfg.device = g_state.device;
                cfg.format = f;
                uint32_t usage = PropU32(desc, "usage", static_cast<uint32_t>(wgpu::TextureUsage::RenderAttachment));
                cfg.usage = wgpu::TextureUsage(usage | static_cast<uint32_t>(wgpu::TextureUsage::CopySrc));
                cfg.width = g_state.width > 1 ? g_state.width : 1;
                cfg.height = g_state.height > 1 ? g_state.height : 1;
                std::string am = PropStr(desc, "alphaMode");
                cfg.alphaMode = (am == "premultiplied")
                    ? wgpu::CompositeAlphaMode::Premultiplied : wgpu::CompositeAlphaMode::Opaque;
                cfg.presentMode = wgpu::PresentMode::Fifo;
                g_state.surface.Configure(&cfg);
                g_surfaceConfigured = true;
                return env.Undefined();
            });
            SetMethod(o, "unconfigure", [](const Napi::CallbackInfo& info) -> Napi::Value {
                if (g_surfaceConfigured)
                {
                    g_state.surface.Unconfigure();
                    g_surfaceConfigured = false;
                }
                return info.Env().Undefined();
            });
            SetMethod(o, "getCurrentTexture", [](const Napi::CallbackInfo& info) -> Napi::Value {
                Napi::Env env = info.Env();
                wgpu::SurfaceTexture st{};
                g_state.surface.GetCurrentTexture(&st);
                if (!st.texture) throw Napi::Error::New(env, "NativeDawn: getCurrentTexture returned null");
                g_currentTextureAcquired = true;
                g_state.currentSurfaceTexture = st.texture;
                return MakeTexture(env, st.texture, g_state.width, g_state.height, 1, 1, 1,
                    textureFormatStr(g_state.surfaceFormat),
                    static_cast<uint32_t>(wgpu::TextureUsage::RenderAttachment), "2d");
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
                if (g_surfaceConfigured && g_currentTextureAcquired)
                {
                    g_state.surface.Present();
                }
                g_currentTextureAcquired = false;
                if (g_state.instance)
                {
                    g_state.instance.ProcessEvents();
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

                wgpu::BufferDescriptor bd{};
                bd.size = bufSize;
                bd.usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::MapRead;
                wgpu::Buffer buf = g_state.device.CreateBuffer(&bd);

                wgpu::TexelCopyTextureInfo src{};
                src.texture = g_state.currentSurfaceTexture;
                src.mipLevel = 0;
                src.origin = {0, 0, 0};
                src.aspect = wgpu::TextureAspect::All;
                wgpu::TexelCopyBufferInfo dst{};
                dst.buffer = buf;
                dst.layout.offset = 0;
                dst.layout.bytesPerRow = padded;
                dst.layout.rowsPerImage = h;
                wgpu::Extent3D ext{w, h, 1};

                wgpu::CommandEncoder enc = g_state.device.CreateCommandEncoder();
                enc.CopyTextureToBuffer(&src, &dst, &ext);
                wgpu::CommandBuffer cmd = enc.Finish();
                g_state.queue.Submit(1, &cmd);

                wgpu::Future f = buf.MapAsync(wgpu::MapMode::Read, 0, bufSize,
                    wgpu::CallbackMode::WaitAnyOnly,
                    [](wgpu::MapAsyncStatus, wgpu::StringView) {});
                g_state.instance.WaitAny(f, UINT64_MAX);

                const uint8_t* mapped = static_cast<const uint8_t*>(buf.GetConstMappedRange(0, bufSize));
                const size_t outSize = static_cast<size_t>(unpadded) * h;
                Napi::ArrayBuffer ab = Napi::ArrayBuffer::New(env, outSize);
                uint8_t* out = static_cast<uint8_t*>(ab.Data());
                const bool bgra = (g_state.surfaceFormat == wgpu::TextureFormat::BGRA8Unorm ||
                                   g_state.surfaceFormat == wgpu::TextureFormat::BGRA8UnormSrgb);
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
                buf.Unmap();

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
                g_state.width = w;
                g_state.height = h;
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
                if (g_surfaceConfigured)
                {
                    wgpu::SurfaceConfiguration cfg{};
                    cfg.device = g_state.device;
                    cfg.format = g_state.surfaceFormat;
                    cfg.usage = wgpu::TextureUsage::RenderAttachment | wgpu::TextureUsage::CopySrc;
                    cfg.width = w;
                    cfg.height = h;
                    cfg.presentMode = wgpu::PresentMode::Fifo;
                    g_state.surface.Configure(&cfg);
                }
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

            std::fprintf(stderr, "[NativeDawn] WebGPU (navigator.gpu) installed\n");
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

        Napi::Object Make2DContext(Napi::Env env, Napi::Object canvas)
        {
            auto canvasRef = std::make_shared<Napi::ObjectReference>(Napi::Persistent(canvas));
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
            SetMethod(ctx, "fillText", [canvasRef](const Napi::CallbackInfo& info) -> Napi::Value {
                // Text rasterization is not supported on the WebGPU backend (the
                // bgfx/nanovg Canvas polyfill is unavailable). Ensure the backing
                // buffer exists so the canvas is still a valid texture source.
                EnsureCanvasBuffer(info.Env(), canvasRef->Value());
                return info.Env().Undefined();
            });
            SetMethod(ctx, "strokeText", Noop);
            SetMethod(ctx, "setLineDash", Noop);
            SetMethod(ctx, "measureText", [](const Napi::CallbackInfo& info) -> Napi::Value {
                Napi::Object o = Napi::Object::New(info.Env());
                o.Set("width", Napi::Number::New(info.Env(), 8));
                return o;
            });
            SetMethod(ctx, "getContextAttributes", [](const Napi::CallbackInfo& info) -> Napi::Value {
                return Napi::Object::New(info.Env());
            });
            SetMethod(ctx, "fillRect", [canvasRef](const Napi::CallbackInfo& info) -> Napi::Value {
                EnsureCanvasBuffer(info.Env(), canvasRef->Value());
                return info.Env().Undefined();
            });

            SetMethod(ctx, "clearRect", [canvasRef](const Napi::CallbackInfo& info) -> Napi::Value {
                Napi::Env env = info.Env();
                Napi::Object canvas = canvasRef->Value();
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

            SetMethod(ctx, "drawImage", [canvasRef, ctm](const Napi::CallbackInfo& info) -> Napi::Value {
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

                Napi::Object canvas = canvasRef->Value();
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

            SetMethod(ctx, "getImageData", [canvasRef](const Napi::CallbackInfo& info) -> Napi::Value {
                Napi::Env env = info.Env();
                Napi::Object canvas = canvasRef->Value();
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

            SetMethod(ctx, "putImageData", [canvasRef](const Napi::CallbackInfo& info) -> Napi::Value {
                Napi::Env env = info.Env();
                if (!info[0].IsObject()) return env.Undefined();
                Napi::Object imgData = info[0].As<Napi::Object>();
                const int dx = info.Length() > 1 ? info[1].ToNumber().Int32Value() : 0;
                const int dy = info.Length() > 2 ? info[2].ToNumber().Int32Value() : 0;
                const uint32_t iw = PropU32(imgData, "width", 0);
                const uint32_t ih = PropU32(imgData, "height", 0);
                Bytes src = GetBytes(imgData.Get("data"));
                if (src.data == nullptr || iw == 0 || ih == 0) return env.Undefined();
                Napi::Object canvas = canvasRef->Value();
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
                    Napi::Object c = Make2DContext(env, self);
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
                std::fprintf(stderr, "[NativeDawn] WebGPUEngine ready\n");
                return env.Undefined();
            });
            Napi::Function onErr = Napi::Function::New(env, [](const Napi::CallbackInfo& info) -> Napi::Value {
                std::string msg = info.Length() > 0 ? info[0].ToString().Utf8Value() : "?";
                std::fprintf(stderr, "[NativeDawn] engine init failed: %s\n", msg.c_str());
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
                    SetMethod(el, "setAttribute", Noop);
                    SetMethod(el, "appendChild", Noop);
                    SetMethod(el, "removeChild", Noop);
                    SetMethod(el, "insertBefore", Noop);
                    SetMethod(el, "addEventListener", Noop);
                    SetMethod(el, "removeEventListener", Noop);
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
                        return Napi::Promise::Deferred::New(info.Env()).Promise();
                    });
                    SetMethod(img, "addEventListener", [](const Napi::CallbackInfo& info) -> Napi::Value {
                        Napi::Object self = info.This().As<Napi::Object>();
                        std::string name = info.Length() > 0 && info[0].IsString() ? info[0].As<Napi::String>().Utf8Value() : "";
                        if (name == "load") self.Set("onload", info[1]);
                        else if (name == "error") self.Set("onerror", info[1]);
                        return info.Env().Undefined();
                    });
                    SetMethod(img, "removeEventListener", Noop);

                    // Define the async-decoding `src` accessor.
                    auto imgRef = std::make_shared<Napi::ObjectReference>(Napi::Persistent(img));
                    Napi::Object desc = Napi::Object::New(env);
                    desc.Set("configurable", Napi::Boolean::New(env, true));
                    desc.Set("get", Napi::Function::New(env, [](const Napi::CallbackInfo& info) -> Napi::Value {
                        return info.This().As<Napi::Object>().Get("_src");
                    }));
                    desc.Set("set", Napi::Function::New(env, [imgRef](const Napi::CallbackInfo& info) -> Napi::Value {
                        Napi::Env env = info.Env();
                        Napi::Value v = info.Length() > 0 ? info[0] : env.Undefined();
                        imgRef->Value().Set("_src", v);
                        Napi::Value p = ToArrayBuffer(env, v);
                        Napi::Function onAb = Napi::Function::New(env, [imgRef](const Napi::CallbackInfo& info) -> Napi::Value {
                            Napi::Env env = info.Env();
                            Napi::Object img = imgRef->Value();
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
                            Napi::Value onerror = imgRef->Value().Get("onerror");
                            if (onerror.IsFunction())
                            {
                                onerror.As<Napi::Function>().Call(imgRef->Value(), {info.Length() > 0 ? info[0] : env.Undefined()});
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
                }
                // Present the frame.
                if (g_surfaceConfigured && g_currentTextureAcquired)
                {
                    g_state.surface.Present();
                }
                g_currentTextureAcquired = false;
                if (g_state.instance)
                {
                    g_state.instance.ProcessEvents();
                }
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
        if (!CreateDeviceAndSurface(window, width, height))
        {
            std::fprintf(stderr, "[NativeDawn] initialization failed\n");
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
        }
    }

    void Tick(Napi::Env)
    {
        if (g_state.ready)
        {
            g_state.instance.ProcessEvents();
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
        if (g_state.width == width && g_state.height == height)
        {
            return;
        }
        g_state.width = width;
        g_state.height = height;
        // Drop the cached current texture (it belongs to the old swapchain).
        g_state.currentSurfaceTexture = nullptr;
        g_currentTextureAcquired = false;
        if (g_surfaceConfigured)
        {
            wgpu::SurfaceConfiguration cfg{};
            cfg.device = g_state.device;
            cfg.format = g_state.surfaceFormat;
            cfg.usage = wgpu::TextureUsage::RenderAttachment | wgpu::TextureUsage::CopySrc;
            cfg.width = width;
            cfg.height = height;
            cfg.presentMode = wgpu::PresentMode::Fifo;
            g_state.surface.Configure(&cfg);
        }
    }
}
