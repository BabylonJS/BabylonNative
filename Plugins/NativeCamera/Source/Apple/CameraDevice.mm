#if ! __has_feature(objc_arc)
#error "ARC is off"
#endif

#import <MetalKit/MetalKit.h>
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

@class CameraTextureDelegate;

#include "../CameraDevice.h"
#include <napi/napi.h>

@interface CameraTextureDelegate : NSObject <AVCaptureVideoDataOutputSampleBufferDelegate>
{
    @public AVCaptureVideoOrientation VideoOrientation;

    CVMetalTextureCacheRef textureCache;
}

- (id)init:(CVMetalTextureCacheRef)textureCache;
- (id<MTLTexture>)getCameraTextureY;
- (id<MTLTexture>)getCameraTextureCbCr;
- (void)reset;

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
        AVCaptureDevice* avDevice{};
        
        bool overrideCameraTexture{};
        CameraDimensions cameraDimensions{};

        CameraTextureDelegate* cameraTextureDelegate{};
        AVCaptureSession* avCaptureSession{};
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

            // Actually start the camera session.
            [implObj->m_impl->avCaptureSession startRunning];

            // To match the web implementation if the sensor is rotated into a portrait orientation then the width and height
            // of the video should be swapped
            return implObj->m_impl->cameraTextureDelegate->VideoOrientation == AVCaptureVideoOrientationLandscapeLeft ||
                implObj->m_impl->cameraTextureDelegate->VideoOrientation == AVCaptureVideoOrientationLandscapeRight ?
                CameraDimensions{implObj->m_impl->cameraDimensions.width, implObj->m_impl->cameraDimensions.height} :
                CameraDimensions{implObj->m_impl->cameraDimensions.width, implObj->m_impl->cameraDimensions.height};
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
                
                switch (m_impl->cameraTextureDelegate->VideoOrientation)
                {
                    case AVCaptureVideoOrientationLandscapeRight:
                    case AVCaptureVideoOrientationLandscapeLeft:
                        width = [textureY width];
                        height = [textureY height];
                        break;
                    case AVCaptureVideoOrientationPortrait:
                    case AVCaptureVideoOrientationPortraitUpsideDown:
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
                    switch (m_impl->cameraTextureDelegate->VideoOrientation)
                    {
                        case AVCaptureVideoOrientationLandscapeLeft:
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
                        case AVCaptureVideoOrientationPortrait:
                            [renderEncoder setVertexBytes:vertices_portrait length:sizeof(vertices_portrait) atIndex:0];
                            break;
                        case AVCaptureVideoOrientationPortraitUpsideDown:
                            [renderEncoder setVertexBytes:vertices_portrait_upsideddown length:sizeof(vertices_portrait_upsideddown) atIndex:0];
                            break;
                        case AVCaptureVideoOrientationLandscapeRight:
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
            }
        });

        // To match the web implementation if the sensor is rotated into a portrait orientation then the width and height
        // of the video should be swapped
        return m_impl->cameraTextureDelegate->VideoOrientation == AVCaptureVideoOrientationLandscapeLeft ||
            m_impl->cameraTextureDelegate->VideoOrientation == AVCaptureVideoOrientationLandscapeRight ?
            CameraDimensions{m_impl->cameraDimensions.width, m_impl->cameraDimensions.height} :
            CameraDimensions{m_impl->cameraDimensions.width, m_impl->cameraDimensions.height};    }

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
    }
}

@implementation CameraTextureDelegate {
    CVMetalTextureRef cameraTextureY;
    CVMetalTextureRef cameraTextureCbCr;
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
    self->VideoOrientation = AVCaptureVideoOrientationLandscapeLeft;
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

    // Convert from UIInterfaceOrientation to AVCaptureVideoOrientation. The conversion is only used becauase
    // MacOS doesn't have access to UIInterfaceOrientation but it does have AVCaptureVideoOrientation which
    // lets us share more code without ifdefs
    switch (orientation)
        {
            case UIInterfaceOrientationUnknown:
            case UIInterfaceOrientationLandscapeLeft:
                self->VideoOrientation = AVCaptureVideoOrientationLandscapeLeft;
                break;
            case UIInterfaceOrientationLandscapeRight:
                self->VideoOrientation = AVCaptureVideoOrientationLandscapeRight;
                break;
            case UIInterfaceOrientationPortrait:
                self->VideoOrientation = AVCaptureVideoOrientationPortrait;
                break;
            case UIInterfaceOrientationPortraitUpsideDown:
                self->VideoOrientation = AVCaptureVideoOrientationPortraitUpsideDown;
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

#include "../CameraDeviceSharedPImpl.h"
