#include "NativeVideoImpl.h"
#include <napi/napi.h>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#import "AVFoundation/AVFoundation.h"
#include <string>

//NSURL* url;
AVAssetReader* reader;
struct NativeVideoProcessing
{
    ~NativeVideoProcessing()
    {
        if (textureCache)
        {
            CVMetalTextureCacheFlush(textureCache, 0);
            CFRelease(textureCache);
        }
    }
    void startProcessing(std::string contentURL);
    void updateTexture(CMSampleBufferRef sampleBuffer);
    
    CVMetalTextureCacheRef textureCache{};
    id <MTLTexture> textureBGRA{};

    // output
    CVPixelBufferRef newPixelBufferFromCGImage(CGImageRef image);
    void OutputTest();
};

NativeVideoProcessing* processing;

CVPixelBufferRef NativeVideoProcessing::newPixelBufferFromCGImage(CGImageRef image)
{
    NSDictionary *options = [NSDictionary dictionaryWithObjectsAndKeys:
        [NSNumber numberWithBool:YES], kCVPixelBufferCGImageCompatibilityKey,
        [NSNumber numberWithBool:YES], kCVPixelBufferCGBitmapContextCompatibilityKey,
        nil];
    CVPixelBufferRef pxbuffer = NULL;
    /*CVReturn status = */CVPixelBufferCreate(kCFAllocatorDefault, 64/*frameSize.width*/,
        64/*frameSize.height*/, kCVPixelFormatType_32ARGB, (CFDictionaryRef) options,
        &pxbuffer);
    //NSParameterAssert(status == kCVReturnSuccess && pxbuffer != NULL);

    CVPixelBufferLockBaseAddress(pxbuffer, 0);
    void *pxdata = CVPixelBufferGetBaseAddress(pxbuffer);
    //NSParameterAssert(pxdata != NULL);

    CGColorSpaceRef rgbColorSpace = CGColorSpaceCreateDeviceRGB();
    CGContextRef context = CGBitmapContextCreate(pxdata, 64/*frameSize.width*/,
        64/*frameSize.height*/, 8, 4*64/*frameSize.width*/, rgbColorSpace,
        kCGImageAlphaNoneSkipFirst);
    //NSParameterAssert(context);
    //CGContextConcatCTM(context, frameTransform);
    CGContextDrawImage(context, CGRectMake(0, 0, CGImageGetWidth(image),
        CGImageGetHeight(image)), image);
    CGColorSpaceRelease(rgbColorSpace);
    CGContextRelease(context);

    CVPixelBufferUnlockBaseAddress(pxbuffer, 0);

    return pxbuffer;
}

void NativeVideoProcessing::OutputTest()
{
    std::string contentURL = "file:///Users/cedricguillemet/output.mp4";
    NSString* urlString = [[NSString stringWithUTF8String:contentURL.data()] stringByAddingPercentEncodingWithAllowedCharacters:NSCharacterSet.URLQueryAllowedCharacterSet];
                NSURL* url{[NSURL URLWithString:urlString]};
    
    NSError *error = nil;
    AVAssetWriter *videoWriter = [[AVAssetWriter alloc] initWithURL:url fileType:AVFileTypeMPEG4 error:&error];

    NSDictionary *videoSettings = [NSDictionary dictionaryWithObjectsAndKeys:
                                   AVVideoCodecTypeH264, AVVideoCodecKey,
        [NSNumber numberWithInt:64], AVVideoWidthKey,
        [NSNumber numberWithInt:64], AVVideoHeightKey,
        nil];
    AVAssetWriterInput* writerInput = [[AVAssetWriterInput
        assetWriterInputWithMediaType:AVMediaTypeVideo
        outputSettings:videoSettings] retain]; //retain should be removed if ARC

    [videoWriter canAddInput:writerInput];
    [videoWriter addInput:writerInput];
    
    
    // start writing
    [videoWriter startWriting];
    [videoWriter startSessionAtSourceTime:kCMTimeZero];
    
    // push frames
    
    CMTime lastSampleTime = kCMTimeZero;
    for (int i = 0;i<2;i++)
    {
        
        //const int width = 64;//CGImageGetWidth(frame);
        //const int height = 64;//CGImageGetHeight(frame);

            // Create a dummy pixel buffer to try the encoding
            // on something simple.
            //CVPixelBufferRef pixelBuffer = NULL;
            /*CVReturn status = */ //CVPixelBufferCreate(kCFAllocatorDefault, width, height,
                //kCVPixelFormatType_32BGRA, NULL, &pixelBuffer);
            //NSParameterAssert(status == kCVReturnSuccess && pixelBuffer != NULL);

        UInt8 pixelData[64 * 64 * 3];

        // fill the raw pixel buffer with arbitrary gray color for test

        for(size_t ui = 0; ui < 64 * 64 * 3; ui++)
          pixelData[ui] = 210;

        CGColorSpaceRef colorspace = CGColorSpaceCreateDeviceRGB();

        CFDataRef rgbData = CFDataCreate(NULL, pixelData, 64 * 64 * 3);

        CGDataProviderRef provider = CGDataProviderCreateWithCFData(rgbData);

        CGImageRef rgbImageRef = CGImageCreate(64, 64, 8, 24, 64 * 3, colorspace, kCGBitmapByteOrderDefault, provider, NULL, true, kCGRenderingIntentDefault);

        
        
        
        
        
        
        
        CVPixelBufferRef pixelBuffer = newPixelBufferFromCGImage(rgbImageRef);
        
            // Sample timing info.
            CMTime frameTime = CMTimeMake(1, 30);
            CMTime currentTime = CMTimeAdd(lastSampleTime, frameTime);
            CMSampleTimingInfo timing = {frameTime, currentTime, kCMTimeInvalid};

            OSStatus result = 0;

            // Sample format.
            CMVideoFormatDescriptionRef videoInfo = NULL;
            result = CMVideoFormatDescriptionCreateForImageBuffer(NULL,
                 pixelBuffer, &videoInfo);
            //NSParameterAssert(result == 0 && videoInfo != NULL);

            // Create sample buffer.
            CMSampleBufferRef sampleBuffer = NULL;
            result = CMSampleBufferCreateForImageBuffer(kCFAllocatorDefault,
                pixelBuffer, true, NULL, NULL, videoInfo, &timing, &sampleBuffer);
            //NSParameterAssert(result == 0 && sampleBuffer != NULL);

        
        //CVPixelBufferRef sampleBuffer = newPixelBufferFromCGImage(image);
        [writerInput appendSampleBuffer:sampleBuffer];
        
        lastSampleTime = frameTime;
        
        
        
        
        
        
        CFRelease(rgbData);

        CGDataProviderRelease(provider);

        CGColorSpaceRelease(colorspace);

        // use the created CGImage

        CGImageRelease(rgbImageRef);
    }
    
    
    // finish everything
    [writerInput markAsFinished];
    [videoWriter finishWritingWithCompletionHandler:^{
        NSLog(@"Finished writing");
    }];
}


