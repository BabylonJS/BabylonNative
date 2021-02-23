#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>
#import <MetalKit/MetalKit.h>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include "NativeCamera.h"

@class CameraTextureDelegate;

namespace Babylon::Plugins::Internal
{
struct CameraDataApple : public CameraData
{
    CameraTextureDelegate* cameraTextureDelegate;
    AVCaptureSession* avCaptureSession;
    CVMetalTextureCacheRef textureCache;
    id <MTLTexture> textureBGRA;
};
}
@interface CameraTextureDelegate : NSObject <AVCaptureVideoDataOutputSampleBufferDelegate>{
    Babylon::Plugins::Internal::CameraDataApple* cameraDataApple;
}
@end

@implementation CameraTextureDelegate

- (id)init:(Babylon::Plugins::Internal::CameraDataApple*)cameraDataApple {
    self = [super init];
    self->cameraDataApple = cameraDataApple;
    return self;
}

- (void)captureOutput:(AVCaptureOutput *)__unused captureOutput didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer fromConnection:(AVCaptureConnection *)__unused connection
{
    CVPixelBufferRef pixelBuffer = CMSampleBufferGetImageBuffer(sampleBuffer);

    id<MTLTexture> textureBGRA = nil;

    size_t width = CVPixelBufferGetWidthOfPlane(pixelBuffer, 0);
    size_t height = CVPixelBufferGetHeightOfPlane(pixelBuffer, 0);
    MTLPixelFormat pixelFormat = MTLPixelFormatBGRA8Unorm;
    
    CVMetalTextureRef texture = NULL;
    CVReturn status = CVMetalTextureCacheCreateTextureFromImage(NULL, cameraDataApple->textureCache, pixelBuffer, NULL, pixelFormat, width, height, 0, &texture);
    if(status == kCVReturnSuccess)
    {
        textureBGRA = CVMetalTextureGetTexture(texture);
        CFRelease(texture);
    }

    if(textureBGRA != nil)
    {
        dispatch_async(dispatch_get_main_queue(), ^{
            cameraDataApple->textureBGRA = textureBGRA;
        });
    }
}

@end

namespace Babylon::Plugins::Internal
{

CameraData* InitializeCameraTexture()
{
    CameraDataApple* cameraDataApple = new CameraDataApple;
    auto metalDevice = (id<MTLDevice>)bgfx::getInternalData()->context;

    dispatch_sync(dispatch_get_main_queue(), ^{
        
        CVMetalTextureCacheCreate(NULL, NULL, metalDevice, NULL, &cameraDataApple->textureCache);
        
        cameraDataApple->cameraTextureDelegate = [[CameraTextureDelegate alloc]init:cameraDataApple];
        
        cameraDataApple->avCaptureSession = [[AVCaptureSession alloc] init];

        NSError *error;
        // Set camera capture device to default and the media type to video.
        AVCaptureDevice *captureDevice = [AVCaptureDevice defaultDeviceWithMediaType:AVMediaTypeVideo ];
        // Set video capture input: If there a problem initialising the camera, it will give am error.
        AVCaptureDeviceInput *input = [AVCaptureDeviceInput deviceInputWithDevice:captureDevice error:&error];

        if (!input)
        {
            NSLog(@"Error Getting Camera Input");
            return;
        }
        // Adding input souce for capture session. i.e., Camera
        [cameraDataApple->avCaptureSession addInput:input];

        dispatch_queue_t sampleBufferQueue = dispatch_queue_create("CameraMulticaster", DISPATCH_QUEUE_SERIAL);

        AVCaptureVideoDataOutput * dataOutput = [[AVCaptureVideoDataOutput alloc] init];
        [dataOutput setAlwaysDiscardsLateVideoFrames:YES];
        [dataOutput setVideoSettings:@{(id)kCVPixelBufferPixelFormatTypeKey: @(kCVPixelFormatType_32BGRA)}];
        [dataOutput setSampleBufferDelegate:cameraDataApple->cameraTextureDelegate queue:sampleBufferQueue];

        [cameraDataApple->avCaptureSession addOutput:dataOutput];
        [cameraDataApple->avCaptureSession commitConfiguration];
        [cameraDataApple->avCaptureSession startRunning];
    });
    return cameraDataApple;
}

void UpdateCameraTexture(bgfx::TextureHandle textureHandle, const CameraData* cameraData)
{
    auto cameraDataApple = static_cast<const CameraDataApple*>(cameraData);
    if (cameraDataApple->textureBGRA)
    {
        bgfx::overrideInternal(textureHandle, reinterpret_cast<uintptr_t>(cameraDataApple->textureBGRA));
    }
}

void DisposeCameraTexture(const CameraData* cameraData)
{
    auto cameraDataApple = static_cast<const CameraDataApple*>(cameraData);
    [cameraDataApple->avCaptureSession stopRunning];
    [cameraDataApple->avCaptureSession release];
    [cameraDataApple->cameraTextureDelegate release];
    CVMetalTextureCacheFlush(cameraDataApple->textureCache, 0);
    CFRelease(cameraDataApple->textureCache);
    delete cameraDataApple;
}

}
