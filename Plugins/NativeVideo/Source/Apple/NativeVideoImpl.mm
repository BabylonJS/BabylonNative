#include "NativeVideoImpl.h"
#include <napi/napi.h>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#import "AVFoundation/AVFoundation.h"
#include <string>

namespace Babylon::Plugins
{
    struct Video::Impl::ReaderProcessor
    {
        ReaderProcessor(const std::string& contentURL);
        ~ReaderProcessor()
        {
            if (m_textureCache)
            {
                CVMetalTextureCacheFlush(m_textureCache, 0);
                CFRelease(m_textureCache);
            }
        }

        void UpdateTexture(CMSampleBufferRef sampleBuffer);

        AVAssetReader* m_reader;
        CVMetalTextureCacheRef m_textureCache{};
        id <MTLTexture> m_textureBGRA{};
    };

    struct Video::Impl::WriterProcessor
    {
        WriterProcessor(const std::string& contentURL, size_t width, size_t height);
        ~WriterProcessor();

        void PushFrame(uint8_t* frame);
        void StopOutput();
        CVPixelBufferRef NewPixelBufferFromCGImage(CGImageRef image);
        size_t m_outputWidth;
        size_t m_outputHeight;
        AVAssetWriter* m_videoWriter;
        AVAssetWriterInput* m_writerInput;
        CMTime m_lastSampleTime;
    };

    CVPixelBufferRef Video::Impl::WriterProcessor::NewPixelBufferFromCGImage(CGImageRef image)
    {
        NSDictionary *options = [NSDictionary dictionaryWithObjectsAndKeys:
            [NSNumber numberWithBool:YES], kCVPixelBufferCGImageCompatibilityKey,
            [NSNumber numberWithBool:YES], kCVPixelBufferCGBitmapContextCompatibilityKey,
            nil];
        CVPixelBufferRef pxbuffer = NULL;
        CVPixelBufferCreate(kCFAllocatorDefault, m_outputWidth, m_outputHeight, kCVPixelFormatType_32ARGB, (CFDictionaryRef) options, &pxbuffer);

        CVPixelBufferLockBaseAddress(pxbuffer, 0);
        void *pxdata = CVPixelBufferGetBaseAddress(pxbuffer);

        CGColorSpaceRef rgbColorSpace = CGColorSpaceCreateDeviceRGB();
        CGContextRef context = CGBitmapContextCreate(pxdata, m_outputWidth, m_outputHeight, 8, 4 * m_outputWidth, rgbColorSpace,kCGImageAlphaNoneSkipFirst);
        CGContextDrawImage(context, CGRectMake(0, 0, CGImageGetWidth(image), CGImageGetHeight(image)), image);
        CGColorSpaceRelease(rgbColorSpace);
        CGContextRelease(context);

        CVPixelBufferUnlockBaseAddress(pxbuffer, 0);

        return pxbuffer;
    }

    //std::string contentURL = "file:///Users/cedricguillemet/output.mp4";
    Video::Impl::WriterProcessor::WriterProcessor(const std::string& contentURL, size_t width, size_t height)
    {
        m_outputWidth = width;
        m_outputHeight = height;

        NSString* urlString = [[NSString stringWithUTF8String:contentURL.data()] stringByAddingPercentEncodingWithAllowedCharacters:NSCharacterSet.URLQueryAllowedCharacterSet];
        NSURL* url{[NSURL URLWithString:urlString]};

        NSError *error = nil;
        m_videoWriter = [[AVAssetWriter alloc] initWithURL:url fileType:AVFileTypeMPEG4 error:&error];

        NSDictionary *videoSettings = [NSDictionary dictionaryWithObjectsAndKeys:
                                       AVVideoCodecTypeH264, AVVideoCodecKey,
                                       [NSNumber numberWithLongLong:width], AVVideoWidthKey,
                                       [NSNumber numberWithLongLong:height], AVVideoHeightKey,
                                       nil];
        m_writerInput = [[AVAssetWriterInput assetWriterInputWithMediaType:AVMediaTypeVideo outputSettings:videoSettings] retain]; //retain should be removed if ARC

        [m_videoWriter canAddInput:m_writerInput];
        [m_videoWriter addInput:m_writerInput];

        // start writing
        [m_videoWriter startWriting];
        [m_videoWriter startSessionAtSourceTime:kCMTimeZero];
        
        m_lastSampleTime = kCMTimeZero;
    }

