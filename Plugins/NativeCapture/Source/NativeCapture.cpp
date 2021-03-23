#include "NativeCapture.h"

#include <arcana/containers/ticketed_collection.h>

#include <Babylon/JsRuntime.h>
#include <NativeEngine.h>
#include <GraphicsImpl.h>
#include <FrameBuffer.h>

#include <vector>

namespace
{
    using FrameCallback = std::function<void(uint32_t width, uint32_t height, bgfx::TextureFormat::Enum format, bool yFlip, gsl::span<const uint8_t> data)>;
    using FrameProviderCleanup = std::function<void()>;
    using FrameProviderTicket = gsl::final_action<FrameProviderCleanup>;

    FrameProviderTicket BeginFrameCapture(Babylon::Graphics::Impl& graphicsImpl, bgfx::FrameBufferHandle frameBufferHandle, FrameCallback callback)
    {
        class DefaultBufferFrameProvider final : public std::enable_shared_from_this<DefaultBufferFrameProvider>
        {
        public:
            static FrameProviderTicket Create(Babylon::Graphics::Impl& graphicsImpl, FrameCallback callback)
            {
                std::shared_ptr<DefaultBufferFrameProvider> frameProvider{new DefaultBufferFrameProvider(graphicsImpl, std::move(callback))};
                frameProvider->StartCapture();
                return gsl::finally<FrameProviderCleanup>([frameProvider{std::move(frameProvider)}]() mutable {
                    frameProvider->m_ticket.reset();
                });
            }

        private:
            DefaultBufferFrameProvider(Babylon::Graphics::Impl& graphicsImpl, FrameCallback callback)
                : m_graphicsImpl{graphicsImpl}
                , m_frameCallback{std::move(callback)}
            {
            }

            void StartCapture()
            {
                m_ticket = std::make_unique<Babylon::Graphics::Impl::CaptureCallbackTicketT>(m_graphicsImpl.AddCaptureCallback([thisRef{shared_from_this()}](auto& data) {
                    thisRef->m_frameCallback(data.Width, data.Height, data.Format, data.YFlip, {static_cast<const uint8_t*>(data.Data), static_cast<std::ptrdiff_t>(data.DataSize)});
                }));
            }

        private:
            Babylon::Graphics::Impl& m_graphicsImpl;
            std::unique_ptr<Babylon::Graphics::Impl::CaptureCallbackTicketT> m_ticket{};
            FrameCallback m_frameCallback{};
        };

        class OffScreenBufferFrameProvider final : public std::enable_shared_from_this<OffScreenBufferFrameProvider>
        {
        public:
            static FrameProviderTicket Create(Babylon::Graphics::Impl& graphicsImpl, bgfx::FrameBufferHandle frameBufferHandle, FrameCallback callback)
            {
                std::shared_ptr<OffScreenBufferFrameProvider> frameProvider{new OffScreenBufferFrameProvider(graphicsImpl, frameBufferHandle, std::move(callback))};
                frameProvider->ReadTextureAsync();
                return gsl::finally<FrameProviderCleanup>([frameProvider{std::move(frameProvider)}]() mutable {
                    frameProvider->m_cancellationToken.cancel();
                });
            }

            ~OffScreenBufferFrameProvider()
            {
                bgfx::destroy(m_blitTextureHandle);
            }

        private:
            OffScreenBufferFrameProvider(Babylon::Graphics::Impl& graphicsImpl, bgfx::FrameBufferHandle frameBufferHandle, FrameCallback callback)
                : m_graphicsImpl{graphicsImpl}
                , m_frameBufferTextureHandle{bgfx::getTexture(frameBufferHandle)}
                , m_frameCallback{std::move(callback)}
                , m_textureInfo{graphicsImpl.GetTextureInfo(m_frameBufferTextureHandle)}
                , m_blitTextureHandle{bgfx::createTexture2D(m_textureInfo.Width, m_textureInfo.Height, m_textureInfo.HasMips, m_textureInfo.NumLayers, m_textureInfo.Format, BGFX_TEXTURE_BLIT_DST | BGFX_TEXTURE_READ_BACK)}
            {
                bgfx::TextureInfo textureInfo{};
                bgfx::calcTextureSize(textureInfo, m_textureInfo.Width, m_textureInfo.Height, 1, false, m_textureInfo.HasMips, m_textureInfo.NumLayers, m_textureInfo.Format);
                m_textureBuffer.resize(textureInfo.storageSize);
            }

            arcana::task<void, std::exception_ptr> ReadTextureAsync()
            {
                return arcana::make_task(m_graphicsImpl.AfterRenderScheduler(), m_cancellationToken, [thisRef{shared_from_this()}]{
                    bgfx::blit(bgfx::getCaps()->limits.maxViews - 1, thisRef->m_blitTextureHandle, 0, 0, thisRef->m_frameBufferTextureHandle);
                    return arcana::when_all(thisRef->m_graphicsImpl.ReadTextureAsync(thisRef->m_blitTextureHandle, thisRef->m_textureBuffer).then(arcana::inline_scheduler, thisRef->m_cancellationToken, [thisRef]{
                        thisRef->m_frameCallback(thisRef->m_textureInfo.Width, thisRef->m_textureInfo.Height, thisRef->m_textureInfo.Format, bgfx::getCaps()->originBottomLeft, thisRef->m_textureBuffer);
                    }),
                    thisRef->ReadTextureAsync()).then(arcana::inline_scheduler, arcana::cancellation::none(), [](const std::tuple<arcana::void_placeholder, arcana::void_placeholder>&){
                        // Nothing to do, just converting to task<void, std::exception_ptr>
                    });
                });
            }

        private:
            Babylon::Graphics::Impl& m_graphicsImpl;
            bgfx::TextureHandle m_frameBufferTextureHandle{bgfx::kInvalidHandle};
            FrameCallback m_frameCallback{};
            Babylon::Graphics::Impl::TextureInfo m_textureInfo{};
            bgfx::TextureHandle m_blitTextureHandle{bgfx::kInvalidHandle};
            std::vector<uint8_t> m_textureBuffer{};
            arcana::cancellation_source m_cancellationToken{};
        };

        if (!bgfx::isValid(frameBufferHandle))
        {
            return DefaultBufferFrameProvider::Create(graphicsImpl, std::move(callback));
        }
        else
        {
            return OffScreenBufferFrameProvider::Create(graphicsImpl, frameBufferHandle, std::move(callback));
        }
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

            FrameCallback frameCallback{[this](uint32_t width, uint32_t height, bgfx::TextureFormat::Enum format, bool yFlip, gsl::span<const uint8_t> data){
                CaptureDataReceived(width, height, format, yFlip, data);
            }};

            bgfx::FrameBufferHandle frameBufferHandle{bgfx::kInvalidHandle};

            if (info.Length() > 0 && info[0].IsExternal())
            {
                auto& frameBuffer = *info[0].As<Napi::External<FrameBuffer>>().Data();
                frameBufferHandle = frameBuffer.Handle();
            }

            m_frameProviderTicket.emplace(BeginFrameCapture(graphicsImpl, frameBufferHandle, std::move(frameCallback)));
        }

