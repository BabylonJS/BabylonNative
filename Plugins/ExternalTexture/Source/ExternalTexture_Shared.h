// Shared pimpl code (not an actual header)

namespace Babylon::Plugins
{
    ExternalTexture::ExternalTexture(Graphics::TextureT ptr)
        : m_impl{std::make_unique<Impl>(ptr)}
    {
    }

    // Copy semantics
    ExternalTexture::ExternalTexture(const ExternalTexture&) = default;
    ExternalTexture& ExternalTexture::operator=(const ExternalTexture&) = default;

    // Move semantics
    ExternalTexture::ExternalTexture(ExternalTexture&&) noexcept = default;
    ExternalTexture& ExternalTexture::operator=(ExternalTexture&&) noexcept = default;

    ExternalTexture::~ExternalTexture() = default;

    uint32_t ExternalTexture::Width() const 
    {
        return m_impl->Width();
    }

    uint32_t ExternalTexture::Height() const
    {
        return m_impl->Height();
    }

    Napi::Promise ExternalTexture::AddToContextAsync(Napi::Env& env) const
    {
        Graphics::DeviceContext& context = Graphics::DeviceContext::GetFromJavaScript(env);
        JsRuntime& runtime = JsRuntime::GetFromJavaScript(env);

        auto deferred{Napi::Promise::Deferred::New(env)};
        auto promise{deferred.Promise()};

        arcana::make_task(context.BeforeRenderScheduler(), arcana::cancellation_source::none(),
            [&context, &runtime, deferred = std::move(deferred), impl = m_impl]()
        {
            // REVIEW: The bgfx texture handle probably needs to be an RAII object to make sure it gets clean up during the asynchrony.
            //         For example, if any of the schedulers/dispatches below don't fire, then the texture handle will leak.
            bgfx::TextureHandle handle = bgfx::createTexture2D(impl->Width(), impl->Height(), impl->HasMips(), 1, impl->Format(), impl->Flags());
            if (!bgfx::isValid(handle))
            {
                runtime.Dispatch([deferred{std::move(deferred)}](Napi::Env env)
                {
                    deferred.Reject(Napi::Error::New(env, "Failed to create native texture").Value());
                });

                return;
            }

            arcana::make_task(context.AfterRenderScheduler(), arcana::cancellation_source::none(), [&context, &runtime, deferred = std::move(deferred), handle, impl = std::move(impl)]()
            {
                if (bgfx::overrideInternal(handle, impl->Ptr()) == 0)
                {
                    runtime.Dispatch([deferred = std::move(deferred), handle](Napi::Env env)
                    {
                        bgfx::destroy(handle);
                        deferred.Reject(Napi::Error::New(env, "Failed to override native texture").Value()); 
                    });

                    return;
                }

                context.AddTexture(handle, 0, 0, impl->HasMips(), 0, impl->Format());

                runtime.Dispatch([deferred = std::move(deferred), handle, impl = std::move(impl)](Napi::Env env)
                {
                    auto* texture = new Graphics::TextureData{handle, true, impl->Width(), impl->Height()};

                    auto jsObject = Napi::Pointer<Graphics::TextureData>::Create(env, texture, [texture] {
                        delete texture;
                    });

                    deferred.Resolve(jsObject);
                });
            });
        });

        return promise;
    }
}
