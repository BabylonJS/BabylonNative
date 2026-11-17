#include <gtest/gtest.h>

#include <Babylon/AppRuntime.h>
#include <Babylon/Graphics/Device.h>
#include <Babylon/Graphics/Texture.h>
#include <Babylon/Polyfills/Console.h>
#include <Babylon/Polyfills/Window.h>
#include <Babylon/Plugins/ExternalTexture.h>
#include <Babylon/Plugins/NativeEngine.h>
#include <napi/pointer.h>

#include <winrt/base.h>

#include <array>
#include <chrono>
#include <cstring>
#include <future>
#include <iostream>
#include <stdexcept>

extern Babylon::Graphics::Configuration g_deviceConfig;

namespace
{
    winrt::com_ptr<ID3D12Resource> CreateR32TypelessTexture(ID3D12Device* device, float value)
    {
        D3D12_RESOURCE_DESC textureDesc{};
        textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        textureDesc.Width = 1;
        textureDesc.Height = 1;
        textureDesc.DepthOrArraySize = 1;
        textureDesc.MipLevels = 1;
        textureDesc.Format = DXGI_FORMAT_R32_TYPELESS;
        textureDesc.SampleDesc.Count = 1;
        textureDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;

        D3D12_HEAP_PROPERTIES defaultHeap{};
        defaultHeap.Type = D3D12_HEAP_TYPE_DEFAULT;
        defaultHeap.CreationNodeMask = 1;
        defaultHeap.VisibleNodeMask = 1;

        winrt::com_ptr<ID3D12Resource> texture;
        winrt::check_hresult(device->CreateCommittedResource(
            &defaultHeap,
            D3D12_HEAP_FLAG_NONE,
            &textureDesc,
            D3D12_RESOURCE_STATE_COPY_DEST,
            nullptr,
            IID_PPV_ARGS(texture.put())));

        D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint{};
        UINT64 uploadSize{};
        device->GetCopyableFootprints(&textureDesc, 0, 1, 0, &footprint, nullptr, nullptr, &uploadSize);

        D3D12_RESOURCE_DESC uploadDesc{};
        uploadDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        uploadDesc.Width = uploadSize;
        uploadDesc.Height = 1;
        uploadDesc.DepthOrArraySize = 1;
        uploadDesc.MipLevels = 1;
        uploadDesc.SampleDesc.Count = 1;
        uploadDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

        D3D12_HEAP_PROPERTIES uploadHeap{};
        uploadHeap.Type = D3D12_HEAP_TYPE_UPLOAD;
        uploadHeap.CreationNodeMask = 1;
        uploadHeap.VisibleNodeMask = 1;

        winrt::com_ptr<ID3D12Resource> upload;
        winrt::check_hresult(device->CreateCommittedResource(
            &uploadHeap,
            D3D12_HEAP_FLAG_NONE,
            &uploadDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(upload.put())));

        void* mapped{};
        winrt::check_hresult(upload->Map(0, nullptr, &mapped));
        std::memcpy(static_cast<uint8_t*>(mapped) + footprint.Offset, &value, sizeof(value));
        upload->Unmap(0, nullptr);

        winrt::com_ptr<ID3D12CommandQueue> queue;
        D3D12_COMMAND_QUEUE_DESC queueDesc{};
        queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        winrt::check_hresult(device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(queue.put())));

        winrt::com_ptr<ID3D12CommandAllocator> allocator;
        winrt::check_hresult(device->CreateCommandAllocator(
            D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(allocator.put())));

        winrt::com_ptr<ID3D12GraphicsCommandList> commandList;
        winrt::check_hresult(device->CreateCommandList(
            0,
            D3D12_COMMAND_LIST_TYPE_DIRECT,
            allocator.get(),
            nullptr,
            IID_PPV_ARGS(commandList.put())));

        D3D12_TEXTURE_COPY_LOCATION source{};
        source.pResource = upload.get();
        source.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
        source.PlacedFootprint = footprint;

        D3D12_TEXTURE_COPY_LOCATION destination{};
        destination.pResource = texture.get();
        destination.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;

        commandList->CopyTextureRegion(&destination, 0, 0, 0, &source, nullptr);

        D3D12_RESOURCE_BARRIER barrier{};
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Transition.pResource = texture.get();
        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
        barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COMMON;
        barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        commandList->ResourceBarrier(1, &barrier);
        winrt::check_hresult(commandList->Close());

        ID3D12CommandList* commandLists[]{commandList.get()};
        queue->ExecuteCommandLists(1, commandLists);

