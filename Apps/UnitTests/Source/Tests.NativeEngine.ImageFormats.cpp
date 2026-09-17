#include <Babylon/Graphics/ImageFormat.h>
#include <bx/allocator.h>
#include <gtest/gtest.h>

#include <array>
#include <memory>

namespace
{
    using Image = std::unique_ptr<bimg::ImageContainer, decltype(&bimg::imageFree)>;
}

TEST(NativeEngineImageFormats, Png16UsesBrowserHalfFloatQuantization)
{
    bx::DefaultAllocator allocator;
    const std::array<uint16_t, 16> pixels{
        32767, 56917, 58478, 65535,
        0, 65535, 32768, 32767,
        56917, 58478, 0, 0,
        65535, 1, 65534, 56917};
    auto* source = bimg::imageAlloc(&allocator, bimg::TextureFormat::RGBA16, 4, 1, 0, 1, false, false, pixels.data());
    ASSERT_NE(source, nullptr);
    source->m_parser = bimg::ImageParser::Png;
    source->m_hasAlpha = true;
    Image image{Babylon::Graphics::NormalizePngImage(allocator, source), bimg::imageFree};
    ASSERT_NE(image, nullptr);
    EXPECT_EQ(image->m_format, bimg::TextureFormat::RGBA8);
    EXPECT_EQ(image->m_parser, bimg::ImageParser::Png);
    EXPECT_TRUE(image->m_hasAlpha);
    const std::array<uint8_t, 16> expected{
        128, 222, 227, 255,
        0, 255, 128, 128,
        222, 227, 0, 0,
        255, 0, 255, 222};
    ASSERT_EQ(image->m_size, expected.size());
    const auto* actual = static_cast<const uint8_t*>(image->m_data);
    for (size_t index = 0; index < expected.size(); ++index)
    {
        EXPECT_EQ(actual[index], expected[index]) << index;
    }
}

TEST(NativeEngineImageFormats, Png16GrayscaleReplicatesColorAndPreservesAlpha)
{
    bx::DefaultAllocator allocator;
    for (const auto format : {bimg::TextureFormat::R16, bimg::TextureFormat::RG16})
    {
        const std::array<uint16_t, 2> pixels{32767, 56917};
        auto* source = bimg::imageAlloc(&allocator, format, 1, 1, 0, 1, false, false, pixels.data());
        ASSERT_NE(source, nullptr);
        source->m_parser = bimg::ImageParser::Png;
        Image image{Babylon::Graphics::NormalizePngImage(allocator, source), bimg::imageFree};
        ASSERT_NE(image, nullptr);
        EXPECT_EQ(image->m_format, bimg::TextureFormat::RGBA8);
        const auto* actual = static_cast<const uint8_t*>(image->m_data);
        EXPECT_EQ(actual[0], 128);
        EXPECT_EQ(actual[1], 128);
        EXPECT_EQ(actual[2], 128);
        EXPECT_EQ(actual[3], format == bimg::TextureFormat::RG16 ? 222 : 255);
    }
}

TEST(NativeEngineImageFormats, Png8GrayscaleReplicatesColorAndPreservesAlpha)
{
    bx::DefaultAllocator allocator;
    for (const auto format : {bimg::TextureFormat::R8, bimg::TextureFormat::RG8})
    {
        const std::array<uint8_t, 2> pixels{127, 64};
        auto* source = bimg::imageAlloc(&allocator, format, 1, 1, 0, 1, false, false, pixels.data());
        ASSERT_NE(source, nullptr);
        source->m_parser = bimg::ImageParser::Png;
        source->m_hasAlpha = format == bimg::TextureFormat::RG8;
        source->m_srgb = true;
        Image image{Babylon::Graphics::NormalizePngImage(allocator, source), bimg::imageFree};
        ASSERT_NE(image, nullptr);
        EXPECT_EQ(image->m_format, bimg::TextureFormat::RGBA8);
        const auto* actual = static_cast<const uint8_t*>(image->m_data);
        EXPECT_EQ(actual[0], 127);
        EXPECT_EQ(actual[1], 127);
        EXPECT_EQ(actual[2], 127);
        EXPECT_EQ(actual[3], format == bimg::TextureFormat::RG8 ? 64 : 255);
        EXPECT_EQ(image->m_hasAlpha, format == bimg::TextureFormat::RG8);
        EXPECT_TRUE(image->m_srgb);
    }
}

TEST(NativeEngineImageFormats, PreservesOrdinaryAndAuthoredHighPrecisionFormats)
{
    bx::DefaultAllocator allocator;
    for (const auto parser : {bimg::ImageParser::Png, bimg::ImageParser::Dds, bimg::ImageParser::Exr, bimg::ImageParser::Count})
    {
        for (const auto format : {bimg::TextureFormat::RGBA8, bimg::TextureFormat::RGBA16, bimg::TextureFormat::RGBA16F, bimg::TextureFormat::RGBA32F})
        {
            if (parser == bimg::ImageParser::Png && format == bimg::TextureFormat::RGBA16)
            {
                continue;
            }
            auto* source = bimg::imageAlloc(&allocator, format, 4, 4, 0, 1, false, true);
            ASSERT_NE(source, nullptr);
            source->m_parser = parser;
            Image image{Babylon::Graphics::NormalizePngImage(allocator, source), bimg::imageFree};
            EXPECT_EQ(image.get(), source);
            EXPECT_EQ(image->m_format, format);
            EXPECT_EQ(image->m_numMips, 3);
        }
    }
}
