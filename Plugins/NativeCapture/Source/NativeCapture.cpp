#include "NativeCapture.h"

#include <Babylon/JsRuntime.h>
#include <GraphicsImpl.h>
#include <FrameBuffer.h>

#include <arcana/containers/ticketed_collection.h>

#include <vector>

namespace
{
    using FrameCallback = std::function<void(uint32_t width, uint32_t height, bgfx::TextureFormat::Enum format, bool yFlip, gsl::span<const uint8_t> data)>;
    using FrameProviderCleanup = std::function<void()>;
    using FrameProviderTicket = gsl::final_action<FrameProviderCleanup>;

    FrameProviderTicket BeginFrameCapture(Babylon::GraphicsImpl& graphicsImpl, bgfx::FrameBufferHandle frameBufferHandle, FrameCallback callback)
    {
        class DefaultBufferFrameProvider final : public std::enable_shared_from_this<DefaultBufferFrameProvider>
        {
        public:
            static FrameProviderTicket Create(Babylon::GraphicsImpl& graphicsImpl, FrameCallback callback)
            {
                std::shared_ptr<DefaultBufferFrameProvider> frameProvider{new DefaultBufferFrameProvider(graphicsImpl, std::move(callback))};
                frameProvider->StartCapture();
                return gsl::finally<FrameProviderCleanup>([frameProvider{std::move(frameProvider)}]() mutable {
                    frameProvider->m_ticket.reset();
                });
            }

        private:
            DefaultBufferFrameProvider(Babylon::GraphicsImpl& graphicsImpl, FrameCallback callback)
                : m_graphicsImpl{graphicsImpl}
                , m_frameCallback{std::move(callback)}
            {
            }

            void StartCapture()
            {
                m_ticket = std::make_unique<Babylon::GraphicsImpl::CaptureCallbackTicketT>(m_graphicsImpl.AddCaptureCallback([thisRef{shared_from_this()}](auto& data) {
                    thisRef->m_frameCallback(data.Width, data.Height, data.Format, data.YFlip, {static_cast<const uint8_t*>(data.Data), static_cast<std::ptrdiff_t>(data.DataSize)});
                }));
            }

        private:
            Babylon::GraphicsImpl& m_graphicsImpl;
            std::unique_ptr<Babylon::GraphicsImpl::CaptureCallbackTicketT> m_ticket{};
            FrameCallback m_frameCallback{};
        };

        class OffScreenBufferFrameProvider final : public std::enable_shared_from_this<OffScreenBufferFrameProvider>
        {
        public:
            static FrameProviderTicket Create(Babylon::GraphicsImpl& graphicsImpl, bgfx::FrameBufferHandle frameBufferHandle, FrameCallback callback)
            {
                std::shared_ptr<OffScreenBufferFrameProvider> frameProvider{new OffScreenBufferFrameProvider(graphicsImpl, frameBufferHandle, std::move(callback))};
                // Note: ReadTextureAsync is "asynchronously recursive" (it calls itself to read the next frame).
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
            OffScreenBufferFrameProvider(Babylon::GraphicsImpl& graphicsImpl, bgfx::FrameBufferHandle frameBufferHandle, FrameCallback callback)
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
                return arcana::make_task(m_graphicsImpl.AfterRenderScheduler(), m_cancellationToken, [thisRef{shared_from_this()}] {
                    // bgfx does not allow readback of render textures, so the frame buffer render texture needs to be blitted to a texture with readback enabled.
                    bgfx::blit(static_cast<uint16_t>(bgfx::getCaps()->limits.maxViews - 1), thisRef->m_blitTextureHandle, 0, 0, thisRef->m_frameBufferTextureHandle);

                    // Reading the texture is an async operation, but everything that needs to be done prior to future write operations on that texture is completed synchronously,
                    // so we kick off the read for the next frame prior to the read for the current frame completes.

                    arcana::task<void, std::exception_ptr> readCurrentFrameTask{thisRef->m_graphicsImpl.ReadTextureAsync(thisRef->m_blitTextureHandle, thisRef->m_textureBuffer)
                        .then(arcana::inline_scheduler, thisRef->m_cancellationToken, [thisRef] {
                            thisRef->m_frameCallback(thisRef->m_textureInfo.Width, thisRef->m_textureInfo.Height, thisRef->m_textureInfo.Format, bgfx::getCaps()->originBottomLeft, thisRef->m_textureBuffer);
                        })};

                    arcana::task<void, std::exception_ptr> readNextFrameTask{thisRef->ReadTextureAsync()};

                    return arcana::when_all(readCurrentFrameTask, readNextFrameTask)
                        .then(arcana::inline_scheduler, arcana::cancellation::none(), [](const auto&) {
                            // Nothing to do, just converting to task<void, std::exception_ptr>
                        });
                });
            }

        private:
            Babylon::GraphicsImpl& m_graphicsImpl;
            bgfx::TextureHandle m_frameBufferTextureHandle{bgfx::kInvalidHandle};
            FrameCallback m_frameCallback{};
            Babylon::GraphicsImpl::TextureInfo m_textureInfo{};
            bgfx::TextureHandle m_blitTextureHandle{bgfx::kInvalidHandle};
            std::vector<uint8_t> m_textureBuffer{};
            arcana::cancellation_source m_cancellationToken{};
        };

        // An invalid frame buffer handle indicates the on screen / default frame buffer, otherwise we are dealing with an off screen frame buffer.
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
            auto& graphicsImpl{GraphicsImpl::GetFromJavaScript(info.Env())};

            Napi::Object jsData = m_jsData.Value();
            jsData.Set("data", Napi::ArrayBuffer::New(info.Env(), 0));

            FrameCallback frameCallback{[this](uint32_t width, uint32_t height, bgfx::TextureFormat::Enum format, bool yFlip, gsl::span<const uint8_t> data){
                CaptureDataReceived(width, height, format, yFlip, data);
            }};

            bgfx::FrameBufferHandle frameBufferHandle{bgfx::kInvalidHandle};

            // For an off screen frame buffer, the frame buffer must be passed into the constructor. Otherwise, we capture the on screen / default frame buffer.
            if (info.Length() > 1)
            {
                throw Napi::Error::New(info.Env(), "Too many arguments passed to NativeCapture constructor.");
            }
            else if (info.Length() > 0 && !info[0].IsNull() && !info[0].IsUndefined())
            {
                if (!info[0].IsExternal())
                {
                    throw Napi::Error::New(info.Env(), "Argument passed to NativeCapture constructor must be a Napi::External containing a native FrameBuffer.");
                }
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
