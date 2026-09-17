#include "../../../Plugins/NativeXr/Source/ImageTracking.h"
#include "../../../Dependencies/xr/Source/ARKit/ImageTrackingValidationState.h"

#include <Babylon/AppRuntime.h>
#include <gtest/gtest.h>

#include <future>
#include <limits>
#include <thread>

#ifdef __APPLE__
#include "../../../Dependencies/xr/Source/ARKit/ImageTrackingData.h"
#endif

namespace
{
    template<typename Callback>
    void RunImageTest(Callback callback)
    {
        Babylon::AppRuntime runtime{};
        std::promise<void> completed;
        auto future{completed.get_future()};
        runtime.Dispatch([&](Napi::Env env) {
            try
            {
                callback(env);
                completed.set_value();
            }
            catch (...)
            {
                completed.set_exception(std::current_exception());
            }
        });
        future.get();
    }

    Napi::Object MakeRequest(Napi::Env env, Napi::Value data, uint32_t width = 2, uint32_t height = 2)
    {
        auto image{Napi::Object::New(env)};
        image.Set("data", data);
        image.Set("width", width);
        image.Set("height", height);
        image.Set("depth", 1);
        auto request{Napi::Object::New(env)};
        request.Set("image", image);
        request.Set("widthInMeters", 0.25);
        return request;
    }
}

TEST(NativeXrImageTracking, SnapshotOwnsOnlyTheTypedArrayView)
{
    xr::System::Session::ImageTrackingRequest snapshot;
    RunImageTest([&](Napi::Env env) {
        auto bytes{Napi::Uint8Array::New(env, 24)};
        for (size_t i = 0; i < bytes.ElementLength(); ++i)
        {
            bytes[i] = static_cast<uint8_t>(i);
        }
        auto view{Napi::Uint8Array::New(env, 16, bytes.ArrayBuffer(), 4)};
        snapshot = Babylon::Plugins::CreateImageTrackingRequest(MakeRequest(env, view));
        bytes[4] = 255;
        EXPECT_EQ(snapshot.stride, 8u);
        EXPECT_EQ(snapshot.measuredWidthInMeters, 0.25f);
    });

    // The JS runtime and its buffers are gone; the request must still own its snapshot.
    ASSERT_EQ(snapshot.data->size(), 16u);
    for (size_t i = 0; i < snapshot.data->size(); ++i)
    {
        EXPECT_EQ((*snapshot.data)[i], i + 4);
    }
    auto copied{snapshot};
    snapshot = {};
    EXPECT_EQ(copied.data->front(), 4u);
}

TEST(NativeXrImageTracking, AcceptsSupportedByteLayouts)
{
    RunImageTest([](Napi::Env env) {
        for (const size_t pixelStride : {1, 2, 3, 4, 6, 8})
        {
            auto request{MakeRequest(env, Napi::Uint8Array::New(env, 4 * pixelStride))};
            EXPECT_EQ(Babylon::Plugins::CreateImageTrackingRequest(request).stride, 2 * pixelStride);
        }
        auto clamped{Napi::Uint8Array::New(env, 16, napi_uint8_clamped_array)};
        EXPECT_EQ(Babylon::Plugins::CreateImageTrackingRequest(MakeRequest(env, clamped)).data->size(), 16u);
    });
}

TEST(NativeXrImageTracking, RejectsInvalidDimensionsAndPhysicalWidth)
{
    RunImageTest([](Napi::Env env) {
        for (const char* dimension : {"width", "height", "depth"})
        {
            for (const double value : {0.0, -1.0, 0.5, 2147483648.0, 4294967297.0,
                std::numeric_limits<double>::infinity(), std::numeric_limits<double>::quiet_NaN()})
            {
                auto request{MakeRequest(env, Napi::Uint8Array::New(env, 16))};
                request.Get("image").As<Napi::Object>().Set(dimension, value);
                EXPECT_THROW(Babylon::Plugins::CreateImageTrackingRequest(request), Napi::Error);
            }
        }
        for (const double value : {-1.0, std::numeric_limits<double>::infinity(), std::numeric_limits<double>::quiet_NaN()})
        {
            auto request{MakeRequest(env, Napi::Uint8Array::New(env, 16))};
            request.Set("widthInMeters", value);
            EXPECT_THROW(Babylon::Plugins::CreateImageTrackingRequest(request), Napi::Error);
        }
        auto request{MakeRequest(env, Napi::Uint8Array::New(env, 16))};
        request.Set("widthInMeters", 0);
        EXPECT_EQ(Babylon::Plugins::CreateImageTrackingRequest(request).measuredWidthInMeters, 0);
    });
}

