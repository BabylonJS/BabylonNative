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

        Set(ptr);
    }

    void ExternalTexture::Impl::Update(Graphics::TextureT ptr, std::optional<Graphics::TextureFormatT> overrideFormat)
    {
        Info info;
        GetInfo(ptr, overrideFormat, info);

        DEBUG_TRACE("ExternalTexture [0x%p] Update %d x %d %d mips %d layers",
            this, int(info.Width), int(info.Height), int(info.MipLevels), int(info.NumLayers));

        m_info = info;

        Set(ptr);
        UpdateTextures(ptr);
    }

    ExternalTexture::ExternalTexture(Graphics::TextureT ptr, std::optional<Graphics::TextureFormatT> overrideFormat)
        : m_impl{std::make_shared<Impl>(ptr, overrideFormat)}
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
        std::scoped_lock lock{m_impl->Mutex()};

        return m_impl->Width();
    }

    uint32_t ExternalTexture::Height() const
    {
        std::scoped_lock lock{m_impl->Mutex()};

        return m_impl->Height();
    }

    Graphics::TextureT ExternalTexture::Get() const
    {
        std::scoped_lock lock{m_impl->Mutex()};

        return m_impl->Get();
    }

    Napi::Value ExternalTexture::CreateForJavaScript(Napi::Env env) const
    {
        std::scoped_lock lock{m_impl->Mutex()};

        Graphics::DeviceContext& context = Graphics::DeviceContext::GetFromJavaScript(env);

        // Create a placeholder bgfx texture. The native resource backing is
        // applied via overrideInternal on the AfterRenderScheduler, which runs
        // during bgfx::frame(). This two-step approach is required because
        // bgfx's _external parameter to createTexture2D causes
        // CreateShaderResourceView failures on WARP (E_INVALIDARG).
        // The caller must pump one frame (FinishRenderingCurrentFrame) before
        // the texture is usable for rendering.
        bgfx::TextureHandle handle = bgfx::createTexture2D(
            m_impl->Width(),
            m_impl->Height(),
            m_impl->HasMips(),
            m_impl->NumLayers(),
            m_impl->Format(),
            m_impl->Flags()
        );

        DEBUG_TRACE("ExternalTexture [0x%p] CreateForJavaScript %d x %d %d mips %d layers. Format : %d Flags : %d. (bgfx handle id %d)",
            m_impl.get(), int(m_impl->Width()), int(m_impl->Height()), int(m_impl->HasMips()), int(m_impl->NumLayers()), int(m_impl->Format()), int(m_impl->Flags()), int(handle.idx));

        if (!bgfx::isValid(handle))
        {
            throw Napi::Error::New(env, "Failed to create external texture");
        }

        // Schedule the native resource override for the render thread.
        arcana::make_task(context.AfterRenderScheduler(), arcana::cancellation_source::none(),
            [handle, impl = m_impl]() {
                bgfx::overrideInternal(handle, NativeHandleToUintPtr(impl->Get()));
            });

        auto* texture = new Graphics::Texture{context};
        texture->Attach(handle, true, m_impl->Width(), m_impl->Height(), m_impl->HasMips(), m_impl->NumLayers(), m_impl->Format(), m_impl->Flags());

        m_impl->AddTexture(texture);

        auto jsObject = Napi::Pointer<Graphics::Texture>::Create(env, texture, [texture, weakImpl = std::weak_ptr{m_impl}] {
            if (auto impl = weakImpl.lock())
            {
                std::scoped_lock lock{impl->Mutex()};

                impl->RemoveTexture(texture);
            }

            delete texture;
        });

        return jsObject;
    }

    void ExternalTexture::Update(Graphics::TextureT ptr, std::optional<Graphics::TextureFormatT> overrideFormat)
    {
        std::scoped_lock lock{m_impl->Mutex()};

        m_impl->Update(ptr, overrideFormat);
    }
}
