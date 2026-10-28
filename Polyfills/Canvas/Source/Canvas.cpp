#include "Canvas.h"
#include "CanvasConstants.h"
#include "Image.h"
#include "Path2D.h"
#include "Context.h"
#include "NativeInstanceRegistry.h"
#include <bgfx/bgfx.h>
#include <bgfx/embedded_shader.h>
#include <napi/pointer.h>
#include <array>
#include <cassert>
#include <cstring>
#include <string>
#include <vector>
#include "Colors.h"
#include "Gradient.h"
#include "Font.h"
#include <basen.hpp>
#include <bimg/encode.h>
#include <bx/allocator.h>
#include <bx/readerwriter.h>

#include "Shaders/dxbc/vs_fspass.h"
#include "Shaders/dxil/vs_fspass.h"
#include "Shaders/metal/vs_fspass.h"
#include "Shaders/glsl/vs_fspass.h"
#include "Shaders/essl/vs_fspass.h"
#include "Shaders/spirv/vs_fspass.h"
#include "Shaders/dxbc/fs_canvas_unpremultiply.h"
#include "Shaders/dxil/fs_canvas_unpremultiply.h"
#include "Shaders/metal/fs_canvas_unpremultiply.h"
#include "Shaders/glsl/fs_canvas_unpremultiply.h"
#include "Shaders/essl/fs_canvas_unpremultiply.h"
#include "Shaders/spirv/fs_canvas_unpremultiply.h"

namespace
{
    constexpr auto JS_CANVAS_NAME = "_CanvasImpl";
    constexpr uint8_t FRAME_BUFFER_TEXTURE_COUNT = 2;
    constexpr uint8_t COLOR_ATTACHMENT_INDEX = 0;
    constexpr uint8_t DEPTH_STENCIL_ATTACHMENT_INDEX = 1;
    constexpr uint32_t FULLSCREEN_TRIANGLE_VERTEX_COUNT = 3;
    constexpr uint8_t POSITION_COMPONENT_COUNT = 3;
    constexpr uint8_t TEXCOORD_COMPONENT_COUNT = 2;
    constexpr size_t FULLSCREEN_VERTEX_COMPONENT_COUNT = POSITION_COMPONENT_COUNT + TEXCOORD_COMPONENT_COUNT;
    constexpr size_t COPY_PARAMETER_COUNT = 4;
    const bgfx::EmbeddedShader CanvasCopyShaders[]{
        BGFX_EMBEDDED_SHADER(vs_fspass),
        BGFX_EMBEDDED_SHADER(fs_canvas_unpremultiply),
        BGFX_EMBEDDED_SHADER_END()};
}

namespace Babylon::Polyfills::Internal
{
    static constexpr auto JS_CONSTRUCTOR_NAME = "Canvas";

    void NativeCanvas::Initialize(Napi::Env env)
    {
        Napi::HandleScope scope{env};

        Napi::Function func = DefineClass(
            env,
            JS_CONSTRUCTOR_NAME,
            {
                StaticMethod("loadTTF", &NativeCanvas::LoadTTF),
                StaticMethod("loadTTFAsync", &NativeCanvas::LoadTTFAsync),
                InstanceAccessor("width", &NativeCanvas::GetWidth, &NativeCanvas::SetWidth),
                InstanceAccessor("height", &NativeCanvas::GetHeight, &NativeCanvas::SetHeight),
                InstanceMethod("getContext", &NativeCanvas::GetContext),
                InstanceMethod("getCanvasTexture", &NativeCanvas::GetCanvasTexture),
                InstanceMethod("toDataURL", &NativeCanvas::ToDataURL),
                InstanceMethod("dispose", &NativeCanvas::Dispose),
                InstanceMethod("remove", &NativeCanvas::Remove),
                StaticMethod("parseColor", &NativeCanvas::ParseColor)});

        JsRuntime::NativeObject::GetFromJavaScript(env).Set(JS_CONSTRUCTOR_NAME, func);
    }

