#include <gtest/gtest.h>

#include <Babylon/AppRuntime.h>
#include <Babylon/Graphics/Device.h>
#include <Babylon/Graphics/DeviceContext.h>
#include <Babylon/Graphics/FrameBuffer.h>
#include <Babylon/Graphics/Texture.h>

#include <array>
#include <chrono>
#include <cstdlib>
#include <future>
#include <memory>
#include <stdexcept>
#include <vector>

extern Babylon::Graphics::Configuration g_deviceConfig;

TEST(NativeEngineIntegerClear, PartialClearPreservesMixedFramebufferAttachments)
{
#if defined(USE_NOOP_METAL_DEVICE) || defined(SKIP_RENDER_TESTS)
    GTEST_SKIP() << "GPU rendering/readback is unavailable in this test configuration";
#else
    Babylon::Graphics::Device device{g_deviceConfig};
    device.StartRenderingCurrentFrame();
    if (bgfx::getRendererType() == bgfx::RendererType::Noop)
    {
        device.FinishRenderingCurrentFrame();
        GTEST_SKIP() << "The no-op test device does not execute GPU clears or readbacks";
    }

    constexpr uint16_t size = 8;
    constexpr uint64_t renderTargetFlags = BGFX_TEXTURE_RT;
    if (!bgfx::isTextureValid(0, false, 1, bgfx::TextureFormat::RGBA8, renderTargetFlags) ||
        !bgfx::isTextureValid(0, false, 1, bgfx::TextureFormat::RGBA16U, renderTargetFlags) ||
        !bgfx::isTextureValid(0, false, 1, bgfx::TextureFormat::D32F, BGFX_TEXTURE_RT_WRITE_ONLY) ||
        !bgfx::isTextureValid(0, false, 1, bgfx::TextureFormat::RGBA8, BGFX_TEXTURE_BLIT_DST | BGFX_TEXTURE_READ_BACK))
    {
        device.FinishRenderingCurrentFrame();
        GTEST_SKIP() << "Mixed integer framebuffer rendering and RGBA8 readback are required";
    }

    std::vector<uint8_t> pixels(size * size * 4);
    std::promise<void> completed;
    auto future = completed.get_future();
    Babylon::AppRuntime runtime{};
    runtime.Dispatch([&](Napi::Env env) {
        try
        {
            device.AddToJavaScript(env);
            auto& context = Babylon::Graphics::DeviceContext::GetFromJavaScript(env);
            auto scope = context.AcquireFrameCompletionScope();

            Babylon::Graphics::Texture color{context};
            Babylon::Graphics::Texture integer{context};
            Babylon::Graphics::Texture depth{context};
            color.Create2D(size, size, false, 1, bgfx::TextureFormat::RGBA8, renderTargetFlags);
            integer.Create2D(size, size, false, 1, bgfx::TextureFormat::RGBA16U, renderTargetFlags);
            depth.Create2D(size, size, false, 1, bgfx::TextureFormat::D32F, BGFX_TEXTURE_RT_WRITE_ONLY);

            std::array<bgfx::Attachment, 3> attachments{};
            attachments[0].init(color.Handle(), bgfx::Access::Write, 0, 1, 0, BGFX_ATTACHMENT_NONE);
            attachments[1].init(integer.Handle(), bgfx::Access::Write, 0, 1, 0, BGFX_ATTACHMENT_NONE);
            attachments[2].init(depth.Handle(), bgfx::Access::Write, 0, 1, 0, BGFX_ATTACHMENT_NONE);
            Babylon::Graphics::FrameBuffer frameBuffer{
                context, bgfx::createFrameBuffer(static_cast<uint8_t>(attachments.size()), attachments.data()),
                size, size, false, true, false};
            if (!bgfx::isValid(frameBuffer.Handle()))
            {
                throw std::runtime_error{"Failed to create a mixed integer framebuffer"};
            }

            constexpr std::array<uint8_t, 4> outside{17, 34, 51, 255};
            constexpr std::array<uint8_t, 4> inside{170, 187, 204, 255};
            frameBuffer.Clear(*context.GetActiveEncoder(), BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH,
                outside[0] / 255.0f, outside[1] / 255.0f, outside[2] / 255.0f, outside[3] / 255.0f, 0.75f, 0);
            frameBuffer.SetScissor(2, 2, 4, 4);
            frameBuffer.Clear(*context.GetActiveEncoder(), BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH,
                inside[0] / 255.0f, inside[1] / 255.0f, inside[2] / 255.0f, inside[3] / 255.0f, 0.25f, 0);

            auto readback = std::make_shared<Babylon::Graphics::Texture>(context);
            readback->Create2D(size, size, false, 1, bgfx::TextureFormat::RGBA8,
                BGFX_TEXTURE_BLIT_DST | BGFX_TEXTURE_READ_BACK);
            bgfx::TextureRegion source{};
            source.init(color.Handle());
            bgfx::TextureRegion destination{};
            destination.init(readback->Handle());
            context.GetActiveEncoder()->blit(context.AcquireNewViewId(), destination, source);
            context.ReadTextureAsync(readback->Handle(), pixels)
                .then(arcana::inline_scheduler, arcana::cancellation::none(),
                    [readback, &completed](const arcana::expected<void, std::exception_ptr>& result) {
                        if (result.has_error())
                        {
                            completed.set_exception(result.error());
                        }
                        else
                        {
                            completed.set_value();
                        }
                    });
        }
        catch (...)
        {
            completed.set_exception(std::current_exception());
        }
    });

    const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds{30};
    while (future.wait_for(std::chrono::milliseconds{16}) != std::future_status::ready)
    {
        if (std::chrono::steady_clock::now() >= deadline)
        {
            ADD_FAILURE() << "Timed out waiting for the mixed framebuffer readback";
            std::quick_exit(1);
        }
        device.FinishRenderingCurrentFrame();
        device.StartRenderingCurrentFrame();
    }
    ASSERT_NO_THROW(future.get());
    device.FinishRenderingCurrentFrame();

    constexpr std::array<uint8_t, 4> outside{17, 34, 51, 255};
    constexpr std::array<uint8_t, 4> inside{170, 187, 204, 255};
    for (uint16_t y = 0; y < size; ++y)
    {
        for (uint16_t x = 0; x < size; ++x)
        {
            const auto& expected = x >= 2 && x < 6 && y >= 2 && y < 6 ? inside : outside;
            const size_t offset = (static_cast<size_t>(y) * size + x) * 4;
            const std::array<uint8_t, 4> actual{
                pixels[offset], pixels[offset + 1], pixels[offset + 2], pixels[offset + 3]};
            EXPECT_EQ(actual, expected)
                << "Unexpected color at (" << x << ", " << y << ")";
        }
    }
#endif
}
