#pragma once

#include <bimg/bimg.h>
#include <memory>

namespace Babylon::Graphics
{
    // Consumes image, preserving non-PNG and floating-point texture formats.
    // Returns nullptr on conversion failure; callers report their image-load error.
    inline bimg::ImageContainer* NormalizePngImage(bx::AllocatorI& allocator, bimg::ImageContainer* image)
    {
        const auto format = image->m_format;
        if (image->m_parser != bimg::ImageParser::Png ||
            (format != bimg::TextureFormat::R8 &&
             format != bimg::TextureFormat::RG8 &&
             format != bimg::TextureFormat::R16 &&
             format != bimg::TextureFormat::RG16 &&
             format != bimg::TextureFormat::RGBA16))
        {
            return image;
        }

        using Image = std::unique_ptr<bimg::ImageContainer, decltype(&bimg::imageFree)>;
        Image source{image, bimg::imageFree};
        // Match the RGBA_F16 intermediate used by browser high-bit-depth PNG decoding.
        // Direct UNORM16-to-8 conversion gives different normal vectors.
        Image half{nullptr, bimg::imageFree};
        if (format == bimg::TextureFormat::R16 || format == bimg::TextureFormat::RG16 || format == bimg::TextureFormat::RGBA16)
        {
            half.reset(bimg::imageConvert(&allocator, bimg::TextureFormat::RGBA16F, *source, false));
            if (!half)
            {
                return nullptr;
            }
        }
        Image output{bimg::imageConvert(&allocator, bimg::TextureFormat::RGBA8, half ? *half : *source, false), bimg::imageFree};
        if (!output)
        {
            return nullptr;
        }
        output->m_hasAlpha = source->m_hasAlpha;
        output->m_srgb = source->m_srgb;
        if (format == bimg::TextureFormat::R8 || format == bimg::TextureFormat::RG8 ||
            format == bimg::TextureFormat::R16 || format == bimg::TextureFormat::RG16)
        {
            auto* pixels = static_cast<uint8_t*>(output->m_data);
            for (uint32_t offset = 0; offset < output->m_size; offset += 4)
            {
                pixels[offset + 3] = format == bimg::TextureFormat::RG8 || format == bimg::TextureFormat::RG16 ? pixels[offset + 1] : 255;
                pixels[offset + 1] = pixels[offset + 2] = pixels[offset];
            }
        }
        return output.release();
    }
}