    NativeCanvas* NativeCanvas::TryUnwrap(Napi::Env env, const Napi::Value& value)
    {
        return NativeInstanceRegistry<NativeCanvas>::TryUnwrap(env, value);
    }

    NativeCanvas::NativeCanvas(const Napi::CallbackInfo& info)
        : Napi::ObjectWrap<NativeCanvas>{info}
        , m_graphicsContext{Graphics::DeviceContext::GetFromJavaScript(info.Env())}
        , Polyfills::Canvas::Impl::MonitoredResource{Polyfills::Canvas::Impl::GetFromJavaScript(info.Env())}
    {
        NativeInstanceRegistry<NativeCanvas>::Add(info, this);
    }

    NativeCanvas::~NativeCanvas()
    {
        NativeInstanceRegistry<NativeCanvas>::Remove(this);

        // Canvas and Context form a JS cycle; finalizer order is not guaranteed.
        // Clear the reverse pointer first so Context::~Context cannot touch us.
        if (m_context != nullptr)
        {
            m_context->DetachCanvas();
            m_context = nullptr;
        }
        Dispose();
    }

    void NativeCanvas::FlushGraphicResources()
    {
        Dispose();
    }

    void NativeCanvas::Remove(const Napi::CallbackInfo&)
    {
    }

    void NativeCanvas::LoadTTF(const Napi::CallbackInfo& info)
    {
        LoadTTFCore(info, "Canvas.loadTTF");
    }

    void NativeCanvas::LoadTTFCore(const Napi::CallbackInfo& info, const char* methodName)
    {
        if (info.Length() < 1 || !info[0].IsString())
        {
            throw Napi::TypeError::New(info.Env(), std::string{methodName} + " expects the font name as a string in argument 1.");
        }

        // NanoVG retains the original font buffer.
        auto fontName = info[0].As<Napi::String>().Utf8Value();
        if (fontsInfos.find(fontName) == fontsInfos.end())
        {
            fontsInfos[fontName] = GetFontDataArgument(info, 1, methodName);
        }
    }

    // @deprecated: LoadTTFAsync is always synchronous, use LoadTTF instead
    Napi::Value NativeCanvas::LoadTTFAsync(const Napi::CallbackInfo& info)
    {
        LoadTTFCore(info, "Canvas.loadTTFAsync");

        auto deferred{Napi::Promise::Deferred::New(info.Env())};
        deferred.Resolve(info.Env().Undefined());
        return deferred.Promise();
    }

    Napi::Value NativeCanvas::GetContext(const Napi::CallbackInfo& info)
    {
        auto thisObj = info.This().ToObject();
        const auto contextPropertyName = Napi::Value::From(Env(), "_context");

        auto context = thisObj.Get(contextPropertyName);
        if (context.IsUndefined())
        {
            context = Context::CreateInstance(info.Env(), info.This());
            thisObj.Set(contextPropertyName, context);
            if (context.IsObject())
            {
                m_context = Context::Unwrap(context.As<Napi::Object>());
            }
        }

        return context;
    }

    Napi::Value NativeCanvas::GetWidth(const Napi::CallbackInfo&)
    {
        return Napi::Value::From(Env(), m_width);
    }

    void NativeCanvas::SetWidth(const Napi::CallbackInfo&, const Napi::Value& value)
    {
        SetDimension(m_width, value);
    }

    Napi::Value NativeCanvas::GetHeight(const Napi::CallbackInfo&)
    {
        return Napi::Value::From(Env(), m_height);
    }

    void NativeCanvas::SetHeight(const Napi::CallbackInfo&, const Napi::Value& value)
    {
        SetDimension(m_height, value);
    }

    void NativeCanvas::SetDimension(uint16_t& dimension, const Napi::Value& value)
    {
        const auto newDimension = static_cast<uint16_t>(value.As<Napi::Number>().Uint32Value());
        if (!newDimension)
        {
            return;
        }

        if (newDimension == dimension)
        {
            m_clear = true;
        }
        else
        {
            dimension = newDimension;
            m_dirty = true;
        }
    }

