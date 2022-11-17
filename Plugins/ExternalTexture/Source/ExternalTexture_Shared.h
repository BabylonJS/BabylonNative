// Shared code (not an actual header)

namespace Babylon::Plugins
{
    ExternalTexture::Impl::Impl(Graphics::TextureT ptr)
    {
        GetInfo(ptr, m_info);

        if (m_info.MipLevels != 1 && m_info.MipLevels != 0 && !IsFullMipChain(m_info.MipLevels, m_info.Width, m_info.Height))
        {
            throw std::runtime_error{"Unsupported texture mip levels"};
        }

        Assign(ptr);
    }

    void ExternalTexture::Impl::Update(Graphics::TextureT ptr)
    {
        Info info;
        GetInfo(ptr, info);
        if (info != m_info)
        {
            throw std::runtime_error{"Textures must have same info"};
        }

        Assign(ptr);
    }

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

    Napi::Value ExternalTexture::AddToContextSync(Napi::Env env) const
    {
        Graphics::DeviceContext& context = Graphics::DeviceContext::GetFromJavaScript(env);

        bool isDone = false;
        bool succeeded = false;
        Graphics::Texture* texture;

        arcana::make_task(context.BeforeRenderScheduler(), arcana::cancellation_source::none(),
            [&context, &isDone, &succeeded, &texture, impl = m_impl]()
        {
            // REVIEW: The bgfx texture handle probably needs to be an RAII object to make sure it gets clean up during the asynchrony.
            //         For example, if any of the schedulers/dispatches below don't fire, then the texture handle will leak.
            bgfx::TextureHandle handle = bgfx::createTexture2D(impl->Width(), impl->Height(), impl->HasMips(), 1, impl->Format(), impl->Flags());
            
            if (!bgfx::isValid(handle))
            {
                succeeded = false;
                isDone = true;
                return;
            }

            arcana::make_task(context.AfterRenderScheduler(), arcana::cancellation_source::none(), [&context, &isDone, &succeeded, &texture,handle, impl = std::move(impl)]()
            {
                if (bgfx::overrideInternal(handle, impl->Ptr()) == 0)
                {
                    bgfx::destroy(handle);
                    succeeded = false;
                    isDone = true;
                    return;
                }

                context.AddTexture(handle, 0, 0, impl->HasMips(), 0, impl->Format());

                texture = new Graphics::Texture{};
                texture->Attach(handle, true, impl->Width(), impl->Height());
                succeeded = true;
                isDone = true;
            });
        });

        while(!isDone)
        {
        }

        if(!succeeded){
            return Napi::Error::New(env, "Failed to create native texture").Value();
        }

        auto jsObject = Napi::Pointer<Graphics::Texture>::Create(env, texture, [texture] 
        {
            delete texture;
        });

        return jsObject;
    }

    Napi::Promise ExternalTexture::AddToContextAsync(Napi::Env env) const
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
                    auto* texture = new Graphics::Texture{};
                    texture->Attach(handle, true, impl->Width(), impl->Height(), impl->HasMips(), 1, impl->Format(), impl->Flags());

                    impl->AddHandle(texture->Handle());

                    auto jsObject = Napi::Pointer<Graphics::Texture>::Create(env, texture, [texture, weakImpl = std::weak_ptr{impl}] {
                        if (auto impl = weakImpl.lock())
                        {
                            impl->RemoveHandle(texture->Handle());
                        }

                        delete texture;
                    });

                    deferred.Resolve(jsObject);
                });
            });
        });

        return promise;
    }

    void ExternalTexture::AddToContextAsync(Napi::Env env, std::function<void(Napi::Env, Napi::Value)> succeed, std::function<void(Napi::Env, Napi::Value)> fail) const
    {
        Graphics::DeviceContext& context = Graphics::DeviceContext::GetFromJavaScript(env);
        JsRuntime& runtime = JsRuntime::GetFromJavaScript(env);

        arcana::make_task(context.BeforeRenderScheduler(), arcana::cancellation_source::none(),
            [&context, &runtime, succeed = std::move(succeed), fail = std::move(fail), impl = m_impl]()
            {
                // REVIEW: The bgfx texture handle probably needs to be an RAII object to make sure it gets clean up during the asynchrony.
                //         For example, if any of the schedulers/dispatches below don't fire, then the texture handle will leak.
                bgfx::TextureHandle handle = bgfx::createTexture2D(impl->Width(), impl->Height(), impl->HasMips(), 1, impl->Format(), impl->Flags());
                if (!bgfx::isValid(handle))
                {
                    runtime.Dispatch([fail{std::move(fail)}](Napi::Env env)
                    {
                        fail(env, Napi::Error::New(env, "Failed to create native texture").Value());
                    });

                    return;
                }

                arcana::make_task(context.AfterRenderScheduler(), arcana::cancellation_source::none(), [&context, &runtime, succeed = std::move(succeed), fail = std::move(fail), handle, impl = std::move(impl)]()
                    {
                        if (bgfx::overrideInternal(handle, impl->Ptr()) == 0)
                        {
                            runtime.Dispatch([fail = std::move(fail), handle](Napi::Env env)
                                {
                                    bgfx::destroy(handle);
                                    fail(env, Napi::Error::New(env, "Failed to override native texture").Value());
                                });

                            return;
                        }

                        context.AddTexture(handle, 0, 0, impl->HasMips(), 0, impl->Format());

                        runtime.Dispatch([succeed = std::move(succeed), handle, impl = std::move(impl)](Napi::Env env)
                            {
                                auto* texture = new Graphics::Texture{};
                                texture->Attach(handle, true, impl->Width(), impl->Height());

                                auto jsObject = Napi::Pointer<Graphics::Texture>::Create(env, texture, [texture]
                                    { delete texture; });

                                succeed(env, jsObject);
                            });
                    });
            });
    }

    void ExternalTexture::Update(Graphics::TextureT ptr)
    {
        m_impl->Update(ptr);
    }
}
