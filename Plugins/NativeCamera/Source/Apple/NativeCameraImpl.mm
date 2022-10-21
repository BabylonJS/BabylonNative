#if ! __has_feature(objc_arc)
#error "ARC is off"
#endif

#import <MetalKit/MetalKit.h>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include "NativeCamera.h"
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

#include "../NativeCameraImpl.h"
#include <napi/napi.h>

@interface CameraTextureDelegate : NSObject <AVCaptureVideoDataOutputSampleBufferDelegate>
{
    @public AVCaptureVideoOrientation videoOrientation;

    CVMetalTextureCacheRef textureCache;
    bool orientationUpdated;
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

    constexpr Vertex vertices[] = {
        // 2D positions, UV
        { { -1, -1 },   { 0, 1 } },
        { { -1, 1 },    { 0, 0 } },
        { { 1, -1 },    { 1, 1 } },
        { { 1, 1 },     { 1, 0 } },
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
                        constant Vertex *vertices [[buffer(0)]])
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
    struct CameraTrack::ImplData
    {
        AVCaptureDeviceFormat* avDeviceFormat;
        uint32_t pixelFormat;
    };

    CameraTrack::~CameraTrack() = default;

    struct CameraDevice::ImplData
    {
        AVCaptureDevice* avDevice;
    };

    CameraDevice::~CameraDevice() = default;

    struct Camera::Impl::ImplData
    {
        ImplData(Napi::Env env, bool overrideCameraTexture)
            : deviceContext{nullptr}
            , env{env}
            , overrideCameraTexture{overrideCameraTexture} {};
        
        ~ImplData()
        {
        }
        
        Graphics::DeviceContext* deviceContext;
        Napi::Env env;
        bool overrideCameraTexture{};
        CameraDimensions cameraDimensions{};

        CameraTextureDelegate* cameraTextureDelegate{};
        AVCaptureDevice* avCaptureDevice{};
        AVCaptureSession* avCaptureSession{};
        CVMetalTextureCacheRef textureCache{};
        id<MTLTexture> textureRGBA{};
        id<MTLRenderPipelineState> cameraPipelineState{};
        id<MTLDevice> metalDevice{};
        id<MTLCommandQueue> commandQueue{};
        id<MTLCommandBuffer> currentCommandBuffer{};
        bool isInitialized{false};
        bool refreshBgfxHandle{true};
        
        arcana::background_dispatcher<32> cameraSessionDispatcher{};
    };
    Camera::Impl::Impl(Napi::Env env, bool overrideCameraTexture)
        : m_implData{std::make_unique<ImplData>(env, overrideCameraTexture)}
    {
    }

    Camera::Impl::~Impl()
    {
    }

    std::vector<std::shared_ptr<CameraDevice>> Camera::Impl::GetCameraDevices()
    {
        std::vector<std::shared_ptr<CameraDevice>> cameraDevices{};
        
        NSArray* deviceTypes{@[
            // Use the main camera on the device which is best for general use and will typically be 1x optical zoom and the highest resolution.
            // https://developer.apple.com/documentation/avfoundation/avcapturedevice/devicetype/2361449-builtinwideanglecamera
            AVCaptureDeviceTypeBuiltInWideAngleCamera
        ]};
        bool foundExactMatch{false};

        // The below code would allow us to choose virtual cameras which dynamically switch between the different physical
        // camera sensors based on different zoom levels. Until we support letting the consumer modify the camera's zoom
        // it would result in often being stuck on the most zoomed out camera on the device which is often not desired.

        // if (@available(iOS 13.0, *))
        // {
        //     // Ordered list of cameras by general usage quality.
        //     deviceTypes = @[
        //         AVCaptureDeviceTypeBuiltInTripleCamera,
        //         AVCaptureDeviceTypeBuiltInDualCamera,
        //         AVCaptureDeviceTypeBuiltInDualWideCamera,
        //         AVCaptureDeviceTypeBuiltInWideAngleCamera,
        //         AVCaptureDeviceTypeBuiltInUltraWideCamera,
        //         AVCaptureDeviceTypeBuiltInTelephotoCamera,
        //         AVCaptureDeviceTypeBuiltInTrueDepthCamera
        //     ];
        // }

        AVCaptureDeviceDiscoverySession* discoverySession{[AVCaptureDeviceDiscoverySession
                                                           discoverySessionWithDeviceTypes:deviceTypes
                                                           mediaType:AVMediaTypeVideo
                                                           position:AVCaptureDevicePositionUnspecified]};
        
        for (AVCaptureDevice* device in discoverySession.devices)
        {
            auto cameraDevice{ std::make_shared<CameraDevice>()};
            
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
                
                auto cameraTrack{ std::make_shared<CameraTrack>()};
                cameraTrack->width =  dimensions.width;
                cameraTrack->height = dimensions.height;
                cameraTrack->implData = std::make_unique<CameraTrack::ImplData>();
                cameraTrack->implData->avDeviceFormat = format;
                cameraTrack->implData->pixelFormat = pixelFormat;
                cameraDevice->supportedResolutions.push_back(cameraTrack);
            }
                        
            // update the cameraDevice information
            cameraDevice->facingMode = device.position == AVCaptureDevicePositionFront ? "user" : "environment";
            cameraDevice->supportsTorch = device.isTorchAvailable;
            cameraDevice->implData = std::make_unique<CameraDevice::ImplData>();
            cameraDevice->implData->avDevice = device;

            cameraDevices.push_back(cameraDevice);

            if (foundExactMatch)
            {
                break;
            }
        }
        
