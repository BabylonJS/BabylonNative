// Native image decoder for NativeDawn, backed by bimg (bgfx's image library).
// Decodes PNG/JPEG/etc. encoded bytes to tightly packed 32bpp RGBA so glTF
// embedded textures work in this headless (no-DOM) environment. bimg is the
// same decoder the bgfx-based NativeEngine uses, so behaviour matches the rest
// of Babylon Native (grayscale unpacking, etc.).

#include <bimg/decode.h>
#include <bx/allocator.h>
#include <bx/error.h>

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <vector>

namespace Babylon::Plugins::NativeDawn
{
    namespace
    {
        bx::AllocatorI& Allocator()
        {
            static bx::DefaultAllocator s_allocator;
            return s_allocator;
        }

        // Grayscale textures come back from bimg as R8/RG8. Expand them so RGB is
        // the grayscale value and A is either opaque (R8) or the second channel
        // (RG8), matching NativeEngine's behaviour.
        void UnpackGrayscale(const bimg::ImageContainer& image, std::vector<uint8_t>& out)
        {
            const uint32_t pixels = image.m_width * image.m_height;
            const bool hasAlpha = image.m_format == bimg::TextureFormat::RG8;
            const uint32_t srcBpp = hasAlpha ? 2u : 1u;
            const auto* src = static_cast<const uint8_t*>(image.m_data);
            out.resize(static_cast<size_t>(pixels) * 4u);
            for (uint32_t i = 0; i < pixels; ++i)
            {
                const uint8_t gray = src[i * srcBpp];
                uint8_t* dst = out.data() + static_cast<size_t>(i) * 4u;
                dst[0] = dst[1] = dst[2] = gray;
                dst[3] = hasAlpha ? src[i * srcBpp + 1u] : 0xFFu;
            }
        }
    }

    // Decodes an encoded image to tightly-packed RGBA8 via bimg. Returns false on
    // failure. out is resized to width*height*4 on success.
    bool DecodeRGBA(const uint8_t* data, size_t size, std::vector<uint8_t>& out, int& width, int& height)
    {
        out.clear();
        width = 0;
        height = 0;
        if (data == nullptr || size == 0)
        {
            return false;
        }

        // ErrorIgnore keeps bimg from tripping its internal BX_ERROR_SCOPE assert
        // on unrecognized/corrupt input; it returns nullptr instead.
        bx::ErrorIgnore err;
        bimg::ImageContainer* image = bimg::imageParse(
            &Allocator(), data, static_cast<uint32_t>(size), bimg::TextureFormat::Count, &err);
        if (image == nullptr)
        {
            std::fprintf(stderr, "[NativeDawn][bimg] imageParse failed\n");
            return false;
        }

        bool ok = true;
        switch (image->m_format)
        {
            case bimg::TextureFormat::RGBA8:
            {
                out.resize(image->m_size);
                std::memcpy(out.data(), image->m_data, image->m_size);
                break;
            }
            case bimg::TextureFormat::R8:
            case bimg::TextureFormat::RG8:
            {
                UnpackGrayscale(*image, out);
                break;
            }
            default:
            {
                // Convert any other decoded format (RGB8, BGRA8, float, ...) to RGBA8.
                bimg::ImageContainer* rgba = bimg::imageConvert(
                    &Allocator(), bimg::TextureFormat::RGBA8, *image, false);
                if (rgba == nullptr)
                {
                    std::fprintf(stderr, "[NativeDawn][bimg] imageConvert to RGBA8 failed (format=%d)\n",
                        static_cast<int>(image->m_format));
                    ok = false;
                }
                else
                {
                    out.resize(rgba->m_size);
                    std::memcpy(out.data(), rgba->m_data, rgba->m_size);
                    bimg::imageFree(rgba);
                }
                break;
            }
        }

        if (ok)
        {
            width = static_cast<int>(image->m_width);
            height = static_cast<int>(image->m_height);
        }
        bimg::imageFree(image);
        return ok;
    }
}
