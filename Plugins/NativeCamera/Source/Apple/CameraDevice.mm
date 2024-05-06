#if ! __has_feature(objc_arc)
#error "ARC is off"
#endif

#import <MetalKit/MetalKit.h>
#include <napi/napi.h>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <arcana/macros.h>
#include <arcana/threading/task.h>
#include <arcana/threading/dispatcher.h>
#include <Babylon/JsRuntimeScheduler.h>
#include <Babylon/Graphics/DeviceContext.h>
#include <arcana/threading/task_schedulers.h>
#include <memory>
#include <Foundation/Foundation.h>
#include <AVFoundation/AVFoundation.h>

#include "../CameraDevice.h"

@class CameraTextureDelegate;

enum class VideoOrientation
{
    LandscapeRight,
    LandscapeLeft,
    Portrait,
    PortraitUpsideDown
};

CGFloat VideoOrientationToRotationAngle(VideoOrientation orientation)
{
    switch (orientation)
    {
        case VideoOrientation::LandscapeLeft:
            return -90;
        case VideoOrientation::LandscapeRight:
            return 90;
        case VideoOrientation::PortraitUpsideDown:
            return 180;
        case VideoOrientation::Portrait:
        default:
            return 0;
    }
}

@interface CameraTextureDelegate : NSObject <AVCaptureVideoDataOutputSampleBufferDelegate>
{
    @public VideoOrientation Orientation;
    CVMetalTextureCacheRef textureCache;
    CVMetalTextureRef cameraTextureY;
    CVMetalTextureRef cameraTextureCbCr;
}

- (id)init:(CVMetalTextureCacheRef)textureCache;
- (id<MTLTexture>)getCameraTextureY;
- (id<MTLTexture>)getCameraTextureCbCr;
- (void)reset;

@end

@class PhotoCaptureDelegate;

using TakePhotoTaskCompletionSource = arcana::task_completion_source<Babylon::Plugins::CameraDevice::TakePhotoTask::result_type, Babylon::Plugins::CameraDevice::TakePhotoTask::error_type>;

@interface PhotoCaptureDelegate : NSObject <AVCapturePhotoCaptureDelegate>
{
    TakePhotoTaskCompletionSource taskCompletionSource;
    VideoOrientation orientation;
}

- (id)init:(TakePhotoTaskCompletionSource)taskCompletionSource orientation:(VideoOrientation)orientation;

@end

namespace {
    static bool isPixelFormatSupported(uint32_t pixelFormat)
    {
        switch (pixelFormat) {
            case kCVPixelFormatType_420YpCbCr8BiPlanarVideoRange:
            case kCVPixelFormatType_420YpCbCr8BiPlanarFullRange:
                return true;
        }
        return false;
    }

    struct Vertex {
        vector_float2 position;
        vector_float2 uv;
    };

    // The shader will use different UV coordinates to rotate the video from its natural sensor orientation to the
    // UI orientation. The format is 2D posistions to UV coordinates.
    constexpr Vertex vertices_portrait[] = {
        {{-1, -1},   {0, 1}},
        {{-1, 1},    {1, 1}},
        {{1, -1},    {0, 0}},
        {{1, 1},     {1, 0}},
    };
    constexpr Vertex vertices_landscape_right[] = {
        {{-1, -1},   {0, 0}},
        {{-1, 1},    {0, 1}},
        {{1, -1},    {1, 0}},
        {{1, 1},     {1, 1}},
    };
    constexpr Vertex vertices_landscape_left[] = {
        {{-1, -1},   {1, 1}},
        {{-1, 1},    {1, 0}},
        {{1, -1},    {0, 1}},
        {{1, 1},     {0, 0}},
    };
    constexpr Vertex vertices_portrait_upsideddown[] = {
        {{-1, -1},   {1, 0}},
        {{-1, 1},    {0, 0}},
        {{1, -1},    {1, 1}},
        {{1, 1},     {0, 1}},
    };

    constexpr char shaderSource[] = R"(
        #include <metal_stdlib>
        #include <simd/simd.h>

        using namespace metal;

        #include <simd/simd.h>

        typedef struct
        {
            vector_float2 position;
            vector_float2 uv;
        } Vertex;

        typedef struct
        {
            float4 position [[position]];
            float2 uv;
        } RasterizerData;

        vertex RasterizerData
        vertexShader(uint vertexID [[vertex_id]],
                        constant Vertex* vertices [[buffer(0)]])
        {
            RasterizerData out;
            out.position = vector_float4(vertices[vertexID].position.xy, 0.0, 1.0);
            out.uv = vertices[vertexID].uv;
            return out;
        }

        fragment float4 fragmentShader(RasterizerData in [[stage_in]],
            texture2d<float, access::sample> cameraTextureY [[ texture(1) ]],
            texture2d<float, access::sample> cameraTextureCbCr [[ texture(2) ]])
        {
            constexpr sampler linearSampler(mip_filter::linear, mag_filter::linear, min_filter::linear);

            if (!is_null_texture(cameraTextureY) && !is_null_texture(cameraTextureCbCr))
            {
                const float4 cameraSampleY = cameraTextureY.sample(linearSampler, in.uv);
                const float4 cameraSampleCbCr = cameraTextureCbCr.sample(linearSampler, in.uv);

                const float4x4 ycbcrToRGBTransform = float4x4(
                    float4(+1.0000f, +1.0000f, +1.0000f, +0.0000f),
                    float4(+0.0000f, -0.3441f, +1.7720f, +0.0000f),
                    float4(+1.4020f, -0.7141f, +0.0000f, +0.0000f),
                    float4(-0.7010f, +0.5291f, -0.8860f, +1.0000f)
                );

                float4 ycbcr = float4(cameraSampleY.r, cameraSampleCbCr.rg, 1.0);
                float4 cameraSample = ycbcrToRGBTransform * ycbcr;
                cameraSample.a = 1.0;

                return cameraSample;
            }
            else
            {
                return 0;
            }
        }
    )";

    static id<MTLLibrary> CompileShader(id<MTLDevice> metalDevice, const char* source) {
        NSError* error;
        id<MTLLibrary> lib = [metalDevice newLibraryWithSource:@(source) options:nil error:&error];
        if(nil != error) {
            throw std::runtime_error{[error.localizedDescription cStringUsingEncoding:NSASCIIStringEncoding]};
        }
        return lib;
    }
}

