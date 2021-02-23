#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>
#import <MetalKit/MetalKit.h>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include "NativeCamera.h"

@class CameraTextureDelegate;

namespace Babylon::Plugins::Internal
{
struct CameraInterfaceApple : public CameraInterface
{
    virtual ~CameraInterfaceApple();
    void UpdateCameraTexture(bgfx::TextureHandle textureHandle) override;

    CameraTextureDelegate* cameraTextureDelegate;
    AVCaptureSession* avCaptureSession;
    CVMetalTextureCacheRef textureCache;
    id <MTLTexture> textureBGRA;
};
}
@interface CameraTextureDelegate : NSObject <AVCaptureVideoDataOutputSampleBufferDelegate>{
    Babylon::Plugins::Internal::CameraInterfaceApple* cameraInterfaceApple;
}
@end

@implementation CameraTextureDelegate

- (id)init:(Babylon::Plugins::Internal::CameraInterfaceApple*)cameraInterfaceApple {
    self = [super init];
    self->cameraInterfaceApple = cameraInterfaceApple;
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
    CVReturn status = CVMetalTextureCacheCreateTextureFromImage(NULL, cameraInterfaceApple->textureCache, pixelBuffer, NULL, pixelFormat, width, height, 0, &texture);
    if(status == kCVReturnSuccess)
    {
        textureBGRA = CVMetalTextureGetTexture(texture);
        CFRelease(texture);
    }

    if(textureBGRA != nil)
    {
        dispatch_async(dispatch_get_main_queue(), ^{
            cameraInterfaceApple->textureBGRA = textureBGRA;
        });
    }
}

@end

namespace Babylon::Plugins::Internal
{

CameraInterface* CameraInterface::CreateInterface()
{
    CameraInterfaceApple* cameraInterfaceApple = new CameraInterfaceApple;
    auto metalDevice = (id<MTLDevice>)bgfx::getInternalData()->context;

    dispatch_sync(dispatch_get_main_queue(), ^{
        
        CVMetalTextureCacheCreate(NULL, NULL, metalDevice, NULL, &cameraInterfaceApple->textureCache);
        
        cameraInterfaceApple->cameraTextureDelegate = [[CameraTextureDelegate alloc]init:cameraInterfaceApple];
        
        cameraInterfaceApple->avCaptureSession = [[AVCaptureSession alloc] init];

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
        [cameraInterfaceApple->avCaptureSession addInput:input];

        dispatch_queue_t sampleBufferQueue = dispatch_queue_create("CameraMulticaster", DISPATCH_QUEUE_SERIAL);

        AVCaptureVideoDataOutput * dataOutput = [[AVCaptureVideoDataOutput alloc] init];
        [dataOutput setAlwaysDiscardsLateVideoFrames:YES];
        [dataOutput setVideoSettings:@{(id)kCVPixelBufferPixelFormatTypeKey: @(kCVPixelFormatType_32BGRA)}];
        [dataOutput setSampleBufferDelegate:cameraInterfaceApple->cameraTextureDelegate queue:sampleBufferQueue];

        [cameraInterfaceApple->avCaptureSession addOutput:dataOutput];
        [cameraInterfaceApple->avCaptureSession commitConfiguration];
        [cameraInterfaceApple->avCaptureSession startRunning];
    });
    return cameraInterfaceApple;
}

void CameraInterfaceApple::UpdateCameraTexture(bgfx::TextureHandle textureHandle)
{
    if (textureBGRA)
    {
        bgfx::overrideInternal(textureHandle, reinterpret_cast<uintptr_t>(textureBGRA));
    }
}

CameraInterfaceApple::~CameraInterfaceApple()
{
    [avCaptureSession stopRunning];
    [avCaptureSession release];
    [cameraTextureDelegate release];
    CVMetalTextureCacheFlush(textureCache, 0);
    CFRelease(textureCache);
}

}
