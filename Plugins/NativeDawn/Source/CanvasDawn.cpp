// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
//
// See CanvasDawn.h for why the Dawn path needs its own canvas implementation.

#include "CanvasDawn.h"
#include "ContextDawn.h"

#include "Colors.h"
#include "Font.h"
#include "nanovg/nanovg_dawn.h"

#include <napi/pointer.h>

#include <cstring>
#include <stdexcept>

namespace Babylon::Plugins::Internal
{
    // nanovg_dawn.h declares the backend in the Canvas polyfill's namespace
    // because it lives alongside the bgfx backend it mirrors.
    using Babylon::Polyfills::Internal::NvgDawnStencilFormat;

    namespace
    {
        constexpr auto JS_CONSTRUCTOR_NAME = "Canvas";

        // The one Dawn device/instance NativeDawn created. Every canvas in this
        // environment renders on it.
        WGPUDevice s_device{};
        WGPUInstance s_instance{};

        // Persistent reference to the Canvas constructor, so DOM canvases can be
        // given a real 2D context without going through global lookup.
        Napi::FunctionReference s_constructor{};

        WGPUStringView Label(const char* text)
        {
            return WGPUStringView{text, WGPU_STRLEN};
        }

        void WaitFuture(WGPUFuture future)
        {
            WGPUFutureWaitInfo wait{};
            wait.future = future;
            wgpuInstanceWaitAny(s_instance, 1, &wait, UINT64_MAX);
        }
    }

    void NativeCanvasDawn::Initialize(Napi::Env env, WGPUDevice device, WGPUInstance instance)
    {
        Napi::HandleScope scope{env};

        s_device = device;
        s_instance = instance;

        Napi::Function func = DefineClass(
            env,
            JS_CONSTRUCTOR_NAME,
            {
                StaticMethod("loadTTF", &NativeCanvasDawn::LoadTTF),
                StaticMethod("loadTTFAsync", &NativeCanvasDawn::LoadTTFAsync),
                InstanceAccessor("width", &NativeCanvasDawn::GetWidth, &NativeCanvasDawn::SetWidth),
                InstanceAccessor("height", &NativeCanvasDawn::GetHeight, &NativeCanvasDawn::SetHeight),
                InstanceMethod("getContext", &NativeCanvasDawn::GetContext),
                InstanceMethod("getCanvasTexture", &NativeCanvasDawn::GetCanvasTexture),
                InstanceMethod("dispose", &NativeCanvasDawn::Dispose),
                InstanceMethod("remove", &NativeCanvasDawn::Remove),
                StaticMethod("parseColor", &NativeCanvasDawn::ParseColor)});

        s_constructor = Napi::Persistent(func);
        s_constructor.SuppressDestruct();

        // Overwrites the bgfx NativeCanvas registered by Polyfills::Canvas::Initialize,
        // which runs earlier in Runtime.cpp. Its constructor would throw here anyway
        // (there is no bgfx Graphics::DeviceContext on the Dawn path).
        JsRuntime::NativeObject::GetFromJavaScript(env).Set(JS_CONSTRUCTOR_NAME, func);

        ContextDawn::Initialize(env);
    }

    Napi::Object NativeCanvasDawn::CreateInstance(Napi::Env env)
    {
        return s_constructor.New({});
    }

    NativeCanvasDawn::NativeCanvasDawn(const Napi::CallbackInfo& info)
        : Napi::ObjectWrap<NativeCanvasDawn>{info}
        , m_device{s_device}
    {
    }

    NativeCanvasDawn::~NativeCanvasDawn()
    {
        Dispose();
    }

    void NativeCanvasDawn::Remove(const Napi::CallbackInfo&)
    {
        // called when removed from document which has no meaning for Native
    }

