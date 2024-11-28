// Shared code (not an actual header)

namespace Babylon::Plugins
{
    ExternalTexture::Impl::Impl(Graphics::TextureT ptr, std::optional<Graphics::TextureFormatT> overrideFormat)
    {
        GetInfo(ptr, overrideFormat, m_info);

        if (m_info.MipLevels != 1 && m_info.MipLevels != 0 && !IsFullMipChain(m_info.MipLevels, m_info.Width, m_info.Height))
        {
            throw std::runtime_error{"Unsupported texture mip levels"};
        }

        Assign(ptr);
    }

    bool ExternalTexture::Impl::Update(Graphics::TextureT ptr, std::optional<Graphics::TextureFormatT> overrideFormat)
    {
        Info info;
        GetInfo(ptr, overrideFormat, info);

        if (info.Width != m_info.Width || info.Height != m_info.Height || info.MipLevels != m_info.MipLevels)
        {
            return false;
        }

        DEBUG_TRACE("ExternalTexture [0x%p] Update %d x %d %d mips", this, int(info.Width), int(info.Height), int(info.MipLevels));

        m_info = info;

        Assign(ptr);

        UpdateHandles(Ptr());

        return true;
    }

    ExternalTexture::ExternalTexture(Graphics::TextureT ptr, std::optional<Graphics::TextureFormatT> overrideFormat)
        : m_impl{std::make_unique<Impl>(ptr, overrideFormat)}
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

    Napi::Promise ExternalTexture::AddToContextAsync(Napi::Env env) const
    {
        Graphics::DeviceContext& context = Graphics::DeviceContext::GetFromJavaScript(env);
        JsRuntime& runtime = JsRuntime::GetFromJavaScript(env);

        auto deferred{Napi::Promise::Deferred::New(env)};
        auto promise{deferred.Promise()};

        DEBUG_TRACE("ExternalTexture [0x%p] AddToContextAsync", m_impl.get());

        arcana::make_task(context.BeforeRenderScheduler(), arcana::cancellation_source::none(),
            [&context, &runtime, deferred = std::move(deferred), impl = m_impl]() {
                // REVIEW: The bgfx texture handle probably needs to be an RAII object to make sure it gets clean up during the asynchrony.
                //         For example, if any of the schedulers/dispatches below don't fire, then the texture handle will leak.
                bgfx::TextureHandle handle = bgfx::createTexture2D(impl->Width(), impl->Height(), impl->HasMips(), 1, impl->Format(), impl->Flags());
                DEBUG_TRACE("ExternalTexture [0x%p] create %d x %d %d mips. Format : %d Flags : %d. (bgfx handle id %d)", impl.get(), int(impl->Width()), int(impl->Height()), int(impl->HasMips()), int(impl->Format()), int(impl->Flags()), int(handle.idx));
                if (!bgfx::isValid(handle))
                {
                    DEBUG_TRACE("ExternalTexture [0x%p] is not valid", impl.get());
                    runtime.Dispatch([deferred{std::move(deferred)}](Napi::Env env) {
                        deferred.Reject(Napi::Error::New(env, "Failed to create native texture").Value());
                    });

                    return;
                }

                arcana::make_task(context.AfterRenderScheduler(), arcana::cancellation_source::none(), [&runtime, &context, deferred = std::move(deferred), handle, impl = std::move(impl)]() {
                    if (bgfx::overrideInternal(handle, impl->Ptr()) == 0)
                    {
                        runtime.Dispatch([deferred = std::move(deferred), handle](Napi::Env env) {
                            bgfx::destroy(handle);
                            deferred.Reject(Napi::Error::New(env, "Failed to override native texture").Value());
                        });

                        return;
                    }

                    runtime.Dispatch([deferred = std::move(deferred), handle, &context, impl = std::move(impl)](Napi::Env env) {
                        auto* texture = new Graphics::Texture{context};
                        DEBUG_TRACE("ExternalTexture [0x%p] attach %d x %d %d mips. Format : %d Flags : %d. (bgfx handle id %d)", impl.get(), int(impl->Width()), int(impl->Height()), int(impl->HasMips()), int(impl->Format()), int(impl->Flags()), int(handle.idx));
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

    bool ExternalTexture::Update(Graphics::TextureT ptr, std::optional<Graphics::TextureFormatT> overrideFormat)
    {
        return m_impl->Update(ptr, overrideFormat);
    }
}
