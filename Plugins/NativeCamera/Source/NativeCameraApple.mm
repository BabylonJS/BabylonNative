#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>
#import <MetalKit/MetalKit.h>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include "NativeCamera.h"

@interface CameraFeedImpl : NSObject <AVCaptureVideoDataOutputSampleBufferDelegate>{
}
@end

@implementation CameraFeedImpl

CVMetalTextureCacheRef _textureCache;
id <MTLTexture> _textureBGRA;

- (void)captureOutput:(AVCaptureOutput *)__unused captureOutput didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer fromConnection:(AVCaptureConnection *)__unused connection
{
    CVPixelBufferRef pixelBuffer = CMSampleBufferGetImageBuffer(sampleBuffer);

    id<MTLTexture> textureBGRA = nil;

    size_t width = CVPixelBufferGetWidthOfPlane(pixelBuffer, 0);
    size_t height = CVPixelBufferGetHeightOfPlane(pixelBuffer, 0);
    MTLPixelFormat pixelFormat = MTLPixelFormatBGRA8Unorm;//MTLPixelFormatR8Unorm;
    
    CVMetalTextureRef texture = NULL;
    CVReturn status = CVMetalTextureCacheCreateTextureFromImage(NULL, _textureCache, pixelBuffer, NULL, pixelFormat, width, height, 0, &texture);
    if(status == kCVReturnSuccess)
    {
        textureBGRA = CVMetalTextureGetTexture(texture);
        CFRelease(texture);
    }

    if(textureBGRA != nil)
    {
        dispatch_async(dispatch_get_main_queue(), ^{
            _textureBGRA = textureBGRA;
        });
    }
}

@end

namespace Babylon::Plugins::Internal
{
AVCaptureSession* captureSession;
CameraFeedImpl* impl;

bool initialized = false;


CameraData* InitializeCameraTexture()
{
    return nullptr;
}

void UpdateCameraTexture(bgfx::TextureHandle textureHandle)
{
    if (!initialized)
    {
        initialized = true;
        auto metalDevice = (id<MTLDevice>)bgfx::getInternalData()->context;
        
        dispatch_sync(dispatch_get_main_queue(), ^{
            
            CVMetalTextureCacheCreate(NULL, NULL, metalDevice, NULL, &_textureCache);
            
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
                return;
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
        });
    }
    
    if (_textureBGRA)
    {
        bgfx::overrideInternal(textureHandle, reinterpret_cast<uintptr_t>(_textureBGRA));
    }
}

void DisposeCameraTexture(const CameraData* /*cameraData*/)
{
}

}