    void NativeCanvasDawn::LoadTTF(const Napi::CallbackInfo& info)
    {
        if (info.Length() < 1 || !info[0].IsString())
        {
            throw Napi::TypeError::New(info.Env(), "Canvas.loadTTF expects the font name as a string in argument 1.");
        }

        // don't allow same font to be loaded more than once
        // why? because ContextDawn doesn't update nvgCreateFontMem when old fontBuffer released
        auto fontName = info[0].As<Napi::String>().Utf8Value();
        if (fontsInfos.find(fontName) == fontsInfos.end())
        {
            fontsInfos[fontName] = Babylon::Polyfills::Internal::GetFontDataArgument(info, 1, "Canvas.loadTTF");
        }
    }

    // @deprecated: LoadTTFAsync is always synchronous, use LoadTTF instead
    Napi::Value NativeCanvasDawn::LoadTTFAsync(const Napi::CallbackInfo& info)
    {
        LoadTTF(info);

        auto deferred{Napi::Promise::Deferred::New(info.Env())};
        deferred.Resolve(info.Env().Undefined());
        return deferred.Promise();
    }

    Napi::Value NativeCanvasDawn::GetContext(const Napi::CallbackInfo& info)
    {
        auto thisObj = info.This().ToObject();
        const auto contextPropertyName = Napi::Value::From(Env(), "_context");

        auto context = thisObj.Get(contextPropertyName);
        if (context.IsUndefined())
        {
            context = ContextDawn::CreateInstance(info.Env(), info.This());
            thisObj.Set(contextPropertyName, context);
        }

        return context;
    }

    Napi::Value NativeCanvasDawn::GetWidth(const Napi::CallbackInfo&)
    {
        return Napi::Value::From(Env(), m_width);
    }

    void NativeCanvasDawn::SetWidth(const Napi::CallbackInfo&, const Napi::Value& value)
    {
        auto width = static_cast<uint16_t>(value.As<Napi::Number>().Uint32Value());
        if (!width)
        {
            return;
        }

        if (width == m_width)
        {
            // Assigning the same size is the idiomatic way to clear a canvas.
            m_clear = true;
        }
        else
        {
            m_width = width;
            m_dirty = true;
        }
    }

    Napi::Value NativeCanvasDawn::GetHeight(const Napi::CallbackInfo&)
    {
        return Napi::Value::From(Env(), m_height);
    }

    void NativeCanvasDawn::SetHeight(const Napi::CallbackInfo&, const Napi::Value& value)
    {
        auto height = static_cast<uint16_t>(value.As<Napi::Number>().Uint32Value());
        if (!height)
        {
            return;
        }

        if (height == m_height)
        {
            m_clear = true;
        }
        else
        {
            m_height = height;
            m_dirty = true;
        }
    }

    bool NativeCanvasDawn::UpdateRenderTarget()
    {
        bool needClear = m_clear;
        m_clear = false;

        if (!m_dirty && m_colorTexture != nullptr &&
            m_allocatedWidth == m_width && m_allocatedHeight == m_height)
        {
            return needClear;
        }

        Dispose();

        WGPUTextureDescriptor colorDesc{};
        colorDesc.label = Label("NativeCanvasDawn.color");
        colorDesc.dimension = WGPUTextureDimension_2D;
        colorDesc.size = {m_width, m_height, 1};
        colorDesc.format = ColorFormat();
        colorDesc.mipLevelCount = 1;
        colorDesc.sampleCount = 1;
        // TEXTURE_BINDING so Babylon can sample it, COPY_SRC so
        // copyExternalImageToTexture can do a GPU->GPU copy out of it.
        colorDesc.usage = WGPUTextureUsage_RenderAttachment | WGPUTextureUsage_TextureBinding | WGPUTextureUsage_CopySrc;

        m_colorTexture = wgpuDeviceCreateTexture(m_device, &colorDesc);
        if (m_colorTexture == nullptr)
        {
            throw std::runtime_error{"NativeCanvasDawn: failed to create the canvas colour texture."};
        }

        WGPUTextureDescriptor stencilDesc{};
        stencilDesc.label = Label("NativeCanvasDawn.stencil");
        stencilDesc.dimension = WGPUTextureDimension_2D;
        stencilDesc.size = {m_width, m_height, 1};
        stencilDesc.format = NvgDawnStencilFormat();
        stencilDesc.mipLevelCount = 1;
        stencilDesc.sampleCount = 1;
        stencilDesc.usage = WGPUTextureUsage_RenderAttachment;

        m_stencilTexture = wgpuDeviceCreateTexture(m_device, &stencilDesc);
        if (m_stencilTexture == nullptr)
        {
            wgpuTextureRelease(m_colorTexture);
            m_colorTexture = nullptr;
            throw std::runtime_error{"NativeCanvasDawn: failed to create the canvas depth-stencil texture."};
        }

        m_colorView = wgpuTextureCreateView(m_colorTexture, nullptr);
        m_stencilView = wgpuTextureCreateView(m_stencilTexture, nullptr);

        m_allocatedWidth = m_width;
        m_allocatedHeight = m_height;
        m_dirty = false;

        // A freshly created texture has undefined contents, so the first pass
        // must clear regardless of what the caller asked for.
        return true;
    }