        winrt::com_ptr<ID3D12Fence> fence;
        winrt::check_hresult(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence.put())));
        winrt::check_hresult(queue->Signal(fence.get(), 1));

        winrt::handle event{CreateEvent(nullptr, FALSE, FALSE, nullptr)};
        winrt::check_pointer(event.get());
        winrt::check_hresult(fence->SetEventOnCompletion(1, event.get()));
        WaitForSingleObject(event.get(), INFINITE);

        return texture;
    }
}

TEST(ExternalTexture, ReadsTypelessR32ThroughFloatOverride)
{
    Babylon::Graphics::Device device{g_deviceConfig};
    device.StartRenderingCurrentFrame();

    constexpr float SOURCE_VALUE{0.25f};
    auto nativeTexture = CreateR32TypelessTexture(device.GetPlatformInfo().Device, SOURCE_VALUE);
    Babylon::Plugins::ExternalTexture externalTexture{nativeTexture.get(), DXGI_FORMAT_R32_FLOAT};
    nativeTexture = nullptr;

    std::promise<void> readQueued{};
    std::promise<std::array<float, 4>> readCompleted{};

    Babylon::AppRuntime runtime{};
    runtime.Dispatch([&device, &readQueued, &readCompleted, externalTexture](Napi::Env env) {
        try
        {
            env.Global().Set("globalThis", env.Global());
            device.AddToJavaScript(env);
            Babylon::Polyfills::Console::Initialize(env, [](const char* message, auto) {
                std::cout << message << std::endl;
            });
            Babylon::Polyfills::Window::Initialize(env);
            Babylon::Plugins::NativeEngine::Initialize(env);

            auto jsTexture = externalTexture.CreateForJavaScript(env);
            auto* texture = jsTexture.As<Napi::Pointer<Babylon::Graphics::Texture>>().Get();
            EXPECT_EQ(texture->Format(), bgfx::TextureFormat::R32F);
            EXPECT_EQ(texture->BlitFormat(), bgfx::TextureFormat::R32F);

            env.Global().Set("__externalTexture", jsTexture);
            auto promise = Napi::Eval(env, R"(
                (function () {
                    var engine = new _native.Engine();
                    var output = new ArrayBuffer(16);
                    return engine.readTexture2(__externalTexture, 0, 0, 0, 1, 1, output, 0, 16, -1)
                        .then(function (buffer) {
                            engine.dispose();
                            return buffer;
                        });
                })()
            )", "external-texture-readback-test.js").As<Napi::Promise>();

            auto onFulfilled = Napi::Function::New(env, [&readCompleted](const Napi::CallbackInfo& info) {
                const auto buffer = info[0].As<Napi::ArrayBuffer>();
                if (buffer.ByteLength() != sizeof(float) * 4)
                {
                    readCompleted.set_exception(std::make_exception_ptr(std::runtime_error{"Unexpected readback size"}));
                    return;
                }

                const auto* values = static_cast<const float*>(buffer.Data());
                readCompleted.set_value({values[0], values[1], values[2], values[3]});
            });
            auto onRejected = Napi::Function::New(env, [&readCompleted](const Napi::CallbackInfo& info) {
                readCompleted.set_exception(std::make_exception_ptr(
                    std::runtime_error{Napi::GetErrorString(info[0].As<Napi::Error>())}));
            });
            promise.Get("then").As<Napi::Function>().Call(promise, {onFulfilled, onRejected});
            readQueued.set_value();
        }
        catch (...)
        {
            readQueued.set_exception(std::current_exception());
        }
    });

    ASSERT_NO_THROW(readQueued.get_future().get());

    auto readFuture = readCompleted.get_future();
    const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds{30};
    while (readFuture.wait_for(std::chrono::milliseconds{16}) != std::future_status::ready &&
        std::chrono::steady_clock::now() < deadline)
    {
        device.FinishRenderingCurrentFrame();
        device.StartRenderingCurrentFrame();
    }
    device.FinishRenderingCurrentFrame();

    ASSERT_EQ(readFuture.wait_for(std::chrono::seconds{0}), std::future_status::ready);
    const auto values = readFuture.get();
    EXPECT_FLOAT_EQ(values[0], SOURCE_VALUE);
    EXPECT_FLOAT_EQ(values[1], 0.0f);
    EXPECT_FLOAT_EQ(values[2], 0.0f);
    EXPECT_FLOAT_EQ(values[3], 1.0f);
}