namespace Babylon::Plugins
{
    struct CameraTrack::Impl
    {
        int32_t width{};
        int32_t height{};
        AVCaptureDeviceFormat* avDeviceFormat{};
        uint32_t pixelFormat{};
    };

    struct CameraDevice::Impl
    {
        Impl(Napi::Env env)
            : deviceContext{nullptr}
            , env{env}
        {
        }

        Graphics::DeviceContext* deviceContext;
        Napi::Env env;

        arcana::affinity threadAffinity{};

        std::vector<CameraTrack> supportedResolutions{};
        std::vector<std::unique_ptr<Capability>> capabilities{};
        std::vector<CMVideoDimensions> supportedMaxPhotoDimensions{};
        std::optional<Plugins::PhotoCapabilities> photoCapabilities{};
        std::optional<Plugins::PhotoSettings> defaultPhotoSettings{};
        AVCaptureDevice* avDevice{};

        bool overrideCameraTexture{};
        CameraDimensions cameraDimensions{};

        CameraTextureDelegate* cameraTextureDelegate{};
        PhotoCaptureDelegate* photoCaptureDelegate{};
        AVCaptureSession* avCaptureSession{};
        AVCapturePhotoOutput* avCapturePhotoOutput{};
        CVMetalTextureCacheRef textureCache{};
        id<MTLTexture> textureRGBA{};
        id<MTLRenderPipelineState> cameraPipelineState{};
        id<MTLDevice> metalDevice{};
        id<MTLCommandQueue> commandQueue{};
        id<MTLCommandBuffer> currentCommandBuffer{};
        bool isInitialized{false};
        bool refreshBgfxHandle{true};
        bgfx::TextureHandle textureHandle{};

        arcana::background_dispatcher<32> cameraSessionDispatcher{};
        std::shared_ptr<arcana::cancellation_source> cancellationSource{std::make_shared<arcana::cancellation_source>()};
    };

    std::vector<CameraDevice> CameraDevice::GetCameraDevices(Napi::Env env)
    {
        std::vector<CameraDevice> cameraDevices{};

        NSArray* deviceTypes{@[
            // Use the main camera on the device which is best for general use and will typically be 1x optical zoom and the highest resolution.
            // https://developer.apple.com/documentation/avfoundation/avcapturedevice/devicetype/2361449-builtinwideanglecamera
            AVCaptureDeviceTypeBuiltInWideAngleCamera
        ]};
        bool foundExactMatch{false};

#if (TARGET_OS_IPHONE)
        if (@available(iOS 13.0, *))
        {
            // Ordered list of cameras by general usage quality.
            deviceTypes = @[
                AVCaptureDeviceTypeBuiltInTripleCamera,
                AVCaptureDeviceTypeBuiltInDualCamera,
                AVCaptureDeviceTypeBuiltInDualWideCamera,
                AVCaptureDeviceTypeBuiltInWideAngleCamera,
                AVCaptureDeviceTypeBuiltInUltraWideCamera,
                AVCaptureDeviceTypeBuiltInTelephotoCamera,
            ];
        }
#endif

        AVCaptureDeviceDiscoverySession* discoverySession{[AVCaptureDeviceDiscoverySession
                                                           discoverySessionWithDeviceTypes:deviceTypes
                                                           mediaType:AVMediaTypeVideo
                                                           position:AVCaptureDevicePositionUnspecified]};

        for (AVCaptureDevice* device in discoverySession.devices)
        {
            auto cameraDeviceImpl{std::make_unique<CameraDevice::Impl>(env)};

            for (AVCaptureDeviceFormat* format in device.formats)
            {
                CMVideoFormatDescriptionRef videoFormatRef{static_cast<CMVideoFormatDescriptionRef>(format.formatDescription)};
                CMVideoDimensions dimensions{CMVideoFormatDescriptionGetDimensions(videoFormatRef)};
                uint32_t pixelFormat{static_cast<uint32_t>(CMFormatDescriptionGetMediaSubType(videoFormatRef))};

                // Reject unsupported pixel formats.
                if (!isPixelFormatSupported(pixelFormat))
                {
                    continue;
                }

                auto trackImpl = std::make_unique<CameraTrack::Impl>();
                trackImpl->width = dimensions.width;
                trackImpl->height = dimensions.height;
                trackImpl->avDeviceFormat = format;
                trackImpl->pixelFormat = pixelFormat;

                cameraDeviceImpl->supportedResolutions.push_back(CameraTrack{std::move(trackImpl)});
            }

            // update the cameraDevice information
            cameraDeviceImpl->avDevice = device;

            cameraDeviceImpl->capabilities.push_back(std::make_unique<CameraCapabilityTemplate<std::string>>
            (
                    Capability::Feature::FacingMode,
                device.position == AVCaptureDevicePositionFront ? "user" : "environment",
                device.position == AVCaptureDevicePositionFront ? "user" : "environment",
                device.position == AVCaptureDevicePositionFront ? std::vector<std::string>{"user"} : std::vector<std::string>{"environment"}
            ));

            cameraDeviceImpl->capabilities.push_back(std::make_unique<CameraCapabilityTemplate<bool>>
            (
                    Capability::Feature::Torch,
                    false,
                    false,
                device.isTorchAvailable ? std::vector<bool>{false, true} : std::vector<bool>{false},
                    [device](bool newValue)
                {
                    NSError* error{nil};
                    AVCaptureTorchMode torchMode{newValue ? AVCaptureTorchModeOn : AVCaptureTorchModeOff};
                    [device lockForConfiguration:&error];
                    [device setTorchMode:torchMode];
                    [device unlockForConfiguration];
                    if (error != nil)
                    {
                        return false;
                    }
                    return true;
                }
            ));

#if (TARGET_OS_IPHONE)
            // iOS Zoom factors always start at 1.0 and go up from there slide the scale based on
            // the device type (if it starts with an ultrawide sensor or telephoto)
            double zoomFactorScale{1.0};
            if (@available(iOS 13.0, *))
            {
                if (device.deviceType == AVCaptureDeviceTypeBuiltInTripleCamera ||
                    device.deviceType == AVCaptureDeviceTypeBuiltInDualWideCamera ||
                    device.deviceType == AVCaptureDeviceTypeBuiltInUltraWideCamera)
                {
                    zoomFactorScale = 0.5;
                }
            }
            if (device.deviceType == AVCaptureDeviceTypeBuiltInTelephotoCamera)
            {
                // Newer iOS devices might have a telephoto that has a starting zoom higher, but so far
                // as I can tell there is no API to determine the default real world zoom of the telephoto lens
                zoomFactorScale = 2.0;
            }

            cameraDeviceImpl->capabilities.push_back(std::make_unique<CameraCapabilityTemplate<double>>
            (
                Capability::Feature::Zoom,
                1.0 * zoomFactorScale, // Translate the starting zoom value from the iOS 1.0+ scale to our 0.0+ scale.
                1.0, // Set the default target to 1.0 (regardless of zoomFactorScale)
                std::vector<double>{device.minAvailableVideoZoomFactor * zoomFactorScale, device.maxAvailableVideoZoomFactor * zoomFactorScale},
                [device, zoomFactorScale](double newValue)
                {
                    NSError* error{nil};
                    [device lockForConfiguration:&error];
                    [device setVideoZoomFactor:newValue / zoomFactorScale];
                    [device unlockForConfiguration];
                    if (error != nil)
                    {
                        return false;
                    }
                    return true;
                }
            ));
#endif

            cameraDevices.push_back(CameraDevice{std::move(cameraDeviceImpl)});

            if (foundExactMatch)
            {
                break;
            }
        }

        return cameraDevices;
    }