    NativeCanvasDawn::RenderPass NativeCanvasDawn::BeginFrame()
    {

        RenderPass pass{};

        if (m_width == 0 || m_height == 0)
        {
            return pass;
        }

        const bool needClear = UpdateRenderTarget();

        WGPUCommandEncoderDescriptor encoderDesc{};
        encoderDesc.label = Label("NativeCanvasDawn.flush");
        pass.commandEncoder = wgpuDeviceCreateCommandEncoder(m_device, &encoderDesc);

        WGPURenderPassColorAttachment color{};
        color.view = m_colorView;
        color.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;
        color.loadOp = needClear ? WGPULoadOp_Clear : WGPULoadOp_Load;
        color.storeOp = WGPUStoreOp_Store;
        // Matches the bgfx path, which clears the canvas render target to
        // transparent black (see the WebGL spec reference in NativeCanvas).
        color.clearValue = {0.0, 0.0, 0.0, 0.0};

        WGPURenderPassDepthStencilAttachment depthStencil{};
        depthStencil.view = m_stencilView;
        depthStencil.depthLoadOp = WGPULoadOp_Clear;
        depthStencil.depthStoreOp = WGPUStoreOp_Store;
        depthStencil.depthClearValue = 1.0f;
        // nanovg's non-convex fill assumes the stencil starts at 0 and restores
        // it to 0, so it must be cleared at the start of every pass.
        depthStencil.stencilLoadOp = WGPULoadOp_Clear;
        depthStencil.stencilStoreOp = WGPUStoreOp_Store;
        depthStencil.stencilClearValue = 0;

        WGPURenderPassDescriptor passDesc{};
        passDesc.label = Label("NativeCanvasDawn.pass");
        passDesc.colorAttachmentCount = 1;
        passDesc.colorAttachments = &color;
        passDesc.depthStencilAttachment = &depthStencil;

        pass.encoder = wgpuCommandEncoderBeginRenderPass(pass.commandEncoder, &passDesc);
        return pass;
    }

    void NativeCanvasDawn::EndFrame(RenderPass& pass)
    {

        if (pass.encoder == nullptr)
        {
            return;
        }

        wgpuRenderPassEncoderEnd(pass.encoder);
        wgpuRenderPassEncoderRelease(pass.encoder);
        pass.encoder = nullptr;

        WGPUCommandBufferDescriptor cmdDesc{};
        cmdDesc.label = Label("NativeCanvasDawn.commands");
        WGPUCommandBuffer commands = wgpuCommandEncoderFinish(pass.commandEncoder, &cmdDesc);

        WGPUQueue queue = wgpuDeviceGetQueue(m_device);
        wgpuQueueSubmit(queue, 1, &commands);

        wgpuCommandBufferRelease(commands);
        wgpuQueueRelease(queue);
        wgpuCommandEncoderRelease(pass.commandEncoder);
        pass.commandEncoder = nullptr;
    }

    Napi::Value NativeCanvasDawn::GetCanvasTexture(const Napi::CallbackInfo& info)
    {
        // The bgfx path hands back a Graphics::Texture for NativeEngine to blit.
        // On Dawn, Babylon consumes canvases through copyExternalImageToTexture,
        // which reads the WGPUTexture directly, so expose the raw handle.
        if (m_colorTexture == nullptr)
        {
            return info.Env().Undefined();
        }

        return Napi::External<WGPUTextureImpl>::New(info.Env(), m_colorTexture);
    }

