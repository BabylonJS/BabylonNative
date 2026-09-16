#include <gtest/gtest.h>

#include "../../../Polyfills/Canvas/Source/nanovg/fontstash.h"
#include <stb/stb_truetype.h>

#include <array>
#include <fstream>
#include <iterator>
#include <memory>
#include <vector>

TEST(CanvasTextMetrics, ExcludesAtlasBorderAndBlurPaddingFromInkBounds)
{
    std::ifstream file{CANVAS_TEST_FONT_PATH, std::ios::binary};
    ASSERT_TRUE(file.is_open());
    std::vector<unsigned char> bytes{std::istreambuf_iterator<char>{file}, std::istreambuf_iterator<char>{}};
    ASSERT_FALSE(bytes.empty());
    stbtt_fontinfo font{};
    ASSERT_NE(stbtt_InitFont(&font, bytes.data(), 0), 0);

    for (const auto flags : {FONS_ZERO_TOPLEFT, FONS_ZERO_BOTTOMLEFT})
    {
        FONSparams params{};
        params.width = 512;
        params.height = 512;
        params.flags = static_cast<unsigned char>(flags);
        std::unique_ptr<FONScontext, decltype(&fonsDeleteInternal)> stash{fonsCreateInternal(&params), fonsDeleteInternal};
        ASSERT_NE(stash, nullptr);
        const int face = fonsAddFontMem(stash.get(), "test", bytes.data(), static_cast<int>(bytes.size()), 0);
        ASSERT_NE(face, FONS_INVALID);
        fonsSetFont(stash.get(), face);
        fonsSetAlign(stash.get(), FONS_ALIGN_LEFT | FONS_ALIGN_BASELINE);

        for (const float size : {24.0f, 48.0f})
        {
            fonsSetSize(stash.get(), size);
            const float scale = stbtt_ScaleForMappingEmToPixels(&font, size);
            for (const char character : {'A', 'j', 'g'})
            {
                const char text[]{character, '\0'};
                int x0{}, y0{}, x1{}, y1{};
                stbtt_GetCodepointBitmapBox(&font, character, scale, scale, &x0, &y0, &x1, &y1);
                const std::array<float, 4> expected{
                    100.0f + static_cast<float>(x0),
                    flags == FONS_ZERO_TOPLEFT ? 100.0f + static_cast<float>(y0) : 100.0f - static_cast<float>(y1),
                    100.0f + static_cast<float>(x1),
                    flags == FONS_ZERO_TOPLEFT ? 100.0f + static_cast<float>(y1) : 100.0f - static_cast<float>(y0)};
                float unblurredAdvance{};
                for (const float blur : {0.0f, 4.0f, 20.0f, 40.0f})
                {
                    SCOPED_TRACE(::testing::Message() << flags << ", " << size << ", " << character << ", blur=" << blur);
                    fonsSetBlur(stash.get(), blur);
                    std::array<float, 4> actual{};
                    const float advance = fonsTextBounds(stash.get(), 100.0f, 100.0f, text, nullptr, actual.data());
                    EXPECT_EQ(actual, expected);
                    if (blur == 0.0f)
                    {
                        unblurredAdvance = advance;
                    }
                    EXPECT_FLOAT_EQ(advance, unblurredAdvance);
                }
            }

        }
    }
}

TEST(CanvasTextMetrics, WhitespaceAdvancesWithoutContributingInkBounds)
{
    std::ifstream file{CANVAS_TEST_FONT_PATH, std::ios::binary};
    ASSERT_TRUE(file.is_open());
    std::vector<unsigned char> bytes{std::istreambuf_iterator<char>{file}, std::istreambuf_iterator<char>{}};
    ASSERT_FALSE(bytes.empty());

    for (const auto flags : {FONS_ZERO_TOPLEFT, FONS_ZERO_BOTTOMLEFT})
    {
        FONSparams params{};
        params.width = 512;
        params.height = 512;
        params.flags = static_cast<unsigned char>(flags);
        std::unique_ptr<FONScontext, decltype(&fonsDeleteInternal)> stash{fonsCreateInternal(&params), fonsDeleteInternal};
        ASSERT_NE(stash, nullptr);
        const int face = fonsAddFontMem(stash.get(), "test", bytes.data(), static_cast<int>(bytes.size()), 0);
        ASSERT_NE(face, FONS_INVALID);
        fonsSetFont(stash.get(), face);
        fonsSetAlign(stash.get(), FONS_ALIGN_LEFT | FONS_ALIGN_BASELINE);
        fonsSetSize(stash.get(), 48.0f);

        std::array<float, 4> bounds{};
        const float advance = fonsTextBounds(stash.get(), 100.0f, 100.0f, " ", nullptr, bounds.data());
        EXPECT_GT(advance, 0.0f);
        EXPECT_EQ(bounds, (std::array<float, 4>{100.0f, 100.0f, 100.0f, 100.0f}));
    }
}