    arcana::task<CameraDevice::CameraDimensions, std::exception_ptr> CameraDevice::OpenAsync(const CameraTrack& resolution)
    {
        NSError* error{nil};

        // This is the first time the camera has been opened, perform some one time setup.
        if (!m_impl->isInitialized) {
            // Set the thread affinity to the current thread
            m_impl->threadAffinity = std::this_thread::get_id();

            m_impl->commandQueue = (__bridge id<MTLCommandQueue>)bgfx::getInternalData()->commandQueue;
            m_impl->metalDevice = (__bridge id<MTLDevice>)bgfx::getInternalData()->context;
            m_impl->deviceContext = &Graphics::DeviceContext::GetFromJavaScript(m_impl->env);

            // Compile shaders used for converting camera output to RGBA.
            id<MTLLibrary> lib = CompileShader(m_impl->metalDevice, shaderSource);
            id<MTLFunction> vertexFunction = [lib newFunctionWithName:@"vertexShader"];
            id<MTLFunction> fragmentFunction = [lib newFunctionWithName:@"fragmentShader"];

            // Create a pipeline state for converting the camera output to RGBA.
            MTLRenderPipelineDescriptor* pipelineStateDescriptor = [[MTLRenderPipelineDescriptor alloc] init];
            pipelineStateDescriptor.label = @"Native Camera YCbCr to RGBA Pipeline";
            pipelineStateDescriptor.vertexFunction = vertexFunction;
            pipelineStateDescriptor.fragmentFunction = fragmentFunction;
            pipelineStateDescriptor.colorAttachments[0].pixelFormat = MTLPixelFormatRGBA8Unorm;
            m_impl->cameraPipelineState = [m_impl->metalDevice newRenderPipelineStateWithDescriptor:pipelineStateDescriptor error:&error];

            if (!m_impl->cameraPipelineState) {
                return arcana::task_from_error<CameraDimensions>(std::make_exception_ptr(std::runtime_error{
                    std::string("Failed to create camera pipeline state: ") + [error.localizedDescription cStringUsingEncoding:NSASCIIStringEncoding]}));
            }

            m_impl->isInitialized = true;
        } else {
            // Always refresh the bgfx handle to point to textureRGBA on re-open.
            m_impl->refreshBgfxHandle = true;
        }

        // Construct the camera texture delegate, which is responsible for handling updates for device orientation and the capture session.
        CVMetalTextureCacheCreate(nullptr, nullptr, m_impl->metalDevice, nullptr, &m_impl->textureCache);
        m_impl->cameraTextureDelegate = [[CameraTextureDelegate alloc]init:m_impl->textureCache];

        // Lock camera device and set up camera format. If there a problem initialising the camera it will give an error.
        [m_impl->avDevice lockForConfiguration:&error];
        if (error != nil)
        {
            return arcana::task_from_error<CameraDimensions>(std::make_exception_ptr(std::runtime_error{"Failed to lock camera"}));
        }

        [m_impl->avDevice setActiveFormat:resolution.m_impl->avDeviceFormat];
        AVCaptureDeviceInput* input{[AVCaptureDeviceInput deviceInputWithDevice:m_impl->avDevice error:&error]};
        [m_impl->avDevice unlockForConfiguration];

        // Capture the format dimensions.
        CMVideoFormatDescriptionRef videoFormatRef{static_cast<CMVideoFormatDescriptionRef>(resolution.m_impl->avDeviceFormat.formatDescription)};
        CMVideoDimensions dimensions{CMVideoFormatDescriptionGetDimensions(videoFormatRef)};

        m_impl->cameraDimensions = CameraDimensions{static_cast<uint32_t>(dimensions.width), static_cast<uint32_t>(dimensions.height)};

        m_impl->supportedMaxPhotoDimensions.clear();
#if (TARGET_OS_IOS)
#if (__IPHONE_OS_VERSION_MAX_ALLOWED >= 160000)
        if (@available(iOS 16.0, *))
        {
            for (NSValue* dimensions in resolution.m_impl->avDeviceFormat.supportedMaxPhotoDimensions)
            {
                m_impl->supportedMaxPhotoDimensions.push_back(dimensions.CMVideoDimensionsValue);
            }
        }
        else
#endif
        {
#if (__IPHONE_OS_VERSION_MIN_REQUIRED < 160000)
            m_impl->supportedMaxPhotoDimensions.push_back(dimensions);
            m_impl->supportedMaxPhotoDimensions.push_back(resolution.m_impl->avDeviceFormat.highResolutionStillImageDimensions);
#endif
        }
#endif

        // Check for failed initialisation.
        if (!input)
        {
            return arcana::task_from_error<CameraDimensions>(std::make_exception_ptr(std::runtime_error{"Error Getting Camera Input"}));
        }

        // Kick off camera session on a background thread.
        return arcana::make_task(m_impl->cameraSessionDispatcher, arcana::cancellation::none(), [implObj = shared_from_this(), pixelFormat = resolution.m_impl->pixelFormat, input]() mutable {
            if (implObj->m_impl->avCaptureSession == nil) {
                implObj->m_impl->avCaptureSession = [[AVCaptureSession alloc] init];
            } else {
                for (AVCaptureInput* input in [implObj->m_impl->avCaptureSession inputs]) {
                    [implObj->m_impl->avCaptureSession removeInput: input];
                }

                for (AVCaptureOutput* output in [implObj->m_impl->avCaptureSession outputs]) {
                    [implObj->m_impl->avCaptureSession removeOutput: output];
                }
            }

#if (TARGET_OS_IPHONE)
            [implObj->m_impl->avCaptureSession setSessionPreset:AVCaptureSessionPresetInputPriority];
#endif

            // Add camera input source to the capture session.
            [implObj->m_impl->avCaptureSession addInput:input];

            // Create the camera buffer, and set up camera texture delegate to capture frames.
            dispatch_queue_t sampleBufferQueue{dispatch_queue_create("CameraMulticaster", DISPATCH_QUEUE_SERIAL)};
            AVCaptureVideoDataOutput* dataOutput{[[AVCaptureVideoDataOutput alloc] init]};
            [dataOutput setAlwaysDiscardsLateVideoFrames:YES];
            [dataOutput setVideoSettings:@{(id)kCVPixelBufferPixelFormatTypeKey: @(pixelFormat)}];
            [dataOutput setSampleBufferDelegate:implObj->m_impl->cameraTextureDelegate queue:sampleBufferQueue];
            [implObj->m_impl->avCaptureSession addOutput:dataOutput];

            // Setup high resolution photo capture.
            implObj->m_impl->avCapturePhotoOutput = [[AVCapturePhotoOutput alloc] init];
            [implObj->m_impl->avCaptureSession addOutput:implObj->m_impl->avCapturePhotoOutput];

            auto redEyeReduction = RedEyeReduction::Never;
#if (TARGET_OS_IOS)
            if (implObj->m_impl->avCapturePhotoOutput.isAutoRedEyeReductionSupported)
            {
                redEyeReduction = RedEyeReduction::Controllable;
            }
#endif

            std::set<FillLightMode> fillLightModes{};
            fillLightModes.insert(FillLightMode::Off);
            FillLightMode defaultFillLightMode = FillLightMode::Off;
#if (TARGET_OS_IOS)
            if ([implObj->m_impl->avCapturePhotoOutput.supportedFlashModes containsObject:@(AVCaptureFlashModeAuto)])
            {
                fillLightModes.insert(FillLightMode::Auto);
                defaultFillLightMode = FillLightMode::Auto;
            }
            if ([implObj->m_impl->avCapturePhotoOutput.supportedFlashModes containsObject:@(AVCaptureFlashModeOn)])
            {
                fillLightModes.insert(FillLightMode::Flash);
            }
#endif

            implObj->m_impl->photoCapabilities =
            {
                redEyeReduction,
                fillLightModes,
                gsl::narrow<uint32_t>(implObj->m_impl->supportedMaxPhotoDimensions.front().width),
                gsl::narrow<uint32_t>(implObj->m_impl->supportedMaxPhotoDimensions.back().width),
                1,
                gsl::narrow<uint32_t>(implObj->m_impl->supportedMaxPhotoDimensions.front().height),
                gsl::narrow<uint32_t>(implObj->m_impl->supportedMaxPhotoDimensions.back().height),
                1,
            };

            implObj->m_impl->defaultPhotoSettings =
            {
                implObj->m_impl->photoCapabilities->RedEyeReduction != RedEyeReduction::Never,
                defaultFillLightMode,
                implObj->m_impl->photoCapabilities->MaxWidth,
                implObj->m_impl->photoCapabilities->MaxHeight,
            };
            
#if (TARGET_OS_IOS)
#if (__IPHONE_OS_VERSION_MAX_ALLOWED >= 160000)
            if (@available(iOS 16.0, *))
            {
                implObj->m_impl->avCapturePhotoOutput.maxPhotoDimensions = {
                    gsl::narrow<int32_t>(implObj->m_impl->photoCapabilities->MaxWidth),
                    gsl::narrow<int32_t>(implObj->m_impl->photoCapabilities->MaxHeight)
                };
            }
            else
#endif
            {
#if (__IPHONE_OS_VERSION_MIN_REQUIRED < 160000)
                implObj->m_impl->avCapturePhotoOutput.highResolutionCaptureEnabled = true;
#endif
            }
#endif

            // Actually start the camera session.
            [implObj->m_impl->avCaptureSession startRunning];
            // To match the web implementation if the sensor is rotated into a portrait orientation then the width and height
            // of the video should be swapped
            // NOTE: This code returns (width, height) independently of the VideoOrientation. As no bug as been reported, this code
            // remains unchanged. Fix when a proper test procedure has been found.
            /* potential fix
            return implObj->m_impl->cameraTextureDelegate->VideoOrientation == VideoOrientation::LandscapeLeft ||
                implObj->m_impl->cameraTextureDelegate->VideoOrientation == VideoOrientation::LandscapeRight ?
                CameraDimensions{implObj->m_impl->cameraDimensions.width, implObj->m_impl->cameraDimensions.height} :
                CameraDimensions{implObj->m_impl->cameraDimensions.height, implObj->m_impl->cameraDimensions.width};
            */
            return CameraDimensions{implObj->m_impl->cameraDimensions.width, implObj->m_impl->cameraDimensions.height};
        });
    }

