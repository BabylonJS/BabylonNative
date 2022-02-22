#import <MetalKit/MetalKit.h>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include "NativeCamera.h"
#include <arcana/macros.h>
#include <arcana/threading/task.h>
#include <arcana/threading/dispatcher.h>
#include <Babylon/JsRuntimeScheduler.h>
#include <GraphicsImpl.h>
#include <arcana/threading/task_schedulers.h>
#include <memory>
#include <array>
#include <Foundation/Foundation.h>
#include <AVFoundation/AVFoundation.h>

@class CameraTextureDelegate;

#include "NativeCameraImpl.h"
#include <napi/napi.h>

@interface CameraTextureDelegate : NSObject <AVCaptureVideoDataOutputSampleBufferDelegate>{
    std::shared_ptr<Babylon::Plugins::Camera::Impl::ImplData> implData;
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
            for (auto textureRef : texture)
            {
                if (textureRef)
                {
                    CFRelease(textureRef);
                }
            }
        }

        CameraTextureDelegate* cameraTextureDelegate{};
        AVCaptureSession* avCaptureSession{};
        CVMetalTextureCacheRef textureCache{};
        /*
         AVfoundation might update a texture while it's still being processed by the rendering. If it happens, slowdowns or no update at all will occur.
         To fix this issue, a texture array is used with enough slots. 1 texture is used for rendering while another is updated.
         */
        std::array<CVMetalTextureRef, 4> texture{};
        id <MTLTexture> textureBGRA{};
        int textureFrontIndex{};
    };

    Camera::Impl::Impl(Napi::Env env, bool overrideCameraTexture)
        : m_graphicsImpl{GraphicsImpl::GetFromJavaScript(env)}
        , m_implData{std::make_unique<ImplData>()}
        , m_overrideCameraTexture{overrideCameraTexture}
    {
    }

    Camera::Impl::~Impl()
    {
    }

    void Camera::Impl::Open(uint32_t /*width*/, uint32_t /*height*/, bool frontCamera)
    {
        auto metalDevice = (id<MTLDevice>)bgfx::getInternalData()->context;

        dispatch_sync(dispatch_get_main_queue(), ^{
            
            CVMetalTextureCacheCreate(NULL, NULL, metalDevice, NULL, &m_implData->textureCache);
            
            m_implData->cameraTextureDelegate = [[CameraTextureDelegate alloc]init:m_implData];
            
            m_implData->avCaptureSession = [[AVCaptureSession alloc] init];
            
            NSError *error;
#if (TARGET_OS_IPHONE)
            AVCaptureDevicePosition preferredPosition;
            AVCaptureDeviceType preferredDeviceType;
            
            if (frontCamera) {
                preferredPosition = AVCaptureDevicePositionFront;
                preferredDeviceType = AVCaptureDeviceTypeBuiltInTrueDepthCamera;
            } else {
                preferredPosition = AVCaptureDevicePositionBack;
                preferredDeviceType = AVCaptureDeviceTypeBuiltInDualCamera;
            }

            // Set camera capture device to default and the media type to video.
            AVCaptureDevice* captureDevice = [AVCaptureDevice defaultDeviceWithDeviceType:preferredDeviceType mediaType:AVMediaTypeVideo position:preferredPosition];
            if (!captureDevice) {
                // If a rear dual camera is not available, default to the rear wide angle camera.
                captureDevice = [AVCaptureDevice defaultDeviceWithDeviceType:AVCaptureDeviceTypeBuiltInWideAngleCamera mediaType:AVMediaTypeVideo position:AVCaptureDevicePositionBack];
                
                // In the event that the rear wide angle camera isn't available, default to the front wide angle camera.
                if (!captureDevice) {
                    captureDevice = [AVCaptureDevice defaultDeviceWithDeviceType:AVCaptureDeviceTypeBuiltInWideAngleCamera mediaType:AVMediaTypeVideo position:AVCaptureDevicePositionFront];
                }
            }
#else
            UNUSED(frontCamera);
            AVCaptureDevice* captureDevice = [AVCaptureDevice defaultDeviceWithMediaType:AVMediaTypeVideo];
#endif
            // Set video capture input: If there a problem initialising the camera, it will give am error.
            AVCaptureDeviceInput *input = [AVCaptureDeviceInput deviceInputWithDevice:captureDevice error:&error];

            if (!input) {
                NSLog(@"Error Getting Camera Input");
                return;
            }
            // Adding input souce for capture session. i.e., Camera
            [m_implData->avCaptureSession addInput:input];

            dispatch_queue_t sampleBufferQueue = dispatch_queue_create("CameraMulticaster", DISPATCH_QUEUE_SERIAL);

            AVCaptureVideoDataOutput * dataOutput = [[AVCaptureVideoDataOutput alloc] init];
            [dataOutput setAlwaysDiscardsLateVideoFrames:YES];
            [dataOutput setVideoSettings:@{(id)kCVPixelBufferPixelFormatTypeKey: @(kCVPixelFormatType_32BGRA)}];
            [dataOutput setSampleBufferDelegate:m_implData->cameraTextureDelegate queue:sampleBufferQueue];

            [m_implData->avCaptureSession addOutput:dataOutput];
            [m_implData->avCaptureSession commitConfiguration];
            [m_implData->avCaptureSession startRunning];
        });
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
        arcana::make_task(m_graphicsImpl.BeforeRenderScheduler(), arcana::cancellation::none(), [this, textureHandle] {
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

- (id)init:(std::shared_ptr<Babylon::Plugins::Camera::Impl::ImplData>)implData {
    self = [super init];
    self->implData = implData;
    return self;
}

- (void)captureOutput:(AVCaptureOutput *)__unused captureOutput didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer fromConnection:(AVCaptureConnection *)__unused connection {
    
    CVPixelBufferRef pixelBuffer = CMSampleBufferGetImageBuffer(sampleBuffer);

    size_t width = CVPixelBufferGetWidthOfPlane(pixelBuffer, 0);
    size_t height = CVPixelBufferGetHeightOfPlane(pixelBuffer, 0);
    MTLPixelFormat pixelFormat = MTLPixelFormatBGRA8Unorm;

    const auto textureCycleCount{implData->texture.size()};
    int back = (implData->textureFrontIndex+1) % textureCycleCount;
    int front = implData->textureFrontIndex % textureCycleCount;
    CVReturn status = CVMetalTextureCacheCreateTextureFromImage(nullptr, implData->textureCache, pixelBuffer, nullptr, pixelFormat, width, height, 0, &implData->texture[front]);
    if (status == kCVReturnSuccess)
    {
        if (implData->texture[back])
        {
            CFRelease(implData->texture[back]);
            implData->texture[back] = nullptr;
        }
        if (implData->texture[front])
        {
            implData->textureBGRA = CVMetalTextureGetTexture(implData->texture[front]);;
            implData->textureFrontIndex++;
        }
    }
}

@end