TEST(NativeXrImageTracking, RejectsUnsupportedPixelStorage)
{
    RunImageTest([](Napi::Env env) {
        for (const size_t size : {0, 3, 6, 20, 28})
        {
            EXPECT_THROW(Babylon::Plugins::CreateImageTrackingRequest(MakeRequest(env, Napi::Uint8Array::New(env, size))), Napi::Error);
        }
        EXPECT_THROW(Babylon::Plugins::CreateImageTrackingRequest(MakeRequest(env, Napi::Uint16Array::New(env, 8))), Napi::Error);
        EXPECT_THROW(Babylon::Plugins::CreateImageTrackingRequest(MakeRequest(env, Napi::ArrayBuffer::New(env, 16))), Napi::Error);
        auto request{MakeRequest(env, Napi::Uint8Array::New(env, 16))};
        request.Get("image").As<Napi::Object>().Set("depth", 2);
        EXPECT_THROW(Babylon::Plugins::CreateImageTrackingRequest(request), Napi::Error);
    });
}

TEST(NativeXrImageTracking, PublishesScoresOnlyAfterAllValidationsIncludingUntrackable)
{
    auto state{std::make_shared<xr::ImageTrackingValidationState>()};
    state->Scores.resize(2);
    std::vector<arcana::task_completion_source<int, std::exception_ptr>> completions(2);
    std::vector<arcana::task<int, std::exception_ptr>> tasks;
    for (auto& completion : completions)
    {
        tasks.emplace_back(completion.as_task());
    }
    arcana::when_all(gsl::make_span(tasks)).then(arcana::inline_scheduler, arcana::cancellation::none(),
        [state](std::vector<int>) { state->Complete([] {}); });

    std::thread first{[state, completion{completions[0]}]() mutable {
        state->Scores[0] = xr::ImageTrackingScore::UNTRACKABLE;
        completion.complete(-1);
    }};
    first.join();
    EXPECT_EQ(state->GetScores(), nullptr);
    std::thread second{[state, completion{completions[1]}]() mutable {
        state->Scores[1] = xr::ImageTrackingScore::UNTRACKABLE;
        completion.complete(-1);
    }};
    second.join();
    ASSERT_NE(state->GetScores(), nullptr);
    EXPECT_EQ(*state->GetScores(), (std::vector<xr::ImageTrackingScore>(2, xr::ImageTrackingScore::UNTRACKABLE)));
}

TEST(NativeXrImageTracking, LateCompletionCannotConfigureEndedOrReplacedSession)
{
    auto state{std::make_shared<xr::ImageTrackingValidationState>()};
    std::weak_ptr<xr::ImageTrackingValidationState> oldState{state};
    arcana::task_completion_source<int, std::exception_ptr> completion;
    bool applied{};
    completion.as_task().then(arcana::inline_scheduler, arcana::cancellation::none(),
        [state, &applied](int) { state->Complete([&] { applied = true; }); });
    state->Deactivate();
    state = std::make_shared<xr::ImageTrackingValidationState>();
    EXPECT_FALSE(oldState.expired());
    completion.complete(0);
    EXPECT_FALSE(applied);
    EXPECT_EQ(state->GetScores(), nullptr);
    EXPECT_TRUE(oldState.expired());
}

#ifdef __APPLE__
TEST(NativeXrImageTracking, CoreGraphicsRetainsPixelsUntilProviderRelease)
{
    auto pixels{std::make_shared<const std::vector<uint8_t>>(std::initializer_list<uint8_t>{1, 2, 3, 4})};
    std::weak_ptr<const std::vector<uint8_t>> weakPixels{pixels};
    auto provider{xr::CreateImageTrackingDataProvider(pixels)};
    ASSERT_NE(provider, nullptr);
    pixels.reset();
    EXPECT_FALSE(weakPixels.expired());
    const auto data{CGDataProviderCopyData(provider)};
    ASSERT_NE(data, nullptr);
    EXPECT_EQ(CFDataGetLength(data), 4);
    EXPECT_EQ(CFDataGetBytePtr(data)[3], 4);
    CFRelease(data);
    CGDataProviderRelease(provider);
    EXPECT_TRUE(weakPixels.expired());
}
#endif