    CameraDevice::CameraDimensions CameraDevice::UpdateCameraTexture(bgfx::TextureHandle textureHandle)
    {
        // Hook into AfterRender to copy over the texture, ensuring that the textureHandle has already been initialized by bgfx.
        // Capture the cancellation token so that the shared pointer is kept alive when arcana checks internally for cancellation.
        arcana::make_task(m_impl->deviceContext->AfterRenderScheduler(), *m_impl->cancellationSource, [this, textureHandle, cancellationSource{m_impl->cancellationSource}] {
            id<MTLTexture> textureY{};
            id<MTLTexture> textureCbCr{};
            int64_t width{0};
            int64_t height{0};

            @synchronized(m_impl->cameraTextureDelegate) {
                textureY = [m_impl->cameraTextureDelegate getCameraTextureY];
                textureCbCr = [m_impl->cameraTextureDelegate getCameraTextureCbCr];

                switch (m_impl->cameraTextureDelegate->Orientation)
                {
                    case VideoOrientation::LandscapeRight:
                    case VideoOrientation::LandscapeLeft:
                        width = [textureY width];
                        height = [textureY height];
                        break;
                    case VideoOrientation::Portrait:
                    case VideoOrientation::PortraitUpsideDown:
                        // In portrait orientation the camera sensor is rotated 90 degrees so the width and height should be swapped
                        width = [textureY height];
                        height = [textureY width];
                        break;
                }
            }

            // Skip processing this frame if width and height are invalid.
            if (width == 0 || height == 0) {
                return;
            }

            // Check if the we've been handed a new texture handle and if so refresh our override
            if (m_impl->textureHandle.idx != textureHandle.idx)
            {
                m_impl->refreshBgfxHandle = true;
                m_impl->textureHandle = textureHandle;
            }

            // Recreate the output texture when the camera dimensions change.
            if (m_impl->textureRGBA == nil || m_impl->cameraDimensions.width != width || m_impl->cameraDimensions.height != height)
            {
                MTLTextureDescriptor* textureDescriptor = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatRGBA8Unorm width:width height:height mipmapped:NO];
                textureDescriptor.usage = MTLTextureUsageRenderTarget | MTLTextureUsageShaderRead;
                m_impl->textureRGBA = [m_impl->metalDevice newTextureWithDescriptor:textureDescriptor];
                m_impl->cameraDimensions.width = static_cast<uint32_t>(width);
                m_impl->cameraDimensions.height = static_cast<uint32_t>(height);
                // Setting up the bgfx texture may fail if the textureHandle hasn't been initialized in a bgfx::frame call yet, if so try agin on
                // the next frame to override it.
                m_impl->refreshBgfxHandle = bgfx::overrideInternal(textureHandle, reinterpret_cast<uintptr_t>(m_impl->textureRGBA)) == 0;
            }
            else if (m_impl->refreshBgfxHandle)
            {
                m_impl->refreshBgfxHandle = bgfx::overrideInternal(textureHandle, reinterpret_cast<uintptr_t>(m_impl->textureRGBA)) == 0;
            }

            if (textureY != nil && textureCbCr != nil && m_impl->textureRGBA != nil && !m_impl->refreshBgfxHandle)
            {
                m_impl->currentCommandBuffer = [m_impl->commandQueue commandBuffer];
                m_impl->currentCommandBuffer.label = @"NativeCameraCommandBuffer";
                MTLRenderPassDescriptor* renderPassDescriptor = [MTLRenderPassDescriptor renderPassDescriptor];

                if (renderPassDescriptor != nil) {
                    // Attach the color texture, on which we'll draw the camera texture (so no need to clear on load).
                    renderPassDescriptor.colorAttachments[0].texture = m_impl->textureRGBA;
                    renderPassDescriptor.colorAttachments[0].loadAction = MTLLoadActionDontCare;
                    renderPassDescriptor.colorAttachments[0].storeAction = MTLStoreActionStore;

                    // Create and end the render encoder.
                    id<MTLRenderCommandEncoder> renderEncoder = [m_impl->currentCommandBuffer renderCommandEncoderWithDescriptor:renderPassDescriptor];
                    renderEncoder.label = @"NativeCameraEncoder";

                    // Set the shader pipeline.
                    [renderEncoder setRenderPipelineState:m_impl->cameraPipelineState];

                    // Set the vertex & UV data based on current orientation
                    switch (m_impl->cameraTextureDelegate->Orientation)
                    {
                        case VideoOrientation::LandscapeLeft:
                            if (m_impl->avDevice.position == AVCaptureDevicePositionFront)
                            {
                                // The front camera sensor is oriented 180 out of sync from the rear sensor on iOS devices. Swap landscape orientations.
                                [renderEncoder setVertexBytes:vertices_landscape_right length:sizeof(vertices_landscape_right) atIndex:0];
                            }
                            else
                            {
                                [renderEncoder setVertexBytes:vertices_landscape_left length:sizeof(vertices_landscape_left) atIndex:0];
                            }
                            break;
                        case VideoOrientation::Portrait:
                            [renderEncoder setVertexBytes:vertices_portrait length:sizeof(vertices_portrait) atIndex:0];
                            break;
                        case VideoOrientation::PortraitUpsideDown:
                            [renderEncoder setVertexBytes:vertices_portrait_upsideddown length:sizeof(vertices_portrait_upsideddown) atIndex:0];
                            break;
                        case VideoOrientation::LandscapeRight:
                            if (m_impl->avDevice.position == AVCaptureDevicePositionFront)
                            {
                                // The front camera sensor is oriented 180 out of sync from the rear sensor on iOS devices. Swap landscape orientations.
                                [renderEncoder setVertexBytes:vertices_landscape_left length:sizeof(vertices_landscape_left) atIndex:0];
                            }
                            else
                            {
                                [renderEncoder setVertexBytes:vertices_landscape_right length:sizeof(vertices_landscape_right) atIndex:0];
                            }
                            break;
                    }

                    // Set the textures.
                    [renderEncoder setFragmentTexture:textureY atIndex:1];
                    [renderEncoder setFragmentTexture:textureCbCr atIndex:2];

                    // Draw the triangles.
                    [renderEncoder drawPrimitives:MTLPrimitiveTypeTriangleStrip vertexStart:0 vertexCount:4];

                    [renderEncoder endEncoding];

                    [m_impl->currentCommandBuffer addCompletedHandler:^(id<MTLCommandBuffer>) {
                        if (textureY != nil) {
                            [textureY setPurgeableState:MTLPurgeableStateEmpty];
                        }

                        if (textureCbCr != nil) {
                            [textureCbCr setPurgeableState:MTLPurgeableStateEmpty];
                        }
                    }];
                }

                // Finalize rendering here & push the command buffer to the GPU.
                [m_impl->currentCommandBuffer commit];
                
                [m_impl->currentCommandBuffer waitUntilCompleted];
            }
        });
        // To match the web implementation if the sensor is rotated into a portrait orientation then the width and height
        // of the video should be swapped
        // NOTE: This code returns (width, height) independently of the VideoOrientation. As no bug as been reported, this code
        // remains unchanged. Fix when a proper test procedure has been found.
        /* potential fix:
        return m_impl->cameraTextureDelegate->VideoOrientation == VideoOrientation::LandscapeLeft ||
            m_impl->cameraTextureDelegate->VideoOrientation == VideoOrientation::LandscapeRight ?
            CameraDimensions{m_impl->cameraDimensions.width, m_impl->cameraDimensions.height} :
            CameraDimensions{m_impl->cameraDimensions.height, m_impl->cameraDimensions.width};
        */
        return CameraDimensions{m_impl->cameraDimensions.width, m_impl->cameraDimensions.height};
    }

    CameraDevice::TakePhotoTask CameraDevice::TakePhotoAsync(PhotoSettings photoSettings)
    {
        bool foundMaxPhotoDimensions = false;
        for (auto maxSupportedPhotoDimensions = m_impl->supportedMaxPhotoDimensions.rbegin(); maxSupportedPhotoDimensions != m_impl->supportedMaxPhotoDimensions.rend(); ++maxSupportedPhotoDimensions)
        {
            if (gsl::narrow<uint32_t>(maxSupportedPhotoDimensions->width) <= photoSettings.Width && gsl::narrow<uint32_t>(maxSupportedPhotoDimensions->height) <= photoSettings.Height)
            {
                photoSettings.Width = maxSupportedPhotoDimensions->width;
                photoSettings.Height = maxSupportedPhotoDimensions->height;
                foundMaxPhotoDimensions = true;
                break;
            }
        }

        if (!foundMaxPhotoDimensions)
        {
            return arcana::task_from_error<CameraDevice::TakePhotoTask::result_type>(std::make_exception_ptr(std::runtime_error{"All supported resolutions exceed the requested resolution."}));
        }

        AVCapturePhotoSettings* capturePhotoSettings{[AVCapturePhotoSettings photoSettingsWithFormat:@{ AVVideoCodecKey : AVVideoCodecTypeJPEG}]};
#if (TARGET_OS_IOS)
#if (__IPHONE_OS_VERSION_MAX_ALLOWED >= 160000)
        if (@available(iOS 16.0, *))
        {
            capturePhotoSettings.maxPhotoDimensions = {gsl::narrow<int32_t>(photoSettings.Width), gsl::narrow<int32_t>(photoSettings.Height)};
        }
        else
#endif
        {
#if (__IPHONE_OS_VERSION_MIN_REQUIRED < 160000)
            // TODO: If we can't control the resolution on older iOS versions, we could resize the image we get back to "simulate" a lower res capture.
            capturePhotoSettings.highResolutionPhotoEnabled = true;
#endif
        }
#endif

#if (TARGET_OS_IOS)
        switch (photoSettings.FillLightMode)
        {
            case FillLightMode::Auto:
                capturePhotoSettings.flashMode = AVCaptureFlashMode::AVCaptureFlashModeAuto;
                break;
            case FillLightMode::Flash:
                capturePhotoSettings.flashMode = AVCaptureFlashMode::AVCaptureFlashModeOn;
                break;
            case FillLightMode::Off:
                capturePhotoSettings.flashMode = AVCaptureFlashMode::AVCaptureFlashModeOff;
                break;
        }
#endif

#if (TARGET_OS_IPHONE)
        capturePhotoSettings.autoRedEyeReductionEnabled = photoSettings.RedEyeReduction;
#endif

        TakePhotoTaskCompletionSource taskCompletionSource{};
        m_impl->photoCaptureDelegate = [[PhotoCaptureDelegate alloc]init: taskCompletionSource orientation:m_impl->cameraTextureDelegate->Orientation];

        // Update photo output's videoOrientation so the final photo orientation is correct.
        // This reflects both the camera sensor's orientation relative to the device's default orientation, and the UI orientation.
        // If the device has rotation lock enabled, then the UI orientation will be locked, and the final photo will be rotated.
        // This matches the behavior of the ImageCapture API in Chrome on Android (the only place ImageCapture is implemented in a mobile browser currently).
        AVCaptureConnection* photoOutputConnection = [m_impl->avCapturePhotoOutput connectionWithMediaType:AVMediaTypeVideo];
        if (photoOutputConnection)
        {
            photoOutputConnection.videoRotationAngle = VideoOrientationToRotationAngle(m_impl->cameraTextureDelegate->Orientation);
        }
        [m_impl->avCapturePhotoOutput capturePhotoWithSettings:capturePhotoSettings delegate:m_impl->photoCaptureDelegate];

        return taskCompletionSource.as_task();
    }

    void CameraDevice::Close()
    {
        if (m_impl->cameraTextureDelegate == nil)
        {
            // This device was either never opened, or has already been closed.
            // No action is required.
            return;
        }

        // Cancel any pending async operations
        m_impl->cancellationSource->cancel();

        // Complete any running command buffers before destroying the cache.
        if (m_impl->currentCommandBuffer != nil) {
            [m_impl->currentCommandBuffer waitUntilCompleted];
        }

        if (m_impl->avCaptureSession != nil) {
            // Stopping the capture session is a synchronous call that requires marshalling to the main thread. Calling it from background thread can lead
            // to a deadlock where Babylon is waiting for the frame to finish render on the main thread and AVCaptureSession::stopRunning is waiting
            // for the main thread to free up while blocking the current frame from rendering.
            //
            // Capturing textureRGBA, textureDelegate, and textureCache is done here because it's used in bgfx::overrideInternal but due to ARC being enabled in this project the lifetime of the texture
            // needs to be maintained until after the render pass. Otherwise bgfx will try to access a destroyed texture handle during the render pass.
            arcana::make_task(m_impl->deviceContext->AfterRenderScheduler(), arcana::cancellation::none(),
                [avCaptureSession = m_impl->avCaptureSession, textureRGBA = m_impl->textureRGBA, textureDelegate = m_impl->cameraTextureDelegate, textureCache = m_impl->textureCache]
            {
                [avCaptureSession stopRunning];

                // Stop collecting frames, release camera texture delegate.
                [textureDelegate reset];

                // Free the texture cache.
                if (textureCache)
                {
                    CVMetalTextureCacheFlush(textureCache, 0);
                    CFRelease(textureCache);
                }
            });
        }

        m_impl->photoCapabilities.reset();
        m_impl->defaultPhotoSettings.reset();
        m_impl->supportedMaxPhotoDimensions.clear();
    }
}

