#include <gtest/gtest.h>

#include <Babylon/Graphics/BgfxCallback.h>

#include <array>
#include <optional>
#include <system_error>

using Babylon::Graphics::BgfxCallback;

namespace
{
    class TestBgfxCallback : public BgfxCallback
    {
    public:
        using BgfxCallback::BgfxCallback;
        using BgfxCallback::fatal;
    };
}

TEST(BgfxCallback, DeviceLossIsLatchedUntilCleared)
{
    TestBgfxCallback callback{[](const auto&) {}};
    EXPECT_FALSE(callback.IsDeviceLost());
    callback.fatal(__FILE__, __LINE__, bgfx::Fatal::DeviceLost, "simulated device loss");
    EXPECT_TRUE(callback.IsDeviceLost());
    callback.ClearDeviceLost();
    EXPECT_FALSE(callback.IsDeviceLost());
}

TEST(BgfxCallback, FailedScreenshotsAreNotReplayedAfterDeviceLoss)
{
    const std::array<uint8_t, 4> pixels{1, 2, 3, 4};
    const BgfxCallback::CaptureData data{1, 1, 4, bgfx::TextureFormat::RGBA8, false, pixels.data(), 4};
    size_t captures{};
    TestBgfxCallback callback{[&](const auto&) { ++captures; }};
    size_t canceled{};
    for (size_t i = 0; i < 2; ++i)
    {
        callback.AddScreenShotCallback([&](auto result) {
            ++canceled;
            ASSERT_TRUE(result.has_error());
            try
            {
                std::rethrow_exception(result.error());
            }
            catch (const std::system_error& error)
            {
                EXPECT_EQ(error.code(), std::errc::operation_canceled);
            }
        });
    }
    callback.CaptureNextScreenShot();

    EXPECT_TRUE(callback.HasPendingScreenShotCallbacks());
    callback.fatal(__FILE__, __LINE__, bgfx::Fatal::DeviceLost, "simulated device loss");
    callback.CompleteScreenShot(data);
    EXPECT_EQ(canceled, 0u);
    EXPECT_EQ(captures, 0u);
    callback.CancelScreenShots(std::make_exception_ptr(std::system_error(std::make_error_code(std::errc::operation_canceled))));
    EXPECT_EQ(canceled, 2u);
    EXPECT_FALSE(callback.HasPendingScreenShotCallbacks());

    callback.ClearDeviceLost();
    size_t successful{};
    callback.AddScreenShotCallback([&](auto result) {
        ++successful;
        ASSERT_FALSE(result.has_error());
        EXPECT_EQ(result.value(), (std::vector<uint8_t>{1, 2, 3, 4}));
    });
    callback.CompleteScreenShot(data);

    EXPECT_EQ(successful, 1u);
    EXPECT_EQ(canceled, 2u);
    EXPECT_EQ(captures, 0u);
    EXPECT_FALSE(callback.HasPendingScreenShotCallbacks());
}

TEST(BgfxCallback, OtherFatalErrorsStillAbort)
{
    TestBgfxCallback callback{[](const auto&) {}};
    EXPECT_DEATH(callback.fatal(__FILE__, __LINE__, bgfx::Fatal::UnableToInitialize, "simulated init failure"), "FATAL");
}

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
            ASSERT_FALSE(captured.has_error());
            EXPECT_EQ(captured.value(), expected);
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
        ASSERT_FALSE(captured.has_error());
        EXPECT_EQ(captured.value(), (std::vector<uint8_t>{5, 6, 7, 8, 1, 2, 3, 4}));
    });
    callback.CompleteScreenShot(data);
}
