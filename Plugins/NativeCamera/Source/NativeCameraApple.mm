#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>


bgfx::TextureHandle myTexture{bgfx::kInvalidHandle};
size_t width = 0, height = 0;
unsigned char tempBuffer[1920*1080*4];
@interface CameraFeedImpl : NSObject <AVCaptureVideoDataOutputSampleBufferDelegate>{
}
@end

@implementation CameraFeedImpl

- (void)captureOutput:(AVCaptureOutput *)captureOutput didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer fromConnection:(AVCaptureConnection *)connection
{
    CVPixelBufferRef pixelBuffer = CMSampleBufferGetImageBuffer(sampleBuffer);
    width = CVPixelBufferGetWidth(pixelBuffer);
    height = CVPixelBufferGetHeight(pixelBuffer);
    
    auto r = CVPixelBufferGetBytesPerRow(pixelBuffer);
    CVPixelBufferLockBaseAddress(pixelBuffer, kCVPixelBufferLock_ReadOnly);
    
    auto buffer = CVPixelBufferGetBaseAddress(pixelBuffer);
    
    if (buffer) {
        memcpy(tempBuffer, buffer, r * height);
    }
    CVPixelBufferUnlockBaseAddress(pixelBuffer, kCVPixelBufferLock_ReadOnly);
}

@end

namespace CameraFeed
{
AVCaptureSession* captureSession;
CameraFeedImpl* impl;
bool initialized = false;
bgfx::TextureHandle Update(bgfx::TextureHandle textureHandle)
{
    if (!initialized)
    {
        initialized = true;
        
        impl = [[CameraFeedImpl alloc]init];
        
        captureSession = [[AVCaptureSession alloc] init];

        NSError *error;
        // Set camera capture device to default and the media type to video.
        AVCaptureDevice *captureDevice = [AVCaptureDevice defaultDeviceWithMediaType:AVMediaTypeVideo ];
        // Set video capture input: If there a problem initialising the camera, it will give am error.
        AVCaptureDeviceInput *input = [AVCaptureDeviceInput deviceInputWithDevice:captureDevice error:&error];

        if (!input)
        {
            NSLog(@"Error Getting Camera Input");
            return {bgfx::kInvalidHandle};
        }
        // Adding input souce for capture session. i.e., Camera
        [captureSession addInput:input];

        dispatch_queue_t sampleBufferQueue = dispatch_queue_create("CameraMulticaster", DISPATCH_QUEUE_SERIAL);

        AVCaptureVideoDataOutput * dataOutput = [[AVCaptureVideoDataOutput alloc] init];
        [dataOutput setAlwaysDiscardsLateVideoFrames:YES];
        [dataOutput setVideoSettings:@{(id)kCVPixelBufferPixelFormatTypeKey: @(kCVPixelFormatType_32BGRA)}];
        [dataOutput setSampleBufferDelegate:impl queue:sampleBufferQueue];

        [captureSession addOutput:dataOutput];
        [captureSession commitConfiguration];
        [captureSession startRunning];
    }
    
    if (width && height)
    {
        auto mem = bgfx::makeRef(tempBuffer, uint32_t(1920*1080*4));
    
        if (myTexture.idx != bgfx::kInvalidHandle){
            bgfx::destroy(myTexture);
        }
        myTexture = bgfx::createTexture2D(static_cast<uint16_t>(width), static_cast<uint16_t>(height), false, 1, bgfx::TextureFormat::BGRA8, BGFX_TEXTURE_NONE | BGFX_SAMPLER_NONE, mem);
        return myTexture;
    }
    return textureHandle;
}
}