@implementation CameraTextureDelegate {
}

- (id)init:(CVMetalTextureCacheRef)textureCache
{
    self = [super init];
    self->textureCache = textureCache;
#if (TARGET_OS_IPHONE)
    [[NSNotificationCenter defaultCenter]addObserver:self selector:@selector(OrientationDidChange:) name:UIDeviceOrientationDidChangeNotification object:nil];
    [self updateOrientation];
#else
    // Orientation not supported on non-iOS devices. LandscapeLeft assumes the video is already in the correct orientation.
    self->VideoOrientation = VideoOrientation::LandscapeLeft;
#endif
    return self;
}

/**
 Returns the camera Y texture, the caller is responsible for freeing this texture.
 */
- (id<MTLTexture>)getCameraTextureY {
    if (cameraTextureY != nil) {
        id<MTLTexture> mtlTexture = CVMetalTextureGetTexture(cameraTextureY);
        return mtlTexture;
    }

    return nil;
}

/**
 Returns the camera CbCr texture, the caller is responsible for freeing this texture.
 */
- (id<MTLTexture>)getCameraTextureCbCr {
    if (cameraTextureCbCr != nil) {
        id<MTLTexture> mtlTexture = CVMetalTextureGetTexture(cameraTextureCbCr);
        return mtlTexture;
    }

    return nil;
}