    bool NativeCanvas::UpdateRenderTarget()
    {
        bool needClear = m_clear;
        m_clear = false;

        if (m_dirty || !m_frameBuffer)
        {
            // WebGL requires new render targets to be initialized to zero.
            const bgfx::Memory* mem = bgfx::alloc(static_cast<uint32_t>(m_width) * static_cast<uint32_t>(m_height) * CanvasConstants::RGBA_BYTES_PER_PIXEL);
            std::memset(mem->data, 0, mem->size);

            std::array<bgfx::TextureHandle, FRAME_BUFFER_TEXTURE_COUNT> textures{
                bgfx::createTexture2D(m_width, m_height, false, 1, bgfx::TextureFormat::RGBA8, BGFX_TEXTURE_RT, mem),
                bgfx::createTexture2D(m_width, m_height, false, 1, bgfx::TextureFormat::D24S8, BGFX_TEXTURE_RT)};

            // Only request mip generation for formats that support it; depth never does.
            const bgfx::Caps* caps = bgfx::getCaps();
            const uint8_t colorResolve = 0 != (caps->formats[bgfx::TextureFormat::RGBA8] & BGFX_CAPS_FORMAT_TEXTURE_MIP_AUTOGEN)
                ? BGFX_ATTACHMENT_AUTO_GEN_MIPS
                : BGFX_ATTACHMENT_NONE;

            std::array<bgfx::Attachment, textures.size()> attachments{};
            attachments[COLOR_ATTACHMENT_INDEX].init(textures[COLOR_ATTACHMENT_INDEX], bgfx::Access::Write, 0, 1, 0, colorResolve);
            attachments[DEPTH_STENCIL_ATTACHMENT_INDEX].init(textures[DEPTH_STENCIL_ATTACHMENT_INDEX], bgfx::Access::Write, 0, 1, 0, BGFX_ATTACHMENT_NONE);
            auto handle = bgfx::createFrameBuffer(static_cast<uint8_t>(attachments.size()), attachments.data(), true);
            if (!bgfx::isValid(handle))
            {
                // Failed texture creation returns an invalid handle that cannot be destroyed.
                for (auto texture : textures)
                {
                    if (bgfx::isValid(texture))
                    {
                        bgfx::destroy(texture);
                    }
                }
                throw std::runtime_error{"bgfx::createFrameBuffer returned invalid handle (framebuffer pool exhausted; raise BGFX_CONFIG_MAX_FRAME_BUFFERS or audit Canvas/Context lifetime)"};
            }
            m_frameBuffer = std::make_unique<Graphics::FrameBuffer>(
                m_graphicsContext, handle, m_width, m_height, false, false, false,
                CanvasConstants::NO_DEPTH_STENCIL_ATTACHMENT, false);
            m_dirty = false;

            if (m_texture)
            {
                m_texture.reset();
            }
            for (auto& converted : m_convertedTextures)
            {
                converted.Texture.reset();
                converted.FrameBuffer.reset();
            }

            m_frameBufferPool.Clear();
            m_frameBufferPool.SetDimensions(m_width, m_height);
            m_frameBufferPool.SetGraphicsContext(&m_graphicsContext);

            return true;
        }

        return needClear;
    }

