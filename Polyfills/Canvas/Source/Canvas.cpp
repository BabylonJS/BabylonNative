#include "Canvas.h"
#include "Image.h"
#include "Path2D.h"
#include "Context.h"
#include <bgfx/bgfx.h>
#include <napi/pointer.h>
#include <cassert>
#include "Colors.h"
#include "Gradient.h"

const int POOL_SIZE = 3;

namespace
{
    constexpr auto JS_CANVAS_NAME = "_CanvasImpl";
}

namespace Babylon::Polyfills::Internal
{
    static constexpr auto JS_CONSTRUCTOR_NAME = "Canvas";

    void NativeCanvas::CreateInstance(Napi::Env env)
    {
        Napi::HandleScope scope{env};

        Napi::Function func = DefineClass(
            env,
            JS_CONSTRUCTOR_NAME,
            {StaticMethod("loadTTFAsync", &NativeCanvas::LoadTTFAsync),
                InstanceAccessor("width", &NativeCanvas::GetWidth, &NativeCanvas::SetWidth),
                InstanceAccessor("height", &NativeCanvas::GetHeight, &NativeCanvas::SetHeight),
                InstanceMethod("getContext", &NativeCanvas::GetContext),
                InstanceMethod("getCanvasTexture", &NativeCanvas::GetCanvasTexture),
                InstanceMethod("dispose", &NativeCanvas::Dispose),
                InstanceMethod("remove", &NativeCanvas::Remove),
                StaticMethod("parseColor", &NativeCanvas::ParseColor)});

        JsRuntime::NativeObject::GetFromJavaScript(env).Set(JS_CONSTRUCTOR_NAME, func);
    }

    NativeCanvas::NativeCanvas(const Napi::CallbackInfo& info)
        : Napi::ObjectWrap<NativeCanvas>{info}
        , m_graphicsContext{Graphics::DeviceContext::GetFromJavaScript(info.Env())}
        , Polyfills::Canvas::Impl::MonitoredResource{Polyfills::Canvas::Impl::GetFromJavaScript(info.Env())}
    {
    }

    NativeCanvas::~NativeCanvas()
    {
        Dispose();
    }

    void NativeCanvas::FlushGraphicResources()
    {
        Dispose();
    }

    void NativeCanvas::Remove(const Napi::CallbackInfo&)
    {
        // called when removed from document which has no meaning for Native
    }

    Napi::Value NativeCanvas::LoadTTFAsync(const Napi::CallbackInfo& info)
    {
        const auto buffer = info[1].As<Napi::ArrayBuffer>();
        std::vector<uint8_t> fontBuffer(buffer.ByteLength());
        memcpy(fontBuffer.data(), (uint8_t*)buffer.Data(), buffer.ByteLength());

        auto& graphicsContext{Graphics::DeviceContext::GetFromJavaScript(info.Env())};
        auto update = graphicsContext.GetUpdate("update");
        std::shared_ptr<JsRuntimeScheduler> runtimeScheduler{std::make_shared<JsRuntimeScheduler>(JsRuntime::GetFromJavaScript(info.Env()))};
        auto deferred{Napi::Promise::Deferred::New(info.Env())};
        arcana::make_task(update.Scheduler(), arcana::cancellation::none(), [fontName{info[0].As<Napi::String>().Utf8Value()}, fontData{std::move(fontBuffer)}]() {
            fontsInfos[fontName] = fontData;
        }).then(*runtimeScheduler, arcana::cancellation::none(), [runtimeScheduler /*Keep reference alive*/, env{info.Env()}, deferred]() {
            deferred.Resolve(env.Undefined());
        });

        return deferred.Promise();
    }

    Napi::Value NativeCanvas::GetContext(const Napi::CallbackInfo& info)
    {
        return Context::CreateInstance(info.Env(), this);
    }

    Napi::Value NativeCanvas::GetWidth(const Napi::CallbackInfo&)
    {
        return Napi::Value::From(Env(), m_width);
    }

    void NativeCanvas::SetWidth(const Napi::CallbackInfo&, const Napi::Value& value)
    {
        auto width = static_cast<uint16_t>(value.As<Napi::Number>().Uint32Value());
        if (width != m_width && width)
        {
            m_width = width;
            m_dirty = true;
        }
    }

    Napi::Value NativeCanvas::GetHeight(const Napi::CallbackInfo&)
    {
        return Napi::Value::From(Env(), m_height);
    }

    void NativeCanvas::SetHeight(const Napi::CallbackInfo&, const Napi::Value& value)
    {
        auto height = value.As<Napi::Number>().Uint32Value();
        if (height != m_height && height)
        {
            m_height = height;
            m_dirty = true;
        }
    }

    void NativeCanvas::PoolInit(int size)
    {
        for (int i = 0; i < size; ++i)
        {

            bgfx::FrameBufferHandle TextBuffer{ bgfx::kInvalidHandle };
            Graphics::FrameBuffer* FrameBuffer;

            int width(256), height(256);
            std::array<bgfx::TextureHandle, 2> textures{
            bgfx::createTexture2D(width, height, false, 1, bgfx::TextureFormat::RGBA8, BGFX_TEXTURE_RT),
            bgfx::createTexture2D(width, height, false, 1, bgfx::TextureFormat::D24S8, BGFX_TEXTURE_RT) };

            std::array<bgfx::Attachment, textures.size()> attachments{};
            for (size_t idx = 0; idx < attachments.size(); ++idx)
            {
                attachments[idx].init(textures[idx]);
            }
            TextBuffer = bgfx::createFrameBuffer(static_cast<uint8_t>(attachments.size()), attachments.data(), true);

            FrameBuffer = new Graphics::FrameBuffer(m_graphicsContext, TextBuffer, width, height, false, false, false);
            // TODO: confirm why m_texture is reset, esp for pool framebuffers
            if (m_texture)
            {
                m_texture.reset();
            }
            mPoolBuffers.push_back({FrameBuffer, true});
        }
    }