- (void) reset {
    @synchronized (self) {
        [self cleanupTextures];
        self->textureCache = nil;
    }
}

#if (TARGET_OS_IPHONE)
/**
 Updates target video orientation.
*/
- (void)updateOrientation {
    UIApplication* sharedApplication{[UIApplication sharedApplication]};
    UIInterfaceOrientation orientation{UIInterfaceOrientationUnknown};
#if (__IPHONE_OS_VERSION_MIN_REQUIRED >= __IPHONE_13_0)
    UIScene* scene{[[[sharedApplication connectedScenes] allObjects] firstObject]};
    orientation = [(UIWindowScene*)scene interfaceOrientation];
#else
    if (@available(iOS 13.0, *)) {
        orientation = [[[[sharedApplication windows] firstObject] windowScene] interfaceOrientation];
    }
    else {
        orientation = [sharedApplication statusBarOrientation];
    }
#endif

    // Convert from UIInterfaceOrientation to VideoOrientation.
    switch (orientation)
    {
        case UIInterfaceOrientationUnknown:
        case UIInterfaceOrientationLandscapeLeft:
            self->Orientation = VideoOrientation::LandscapeLeft;
            break;
        case UIInterfaceOrientationLandscapeRight:
            self->Orientation = VideoOrientation::LandscapeRight;
            break;
        case UIInterfaceOrientationPortrait:
            self->Orientation = VideoOrientation::Portrait;
            break;
        case UIInterfaceOrientationPortraitUpsideDown:
            self->Orientation = VideoOrientation::PortraitUpsideDown;
            break;
    }
}