    void Video::Impl::WriterProcessor::PushFrame(uint8_t* frame)
    {
        UInt8* pixelData = new UInt8[m_outputWidth * m_outputHeight * 3];
        UInt8* pixelDataBase =pixelData;
        for (size_t index = 0;index<m_outputWidth * m_outputHeight * 4; index += 4)
        {
            *pixelDataBase++ = frame[index];
            *pixelDataBase++ = frame[index + 1];
            *pixelDataBase++ = frame[index + 2];
        }

        CGColorSpaceRef colorspace = CGColorSpaceCreateDeviceRGB();

        CFDataRef rgbData = CFDataCreate(NULL, pixelData, m_outputWidth * m_outputHeight * 3);

        CGDataProviderRef provider = CGDataProviderCreateWithCFData(rgbData);

        CGImageRef rgbImageRef = CGImageCreate(m_outputWidth, m_outputHeight, 8, 24, m_outputWidth * 3, colorspace, kCGBitmapByteOrderDefault, provider, NULL, true, kCGRenderingIntentDefault);

        CVPixelBufferRef pixelBuffer = NewPixelBufferFromCGImage(rgbImageRef);
        
        // Sample timing info.
        CMTime frameTime = CMTimeMake(1, 30);
        CMTime currentTime = CMTimeAdd(m_lastSampleTime, frameTime);
        CMSampleTimingInfo timing = {frameTime, currentTime, kCMTimeInvalid};

        OSStatus result = 0;

        // Sample format.
        CMVideoFormatDescriptionRef videoInfo = NULL;
        result = CMVideoFormatDescriptionCreateForImageBuffer(NULL, pixelBuffer, &videoInfo);

        // Create sample buffer.
        CMSampleBufferRef sampleBuffer = NULL;
        result = CMSampleBufferCreateForImageBuffer(kCFAllocatorDefault, pixelBuffer, true, NULL, NULL, videoInfo, &timing, &sampleBuffer);

        [m_writerInput appendSampleBuffer:sampleBuffer];
        
        m_lastSampleTime = currentTime;
        CFRelease(rgbData);

        CGDataProviderRelease(provider);

        CGColorSpaceRelease(colorspace);

        CGImageRelease(rgbImageRef);
        delete [] pixelData;
    }

    Video::Impl::WriterProcessor::~WriterProcessor()
    {
        [m_writerInput markAsFinished];
        [m_videoWriter finishWritingWithCompletionHandler:^{
            NSLog(@"Finished writing");
        }];
    }

    void Video::Impl::ReaderProcessor::UpdateTexture(CMSampleBufferRef sampleBuffer)
    {
        CVPixelBufferRef pixelBuffer = CMSampleBufferGetImageBuffer(sampleBuffer);

        id<MTLTexture> textureBGRA = nil;

        size_t width = CVPixelBufferGetWidthOfPlane(pixelBuffer, 0);
        size_t height = CVPixelBufferGetHeightOfPlane(pixelBuffer, 0);
        MTLPixelFormat pixelFormat = MTLPixelFormatBGRA8Unorm;
        
        CVMetalTextureRef texture = NULL;
        CVReturn status = CVMetalTextureCacheCreateTextureFromImage(NULL, this->m_textureCache, pixelBuffer, NULL, pixelFormat, width, height, 0, &texture);
        if(status == kCVReturnSuccess)
        {
            textureBGRA = CVMetalTextureGetTexture(texture);
            CFRelease(texture);
        }

        this->m_textureBGRA = textureBGRA;
        if(textureBGRA != nil)
        {
            dispatch_async(dispatch_get_main_queue(), ^{
                //implData->textureBGRA = textureBGRA;
            });
        }
    }

    Video::Impl::ReaderProcessor::ReaderProcessor(const std::string& contentURL)
    {
        auto metalDevice = (id<MTLDevice>)bgfx::getInternalData()->context;
        dispatch_sync(dispatch_get_main_queue(), ^{
            
        CVMetalTextureCacheCreate(NULL, NULL, metalDevice, NULL, &this->m_textureCache);

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
            if (sampleBufferRef)
            {
                NSLog(@"Has buffer");
                UpdateTexture(sampleBufferRef);
            }
        }
        });// sync
    }

    Video::Impl::Impl(Napi::Env /*env*/)
    {
    }

    Video::Impl::~Impl()
    {
    }

    void Video::Impl::Open(const std::string& contentURL, size_t width, size_t height)
    {
        m_writer = std::make_unique<WriterProcessor>(contentURL, width, height);
    }

    void Video::Impl::Open(const std::string& contentURL)
    {
        m_reader = std::make_unique<ReaderProcessor>(contentURL);
    }

    void Video::Impl::UpdateTexture(bgfx::TextureHandle textureHandle)
    {
        //throw std::runtime_error{ "Not implemented for this platform." };
        if (m_reader)
        {
            bgfx::overrideInternal(textureHandle, reinterpret_cast<uintptr_t>(m_reader->m_textureBGRA));
        }
    }

    void Video::Impl::AddFrame(uint8_t* data, size_t /*dataLength*/)
    {
        if (m_writer)
        {
            m_writer->PushFrame(data);
        }
    }

    void Video::Impl::Close()
    {
        m_writer.reset();
        m_reader.reset();
    }
}
 
