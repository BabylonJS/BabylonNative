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
    std::shared_ptr<Babylon::Plugins::Camera::Impl::ImplData> implData;
    AVCaptureVideoOrientation videoOrientation;
    bool orientationUpdated;
}

- (id)init:(std::shared_ptr<Babylon::Plugins::Camera::Impl::ImplData>)implData;

@end

namespace Babylon::Plugins
{
    struct Camera::Impl::ImplData
    {
        ~ImplData()
        {
            [avCaptureSession stopRunning];
            [avCaptureSession release];
            [cameraTextureDelegate release];
            if (textureCache)
            {
                CVMetalTextureCacheFlush(textureCache, 0);
                CFRelease(textureCache);
            }
        }
        
        CameraTextureDelegate* cameraTextureDelegate{};
        AVCaptureSession* avCaptureSession{};
        CVMetalTextureCacheRef textureCache{};
        id <MTLTexture> textureBGRA{};
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

    arcana::task<void, std::exception_ptr> Camera::Impl::Open(uint32_t maxWidth, uint32_t maxHeight, bool frontCamera)
    {
        if (maxWidth == 0 || maxWidth > std::numeric_limits<int32_t>::max()) {
            maxWidth = std::numeric_limits<int32_t>::max();
        }
        if (maxHeight == 0 || maxHeight > std::numeric_limits<int32_t>::max()) {
            maxHeight = std::numeric_limits<int32_t>::max();
        }
        
        auto metalDevice = (id<MTLDevice>)bgfx::getInternalData()->context;

        if (!m_deviceContext)
        {
            m_deviceContext = &Graphics::DeviceContext::GetFromJavaScript(m_env);
        }
        
        __block arcana::task_completion_source<void, std::exception_ptr> taskCompletionSource{};

        dispatch_sync(dispatch_get_main_queue(), ^{
            CVMetalTextureCacheCreate(nullptr, nullptr, metalDevice, nullptr, &m_implData->textureCache);
            m_implData->cameraTextureDelegate = [[CameraTextureDelegate alloc]init:m_implData];
            m_implData->avCaptureSession = [[AVCaptureSession alloc] init];
            
#if (TARGET_OS_IPHONE)
            // Loop over all available camera configurations to find a config that most closely matches the constraints.
            AVCaptureDevice* bestDevice{nullptr};
            AVCaptureDeviceFormat* bestFormat{nullptr};
            uint32_t bestPixelCount{0};
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
            
            [bestDevice setActiveFormat:bestFormat];
            AVCaptureDeviceInput *input{[AVCaptureDeviceInput deviceInputWithDevice:bestDevice error:&error]};
            [bestDevice unlockForConfiguration];
#else
            UNUSED(maxWidth);
            UNUSED(maxHeight);
            UNUSED(frontCamera);
            NSError *error{nil};
            AVCaptureDevice* captureDevice{[AVCaptureDevice defaultDeviceWithMediaType:AVMediaTypeVideo]};
            AVCaptureDeviceInput *input{[AVCaptureDeviceInput deviceInputWithDevice:captureDevice error:&error]};
#endif

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
            [dataOutput setVideoSettings:@{(id)kCVPixelBufferPixelFormatTypeKey: @(kCVPixelFormatType_32BGRA)}];
            [dataOutput setSampleBufferDelegate:m_implData->cameraTextureDelegate queue:sampleBufferQueue];

            // Actually start the camera session.
            [m_implData->avCaptureSession addOutput:dataOutput];
            [m_implData->avCaptureSession commitConfiguration];
            [m_implData->avCaptureSession startRunning];
            
            taskCompletionSource.complete();
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
            if (m_implData->textureBGRA)
            {
                bgfx::overrideInternal(textureHandle, reinterpret_cast<uintptr_t>(m_implData->textureBGRA));
            }
        });
    }

    void Camera::Impl::Close()
    {
        m_implData.reset();
    }
}

@implementation CameraTextureDelegate

- (id)init:(std::shared_ptr<Babylon::Plugins::Camera::Impl::ImplData>)implData
{
    self = [super init];
    self->implData = implData;
#if (TARGET_OS_IPHONE)
    [[NSNotificationCenter defaultCenter]addObserver:self selector:@selector(OrientationDidChange:) name:UIDeviceOrientationDidChangeNotification object:nil];
    [self updateOrientation];
    self->orientationUpdated = true;
#else
    // Orientation not supported on these devices.
    self->videoOrientation = AVCaptureVideoOrientationPortrait;
    self->orientationUpdated = false;
#endif

    return self;
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
    id<MTLTexture> textureBGRA{nil};

    size_t width = CVPixelBufferGetWidthOfPlane(pixelBuffer, 0);
    size_t height = CVPixelBufferGetHeightOfPlane(pixelBuffer, 0);
    MTLPixelFormat pixelFormat{MTLPixelFormatBGRA8Unorm};
    
    CVMetalTextureRef texture{nullptr};
    CVReturn status{CVMetalTextureCacheCreateTextureFromImage(nullptr, implData->textureCache, pixelBuffer, nullptr, pixelFormat, width, height, 0, &texture)};
    if (status == kCVReturnSuccess)
    {
        textureBGRA = CVMetalTextureGetTexture(texture);
        CFRelease(texture);
    }

    if (textureBGRA != nil)
    {
        dispatch_async(dispatch_get_main_queue(), ^{
            implData->textureBGRA = textureBGRA;
        });
    }
}

@end
