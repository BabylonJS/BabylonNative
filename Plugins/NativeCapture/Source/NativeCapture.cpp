#include "NativeCapture.h"

#include <arcana/containers/ticketed_collection.h>

#include <Babylon/JsRuntime.h>
#include <NativeEngine.h>
#include <GraphicsImpl.h>
#include <FrameBuffer.h>

#include <vector>

namespace Babylon::Plugins::Internal
{
    class NativeCapture : public Napi::ObjectWrap<NativeCapture>
    {
        using TicketT = arcana::ticketed_collection<std::function<void(const BgfxCallback::CaptureData&)>>::ticket;

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
            , m_graphicsImpl(Graphics::Impl::GetFromJavaScript(info.Env()))
            , m_jsData{Napi::Persistent(Napi::Object::New(info.Env()))}
            , m_cancellationToken{std::make_shared<arcana::cancellation_source>()}
        {
            Napi::Object jsData = m_jsData.Value();
            jsData.Set("data", Napi::ArrayBuffer::New(info.Env(), 0));

            if (info.Length() == 0)
            {
                m_ticket = std::make_unique<TicketT>(m_graphicsImpl.AddCaptureCallback([this](auto& data) { CaptureDataReceived(data); }));
            }
            else
            {
                auto& frameBuffer = *info[0].As<Napi::External<FrameBuffer>>().Data();
                auto textureHandle = bgfx::getTexture(frameBuffer.Handle());
                m_textureData = new TextureData();
                m_textureData->Handle = textureHandle;
                m_textureData->Width = frameBuffer.Width();
                m_textureData->Height = frameBuffer.Height();
                m_textureData->Format = bgfx::TextureFormat::RGBA8;
                m_textureData->StorageSize = frameBuffer.Width() * frameBuffer.Height() * 4 * 4;

                m_blitTexture = bgfx::createTexture2D(m_textureData->Width, m_textureData->Height, false, 1, m_textureData->Format, BGFX_TEXTURE_READ_BACK);

//                m_textureData = info[0].As<Napi::External<TextureData>>().Data();
                m_textureBuffer.resize(m_textureData->StorageSize);
                ReadTextureAsync();
            }
        }

        ~NativeCapture()
        {
            if (!m_cancellationToken->cancelled())
            {
                // If m_cancellationToken is not cancelled, this object is being garbage collected without
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

        arcana::task<void, std::exception_ptr> ReadTextureAsync()
        {
            return arcana::make_task(m_graphicsImpl.AfterRenderScheduler(), *m_cancellationToken, [this, cancellation{m_cancellationToken}]{
                bgfx::blit(10, m_blitTexture, 0, 0, m_textureData->Handle);
                m_graphicsImpl.ReadTextureAsync(m_blitTexture, m_textureBuffer).then(arcana::inline_scheduler, *m_cancellationToken, [this]{
                    CaptureDataReceived(m_textureData->Width, m_textureData->Height, m_textureData->StorageSize / m_textureData->Height, m_textureData->Format, true, m_textureBuffer);
                    return ReadTextureAsync();
                });
            });
        }

        void CaptureDataReceived(const BgfxCallback::CaptureData& data)
        {
            CaptureDataReceived(data.Width, data.Height, data.Pitch, data.Format, data.YFlip, {static_cast<const uint8_t*>(data.Data), data.DataSize});
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
            m_callbacks.clear();
            m_ticket.reset();
            //m_textureBuffer.clear();
            //m_textureBuffer.shrink_to_fit();
            m_cancellationToken->cancel();
        }

        void Dispose(const Napi::CallbackInfo&)
        {
            Dispose();
        }

        JsRuntime& m_runtime;
        Graphics::Impl& m_graphicsImpl;
        std::vector<Napi::FunctionReference> m_callbacks{};
        std::unique_ptr<TicketT> m_ticket{};
        Napi::ObjectReference m_jsData{};
        TextureData* m_textureData{};
        std::vector<uint8_t> m_textureBuffer{};
        std::shared_ptr<arcana::cancellation_source> m_cancellationToken{};
        bgfx::TextureHandle m_blitTexture{};
    };
}

namespace Babylon::Plugins::NativeCapture
{
    void Initialize(Napi::Env env)
    {
        Babylon::Plugins::Internal::NativeCapture::Initialize(env);
    }
}
