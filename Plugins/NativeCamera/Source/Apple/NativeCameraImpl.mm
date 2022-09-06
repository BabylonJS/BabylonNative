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

#include "NativeCameraImpl.h"
#include <napi/napi.h>

@interface CameraTextureDelegate : NSObject <AVCaptureVideoDataOutputSampleBufferDelegate>
{
    @public AVCaptureVideoOrientation videoOrientation;

    std::shared_ptr<Babylon::Plugins::Camera::Impl::ImplData> implData;
    bool orientationUpdated;
}

- (id)init:(std::shared_ptr<Babylon::Plugins::Camera::Impl::ImplData>)implData;
- (id<MTLTexture>)getCameraTextureY;
- (id<MTLTexture>)getCameraTextureCbCr;

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
    struct Camera::Impl::ImplData
    {
        ~ImplData()
        {
            if (currentCommandBuffer != nil) {
                [currentCommandBuffer waitUntilCompleted];
            }

            [avCaptureSession stopRunning];

            if (textureCache)
            {
                CVMetalTextureCacheFlush(textureCache, 0);
                CFRelease(textureCache);
            }
        }
        
        CameraTextureDelegate* cameraTextureDelegate{};
        AVCaptureSession* avCaptureSession{};
        CVMetalTextureCacheRef textureCache{};
        id <MTLTexture> textureRGBA{};
        id<MTLRenderPipelineState> cameraPipelineState{};
        id<MTLDevice> metalDevice{};
        id<MTLCommandQueue> commandQueue{};
        id<MTLCommandBuffer> currentCommandBuffer{};
    };
    Camera::Impl::Impl(Napi::Env env, bool overrideCameraTexture)
        : m_deviceContext{nullptr}
        , m_env{env}
        , m_implData{std::make_unique<ImplData>()}
        , m_overrideCameraTexture{overrideCameraTexture}
    {
    }

    Camera::Impl::~Impl()
    {
    }

    arcana::task<Camera::Impl::CameraDimensions, std::exception_ptr> Camera::Impl::Open(uint32_t maxWidth, uint32_t maxHeight, bool frontCamera)
    {
        m_implData->commandQueue = (__bridge id<MTLCommandQueue>)bgfx::getInternalData()->commandQueue;
        m_implData->metalDevice = (__bridge id<MTLDevice>)bgfx::getInternalData()->context;

        if (maxWidth == 0 || maxWidth > std::numeric_limits<int32_t>::max()) {
            maxWidth = std::numeric_limits<int32_t>::max();
        }
        if (maxHeight == 0 || maxHeight > std::numeric_limits<int32_t>::max()) {
            maxHeight = std::numeric_limits<int32_t>::max();
        }

        if (!m_deviceContext)
        {
            m_deviceContext = &Graphics::DeviceContext::GetFromJavaScript(m_env);
        }
        
        __block arcana::task_completion_source<Camera::Impl::CameraDimensions, std::exception_ptr> taskCompletionSource{};

        dispatch_sync(dispatch_get_main_queue(), ^{
            CVMetalTextureCacheCreate(nullptr, nullptr, m_implData->metalDevice, nullptr, &m_implData->textureCache);
            m_implData->cameraTextureDelegate = [[CameraTextureDelegate alloc]init:m_implData];
            m_implData->avCaptureSession = [[AVCaptureSession alloc] init];

#if (TARGET_OS_IPHONE)
            // Loop over all available camera configurations to find a config that most closely matches the constraints.
            AVCaptureDevice* bestDevice{nullptr};
            AVCaptureDeviceFormat* bestFormat{nullptr};
            uint32_t bestPixelCount{0};
            uint32_t devicePixelFormat{0};
            uint32_t bestDimDiff{0};
            NSArray* deviceTypes{nullptr};
            bool foundExactMatch{false};
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
                    AVCaptureDeviceTypeBuiltInTrueDepthCamera
                ];
            }
            else
            {
                // Only these camera types are available for all devices
                deviceTypes = @[
                    AVCaptureDeviceTypeBuiltInDualCamera,
                    AVCaptureDeviceTypeBuiltInWideAngleCamera
                ];
            }

            AVCaptureDeviceDiscoverySession* discoverySession{[AVCaptureDeviceDiscoverySession
               discoverySessionWithDeviceTypes:deviceTypes
               mediaType:AVMediaTypeVideo position:frontCamera ? AVCaptureDevicePositionFront: AVCaptureDevicePositionBack]};
            for (AVCaptureDevice* device in discoverySession.devices)
            {
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

                    // Reject any resolution that doesn't qualify for the constraint.
                    if (static_cast<uint32_t>(dimensions.width) > maxWidth || static_cast<uint32_t>(dimensions.height) > maxHeight)
                    {
                        continue;
                    }
                    
                    // Calculate pixel count and dimension differential and take the best qualifying one.
                    uint32_t pixelCount{static_cast<uint32_t>(dimensions.width * dimensions.height)};
                    uint32_t dimDiff{(maxWidth - dimensions.width) + (maxHeight - dimensions.height)};
                    if (bestDevice == nullptr || pixelCount > bestPixelCount || (pixelCount == bestPixelCount && dimDiff < bestDimDiff))
                    {
                        bestPixelCount = pixelCount;
                        devicePixelFormat = pixelFormat;
                        bestDevice = device;
                        bestFormat = format;
                        bestDimDiff = dimDiff;
                        
                        // Check if we got an exact match, and exit the loop early in this case.
                        if (static_cast<uint32_t>(dimensions.width) == maxWidth && static_cast<uint32_t>(dimensions.height) == maxHeight)
                        {
                            foundExactMatch = true;
                            break;
                        }
                    }
                }
                
                if (foundExactMatch)
                {
                    break;
                }
            }
            
            // If no matching device, throw an error with the message "ConstraintError" which matches the behavior in the browser.
            if (bestDevice == nullptr)
            {
                taskCompletionSource.complete(arcana::make_unexpected(
                    std::make_exception_ptr(std::runtime_error{"ConstraintError: Unable to match constraints to a supported camera configuration."})));
                return;
            }
                       
            // Lock camera device and set up camera format. If there a problem initialising the camera it will give an error.
            NSError *error{nil};
            [bestDevice lockForConfiguration:&error];
            if (error != nil)
            {
                taskCompletionSource.complete(arcana::make_unexpected(std::make_exception_ptr(std::runtime_error{"Failed to lock camera"})));
                return;
            }
            
            [m_implData->avCaptureSession setSessionPreset:AVCaptureSessionPresetInputPriority];
            [bestDevice setActiveFormat:bestFormat];
            AVCaptureDeviceInput *input{[AVCaptureDeviceInput deviceInputWithDevice:bestDevice error:&error]};
            [bestDevice unlockForConfiguration];
            
            // Capture the format dimensions.
            CMVideoFormatDescriptionRef videoFormatRef{static_cast<CMVideoFormatDescriptionRef>(bestFormat.formatDescription)};
            CMVideoDimensions dimensions{CMVideoFormatDescriptionGetDimensions(videoFormatRef)};
