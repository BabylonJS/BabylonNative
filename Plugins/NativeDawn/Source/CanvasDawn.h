// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
//
// NativeCanvasDawn — the WebGPU-backed counterpart of the Canvas polyfill's
// NativeCanvas.
//
// The Canvas polyfill's render target is bgfx-typed all the way down
// (Graphics::FrameBuffer, Graphics::Texture, bgfx::ViewId) and its constructor
// acquires the bgfx Graphics::DeviceContext, which the Dawn backend never
// creates. Rather than make that shipping code polymorphic, the Dawn plugin
// installs a parallel implementation that renders through nanovg_dawn.
//
// Everything that is not the submission layer — paths, gradients, images,
// fonts, colour parsing — is reused from the Canvas polyfill unchanged.

#pragma once

#include <Babylon/JsRuntime.h>

#include <webgpu/webgpu.h>

#include <cstdint>
#include <map>
#include <string>
#include <vector>

namespace Babylon::Plugins::Internal
{
    class NativeCanvasDawn final : public Napi::ObjectWrap<NativeCanvasDawn>
    {
    public:
        // Registers the `Canvas` constructor on the native object, replacing the
        // bgfx one installed earlier by Polyfills::Canvas::Initialize. `device`
        // and `instance` must outlive the JS environment.
        static void Initialize(Napi::Env env, WGPUDevice device, WGPUInstance instance);

        explicit NativeCanvasDawn(const Napi::CallbackInfo& info);
        virtual ~NativeCanvasDawn();

        uint32_t GetWidth() const { return m_width; }
        uint32_t GetHeight() const { return m_height; }

        WGPUDevice GetDevice() const { return m_device; }

        // Format of the offscreen texture the canvas renders into. NanoVG
        // pipelines are created against this, and Babylon consumes the result
        // through copyExternalImageToTexture, which expects RGBA8.
        static WGPUTextureFormat ColorFormat() { return WGPUTextureFormat_RGBA8Unorm; }

        // Fonts registered through Canvas.loadTTF. Shared by every context.
        static inline std::map<std::string, std::vector<uint8_t>> fontsInfos;

        struct RenderPass
        {
            WGPUCommandEncoder commandEncoder;
            WGPURenderPassEncoder encoder;
        };

        // Allocates/resizes the offscreen texture as needed and opens a render
        // pass on it. The colour attachment is cleared only when the canvas is
        // newly allocated or was explicitly resized/reset, so successive flushes
        // composite into the same image (matching the bgfx path). The stencil
        // attachment is always cleared to 0, which nanovg's fill algorithm
        // requires.
        //
        // Returns a null encoder if the canvas has no usable size.
        RenderPass BeginFrame();

        // Ends and submits the pass opened by BeginFrame.
        void EndFrame(RenderPass& pass);

        // The rendered canvas image. Null until the first BeginFrame.
        WGPUTexture GetTexture() const { return m_colorTexture; }

        // Reads the rendered image back to tightly packed RGBA8 (size
        // width*height*4). Returns false if nothing has been rendered yet.
        bool ReadPixels(std::vector<uint8_t>& out);

        // Creates a canvas JS object and returns it, for callers that need one
        // without going through the `Canvas` constructor from script.
        static Napi::Object CreateInstance(Napi::Env env);

    private:
        Napi::Value GetContext(const Napi::CallbackInfo&);
        Napi::Value GetWidth(const Napi::CallbackInfo&);
        void SetWidth(const Napi::CallbackInfo&, const Napi::Value& value);
        Napi::Value GetHeight(const Napi::CallbackInfo&);
        void SetHeight(const Napi::CallbackInfo&, const Napi::Value& value);
        Napi::Value GetCanvasTexture(const Napi::CallbackInfo& info);
        static void LoadTTF(const Napi::CallbackInfo& info);
        static Napi::Value LoadTTFAsync(const Napi::CallbackInfo& info);
        static Napi::Value ParseColor(const Napi::CallbackInfo& info);
        void Remove(const Napi::CallbackInfo& info);
        void Dispose(const Napi::CallbackInfo& info);
        void Dispose();

        // (Re)creates the colour + depth-stencil textures if the size changed.
        // Returns true when the colour attachment must be cleared this pass.
        bool UpdateRenderTarget();

        Napi::ObjectReference m_contextObject{};

        WGPUDevice m_device{};

        uint16_t m_width{1};
        uint16_t m_height{1};

        WGPUTexture m_colorTexture{};
        WGPUTextureView m_colorView{};
        WGPUTexture m_stencilTexture{};
        WGPUTextureView m_stencilView{};

        uint16_t m_allocatedWidth{};
        uint16_t m_allocatedHeight{};

        bool m_dirty{true};
        bool m_clear{};
    };

    // Attaches a real NanoVG-backed 2D context to a DOM-style canvas object
    // (the plain JS objects NativeDawn's document.createElement("canvas")
    // returns). The returned context's `canvas` property is the DOM object, so
    // Babylon's width/height reads keep working.
    Napi::Value AttachDawn2DContext(Napi::Env env, Napi::Object domCanvas);

    // If `domCanvas` has a NanoVG-backed 2D context attached, flushes any
    // pending drawing and refreshes the canvas' `__pixels` RGBA8 mirror so the
    // existing copyExternalImageToTexture upload path picks up the rendered
    // content. Returns true when it refreshed the mirror.
    bool SyncDawnCanvasPixels(Napi::Env env, Napi::Object domCanvas);
}
