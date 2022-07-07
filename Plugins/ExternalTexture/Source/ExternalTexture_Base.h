#pragma once

#include <Babylon/Graphics/RendererType.h>
#include <bgfx/bgfx.h>
#include <set>
#include <cassert>

#ifdef NDEBUG
    #define CHECK_INFO() 
#else
    #define CHECK_INFO() \
        do \
        { \
            uint16_t width{}; \
            uint16_t height{}; \
            bgfx::TextureFormat::Enum format{bgfx::TextureFormat::Unknown}; \
            uint64_t flags{}; \
            bool hasMips{}; \
            GetInfo(width, height, hasMips, format, flags); \
            assert(width == m_width); \
            assert(height == m_height); \
            assert(format == m_format); \
            assert(flags == m_flags); \
            assert(hasMips == m_hasMips); \
        } \
        while (false)
#endif

namespace
{
    class ImplBase
    {
    public:
        uint16_t Width() const { return m_width; }
        uint16_t Height() const { return m_height; }
        bgfx::TextureFormat::Enum Format() const { return m_format; }
        bool HasMips() const { return m_hasMips; }
        uint64_t Flags() const { return m_flags; }

        void AddHandle(bgfx::TextureHandle handle)
        {
            std::scoped_lock lock{m_mutex};

            if (!m_handles.insert(handle).second)
            {
                assert(!"Failed to insert handle");
            }
        }

        void RemoveHandle(bgfx::TextureHandle handle)
        {
            std::scoped_lock lock{m_mutex};

            auto it = m_handles.find(handle);
            if (it != m_handles.end())
            {
                m_handles.erase(it);
            }
        }

    protected:
        template<typename T1, typename T2, typename T3>
        static bool IsFullMipChain(T1 mipLevel, T2 width, T3 height)
        {
            return mipLevel == static_cast<T1>(std::floorf(std::log2f(std::max(static_cast<float>(width), static_cast<float>(height))) + 1));
        }

        void UpdateHandles(Babylon::Graphics::TextureT ptr)
        {
            std::scoped_lock lock{m_mutex};

            for (auto it = m_handles.begin(); it != m_handles.end(); ++it)
            {
                if (bgfx::overrideInternal(*it, reinterpret_cast<uintptr_t>(ptr)) == 0)
                {
                    assert(!"Failed to override texture");
                }
            }
        }

        uint16_t m_width{};
        uint16_t m_height{};
        bgfx::TextureFormat::Enum m_format{bgfx::TextureFormat::Unknown};
        bool m_hasMips{};
        uint64_t m_flags{};

        struct TextureHandleLess
        {
            bool operator()(const bgfx::TextureHandle& a, const bgfx::TextureHandle& b) const
            {
                return a.idx < b.idx;
            }
        };

        mutable std::mutex m_mutex{};
        std::set<bgfx::TextureHandle, TextureHandleLess> m_handles{};
    };
}