    Napi::Value NativeCanvas::GetCanvasTexture(const Napi::CallbackInfo& info)
    {
        if (!m_frameBuffer)
        {
            throw Napi::Error::New(info.Env(), "Canvas.getCanvasTexture requires a flushed context.");
        }
        // Omitted arguments preserve the raw, premultiplied texture used by older engines.
        const bool premulAlpha = info.Length() == 0 || info[0].IsUndefined() || info[0].As<Napi::Boolean>().Value();
        const bool generateMipMaps = info.Length() > 1 && !info[1].IsUndefined() && info[1].As<Napi::Boolean>().Value();
        if (!premulAlpha || generateMipMaps)
        {
            try
            {
                return Napi::Pointer<Graphics::Texture>::Create(info.Env(), &GetConvertedTexture(premulAlpha, generateMipMaps));
            }
            catch (const std::exception& ex)
            {
                throw Napi::Error::New(info.Env(), ex.what());
            }
        }

        if (!m_texture)
        {
            m_texture = std::make_unique<Graphics::Texture>(m_graphicsContext);
        }

        m_texture->Attach(bgfx::getTexture(m_frameBuffer->Handle()), /*ownsHandle*/ false, m_width, m_height, false, 1, bgfx::TextureFormat::RGBA8, BGFX_TEXTURE_RT);
        // Hand the blit view id reserved during the preceding Context::Flush to the texture so
        // NativeEngine::CopyTexture blits on a view ordered before the consuming layer.
        m_texture->BlitViewId(m_blitViewId, m_blitViewIdGeneration);
        return Napi::Pointer<Graphics::Texture>::Create(info.Env(), m_texture.get());
    }

