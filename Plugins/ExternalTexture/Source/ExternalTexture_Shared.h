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

    void ExternalTexture::Impl::Update(Graphics::TextureT ptr, std::optional<Graphics::TextureFormatT> overrideFormat, std::optional<uint16_t> layerIndex)
    {
        Info info;
        GetInfo(ptr, overrideFormat, info);

        DEBUG_TRACE("ExternalTexture [0x%p] Update %d x %d %d mips %d layers",
            this, int(info.Width), int(info.Height), int(info.MipLevels), int(info.NumLayers));

        // Lock to publish (m_info, m_ptr, recreated bgfx handles) atomically to any JS-thread
        // reader currently in CreateTexture.
        {
            std::scoped_lock lock{m_mutex};
            m_info = info;
            Set(ptr);
            UpdateTextures(ptr, layerIndex);
        }
    }

    Graphics::Texture* ExternalTexture::ImplBase::CreateTexture(Graphics::DeviceContext& context, std::optional<uint16_t> layerIndex)
    {
        std::scoped_lock lock{m_mutex};

        bgfx::TextureHandle handle = bgfx::createTexture2D(
            m_info.Width,
            m_info.Height,
            HasMips(),
            m_info.NumLayers,
            m_info.Format,
            m_info.Flags,
            0,
            NativeHandleToUintPtr(static_cast<Impl*>(this)->Get())
        );

        DEBUG_TRACE("ExternalTexture [0x%p] CreateForJavaScript %d x %d %d mips %d layers. Format : %d Flags : %d. (bgfx handle id %d)",
            this, int(m_info.Width), int(m_info.Height), int(HasMips()), int(m_info.NumLayers), int(m_info.Format), int(m_info.Flags), int(handle.idx));

        if (!bgfx::isValid(handle))
        {
            throw std::runtime_error{"Failed to create external texture"};
        }

        auto* texture = new Graphics::Texture{context};
        texture->Attach(handle, true, m_info.Width, m_info.Height, HasMips(), m_info.NumLayers, m_info.Format, m_info.Flags);
        texture->ViewFirstLayer(layerIndex.value_or(0));
        texture->ViewNumLayers(layerIndex.has_value() ? 1 : 0);

        if (!m_textures.insert(texture).second)
        {
            assert(!"Failed to insert texture");
        }

        return texture;
    }

    void ExternalTexture::ImplBase::DestroyTexture(Graphics::Texture* texture)
    {
        {
            std::scoped_lock lock{m_mutex};
            m_textures.erase(texture);
        }

        delete texture;
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
        return m_impl->Width();
    }

    uint32_t ExternalTexture::Height() const
    {
        return m_impl->Height();
    }

    Graphics::TextureT ExternalTexture::Get() const
    {
        return m_impl->Get();
    }

    Napi::Value ExternalTexture::CreateForJavaScript(Napi::Env env, std::optional<uint16_t> layerIndex) const
    {
        // Resolve the DeviceContext outside any lock: the lookup is a JS property read
        // that may run engine GC/finalizers, and finalizers may re-enter the impl mutex.
        // DeviceContext is process-scoped and does not need synchronization.
        Graphics::DeviceContext& context = Graphics::DeviceContext::GetFromJavaScript(env);

        // CreateTexture locks internally and contains no JS callouts, so the
        // mutex is never held across the JS object allocation below.
        Graphics::Texture* texture = m_impl->CreateTexture(context, layerIndex);

        return Napi::Pointer<Graphics::Texture>::Create(env, texture, [texture, weakImpl = std::weak_ptr{m_impl}] {
            if (auto impl = weakImpl.lock())
            {
                impl->DestroyTexture(texture);
            }
            else
            {
                delete texture;
            }
        });
    }

    void ExternalTexture::Update(Graphics::TextureT ptr, std::optional<Graphics::TextureFormatT> overrideFormat, std::optional<uint16_t> layerIndex)
    {
        m_impl->Update(ptr, overrideFormat, layerIndex);
    }

    Napi::Promise ExternalTexture::AddToContextAsync(Napi::Env env, std::optional<uint16_t> layerIndex) const
    {
        auto deferred = Napi::Promise::Deferred::New(env);
        deferred.Resolve(CreateForJavaScript(env, layerIndex));
        return deferred.Promise();
    }
}
