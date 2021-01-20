#include "NativeCapture.h"

#include <Babylon/JsRuntime.h>
#include <GraphicsImpl.h>

#include <vector>

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
                    NativeCapture::InstanceMethod("addOnCaptureCallback", &NativeCapture::AddOnCaptureCallback),
                    NativeCapture::InstanceMethod("dispose", &NativeCapture::Dispose),
                });

            env.Global().Set(JS_CLASS_NAME, func);
        }

        NativeCapture(const Napi::CallbackInfo& info)
            : Napi::ObjectWrap<NativeCapture>{info}
            , m_runtime{JsRuntime::GetFromJavaScript(info.Env())}
            , m_graphicsImpl(Graphics::Impl::GetFromJavaScript(info.Env()))
            , m_ticket{m_graphicsImpl.AddCaptureCallback([this](auto& data) { CaptureDataReceived(data); })}
        {
            m_graphicsImpl.StartCapture();
        }

    private:
        void AddOnCaptureCallback(const Napi::CallbackInfo& info)
        {
            auto listener = info[0].As<Napi::Function>();
            m_callbacks.push_back(Napi::Persistent(listener));
        }

        void CaptureDataReceived(const BgfxCallback::CaptureData& data)
        {
            std::vector<uint8_t> bytes{};
            bytes.resize(data.DataSize);
            std::memcpy(bytes.data(), data.Data, data.DataSize);
            m_runtime.Dispatch([this, data = data, bytes = std::move(bytes)](Napi::Env env) mutable {
                data.Data = bytes.data();

                auto external = Napi::External<BgfxCallback::CaptureData>::New(env, &data);
                for (const auto& callback : m_callbacks)
                {
                    callback.Call({external});
                }
            });
        }

        void Dispose(const Napi::CallbackInfo&)
        {
            m_graphicsImpl.StopCapture();
            m_callbacks.clear();
        }

        JsRuntime& m_runtime;
        Graphics::Impl& m_graphicsImpl;
        std::vector<Napi::FunctionReference> m_callbacks{};
        arcana::ticketed_collection<std::function<void(const BgfxCallback::CaptureData&)>>::ticket m_ticket;
    };
}

namespace Babylon::Plugins::NativeCapture
{
    void Initialize(Napi::Env env)
    {
        Babylon::Plugins::Internal::NativeCapture::Initialize(env);
    }
}
