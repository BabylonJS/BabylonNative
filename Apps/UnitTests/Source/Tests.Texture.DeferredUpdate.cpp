#include <gtest/gtest.h>

#include <Babylon/AppRuntime.h>
#include <Babylon/Graphics/Device.h>
#include <Babylon/Graphics/DeviceContext.h>
#include <Babylon/Graphics/Texture.h>

#include <future>
#include <memory>
#include <optional>

extern Babylon::Graphics::Configuration g_deviceConfig;

namespace
{
    Babylon::Graphics::DeviceContext& GetContext(Babylon::Graphics::Device& device, Babylon::AppRuntime& runtime)
    {
        std::promise<Babylon::Graphics::DeviceContext*> result;
        auto future = result.get_future();
        runtime.Dispatch([&](Napi::Env env) {
            device.AddToJavaScript(env);
            result.set_value(&Babylon::Graphics::DeviceContext::GetFromJavaScript(env));
        });
        return *future.get();
    }

    void InitializeTexture(Babylon::Graphics::Texture& texture)
    {
        texture.Create2D(4, 4, false, 1, bgfx::TextureFormat::RGBA8, BGFX_TEXTURE_NONE);
    }
}

TEST(Texture, DisposeInvalidatesDeferredUpdate)
{
    Babylon::Graphics::Device device{g_deviceConfig};
    Babylon::AppRuntime runtime{};
    auto& context = GetContext(device, runtime);
    device.StartRenderingCurrentFrame();

    Babylon::Graphics::Texture texture{context};
    InitializeTexture(texture);
    auto update = texture.CreateDeferredUpdate();

    texture.Dispose();
    auto disposedUpdate = texture.CreateDeferredUpdate();

    EXPECT_FALSE(update.Matches(disposedUpdate));
    EXPECT_FALSE(update.TryCreate2D(8, 8, 0));
    EXPECT_FALSE(disposedUpdate.TryCreate2D(8, 8, 0));
    EXPECT_FALSE(texture.IsValid());

    device.FinishRenderingCurrentFrame();
}

TEST(Texture, DestructionInvalidatesDeferredUpdate)
{
    Babylon::Graphics::Device device{g_deviceConfig};
    Babylon::AppRuntime runtime{};
    auto& context = GetContext(device, runtime);
    device.StartRenderingCurrentFrame();

    std::optional<Babylon::Graphics::Texture::DeferredUpdate> update;
    {
        auto texture = std::make_unique<Babylon::Graphics::Texture>(context);
        InitializeTexture(*texture);
        update.emplace(texture->CreateDeferredUpdate());
    }

    EXPECT_FALSE(update->TryCreate2D(8, 8, 0));

    device.FinishRenderingCurrentFrame();
}

TEST(Texture, NewResourceGenerationInvalidatesOlderDeferredUpdate)
{
    Babylon::Graphics::Device device{g_deviceConfig};
    Babylon::AppRuntime runtime{};
    auto& context = GetContext(device, runtime);
    device.StartRenderingCurrentFrame();

    Babylon::Graphics::Texture texture{context};
    InitializeTexture(texture);
    auto staleUpdate = texture.CreateDeferredUpdate();
    auto currentUpdate = staleUpdate;

    ASSERT_TRUE(currentUpdate.TryCreate2D(8, 8, 0));
    EXPECT_EQ(texture.Width(), 8);
    EXPECT_EQ(texture.Height(), 8);
    EXPECT_FALSE(staleUpdate.TryCreate2D(16, 16, 0));
    EXPECT_EQ(texture.Width(), 8);
    EXPECT_EQ(texture.Height(), 8);

    texture.Dispose();
    device.FinishRenderingCurrentFrame();
}