    void NativeCanvas::PoolClear()
    {
        for (auto& buffer : mPoolBuffers)
        {
            if (buffer.frameBuffer)
            {
                // TODO: Dispose?
                //buffer.frameBuffer->Dispose();
                //free(buffer.frameBuffer);

                delete buffer.frameBuffer;
                buffer.frameBuffer = nullptr;
            }
        }
        mPoolBuffers.clear();
    }

    Graphics::FrameBuffer* NativeCanvas::PoolAcquire()
    {
        for (auto& buffer : mPoolBuffers)
        {
            if (buffer.isAvailable)
            {
                buffer.isAvailable = false;
                return buffer.frameBuffer;
            }
        }
        // TODO: If no buffers available, pool wasn't large enough. Should it automatically resize?
        throw std::runtime_error{"No available frame buffer in pool"};
    }

    void NativeCanvas::PoolRelease(Graphics::FrameBuffer* frameBuffer)
    {
        for (auto& buffer : mPoolBuffers)
        {
            if (buffer.frameBuffer == frameBuffer)
            {
                // TODO: clear framebuffer?
                buffer.isAvailable = true;
                return;
            }
        }
    }

    // NOTE: This only runs when canvas size changes (inc. init?). It'll trigger re-creation of framebuffers
    bool NativeCanvas::UpdateRenderTarget()
    {
        if (m_dirty)
        {
            // TODO: Can delete this whole block if we decide to use pool for primary framebuffer
            {
                std::array<bgfx::TextureHandle, 2> textures{
                   bgfx::createTexture2D(m_width, m_height, false, 1, bgfx::TextureFormat::RGBA8, BGFX_TEXTURE_RT),
                   bgfx::createTexture2D(m_width, m_height, false, 1, bgfx::TextureFormat::D24S8, BGFX_TEXTURE_RT) };

                std::array<bgfx::Attachment, textures.size()> attachments{};
                for (size_t idx = 0; idx < attachments.size(); ++idx)
                {
                    attachments[idx].init(textures[idx]);
                }
                auto handle = bgfx::createFrameBuffer(static_cast<uint8_t>(attachments.size()), attachments.data(), true);
                assert(handle.idx != bgfx::kInvalidHandle);
                m_frameBuffer = std::make_unique<Graphics::FrameBuffer>(m_graphicsContext, handle, m_width, m_height, false, false, false);
                m_dirty = false;

                if (m_texture)
                {
                    m_texture.reset();
                }
            }
            {
                PoolClear();
                PoolInit(POOL_SIZE);
            }
            return true;
        }
        return false;
    }

    Napi::Value NativeCanvas::GetCanvasTexture(const Napi::CallbackInfo& info)
    {
        if (!m_texture)
        {
            m_texture = std::make_unique<Graphics::Texture>(m_graphicsContext);
        }

        m_texture->Attach(bgfx::getTexture(m_frameBuffer->Handle()), false, m_width, m_height, false, 1, bgfx::TextureFormat::RGBA8, BGFX_TEXTURE_RT);
        return Napi::Pointer<Graphics::Texture>::Create(info.Env(), m_texture.get());
    }

    Napi::Value NativeCanvas::ParseColor(const Napi::CallbackInfo& info)
    {
        const auto colorString = info[0].As<Napi::String>().Utf8Value();
        const auto color = StringToColor(info.Env(), colorString);

        return Napi::Value::From(info.Env(), ((uint32_t(color.a * 255.f) & 0xFF) << 24) + ((uint32_t(color.b * 255.f) & 0xFF) << 16) + ((uint32_t(color.g * 255.f) & 0xFF) << 8) + (uint32_t(color.r * 255.f) & 0xFF));
    }

    void NativeCanvas::Dispose()
    {
        m_frameBuffer.reset();
        m_texture.reset();
        PoolClear(); // TODO: confirm we want to do this?
    }

    void NativeCanvas::Dispose(const Napi::CallbackInfo& /*info*/)
    {
        Dispose();
    }
}

namespace Babylon::Polyfills
{
    Canvas::Impl::Impl(Napi::Env env)
        : m_env{env}
    {
        AddToJavaScript(env);
    }

    void Canvas::Impl::AddToJavaScript(Napi::Env env)
    {
        JsRuntime::NativeObject::GetFromJavaScript(env)
            .Set(JS_CANVAS_NAME, Napi::External<Canvas::Impl>::New(env, this));
    }

    Canvas::Impl& Canvas::Impl::GetFromJavaScript(Napi::Env env)
    {
        return *JsRuntime::NativeObject::GetFromJavaScript(env)
                    .Get(JS_CANVAS_NAME)
                    .As<Napi::External<Canvas::Impl>>()
                    .Data();
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

        Internal::NativeCanvas::CreateInstance(env);
        Internal::NativeCanvasImage::CreateInstance(env);
        Internal::NativeCanvasPath2D::CreateInstance(env);
        Internal::CanvasGradient::Initialize(env);
        Internal::Context::Initialize(env);

        return {impl};
    }

    void Canvas::FlushGraphicResources()
    {
        m_impl->FlushGraphicResources();
    }
}
