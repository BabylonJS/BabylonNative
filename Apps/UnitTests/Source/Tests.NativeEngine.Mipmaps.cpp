#include <gtest/gtest.h>
#include <bimg/encode.h>
#include <bx/allocator.h>

#include <array>
#include <cmath>
#include <memory>

namespace
{
    using Image = std::unique_ptr<bimg::ImageContainer, decltype(&bimg::imageFree)>;

    double DecodeSRGB(double value)
    {
        return value <= 0.04045 ? value / 12.92 : std::pow((value + 0.055) / 1.055, 2.4);
    }

    double EncodeSRGB(double value)
    {
        return value <= 0.0031308 ? value * 12.92 : 1.055 * std::pow(value, 1.0 / 2.4) - 0.055;
    }
}

TEST(NativeEngineMipmaps, SRGBAndLinearMipChains)
{
    bx::DefaultAllocator allocator;
    std::array<uint8_t, 4 * 4 * 4> pixels{};
    for (size_t pixel = 0; pixel < 16; ++pixel)
    {
        for (size_t channel = 0; channel < 4; ++channel)
        {
            pixels[pixel * 4 + channel] = pixel % 2 == 0 ? 0 : 255;
        }
    }
    Image source{bimg::imageAlloc(&allocator, bimg::TextureFormat::RGBA8, 4, 4, 0, 1, false, false, pixels.data()), bimg::imageFree};
    ASSERT_NE(source, nullptr);
    for (bool srgb : {false, true})
    {
        Image output{bimg::imageGenerateMips(&allocator, *source, srgb), bimg::imageFree};
        ASSERT_NE(output, nullptr);
        ASSERT_EQ(output->m_numMips, 3);
        for (uint8_t level = 0; level < output->m_numMips; ++level)
        {
            bimg::ImageMip mip{};
            ASSERT_TRUE(bimg::imageGetRawData(*output, 0, level, output->m_data, output->m_size, mip));
            for (uint32_t i = 0; i < mip.m_size; ++i)
            {
                const uint8_t expected = level == 0 ? pixels[i] : (srgb && i % 4 != 3 ? 188 : 128);
                EXPECT_EQ(mip.m_data[i], expected) << "sRGB " << srgb << ", level " << int(level) << ", byte " << i;
            }
        }
    }
}

TEST(NativeEngineMipmaps, ByteRangeAndTransferFunction)
{
    for (bool srgb : {false, true})
    {
        for (int value = 0; value < 256; ++value)
        {
            std::array<uint8_t, 16> pixels{};
            pixels.fill(static_cast<uint8_t>(value));
            std::array<uint8_t, 4> output{};
            bimg::imageRgba8Downsample2x2(output.data(), 2, 2, 1, 8, 4, pixels.data(), srgb);
            for (uint8_t channel : output)
            {
                EXPECT_EQ(channel, value) << "constant value, sRGB " << srgb;
            }

            for (size_t i = 0; i < pixels.size(); ++i)
            {
                pixels[i] = static_cast<uint8_t>((value + i * 53) % 256);
            }
            bimg::imageRgba8Downsample2x2(output.data(), 2, 2, 1, 8, 4, pixels.data(), srgb);
            for (size_t channel = 0; channel < 4; ++channel)
            {
                const bool decode = srgb && channel != 3;
                double average{};
                for (size_t pixel = 0; pixel < 4; ++pixel)
                {
                    const double sample = pixels[pixel * 4 + channel] / 255.0;
                    average += (decode ? DecodeSRGB(sample) : sample) * 0.25;
                }
                const auto expected = std::lround((decode ? EncodeSRGB(average) : average) * 255.0);
                EXPECT_NEAR(output[channel], expected, 1) << "sRGB " << srgb << ", value " << value << ", channel " << channel;
            }
        }
    }
}