-(void)OrientationDidChange:(NSNotification*)notification
{
    [self updateOrientation];
}
#endif

- (void)captureOutput:(AVCaptureOutput*)__unused captureOutput didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer fromConnection:(AVCaptureConnection*) connection
{
    CVPixelBufferRef pixelBuffer{CMSampleBufferGetImageBuffer(sampleBuffer)};

    // Update both metal textures used by the renderer to display the camera image.
    CVMetalTextureRef textureY = [self getCameraTexture:pixelBuffer plane:0];
    CVMetalTextureRef textureCbCr = [self getCameraTexture:pixelBuffer plane:1];

    @synchronized(self) {
        // It's possible that the texture cache has been invalidated, in which case we should skip assignment.
        if (self->textureCache == nil) {
            return;
        }

        [self cleanupTextures];
        cameraTextureY = textureY;
        cameraTextureCbCr = textureCbCr;
    }
}

/**
 Updates the captured texture with the current pixel buffer.
*/
- (CVMetalTextureRef)getCameraTexture:(CVPixelBufferRef)pixelBuffer plane:(int)planeIndex {
    CVReturn ret = CVPixelBufferLockBaseAddress(pixelBuffer, kCVPixelBufferLock_ReadOnly);
    if (ret != kCVReturnSuccess) {
        return {};
    }

    size_t planeWidth = CVPixelBufferGetWidthOfPlane(pixelBuffer, planeIndex);
    size_t planeHeight = CVPixelBufferGetHeightOfPlane(pixelBuffer, planeIndex);

    // Plane 0 is the Y plane, which is in R8Unorm format, and the second plane is the CBCR plane which is RG8Unorm format.
    auto pixelFormat = planeIndex ? MTLPixelFormatRG8Unorm : MTLPixelFormatR8Unorm;
    CVMetalTextureRef textureRef;

    @synchronized (self) {
        if (self->textureCache == nil) {
            return{};
        }

        auto status = CVMetalTextureCacheCreateTextureFromImage(kCFAllocatorDefault, textureCache, pixelBuffer, nil, pixelFormat, planeWidth, planeHeight, planeIndex, &textureRef);
        if (status != kCVReturnSuccess) {
            CVBufferRelease(textureRef);
            textureRef = nil;
        }
    }

    return textureRef;
}

