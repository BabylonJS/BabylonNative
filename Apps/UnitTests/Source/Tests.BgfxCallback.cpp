#include <gtest/gtest.h>

#include <Babylon/Graphics/BgfxCallback.h>

#include <array>

using Babylon::Graphics::BgfxCallback;

TEST(BgfxCallback, CoalescesScreenshotsAndCapture)
{
    const std::array<uint8_t, 12> pixels{3, 2, 1, 255, 6, 5, 4, 255, 0, 0, 0, 0};
    const BgfxCallback::CaptureData data{2, 1, 12, bgfx::TextureFormat::BGRA8, false, pixels.data(), 12};
    const std::vector<uint8_t> expected{1, 2, 3, 255, 4, 5, 6, 255};

    size_t captures{};
    BgfxCallback callback{[&](const auto& captured) {
        ++captures;
        EXPECT_EQ(captured.Data, pixels.data());
        EXPECT_EQ(captured.Pitch, 12u);
        EXPECT_EQ(captured.Format, bgfx::TextureFormat::BGRA8);
    }};
    size_t screenshots{};
    for (size_t i = 0; i < 2; ++i)
    {
        callback.AddScreenShotCallback([&](const auto& captured) {
            ++screenshots;
            EXPECT_EQ(captured, expected);
        });
    }
    callback.CaptureNextScreenShot();
    callback.CompleteScreenShot(data);
    EXPECT_EQ(captures, 1u);
    EXPECT_EQ(screenshots, 2u);

    callback.AddScreenShotCallback([&](const auto&) { ++screenshots; });
    callback.CompleteScreenShot(data);
    EXPECT_EQ(captures, 1u);
    EXPECT_EQ(screenshots, 3u);
}

TEST(BgfxCallback, CaptureDoesNotRequireScreenshotCallback)
{
    const std::array<uint8_t, 4> pixels{1, 2, 3, 4};
    const BgfxCallback::CaptureData data{1, 1, 4, bgfx::TextureFormat::RGBA8, true, pixels.data(), 4};
    size_t captures{};
    BgfxCallback callback{[&](const auto& captured) {
        ++captures;
        EXPECT_TRUE(captured.YFlip);
        EXPECT_EQ(captured.DataSize, pixels.size());
    }};
    callback.CaptureNextScreenShot();
    callback.CompleteScreenShot(data);
    EXPECT_EQ(captures, 1u);
}

TEST(BgfxCallback, NormalizesFlippedRgbaScreenshots)
{
    const std::array<uint8_t, 16> pixels{1, 2, 3, 4, 0, 0, 0, 0, 5, 6, 7, 8, 0, 0, 0, 0};
    const BgfxCallback::CaptureData data{1, 2, 8, bgfx::TextureFormat::RGBA8, true, pixels.data(), 16};
    BgfxCallback callback{[](const auto&) {}};
    callback.AddScreenShotCallback([](const auto& captured) {
        EXPECT_EQ(captured, (std::vector<uint8_t>{5, 6, 7, 8, 1, 2, 3, 4}));
    });
    callback.CompleteScreenShot(data);
}