        ~NativeCapture()
        {
            if (m_frameProviderTicket.has_value())
            {
                // If m_frameProviderTicket is still active, this object is being garbage collected without
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

        void CaptureDataReceived(uint32_t width, uint32_t height, bgfx::TextureFormat::Enum format, bool yFlip, gsl::span<const uint8_t> data)
        {
            std::vector<uint8_t> bytes{};
            bytes.resize(data.size());
            std::memcpy(bytes.data(), data.data(), data.size());
            m_runtime.Dispatch([this, width, height, format, yFlip, bytes{std::move(bytes)}](Napi::Env env) mutable {
                Napi::Object jsData = m_jsData.Value();
                jsData.Set("width", static_cast<double>(width));
                jsData.Set("height", static_cast<double>(height));
                constexpr auto FORMAT_MEMBER_NAME = "format";
                switch (format)
                {
                    case bgfx::TextureFormat::RGBA8:
                        jsData.Set(FORMAT_MEMBER_NAME, "RGBA8");
                        break;
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
            m_frameProviderTicket.reset();
            m_callbacks.clear();
        }

        void Dispose(const Napi::CallbackInfo&)
        {
            Dispose();
        }

        JsRuntime& m_runtime;
        std::vector<Napi::FunctionReference> m_callbacks{};
        Napi::ObjectReference m_jsData{};
        std::optional<FrameProviderTicket> m_frameProviderTicket{};
    };
}

namespace Babylon::Plugins::NativeCapture
{
    void Initialize(Napi::Env env)
    {
        Babylon::Plugins::Internal::NativeCapture::Initialize(env);
    }
}
