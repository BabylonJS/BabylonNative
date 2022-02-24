#include <Babylon/Graphics/Device.h>
#include <Babylon/Graphics/Texture.h>
#include <Babylon/Graphics/DeviceContext.h>
#include <Babylon/JsRuntime.h>
#include <Babylon/JsRuntimeScheduler.h>

#include "ExternalTextureImpl.h"

#include <napi/env.h>
#include <napi/napi_pointer.h>


namespace Babylon::Graphics
{
        ExternalTexture::Impl::Impl(TextureType nativeTexture): m_nativeTexture{reinterpret_cast<uintptr_t>(nativeTexture)}
        {
            GetNativeTextureData(nativeTexture);
        }

        ExternalTexture::Impl::~Impl()
        {
        }

        //TODO: Change this to take a context.
        Napi::Promise ExternalTexture::Impl::AddToContext(Napi::Env& env)
        {
            DeviceContext& context = DeviceContext::GetFromJavaScript(env);

            Napi::Promise::Deferred deferred{Napi::Promise::Deferred::New(env)};
            Napi::Promise promise{deferred.Promise()};

            JsRuntime& runtime = JsRuntime::GetFromJavaScript(env);

            auto nativeHandler = m_nativeTexture;
            auto width = m_width;
            auto height = m_height;
            auto format = m_format;
            auto mips = m_mips;

            arcana::make_task(context.BeforeRenderScheduler(), arcana::cancellation_source::none(), [&context, &runtime, deferred{std::move(deferred)}, width, height, format, mips, nativeHandler]() 
                { 
                    auto textureHandle = bgfx::createTexture2D(static_cast<uint16_t>(width), static_cast<uint16_t>(height), mips != 1, 1, format, BGFX_TEXTURE_RT);

                    arcana::make_task(context.AfterRenderScheduler(), arcana::cancellation_source::none(), [&context, &runtime, deferred{std::move(deferred)}, textureHandle, width, height, format, mips, nativeHandler]() 
                    {
                            if (bgfx::overrideInternal(textureHandle, nativeHandler) == 0)
                            {
                                assert(false);
                            }

                            auto* textureData = new TextureData{textureHandle, true, width, height};
                            textureData->OwnsHandle = false;

                            context.AddTexture(textureData->Handle, 0, 0, mips != 1, 0, format);

                            runtime.Dispatch([deferred{std::move(deferred)}, textureData](Napi::Env env) 
                                {
                                    auto jsObject = Napi::Pointer<TextureData>::Create(env, textureData, [textureData]
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

        uint32_t ExternalTexture::Impl::GetWight()
        {
            return m_width;
        }

        uint32_t  ExternalTexture::Impl::GetHeight()
        {
            return m_height;
        }

        uint32_t  ExternalTexture::Impl::GetFormat()
        {
            return static_cast<uint32_t>(m_format);
        }

}