    Napi::Value NativeCanvasDawn::ParseColor(const Napi::CallbackInfo& info)
    {
        const auto colorString = info[0].As<Napi::String>().Utf8Value();
        const auto color = Polyfills::Internal::StringToColor(info.Env(), colorString);

        return Napi::Value::From(info.Env(),
            ((uint32_t(color.a * 255.f) & 0xFF) << 24) +
            ((uint32_t(color.b * 255.f) & 0xFF) << 16) +
            ((uint32_t(color.g * 255.f) & 0xFF) << 8) +
            (uint32_t(color.r * 255.f) & 0xFF));
    }

    void NativeCanvasDawn::Dispose()
    {
        if (m_colorView != nullptr)
        {
            wgpuTextureViewRelease(m_colorView);
            m_colorView = nullptr;
        }
        if (m_stencilView != nullptr)
        {
            wgpuTextureViewRelease(m_stencilView);
            m_stencilView = nullptr;
        }
        if (m_colorTexture != nullptr)
        {
            wgpuTextureDestroy(m_colorTexture);
            wgpuTextureRelease(m_colorTexture);
            m_colorTexture = nullptr;
        }
        if (m_stencilTexture != nullptr)
        {
            wgpuTextureDestroy(m_stencilTexture);
            wgpuTextureRelease(m_stencilTexture);
            m_stencilTexture = nullptr;
        }

        m_allocatedWidth = 0;
        m_allocatedHeight = 0;
        m_dirty = true;
    }

    void NativeCanvasDawn::Dispose(const Napi::CallbackInfo& /*info*/)
    {
        Dispose();
    }

    bool NativeCanvasDawn::ReadPixels(std::vector<uint8_t>& out)
    {
        if (m_colorTexture == nullptr || m_allocatedWidth == 0 || m_allocatedHeight == 0)
        {
            return false;
        }

        const uint32_t w = m_allocatedWidth;
        const uint32_t h = m_allocatedHeight;

        // CopyTextureToBuffer requires bytesPerRow to be a multiple of 256.
        constexpr uint32_t kAlign = 256;
        const uint32_t unpadded = w * 4u;
        const uint32_t padded = (unpadded + kAlign - 1u) / kAlign * kAlign;
        const uint64_t bufSize = static_cast<uint64_t>(padded) * h;

        WGPUBufferDescriptor bd{};
        bd.label = Label("NativeCanvasDawn.readback");
        bd.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_MapRead;
        bd.size = bufSize;
        WGPUBuffer buf = wgpuDeviceCreateBuffer(m_device, &bd);
        if (buf == nullptr)
        {
            return false;
        }

        WGPUTexelCopyTextureInfo src{};
        src.texture = m_colorTexture;
        src.mipLevel = 0;
        src.aspect = WGPUTextureAspect_All;

        WGPUTexelCopyBufferInfo dst{};
        dst.layout.offset = 0;
        dst.layout.bytesPerRow = padded;
        dst.layout.rowsPerImage = h;
        dst.buffer = buf;

        WGPUExtent3D ext{w, h, 1};

        WGPUCommandEncoder enc = wgpuDeviceCreateCommandEncoder(m_device, nullptr);
        wgpuCommandEncoderCopyTextureToBuffer(enc, &src, &dst, &ext);
        WGPUCommandBuffer cmd = wgpuCommandEncoderFinish(enc, nullptr);

        WGPUQueue queue = wgpuDeviceGetQueue(m_device);
        wgpuQueueSubmit(queue, 1, &cmd);
        wgpuCommandBufferRelease(cmd);
        wgpuCommandEncoderRelease(enc);
        wgpuQueueRelease(queue);

        WGPUBufferMapCallbackInfo mapCb{};
        mapCb.mode = WGPUCallbackMode_WaitAnyOnly;
        mapCb.callback = [](WGPUMapAsyncStatus, WGPUStringView, void*, void*) {};
        WaitFuture(wgpuBufferMapAsync(buf, WGPUMapMode_Read, 0, static_cast<size_t>(bufSize), mapCb));

        const auto* mapped = static_cast<const uint8_t*>(
            wgpuBufferGetConstMappedRange(buf, 0, static_cast<size_t>(bufSize)));

        bool ok = false;
        if (mapped != nullptr)
        {
            out.resize(static_cast<size_t>(unpadded) * h);
            for (uint32_t y = 0; y < h; ++y)
            {
                std::memcpy(out.data() + static_cast<size_t>(y) * unpadded,
                    mapped + static_cast<size_t>(y) * padded, unpadded);
            }
            ok = true;
        }

        wgpuBufferUnmap(buf);
        wgpuBufferRelease(buf);
        return ok;
    }

