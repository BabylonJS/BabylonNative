#include "NativeCapture.h"

#include <arcana/containers/ticketed_collection.h>

#include <Babylon/JsRuntime.h>
#include <NativeEngine.h>
#include <GraphicsImpl.h>
#include <FrameBuffer.h>

#include <vector>

namespace
{
    class FrameProvider
    {
    public:
        // TODO: Maybe pass a cancellation token to the constructor and remove stop?
        virtual void Stop() = 0;
        virtual ~FrameProvider()
        {
        }
    };

    using FrameCallback = std::function<void(uint32_t width, uint32_t height, uint32_t pitch, bgfx::TextureFormat::Enum format, bool yFlip, gsl::span<const uint8_t> data)>;

    class DefaultBufferFrameProvider : public FrameProvider, std::enable_shared_from_this<DefaultBufferFrameProvider>
    {
    public:
        DefaultBufferFrameProvider(Babylon::Graphics::Impl& graphicsImpl, FrameCallback callback)
        {
            m_ticket = std::make_unique<Babylon::Graphics::Impl::CaptureCallbackTicketT>(graphicsImpl.AddCaptureCallback([thisRef{shared_from_this()}, callback{std::move(callback)}](auto& data) {
                // TODO: Store this in the base class, and have a base class function that is responsible for allocating the temp buffer and dispatching to the JS thread before calling the callback
                callback(data.Width, data.Height, data.Pitch, data.Format, data.YFlip, {static_cast<const uint8_t*>(data.Data), static_cast<std::ptrdiff_t>(data.DataSize)});
            }));
        }

        void Stop() override
        {
            m_ticket.reset();
        }

    private:
        std::unique_ptr<Babylon::Graphics::Impl::CaptureCallbackTicketT> m_ticket{};
    };

    class OffScreenBufferFrameProvider : public FrameProvider, std::enable_shared_from_this<OffScreenBufferFrameProvider>
    {
    public:
        OffScreenBufferFrameProvider(Babylon::Graphics::Impl& graphicsImpl, bgfx::FrameBufferHandle frameBufferHandle, FrameCallback callback)
            : m_graphicsImpl{graphicsImpl}
            , m_frameBufferTextureHandle{bgfx::getTexture(frameBufferHandle)}
            , m_textureInfo{graphicsImpl.GetTextureInfo(m_frameBufferTextureHandle)}
            , m_blitTextureHandle{bgfx::createTexture2D(m_textureInfo.Width, m_textureInfo.Height, m_textureInfo.HasMips, m_textureInfo.NumLayers, m_textureInfo.Format, BGFX_TEXTURE_BLIT_DST | BGFX_TEXTURE_READ_BACK)}
        {
            bgfx::TextureInfo textureInfo{};
            bgfx::calcTextureSize(textureInfo, m_textureInfo.Width, m_textureInfo.Height, 1, false, m_textureInfo.HasMips, m_textureInfo.NumLayers, m_textureInfo.Format);
            m_textureBuffer.resize(textureInfo.storageSize);

            ReadTextureAsync(std::move(callback));
        }

        ~OffScreenBufferFrameProvider()
        {
            bgfx::destroy(m_blitTextureHandle);
        }

        void Stop() override
        {
            m_cancellationToken.cancel();
        }

    private:
        arcana::task<void, std::exception_ptr> ReadTextureAsync(FrameCallback callback)
        {
            return arcana::make_task(m_graphicsImpl.AfterRenderScheduler(), m_cancellationToken, [thisRef{shared_from_this()}, callback{std::move(callback)}]{
                bgfx::blit(bgfx::getCaps()->limits.maxViews - 1, thisRef->m_blitTextureHandle, 0, 0, thisRef->m_frameBufferTextureHandle);
                // todo: arcana::when_all
                thisRef->m_graphicsImpl.ReadTextureAsync(thisRef->m_blitTextureHandle, thisRef->m_textureBuffer).then(arcana::inline_scheduler, thisRef->m_cancellationToken, [thisRef, callback]{
                    callback(thisRef->m_textureInfo.Width, thisRef->m_textureInfo.Height, 0 /*todo*/, thisRef->m_textureInfo.Format, true /*todo*/, thisRef->m_textureBuffer);
                });
                return thisRef->ReadTextureAsync(callback);
            });
        }

    private:
        Babylon::Graphics::Impl& m_graphicsImpl;
        bgfx::TextureHandle m_frameBufferTextureHandle{bgfx::kInvalidHandle};
        Babylon::Graphics::Impl::TextureInfo m_textureInfo{};
        bgfx::TextureHandle m_blitTextureHandle{bgfx::kInvalidHandle};
        std::vector<uint8_t> m_textureBuffer{};
        arcana::cancellation_source m_cancellationToken{};
    };

    std::shared_ptr<FrameProvider> CreateDefaultBufferFrameProvider(Babylon::Graphics::Impl& graphicsImpl, FrameCallback callback)
    {
        return std::shared_ptr<FrameProvider>{std::make_shared<DefaultBufferFrameProvider>(graphicsImpl, callback)};
    }