    Graphics::Texture& NativeCanvas::GetConvertedTexture(bool premulAlpha, bool generateMipMaps)
    {
        auto scope = m_graphicsContext.AcquireFrameCompletionScope();
        m_graphicsContext.FlushViewsIfNeeded();
        auto* encoder = m_graphicsContext.GetActiveEncoder();
        if (encoder == nullptr)
        {
            throw std::runtime_error{"Canvas alpha conversion requires an active graphics encoder."};
        }

        if (!bgfx::isValid(m_unpremultiplyProgram))
        {
            const auto renderer = bgfx::getRendererType();
            const auto vertex = bgfx::createEmbeddedShader(CanvasCopyShaders, renderer, "vs_fspass");
            const auto fragment = bgfx::createEmbeddedShader(CanvasCopyShaders, renderer, "fs_canvas_unpremultiply");
            if (!bgfx::isValid(vertex) || !bgfx::isValid(fragment))
            {
                if (bgfx::isValid(vertex))
                {
                    bgfx::destroy(vertex);
                }
                if (bgfx::isValid(fragment))
                {
                    bgfx::destroy(fragment);
                }
                throw std::runtime_error{"Cannot create Canvas alpha conversion shaders."};
            }
            m_unpremultiplyProgram = bgfx::createProgram(vertex, fragment, true);
            if (!bgfx::isValid(m_unpremultiplyProgram))
            {
                throw std::runtime_error{"Cannot link Canvas alpha conversion shaders."};
            }
        }
        if (!bgfx::isValid(m_copySampler))
        {
            m_copySampler = bgfx::createUniform("s_tex", bgfx::UniformType::Sampler);
        }
        if (!bgfx::isValid(m_copyParams))
        {
            m_copyParams = bgfx::createUniform("u_canvasCopy", bgfx::UniformType::Vec4);
        }
        if (!bgfx::isValid(m_copySampler) || !bgfx::isValid(m_copyParams))
        {
            throw std::runtime_error{"Cannot allocate Canvas alpha conversion uniforms."};
        }
        if (generateMipMaps && !(bgfx::getCaps()->formats[bgfx::TextureFormat::RGBA8] & BGFX_CAPS_FORMAT_TEXTURE_MIP_AUTOGEN))
        {
            throw std::runtime_error{"Canvas mipmaps require RGBA8 automatic mip generation."};
        }
        // Keep representations separate: queued copies retain these Texture pointers.
        const auto kind = generateMipMaps
            ? (premulAlpha ? ConvertedTextureKind::PremultipliedAlphaMipMaps : ConvertedTextureKind::StraightAlphaMipMaps)
            : ConvertedTextureKind::NoMipMaps;
        auto& converted = m_convertedTextures[static_cast<size_t>(kind)];
        if (!converted.FrameBuffer)
        {
            const auto color = bgfx::createTexture2D(m_width, m_height, generateMipMaps, 1, bgfx::TextureFormat::RGBA8, BGFX_TEXTURE_RT);
            if (!bgfx::isValid(color))
            {
                throw std::runtime_error{"Cannot allocate Canvas straight-alpha texture."};
            }
            bgfx::Attachment attachment{};
            attachment.init(color, bgfx::Access::Write, 0, 1, 0, generateMipMaps ? BGFX_ATTACHMENT_AUTO_GEN_MIPS : BGFX_ATTACHMENT_NONE);
            const auto frameBuffer = bgfx::createFrameBuffer(1, &attachment, true);
            if (!bgfx::isValid(frameBuffer))
            {
                bgfx::destroy(color);
                throw std::runtime_error{"Cannot allocate Canvas straight-alpha framebuffer."};
            }
            converted.FrameBuffer = std::make_unique<Graphics::FrameBuffer>(
                m_graphicsContext, frameBuffer, m_width, m_height, false, false, false,
                CanvasConstants::NO_DEPTH_STENCIL_ATTACHMENT, false);
        }
        if (!converted.Texture)
        {
            converted.Texture = std::make_unique<Graphics::Texture>(m_graphicsContext);
            converted.Texture->Attach(bgfx::getTexture(converted.FrameBuffer->Handle()), false,
                m_width, m_height, generateMipMaps, 1, bgfx::TextureFormat::RGBA8, BGFX_TEXTURE_RT);
        }

        bgfx::VertexLayout layout{};
        layout.begin().add(bgfx::Attrib::Position, POSITION_COMPONENT_COUNT, bgfx::AttribType::Float)
            .add(bgfx::Attrib::TexCoord0, TEXCOORD_COMPONENT_COUNT, bgfx::AttribType::Float).end();
        if (bgfx::getAvailTransientVertexBuffer(FULLSCREEN_TRIANGLE_VERTEX_COUNT, layout) != FULLSCREEN_TRIANGLE_VERTEX_COUNT)
        {
            throw std::runtime_error{"Cannot allocate Canvas alpha conversion vertices."};
        }
        bgfx::TransientVertexBuffer buffer{};
        bgfx::allocTransientVertexBuffer(&buffer, FULLSCREEN_TRIANGLE_VERTEX_COUNT, layout);
        const std::array<std::array<float, FULLSCREEN_VERTEX_COMPONENT_COUNT>, FULLSCREEN_TRIANGLE_VERTEX_COUNT> vertices{{
            {-1.f, 0.f, 0.f, 0.f, 0.f}, {1.f, 0.f, 0.f, 1.f, 0.f}, {1.f, 2.f, 0.f, 1.f, 1.f}}};
        std::memcpy(buffer.data, vertices.data(), sizeof(vertices));

        // Use the canvas reservation for conversion, then reserve the following blit
        // before NativeEngine records the scene that samples the dynamic texture.
        const auto conversionView = !m_blitViewUsedForConversion && m_blitViewId != UINT16_MAX && m_blitViewIdGeneration == m_graphicsContext.ViewIdGeneration()
            ? m_blitViewId : m_graphicsContext.AcquireNewViewId();
        m_blitViewUsedForConversion = true;
        const auto blitView = m_graphicsContext.AcquireNewViewId();
        bgfx::resetView(conversionView);
        bgfx::setViewFrameBuffer(conversionView, converted.FrameBuffer->Handle());
        bgfx::setViewRect(conversionView, 0, 0, m_width, m_height);
        bgfx::setViewMode(conversionView, bgfx::ViewMode::Sequential);
        const std::array<float, COPY_PARAMETER_COUNT> parameters{bgfx::getCaps()->originBottomLeft ? 1.f : 0.f, premulAlpha ? 1.f : 0.f, 0.f, 0.f};
        encoder->discard(BGFX_DISCARD_ALL);
        encoder->setVertexBuffer(0, &buffer);
        encoder->setTexture(0, m_copySampler, bgfx::getTexture(m_frameBuffer->Handle()),
            BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT | BGFX_SAMPLER_MIP_POINT);
        encoder->setUniform(m_copyParams, parameters.data());
        encoder->setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A);
        encoder->submit(conversionView, m_unpremultiplyProgram);
        if (generateMipMaps)
        {
            // A blit-only view need not switch render targets. Touch it so the
            // conversion target resolves/generates its mip chain before the copy.
            bgfx::resetView(blitView);
            bgfx::setViewRect(blitView, 0, 0, m_width, m_height);
            encoder->touch(blitView);
        }
        converted.Texture->BlitViewId(blitView, m_graphicsContext.ViewIdGeneration());
        return *converted.Texture;
    }

    Napi::Value NativeCanvas::ToDataURL(const Napi::CallbackInfo& info)
    {
        // PNG is also the Canvas fallback for empty or unsupported media types.
        const uint32_t width = m_width;
        const uint32_t height = m_height;
        std::vector<uint8_t> rgba(static_cast<size_t>(width) * height * CanvasConstants::RGBA_CHANNEL_COUNT, 0);
        if (m_context != nullptr && width > 0 && height > 0)
        {
            try
            {
                rgba = m_context->CaptureRGBA();
            }
            catch (const std::exception& ex)
            {
                throw Napi::Error::New(info.Env(), ex.what());
            }
        }

        bx::MemoryBlock memoryBlock{&Graphics::DeviceContext::GetDefaultAllocator()};
        bx::MemoryWriter writer{&memoryBlock};
        bx::Error err{};
        bimg::imageWritePng(&writer, width, height, width * CanvasConstants::RGBA_CHANNEL_COUNT, rgba.data(), bimg::TextureFormat::RGBA8, false, &err);
        // MemoryBlock reports allocation capacity, not the number of encoded bytes.
        const size_t pngSize = static_cast<size_t>(bx::getSize(&writer));
        if (!err.isOk() || pngSize == 0)
        {
            throw Napi::Error::New(info.Env(), "Canvas.toDataURL: PNG encode failed.");
        }

        // more(0) returns the buffer start without growing (see bx::MemoryBlock).
        const char* pngBytes = static_cast<const char*>(memoryBlock.more(0));
        std::string encoded;
        bn::encode_b64(pngBytes, pngBytes + pngSize, std::back_inserter(encoded));
        return Napi::String::New(info.Env(), "data:image/png;base64," + encoded);
    }

    Napi::Value NativeCanvas::ParseColor(const Napi::CallbackInfo& info)
    {
        const auto colorString = info[0].As<Napi::String>().Utf8Value();
        const auto color = StringToColor(info.Env(), colorString);

        const auto channel = [](float value) {
            return uint32_t(value * CanvasConstants::COLOR_CHANNEL_MAX) & CanvasConstants::COLOR_CHANNEL_MASK;
        };
        return Napi::Value::From(info.Env(),
            (channel(color.a) << CanvasConstants::ALPHA_CHANNEL_SHIFT) +
            (channel(color.b) << CanvasConstants::BLUE_CHANNEL_SHIFT) +
            (channel(color.g) << CanvasConstants::GREEN_CHANNEL_SHIFT) +
            channel(color.r));
    }

    void NativeCanvas::Dispose()
    {
        for (auto& converted : m_convertedTextures)
        {
            converted.Texture.reset();
            converted.FrameBuffer.reset();
        }
        if (bgfx::isValid(m_unpremultiplyProgram))
        {
            bgfx::destroy(m_unpremultiplyProgram);
            m_unpremultiplyProgram = BGFX_INVALID_HANDLE;
        }
        if (bgfx::isValid(m_copySampler))
        {
            bgfx::destroy(m_copySampler);
            m_copySampler = BGFX_INVALID_HANDLE;
        }
        if (bgfx::isValid(m_copyParams))
        {
            bgfx::destroy(m_copyParams);
            m_copyParams = BGFX_INVALID_HANDLE;
        }
        m_frameBuffer.reset();
        m_texture.reset();
        m_frameBufferPool.Clear();
    }

    void NativeCanvas::Dispose(const Napi::CallbackInfo& /*info*/)
    {
        Dispose();
    }
}