-(void)cleanupTextures {
    if (cameraTextureY != nil) {
        CVBufferRelease(cameraTextureY);
        cameraTextureY = nil;
    }

    if (cameraTextureCbCr != nil) {
        CVBufferRelease(cameraTextureCbCr);
        cameraTextureCbCr = nil;
    }
}

-(void)dealloc {
#if (TARGET_OS_IPHONE)
        [[NSNotificationCenter defaultCenter]removeObserver:self name:UIDeviceOrientationDidChangeNotification object:nil];
#endif

    [self reset];
}

@end

@implementation PhotoCaptureDelegate {
}

- (id)init:(TakePhotoTaskCompletionSource)taskCompletionSource orientation:(VideoOrientation)orientation
{
    self->taskCompletionSource = std::move(taskCompletionSource);
    self->orientation = orientation;
    return self;
}

- (void)captureOutput:(AVCapturePhotoOutput *)output didFinishProcessingPhoto:(AVCapturePhoto *)photo error:(NSError *)error
{
    if (error)
    {
        self->taskCompletionSource.complete(arcana::make_unexpected(std::make_exception_ptr(std::runtime_error{[error.localizedDescription UTF8String]})));
    }
    else
    {
        // Get the image data (as jpeg)
        NSData* imageData = [photo fileDataRepresentation];

        // Saving the photo to storage can be helpful for testing and diagnosing any photo issues. To do so:
        // 1. In the Playground target, go to Build Phases, scroll down to Link Binary with Libraries, and add Photos.framework
        // 2. Add #import <Photos/Photos.h> at the top of this file
        // 3. Uncomment the block below
        /*
        [[PHPhotoLibrary sharedPhotoLibrary] performChanges:^{
            PHAssetCreationRequest *creationRequest = [PHAssetCreationRequest creationRequestForAsset];
            [creationRequest addResourceWithType:PHAssetResourceTypePhoto data:imageData options:nil];
        } completionHandler:^(BOOL success, NSError *error) {
            if (!success) {
                NSLog(@"Error saving image: %@", error);
            } else {
                NSLog(@"Image saved successfully");
            }
        }];
        */

        self->taskCompletionSource.complete(gsl::make_span(static_cast<const uint8_t*>(imageData.bytes), imageData.length));
    }
}

-(void)dealloc
{
    if (!self->taskCompletionSource.completed())
    {
        self->taskCompletionSource.complete(arcana::make_unexpected(std::make_exception_ptr(std::runtime_error{"PhotoCaptureDelegate deallocated before capture completed."})));
    }
}

@end

#include "../CameraDeviceSharedPImpl.h"
