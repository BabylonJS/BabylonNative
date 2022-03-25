#include <Babylon/Graphics/Device.h>
#include <Babylon/Plugins/ExternalTexture.h>
#include <Babylon/Graphics/Texture.h>
#include <Babylon/Graphics/DeviceContext.h>

#include <napi/env.h>
#include <napi/napi_pointer.h>

namespace Babylon::Plugins
{
    ExternalTexture::ExternalTexture(Babylon::Graphics::TextureType nativeTexture)
        : m_nativeTexture{(uintptr_t)(nativeTexture)}
    {
        ReadPropertiesFromNativeTexture(nativeTexture);
    }

    ExternalTexture::~ExternalTexture()
    {
    }

    // TODO: Change this to take a context.
    Napi::Promise ExternalTexture::AddToContext(Napi::Env& env) const
    {
        Babylon::Graphics::DeviceContext& context = Babylon::Graphics::DeviceContext::GetFromJavaScript(env);

        Napi::Promise::Deferred deferred{Napi::Promise::Deferred::New(env)};
        Napi::Promise promise{deferred.Promise()};

        JsRuntime& runtime = JsRuntime::GetFromJavaScript(env);

        auto nativeHandler = m_nativeTexture;
        auto width = m_width;
        auto height = m_height;
        auto format = m_format;
        auto mips = m_mips;
        auto flags = m_flags;

        arcana::make_task(context.BeforeRenderScheduler(), arcana::cancellation_source::none(), [&context, &runtime, deferred{std::move(deferred)}, width, height, format, mips, flags, nativeHandler]()
        {
            auto textureHandle = bgfx::createTexture2D(static_cast<uint16_t>(width), static_cast<uint16_t>(height), mips != 1, 1, static_cast<bgfx::TextureFormat::Enum>(format), flags);

            arcana::make_task(context.AfterRenderScheduler(), arcana::cancellation_source::none(), [&context, &runtime, deferred{std::move(deferred)}, textureHandle, width, height, format, mips, nativeHandler]()
            {
                if (bgfx::overrideInternal(textureHandle, nativeHandler) == 0)
                {
                    runtime.Dispatch([deferred{std::move(deferred)}](Napi::Env env)
                    { 
                        deferred.Reject(Napi::Error::New(env, "Fail to create native texture.").Value()); 
                    });
                    
                    return;
                }

                auto* textureData = new Babylon::Graphics::TextureData{textureHandle, true, width, height};
                textureData->OwnsHandle = false;

                context.AddTexture(textureData->Handle, 0, 0, mips != 1, 0, static_cast<bgfx::TextureFormat::Enum>(format));

                runtime.Dispatch([deferred{std::move(deferred)}, textureData](Napi::Env env)
                {
                    auto jsObject = Napi::Pointer<Babylon::Graphics::TextureData>::Create(env, textureData, [textureData]
                        {
                            bgfx::destroy(textureData->Handle);
                            delete textureData;
                        });

                    deferred.Resolve(jsObject);
                });
            }); 
        });

        return promise;
    }

    uint32_t ExternalTexture::GetWidth() const 
    {
        return m_width;
    }

    uint32_t ExternalTexture::GetHeight() const
    {
        return m_height;
    }

    uint32_t ExternalTexture::GetFormat() const
    {
        return m_format;
    }
}