void NativeVideoProcessing::updateTexture(CMSampleBufferRef sampleBuffer)
{
    CVPixelBufferRef pixelBuffer = CMSampleBufferGetImageBuffer(sampleBuffer);

    id<MTLTexture> textureBGRA = nil;

    size_t width = CVPixelBufferGetWidthOfPlane(pixelBuffer, 0);
    size_t height = CVPixelBufferGetHeightOfPlane(pixelBuffer, 0);
    MTLPixelFormat pixelFormat = MTLPixelFormatBGRA8Unorm;
    
    CVMetalTextureRef texture = NULL;
    CVReturn status = CVMetalTextureCacheCreateTextureFromImage(NULL, this->textureCache, pixelBuffer, NULL, pixelFormat, width, height, 0, &texture);
    if(status == kCVReturnSuccess)
    {
        textureBGRA = CVMetalTextureGetTexture(texture);
        CFRelease(texture);
    }

    this->textureBGRA = textureBGRA;
    if(textureBGRA != nil)
    {
        dispatch_async(dispatch_get_main_queue(), ^{
            //implData->textureBGRA = textureBGRA;
        });
    }
}
void NativeVideoProcessing::startProcessing(std::string contentURL)
{
    auto metalDevice = (id<MTLDevice>)bgfx::getInternalData()->context;
    dispatch_sync(dispatch_get_main_queue(), ^{
        
        CVMetalTextureCacheCreate(NULL, NULL, metalDevice, NULL, &this->textureCache);

        NSString* urlString = [[NSString stringWithUTF8String:contentURL.data()] stringByAddingPercentEncodingWithAllowedCharacters:NSCharacterSet.URLQueryAllowedCharacterSet];
                    NSURL* url{[NSURL URLWithString:urlString]};
        
        
        NSDictionary *inputOptions = [NSDictionary dictionaryWithObject:[NSNumber numberWithBool:YES] forKey:AVURLAssetPreferPreciseDurationAndTimingKey];
            AVURLAsset *inputAsset = [[AVURLAsset alloc] initWithURL:url options:inputOptions];
            
            //GPUImageMovie __block *blockSelf = self;
        NSError *error = nil;
        auto reader = [[AVAssetReader alloc] initWithAsset:inputAsset error:&error];
            NSArray* tracks = [inputAsset tracksWithMediaType:AVMediaTypeVideo];
        if ([tracks count] == 0)
            {
                NSLog(@"No track");
            }
        AVAssetTrack *videoTrack = [tracks objectAtIndex:0];
        
        NSMutableDictionary *outputSettings = [NSMutableDictionary dictionary];
        [outputSettings setObject:@(kCVPixelFormatType_32BGRA) forKey:(id)kCVPixelBufferPixelFormatTypeKey];
        
        auto output = [[AVAssetReaderTrackOutput alloc] initWithTrack:videoTrack outputSettings:outputSettings];
        [reader addOutput:output];
        
        if ([reader startReading])
        {
        
            CMSampleBufferRef sampleBufferRef = [output copyNextSampleBuffer];
            if (sampleBufferRef) {
                NSLog(@"Has buffer");
                updateTexture(sampleBufferRef);
            }
        }
    }
                  );// sync
}

namespace Babylon::Plugins
{
    Video::Impl::Impl(Napi::Env /*env*/)
    {
    }

    Video::Impl::~Impl()
    {
    }

    void Video::Impl::Open(const std::string& source)
    {
        //[[[NativeVideoProcessing alloc]init] startProcessing:source];
        processing = new NativeVideoProcessing;
        processing->startProcessing(source);
        //throw std::runtime_error{ "Not implemented for this platform." };
        
        
        processing->OutputTest();
    }
    
    void Video::Impl::UpdateTexture(bgfx::TextureHandle textureHandle)
    {
        //throw std::runtime_error{ "Not implemented for this platform." };
        if (processing->textureBGRA)
        {
            bgfx::overrideInternal(textureHandle, reinterpret_cast<uintptr_t>(processing->textureBGRA));
        }
    }

    void Video::Impl::Close()
    {
        throw std::runtime_error{ "Not implemented for this platform." };
    }
}
 