        return cameraDevices;
    }


    arcana::task<Camera::Impl::CameraDimensions, std::exception_ptr> Camera::Impl::Open(std::shared_ptr<CameraDevice> cameraDevice, std::shared_ptr<CameraTrack> resolution)
    {
        NSError *error{nil};

        // This is the first time the camera has been opened, perform some one time setup.
        if (!m_implData->isInitialized) {
            m_implData->commandQueue = (__bridge id<MTLCommandQueue>)bgfx::getInternalData()->commandQueue;
            m_implData->metalDevice = (__bridge id<MTLDevice>)bgfx::getInternalData()->context;
            m_implData->deviceContext = &Graphics::DeviceContext::GetFromJavaScript(m_implData->env);

            // Compile shaders used for converting camera output to RGBA.
            id<MTLLibrary> lib = CompileShader(m_implData->metalDevice, shaderSource);
            id<MTLFunction> vertexFunction = [lib newFunctionWithName:@"vertexShader"];
            id<MTLFunction> fragmentFunction = [lib newFunctionWithName:@"fragmentShader"];

            // Create a pipeline state for converting the camera output to RGBA.
            MTLRenderPipelineDescriptor *pipelineStateDescriptor = [[MTLRenderPipelineDescriptor alloc] init];
            pipelineStateDescriptor.label = @"Native Camera YCbCr to RGBA Pipeline";
            pipelineStateDescriptor.vertexFunction = vertexFunction;
            pipelineStateDescriptor.fragmentFunction = fragmentFunction;
            pipelineStateDescriptor.colorAttachments[0].pixelFormat = MTLPixelFormatRGBA8Unorm;
            m_implData->cameraPipelineState = [m_implData->metalDevice newRenderPipelineStateWithDescriptor:pipelineStateDescriptor error:&error];

            if (!m_implData->cameraPipelineState) {
                return arcana::task_from_error<CameraDimensions>(std::make_exception_ptr(std::runtime_error{
                    std::string("Failed to create camera pipeline state: ") + [error.localizedDescription cStringUsingEncoding:NSASCIIStringEncoding]}));
            }

            m_implData->isInitialized = true;
        } else {
            // Always refresh the bgfx handle to point to textureRGBA on re-open.
            m_implData->refreshBgfxHandle = true;
        }

        // Construct the camera texture delegate, which is responsible for handling updates for device orientation and the capture session.
        CVMetalTextureCacheCreate(nullptr, nullptr, m_implData->metalDevice, nullptr, &m_implData->textureCache);
        m_implData->cameraTextureDelegate = [[CameraTextureDelegate alloc]init:m_implData->textureCache];

#if (TARGET_OS_IPHONE)
        // Lock camera device and set up camera format. If there a problem initialising the camera it will give an error.
        [cameraDevice->implData->avDevice lockForConfiguration:&error];
        if (error != nil)
        {
            return arcana::task_from_error<CameraDimensions>(std::make_exception_ptr(std::runtime_error{"Failed to lock camera"}));
        }

        [cameraDevice->implData->avDevice setActiveFormat:resolution->implData->avDeviceFormat];
        AVCaptureDeviceInput *input{[AVCaptureDeviceInput deviceInputWithDevice:cameraDevice->implData->avDevice error:&error]};
        [cameraDevice->implData->avDevice setTorchMode:AVCaptureTorchModeOn];
        [cameraDevice->implData->avDevice unlockForConfiguration];

        // Capture the format dimensions.
        CMVideoFormatDescriptionRef videoFormatRef{static_cast<CMVideoFormatDescriptionRef>(resolution->implData->avDeviceFormat.formatDescription)};
        CMVideoDimensions dimensions{CMVideoFormatDescriptionGetDimensions(videoFormatRef)};
#else
        UNUSED(maxWidth);
        UNUSED(maxHeight);
        UNUSED(frontCamera);
        AVCaptureDevice* captureDevice{[AVCaptureDevice defaultDeviceWithMediaType:AVMediaTypeVideo]};
        AVCaptureDeviceInput *input{[AVCaptureDeviceInput deviceInputWithDevice:captureDevice error:&error]};
        CMVideoFormatDescriptionRef videoFormatRef{static_cast<CMVideoFormatDescriptionRef>(captureDevice.activeFormat.formatDescription)};
        CMVideoDimensions dimensions{CMVideoFormatDescriptionGetDimensions(videoFormatRef)};
        uint32_t devicePixelFormat{static_cast<uint32_t>(CMFormatDescriptionGetMediaSubType(videoFormatRef))};
        if (!isPixelFormatSupported(devicePixelFormat))
        {
            return arcana::task_from_error<CameraDimensions>(std::make_exception_ptr(std::runtime_error{"ConstraintError: Unable to match constraints to a supported camera configuration."}));
        }
#endif

        Camera::Impl::CameraDimensions cameraDimensions{static_cast<uint32_t>(dimensions.width), static_cast<uint32_t>(dimensions.height)};

        // For portrait orientations swap the height and width of the video format dimensions.
        if (m_implData->cameraTextureDelegate->videoOrientation == AVCaptureVideoOrientationPortrait
            ||  m_implData->cameraTextureDelegate->videoOrientation == AVCaptureVideoOrientationPortraitUpsideDown)
        {
            std::swap(cameraDimensions.width, cameraDimensions.height);
        }
        
        // Check for failed initialisation.
        if (!input)
        {
            return arcana::task_from_error<CameraDimensions>(std::make_exception_ptr(std::runtime_error{"Error Getting Camera Input"}));
        }
        
        m_implData->avCaptureDevice = cameraDevice->implData->avDevice;

        // Kick off camera session on a background thread.
        return arcana::make_task(m_implData->cameraSessionDispatcher, arcana::cancellation::none(), [implObj = shared_from_this(), input, resolution, cameraDimensions]() mutable {
            if (implObj->m_implData->avCaptureSession == nil) {
                implObj->m_implData->avCaptureSession = [[AVCaptureSession alloc] init];
            } else {
                for (AVCaptureInput* input in [implObj->m_implData->avCaptureSession inputs]) {
                    [implObj->m_implData->avCaptureSession removeInput: input];
                }

                for (AVCaptureOutput* output in [implObj->m_implData->avCaptureSession outputs]) {
                    [implObj->m_implData->avCaptureSession removeOutput: output];
                }
            }

#if (TARGET_OS_IPHONE)
            [implObj->m_implData->avCaptureSession setSessionPreset:AVCaptureSessionPresetInputPriority];
#endif

            // Add camera input source to the capture session.
            [implObj->m_implData->avCaptureSession addInput:input];

            // Create the camera buffer, and set up camera texture delegate to capture frames.
            dispatch_queue_t sampleBufferQueue{dispatch_queue_create("CameraMulticaster", DISPATCH_QUEUE_SERIAL)};
            AVCaptureVideoDataOutput* dataOutput{[[AVCaptureVideoDataOutput alloc] init]};
            [dataOutput setAlwaysDiscardsLateVideoFrames:YES];
            [dataOutput setVideoSettings:@{(id)kCVPixelBufferPixelFormatTypeKey: @(resolution->implData->pixelFormat)}];
            [dataOutput setSampleBufferDelegate:implObj->m_implData->cameraTextureDelegate queue:sampleBufferQueue];
            [implObj->m_implData->avCaptureSession addOutput:dataOutput];

            // Actually start the camera session.
            [implObj->m_implData->avCaptureSession startRunning];
            return cameraDimensions;
        });
    }

    void Camera::Impl::SetTextureOverride(void* /*texturePtr*/)
    {
        if (!m_implData->overrideCameraTexture)
        {
            throw std::runtime_error{"Trying to override NativeCamera Texture."};
        }
        // stub
    }

    void Camera::Impl::UpdateCameraTexture(bgfx::TextureHandle textureHandle)
    {
        arcana::make_task(m_implData->deviceContext->BeforeRenderScheduler(), arcana::cancellation::none(), [this, textureHandle] {
            id<MTLTexture> textureY{};
            id<MTLTexture> textureCbCr{};
            int64_t width{0};
            int64_t height{0};

            @synchronized(m_implData->cameraTextureDelegate) {
                textureY = [m_implData->cameraTextureDelegate getCameraTextureY];
                textureCbCr = [m_implData->cameraTextureDelegate getCameraTextureCbCr];
                width = [textureY width];
                height = [textureY height];
            }

            // Skip processing this frame if width and height are invalid.
            if (width == 0 || height == 0) {
                return;
            }

            // Recreate the output texture when the camera dimensions change.
            if (m_implData->textureRGBA == nil || m_implData->cameraDimensions.width != width || m_implData->cameraDimensions.height != height)
            {
                MTLTextureDescriptor *textureDescriptor = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatRGBA8Unorm width:width height:height mipmapped:NO];
                textureDescriptor.usage = MTLTextureUsageRenderTarget | MTLTextureUsageShaderRead;
                m_implData->textureRGBA = [m_implData->metalDevice newTextureWithDescriptor:textureDescriptor];
                bgfx::overrideInternal(textureHandle, reinterpret_cast<uintptr_t>(m_implData->textureRGBA));
                m_implData->cameraDimensions.width = static_cast<uint32_t>(width);
                m_implData->cameraDimensions.height = static_cast<uint32_t>(height);
                m_implData->refreshBgfxHandle = false;
            } else if (m_implData->refreshBgfxHandle) {
                // On texture re-use across sessions set the bgfx texture handle.
                bgfx::overrideInternal(textureHandle, reinterpret_cast<uintptr_t>(m_implData->textureRGBA));
                m_implData->refreshBgfxHandle = false;
            }

            if (textureY != nil && textureCbCr != nil && m_implData->textureRGBA != nil)
            {
                m_implData->currentCommandBuffer = [m_implData->commandQueue commandBuffer];
                m_implData->currentCommandBuffer.label = @"NativeCameraCommandBuffer";
                MTLRenderPassDescriptor *renderPassDescriptor = [MTLRenderPassDescriptor renderPassDescriptor];

                if (renderPassDescriptor != nil) {
                    // Attach the color texture, on which we'll draw the camera texture (so no need to clear on load).
                    renderPassDescriptor.colorAttachments[0].texture = m_implData->textureRGBA;
                    renderPassDescriptor.colorAttachments[0].loadAction = MTLLoadActionDontCare;
                    renderPassDescriptor.colorAttachments[0].storeAction = MTLStoreActionStore;

                    // Create and end the render encoder.
                    id<MTLRenderCommandEncoder> renderEncoder = [m_implData->currentCommandBuffer renderCommandEncoderWithDescriptor:renderPassDescriptor];
                    renderEncoder.label = @"NativeCameraEncoder";

                    // Set the shader pipeline.
                    [renderEncoder setRenderPipelineState:m_implData->cameraPipelineState];

                    // Set the vertex data.
                    [renderEncoder setVertexBytes:vertices length:sizeof(vertices) atIndex:0];

                    // Set the textures.
                    [renderEncoder setFragmentTexture:textureY atIndex:1];
                    [renderEncoder setFragmentTexture:textureCbCr atIndex:2];

                    // Draw the triangles.
                    [renderEncoder drawPrimitives:MTLPrimitiveTypeTriangleStrip vertexStart:0 vertexCount:4];

                    [renderEncoder endEncoding];

                    [m_implData->currentCommandBuffer addCompletedHandler:^(id<MTLCommandBuffer>) {
                        if (textureY != nil) {
                            [textureY setPurgeableState:MTLPurgeableStateEmpty];
                        }

                        if (textureCbCr != nil) {
                            [textureCbCr setPurgeableState:MTLPurgeableStateEmpty];
                        }
                    }];
                }

                // Finalize rendering here & push the command buffer to the GPU.
                [m_implData->currentCommandBuffer commit];
            }
        });
    }

    bool Camera::Impl::SetCapability(CameraCapability capability, std::variant<bool, std::string> value)
    {
        NSError* error{nil};

        switch(capability)
        {
            case Torch:
                AVCaptureTorchMode torchMode{ std::get<bool>(value) ? AVCaptureTorchModeOn : AVCaptureTorchModeOff };
                [m_implData->avCaptureDevice lockForConfiguration:&error];
                [m_implData->avCaptureDevice setTorchMode:torchMode];
                [m_implData->avCaptureDevice unlockForConfiguration];
                if (error != nil)
                {
                    return false;
                }
                return true;
        }

        return false;
    }

    void Camera::Impl::Close()
    {
        // Stop collecting frames, release camera texture delegate.
        [m_implData->cameraTextureDelegate reset];
        m_implData->cameraTextureDelegate = nil;

        // Complete any running command buffers before destroying the cache.
        if (m_implData->currentCommandBuffer != nil) {
            [m_implData->currentCommandBuffer waitUntilCompleted];
        }

        // Free the texture cache.
        if (m_implData->textureCache)
        {
            CVMetalTextureCacheFlush(m_implData->textureCache, 0);
            CFRelease(m_implData->textureCache);
            m_implData->textureCache = nil;
        }

        if (m_implData->avCaptureSession != nil) {
            arcana::make_task(m_implData->cameraSessionDispatcher, arcana::cancellation::none(), [implObj = shared_from_this()](){
                [implObj->m_implData->avCaptureSession stopRunning];
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
    self->orientationUpdated = true;
#else
    // Orientation not supported on non-iOS devices.
    self->videoOrientation = AVCaptureVideoOrientationLandscapeLeft;
    self->orientationUpdated = false;
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

    // Determine device orienation, and adjust output to match.
    AVCaptureVideoOrientation newVideoOrientation{AVCaptureVideoOrientationPortraitUpsideDown};
    switch (orientation)
        {
            case UIInterfaceOrientationUnknown:
                return;
            case UIInterfaceOrientationPortrait:
                newVideoOrientation = AVCaptureVideoOrientationPortraitUpsideDown;
                break;
            case UIInterfaceOrientationPortraitUpsideDown:
                newVideoOrientation = AVCaptureVideoOrientationPortrait;
                break;
            case UIInterfaceOrientationLandscapeLeft:
                newVideoOrientation = AVCaptureVideoOrientationLandscapeRight;
                break;
            case UIInterfaceOrientationLandscapeRight:
                newVideoOrientation = AVCaptureVideoOrientationLandscapeLeft;
                break;
        }

    if (newVideoOrientation != self->videoOrientation)
    {
        self->videoOrientation = newVideoOrientation;
        self->orientationUpdated = true;
    }
}

-(void)OrientationDidChange:(NSNotification*)notification
{
    [self updateOrientation];
}
#endif

- (void)captureOutput:(AVCaptureOutput *)__unused captureOutput didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer fromConnection:(AVCaptureConnection *) connection
{
    if (self->orientationUpdated)
    {
        connection.videoMirrored = true;
        connection.videoOrientation = self->videoOrientation;
        self->orientationUpdated = false;
    }

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