    Napi::Value AttachDawn2DContext(Napi::Env env, Napi::Object domCanvas)
    {
        Napi::Object nvgCanvas = NativeCanvasDawn::CreateInstance(env);

        // Mirror the DOM canvas' current size onto the render target.
        const uint32_t w = domCanvas.Get("width").ToNumber().Uint32Value();
        const uint32_t h = domCanvas.Get("height").ToNumber().Uint32Value();
        nvgCanvas.Set("width", Napi::Number::New(env, w == 0 ? 1 : w));
        nvgCanvas.Set("height", Napi::Number::New(env, h == 0 ? 1 : h));

        Napi::Value ctx = nvgCanvas.As<Napi::Object>()
                              .Get("getContext")
                              .As<Napi::Function>()
                              .Call(nvgCanvas, {Napi::String::New(env, "2d")});

        // Babylon reads ctx.canvas.width/height and sometimes assigns to them, so
        // point `canvas` back at the DOM object rather than the internal one.
        ctx.As<Napi::Object>().Set("canvas", domCanvas);

        domCanvas.Set("__nvgCanvas", nvgCanvas);
        return ctx;
    }

    bool SyncDawnCanvasPixels(Napi::Env env, Napi::Object domCanvas)
    {
        Napi::Value nvgVal = domCanvas.Get("__nvgCanvas");
        if (!nvgVal.IsObject())
        {
            return false;
        }

        Napi::Object nvgCanvas = nvgVal.As<Napi::Object>();
        NativeCanvasDawn* canvas = NativeCanvasDawn::Unwrap(nvgCanvas);
        if (canvas == nullptr)
        {
            return false;
        }

        // Babylon resizes the DOM canvas directly (dynamicTexture.getContext()
        // then canvas.width = n), so re-sync before flushing. Only propagate a
        // real size change: assigning the same value is the idiomatic way to
        // clear a canvas, so doing it unconditionally on every upload would
        // discard everything drawn since the previous flush (e.g. the first of
        // two consecutive DynamicTexture.drawText calls).
        const uint32_t w = domCanvas.Get("width").ToNumber().Uint32Value();
        const uint32_t h = domCanvas.Get("height").ToNumber().Uint32Value();
        if (w != 0 && h != 0)
        {
            if (nvgCanvas.Get("width").ToNumber().Uint32Value() != w)
            {
                nvgCanvas.Set("width", Napi::Number::New(env, w));
            }

            if (nvgCanvas.Get("height").ToNumber().Uint32Value() != h)
            {
                nvgCanvas.Set("height", Napi::Number::New(env, h));
            }
        }

        Napi::Value ctxVal = nvgCanvas.Get("_context");
        if (ctxVal.IsObject())
        {
            Napi::Value flush = ctxVal.As<Napi::Object>().Get("flush");
            if (flush.IsFunction())
            {
                flush.As<Napi::Function>().Call(ctxVal, {});
            }
        }

        std::vector<uint8_t> pixels;
        if (!canvas->ReadPixels(pixels))
        {
            return false;
        }

        Napi::ArrayBuffer ab = Napi::ArrayBuffer::New(env, pixels.size());
        std::memcpy(ab.Data(), pixels.data(), pixels.size());
        domCanvas.Set("__pixels", ab);
        return true;
    }
}
