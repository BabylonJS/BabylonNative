#include <gtest/gtest.h>

#include <Babylon/AppRuntime.h>
#include <Babylon/Graphics/Device.h>
#include <Babylon/Graphics/Texture.h>
#include <Babylon/Plugins/ExternalTexture.h>
#include <napi/pointer.h>

#include <winrt/base.h>

#include <future>

extern Babylon::Graphics::Configuration g_deviceConfig;

TEST(ExternalTexture, PreservesNativeFormatForBlits)
{
    Babylon::Graphics::Device device{g_deviceConfig};

    device.StartRenderingCurrentFrame();

    // A typeless resource is the D3D11 way to sample linear storage through an sRGB view.
    D3D11_TEXTURE2D_DESC desc{};
    desc.Width = 256;
    desc.Height = 256;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_TYPELESS;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    winrt::com_ptr<ID3D11Texture2D> nativeTexture;
    ASSERT_HRESULT_SUCCEEDED(device.GetPlatformInfo().Device->CreateTexture2D(&desc, nullptr, nativeTexture.put()));

    Babylon::Plugins::ExternalTexture externalTexture{nativeTexture.get(), DXGI_FORMAT_R8G8B8A8_UNORM_SRGB};
    nativeTexture = nullptr;

    std::promise<void> done{};

    Babylon::AppRuntime runtime{};
    runtime.Dispatch([&device, &done, externalTexture](Napi::Env env) {
        device.AddToJavaScript(env);

        auto jsTexture = externalTexture.CreateForJavaScript(env);
        auto* texture = jsTexture.As<Napi::Pointer<Babylon::Graphics::Texture>>().Get();

        EXPECT_EQ(texture->Format(), bgfx::TextureFormat::RGBA8);
        EXPECT_NE(texture->Flags() & BGFX_TEXTURE_SRGB, 0u);
        EXPECT_EQ(texture->BlitFormat(), bgfx::TextureFormat::RGBA8);
        EXPECT_EQ(texture->BlitFlags() & BGFX_TEXTURE_SRGB, 0u);

        done.set_value();
    });

    done.get_future().wait();

    device.FinishRenderingCurrentFrame();
}