namespace Babylon::Polyfills
{
    struct Canvas::Impl::JavaScriptData
    {
        Canvas::Impl& Owner;
        Napi::FunctionReference WeakSetConstructor;
        Napi::FunctionReference WeakSetAdd;
        Napi::FunctionReference WeakSetHas;
    };

    Canvas::Impl::Impl(Napi::Env env)
        : m_env{env}
    {
        AddToJavaScript(env);
    }

    Canvas::Impl::WeakIdentity Canvas::Impl::CreateWeakIdentity(const Napi::Object& value)
    {
        const auto& data = GetJavaScriptData(m_env);
        const auto receivers = data.WeakSetConstructor.New({});
        data.WeakSetAdd.Call(receivers, {value});
        return {Napi::Persistent(receivers), Napi::Persistent(data.WeakSetHas.Value())};
    }

    void Canvas::Impl::AddToJavaScript(Napi::Env env)
    {
        const auto constructor = env.Global().Get("WeakSet").As<Napi::Function>();
        const auto prototype = constructor.Get("prototype").As<Napi::Object>();
        // The host Canvas may outlive its environment; finalize N-API references with JavaScript.
        auto data = std::make_unique<JavaScriptData>(JavaScriptData{
            *this,
            Napi::Persistent(constructor),
            Napi::Persistent(prototype.Get("add").As<Napi::Function>()),
            Napi::Persistent(prototype.Get("has").As<Napi::Function>())});
        const auto external = Napi::External<JavaScriptData>::New(env, data.get(), [](Napi::Env, JavaScriptData* data) {
            delete data;
        });
        data.release();
        JsRuntime::NativeObject::GetFromJavaScript(env).Set(JS_CANVAS_NAME, external);
    }