#else
            UNUSED(maxWidth);
            UNUSED(maxHeight);
            UNUSED(frontCamera);
            NSError *error{nil};
            AVCaptureDevice* captureDevice{[AVCaptureDevice defaultDeviceWithMediaType:AVMediaTypeVideo]};
            AVCaptureDeviceInput *input{[AVCaptureDeviceInput deviceInputWithDevice:captureDevice error:&error]};
            CMVideoFormatDescriptionRef videoFormatRef{static_cast<CMVideoFormatDescriptionRef>(captureDevice.activeFormat.formatDescription)};
            CMVideoDimensions dimensions{CMVideoFormatDescriptionGetDimensions(videoFormatRef)};
            uint32_t devicePixelFormat{static_cast<uint32_t>(CMFormatDescriptionGetMediaSubType(videoFormatRef))};
            if (!isPixelFormatSupported(devicePixelFormat))
            {
                taskCompletionSource.complete(arcana::make_unexpected(
                    std::make_exception_ptr(std::runtime_error{"ConstraintError: Unable to match constraints to a supported camera configuration."})));
                return;
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
                taskCompletionSource.complete(arcana::make_unexpected(std::make_exception_ptr(std::runtime_error{"Error Getting Camera Input"})));
                return;
            }
            
            // Add camera input source to the capture session.
            [m_implData->avCaptureSession addInput:input];

            // Create the camera buffer.
            dispatch_queue_t sampleBufferQueue{dispatch_queue_create("CameraMulticaster", DISPATCH_QUEUE_SERIAL)};
            AVCaptureVideoDataOutput * dataOutput{[[AVCaptureVideoDataOutput alloc] init]};
            [dataOutput setAlwaysDiscardsLateVideoFrames:YES];
            [dataOutput setVideoSettings:@{(id)kCVPixelBufferPixelFormatTypeKey: @(devicePixelFormat)}];
            [dataOutput setSampleBufferDelegate:m_implData->cameraTextureDelegate queue:sampleBufferQueue];

            // Actually start the camera session.
            [m_implData->avCaptureSession addOutput:dataOutput];
            [m_implData->avCaptureSession commitConfiguration];
            [m_implData->avCaptureSession startRunning];
            
            // Create a pipeline state for converting the camera output to RGBA.
            id<MTLLibrary> lib = CompileShader(m_implData->metalDevice, shaderSource);
            id<MTLFunction> vertexFunction = [lib newFunctionWithName:@"vertexShader"];
            id<MTLFunction> fragmentFunction = [lib newFunctionWithName:@"fragmentShader"];

            MTLRenderPipelineDescriptor *pipelineStateDescriptor = [[MTLRenderPipelineDescriptor alloc] init];
            pipelineStateDescriptor.label = @"Native Camera YCbCr to RGBA Pipeline";
            pipelineStateDescriptor.vertexFunction = vertexFunction;
            pipelineStateDescriptor.fragmentFunction = fragmentFunction;
            pipelineStateDescriptor.colorAttachments[0].pixelFormat = MTLPixelFormatRGBA8Unorm;
            m_implData->cameraPipelineState = [m_implData->metalDevice newRenderPipelineStateWithDescriptor:pipelineStateDescriptor error:&error];

            if (!m_implData->cameraPipelineState) {
                taskCompletionSource.complete(arcana::make_unexpected(std::make_exception_ptr(std::runtime_error{
                    std::string("Failed to create camera pipeline state: ") + [error.localizedDescription cStringUsingEncoding:NSASCIIStringEncoding]})));
                return;
            }

            taskCompletionSource.complete(cameraDimensions);
        });
        
        return taskCompletionSource.as_task();
    }

    void Camera::Impl::SetTextureOverride(void* /*texturePtr*/)
    {
        if (!m_overrideCameraTexture)
        {
            throw std::runtime_error{"Trying to override NativeCamera Texture."};
        }
        // stub
    }

    void Camera::Impl::UpdateCameraTexture(bgfx::TextureHandle textureHandle)
    {
        arcana::make_task(m_deviceContext->BeforeRenderScheduler(), arcana::cancellation::none(), [this, textureHandle] {
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
            
            // Skip processing this frame.
            if (width == 0 || height == 0) {
                return;
            }

            // Recreate the output texture when the camera dimensions change.
            if (m_cameraDimensions.width != width || m_cameraDimensions.height != height)
            {
                MTLTextureDescriptor *textureDescriptor = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatRGBA8Unorm width:width height:height mipmapped:NO];
                textureDescriptor.usage = MTLTextureUsageRenderTarget | MTLTextureUsageShaderRead;
                m_implData->textureRGBA = [m_implData->metalDevice newTextureWithDescriptor:textureDescriptor];
                bgfx::overrideInternal(textureHandle, reinterpret_cast<uintptr_t>(m_implData->textureRGBA));
                m_cameraDimensions.width = static_cast<uint32_t>(width);
                m_cameraDimensions.height = static_cast<uint32_t>(height);
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

    void Camera::Impl::Close()
    {
        m_implData.reset(new ImplData);
    }
}

@implementation CameraTextureDelegate {
    CVMetalTextureRef cameraTextureY;
    CVMetalTextureRef cameraTextureCbCr;
}

- (id)init:(std::shared_ptr<Babylon::Plugins::Camera::Impl::ImplData>)implData
{
    self = [super init];
    self->implData = implData;
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

    // Create a texture from the corresponding plane.
    auto status = CVMetalTextureCacheCreateTextureFromImage(kCFAllocatorDefault, implData->textureCache, pixelBuffer, nil, pixelFormat, planeWidth, planeHeight, planeIndex, &textureRef);
    if (status != kCVReturnSuccess) {
        CVBufferRelease(textureRef);
        textureRef = nil;
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
  [self cleanupTextures];
}

@end