    std::shared_ptr<FrameProvider> CreateOffScreenBufferFrameProvider(Babylon::Graphics::Impl& graphicsImpl, bgfx::FrameBufferHandle frameBufferHandle, FrameCallback callback)
    {
        return std::shared_ptr<FrameProvider>{std::make_shared<OffScreenBufferFrameProvider>(graphicsImpl, frameBufferHandle, callback)};
    }
}

namespace Babylon::Plugins::Internal
{
    class NativeCapture : public Napi::ObjectWrap<NativeCapture>
    {
    public:
        static constexpr auto JS_CLASS_NAME = "NativeCapture";

        static void Initialize(Napi::Env env)
        {
            Napi::HandleScope scope{env};

            Napi::Function func = NativeCapture::DefineClass(
                env,
                JS_CLASS_NAME,
                {
                    NativeCapture::InstanceMethod("addCallback", &NativeCapture::AddCallback),
                    NativeCapture::InstanceMethod("dispose", &NativeCapture::Dispose),
                });

            env.Global().Set(JS_CLASS_NAME, func);
        }

        NativeCapture(const Napi::CallbackInfo& info)
            : Napi::ObjectWrap<NativeCapture>{info}
            , m_runtime{JsRuntime::GetFromJavaScript(info.Env())}
            , m_jsData{Napi::Persistent(Napi::Object::New(info.Env()))}
        {
            auto& graphicsImpl{Graphics::Impl::GetFromJavaScript(info.Env())};

            Napi::Object jsData = m_jsData.Value();
            jsData.Set("data", Napi::ArrayBuffer::New(info.Env(), 0));

            FrameCallback frameCallback{[this](uint32_t width, uint32_t height, uint32_t pitch, bgfx::TextureFormat::Enum format, bool yFlip, gsl::span<const uint8_t> data){
                CaptureDataReceived(width, height, pitch, format, yFlip, data);
            }};

            if (info.Length() == 0)
            {
                m_frameProvider = CreateDefaultBufferFrameProvider(graphicsImpl, std::move(frameCallback));
            }
            else
            {
                auto& frameBuffer = *info[0].As<Napi::External<FrameBuffer>>().Data();
                m_frameProvider = CreateOffScreenBufferFrameProvider(graphicsImpl, frameBuffer.Handle(), std::move(frameCallback));
            }
        }

        ~NativeCapture()
        {
            if (m_frameProvider != nullptr)
            {
                // If m_frameProvider is still active, this object is being garbage collected without
                // having been disposed, so it must dispose itself.
                Dispose();
            }
        }

    private:
        void AddCallback(const Napi::CallbackInfo& info)
        {
            auto listener = info[0].As<Napi::Function>();
            m_callbacks.push_back(Napi::Persistent(listener));
        }

        void CaptureDataReceived(uint32_t width, uint32_t height, uint32_t pitch, bgfx::TextureFormat::Enum format, bool yFlip, gsl::span<const uint8_t> data)
        {
            std::vector<uint8_t> bytes{};
            bytes.resize(data.size());
            std::memcpy(bytes.data(), data.data(), data.size());
            m_runtime.Dispatch([this, width, height, pitch, format, yFlip, bytes{std::move(bytes)}](Napi::Env env) mutable {
                Napi::Object jsData = m_jsData.Value();
                jsData.Set("width", static_cast<double>(width));
                jsData.Set("height", static_cast<double>(height));
                jsData.Set("pitch", static_cast<double>(pitch));
                constexpr auto FORMAT_MEMBER_NAME = "format";
                switch (format)
                {
                    case bgfx::TextureFormat::BGRA8:
                        jsData.Set(FORMAT_MEMBER_NAME, "BGRA8");
                        break;
                    default:
                        jsData.Set(FORMAT_MEMBER_NAME, env.Undefined());
                        break;
                }
                jsData.Set("yFlip", yFlip);

                auto jsBytes = jsData.Get("data").As<Napi::ArrayBuffer>();
                if (static_cast<size_t>(bytes.size()) != jsBytes.ByteLength())
                {
                    jsBytes = Napi::ArrayBuffer::New(env, bytes.size());
                    jsData.Set("data", jsBytes);
                }
                std::memcpy(jsBytes.Data(), bytes.data(), bytes.size());
                bytes.clear();

                for (const auto& callback : m_callbacks)
                {
                    callback.Call({jsData});
                }
            });
        }

        void Dispose()
        {
            m_frameProvider->Stop();
            m_frameProvider.reset();
            m_callbacks.clear();
        }

        void Dispose(const Napi::CallbackInfo&)
        {
            Dispose();
        }

        JsRuntime& m_runtime;
        std::vector<Napi::FunctionReference> m_callbacks{};
        Napi::ObjectReference m_jsData{};
        std::shared_ptr<FrameProvider> m_frameProvider{};
    };
}

namespace Babylon::Plugins::NativeCapture
{
    void Initialize(Napi::Env env)
    {
        Babylon::Plugins::Internal::NativeCapture::Initialize(env);
    }
}