    Canvas::Impl::JavaScriptData& Canvas::Impl::GetJavaScriptData(Napi::Env env)
    {
        return *JsRuntime::NativeObject::GetFromJavaScript(env)
                    .Get(JS_CANVAS_NAME)
                    .As<Napi::External<JavaScriptData>>()
                    .Data();
    }

    Canvas::Impl& Canvas::Impl::GetFromJavaScript(Napi::Env env)
    {
        return GetJavaScriptData(env).Owner;
    }

    void Canvas::Impl::AddMonitoredResource(MonitoredResource* monitoredResource)
    {
        if (std::find(m_monitoredResources.begin(), m_monitoredResources.end(), monitoredResource) == m_monitoredResources.end())
        {
            m_monitoredResources.push_back(monitoredResource);
        }
    }

    void Canvas::Impl::RemoveMonitoredResource(MonitoredResource* monitoredResource)
    {
        if (m_monitoredResources.empty())
        {
            return;
        }
        auto iter = std::find(m_monitoredResources.begin(), m_monitoredResources.end(), monitoredResource);
        if (iter != m_monitoredResources.end())
        {
            m_monitoredResources.erase(iter);
        }
    }

    void Canvas::Impl::FlushGraphicResources()
    {
        for (auto monitoredResource : m_monitoredResources)
        {
            monitoredResource->FlushGraphicResources();
        }
    }

    Canvas::Canvas(std::shared_ptr<Impl> impl)
        : m_impl{std::move(impl)}
    {
    }

    Canvas::~Canvas()
    {
    }

    Canvas BABYLON_API Canvas::Initialize(Napi::Env env)
    {
        auto impl{std::make_shared<Canvas::Impl>(env)};

        Internal::NativeCanvas::Initialize(env);
        Internal::NativeCanvasImage::Initialize(env);
        Internal::NativeCanvasPath2D::Initialize(env);
        Internal::CanvasGradient::Initialize(env);
        Internal::Context::Initialize(env);

        return {impl};
    }

    void Canvas::FlushGraphicResources()
    {
        m_impl->FlushGraphicResources();
    }
}
