#if ! __has_feature(objc_arc)
#error "ARC is off"
#endif

// Apple implementation of VideoPlayer (see ../VideoPlayer.h) backed by AVPlayer +
// AVPlayerItemVideoOutput. Decoded frames arrive as BGRA pixel buffers (no YUV pass
// needed) and are blitted into a persistent MTLTexture, which is wired to Babylon's
// InternalTexture with the same bgfx::overrideInternal retry pattern used by
// CameraDevice::UpdateCameraTexture.

#import <AVFoundation/AVFoundation.h>
#import <Metal/Metal.h>
#import <CoreVideo/CoreVideo.h>
#import <QuartzCore/QuartzCore.h>

#include <napi/napi.h>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <arcana/threading/task.h>
#include <Babylon/Graphics/DeviceContext.h>

#include "../VideoPlayer.h"

#include <atomic>
#include <memory>

@class VideoPlayerItemObserver;

namespace Babylon::Plugins
{
    namespace
    {
        NSURL* ResolveUrl(const std::string& url)
        {
            NSString* str = [NSString stringWithUTF8String:url.c_str()];
            if ([str hasPrefix:@"app://"])
            {
                // Match the app:/// scheme used by ScriptLoader: resolve against the bundle.
                NSURL* parsed = [NSURL URLWithString:str];
                NSString* relativePath = parsed.path; // e.g. "/Scripts/video.mp4"
                NSString* fullPath = [[[NSBundle mainBundle] resourcePath] stringByAppendingString:relativePath];
                return [NSURL fileURLWithPath:fullPath];
            }
            if ([str hasPrefix:@"/"])
            {
                return [NSURL fileURLWithPath:str];
            }
            return [NSURL URLWithString:str];
        }
    }

    class VideoPlayerApple final : public VideoPlayer
    {
    public:
        // Shared with render-thread tasks and ObjC observers so in-flight work stays
        // valid after the VideoPlayer is destroyed.
        struct State
        {
            Graphics::DeviceContext* deviceContext{};
            EventCallbackT eventCallback{};

            AVPlayer* player{};
            AVPlayerItem* item{};
            AVPlayerItemVideoOutput* videoOutput{};
            VideoPlayerItemObserver* observer{};
            id endObserverToken{};

            // Metal resources (initialized lazily on the render thread).
            id<MTLDevice> metalDevice{};
            id<MTLCommandQueue> commandQueue{};
            CVMetalTextureCacheRef textureCache{};
            id<MTLTexture> textureRGBA{};

            bgfx::TextureHandle bgfxHandle{bgfx::kInvalidHandle};
            bool refreshBgfxHandle{true};

            std::atomic<bool> ready{false};
            std::atomic<bool> loop{false};
            std::atomic<bool> invalid{false};
            std::atomic<uint32_t> width{0};
            std::atomic<uint32_t> height{0};

            ~State()
            {
                if (textureCache)
                {
                    CVMetalTextureCacheFlush(textureCache, 0);
                    CFRelease(textureCache);
                }
            }
        };

        VideoPlayerApple(Graphics::DeviceContext& deviceContext, const std::string& url, EventCallbackT eventCallback);
        ~VideoPlayerApple() override;

        void Play() override
        {
            [m_state->player play];
        }

        void Pause() override
        {
            [m_state->player pause];
        }

        void SetLoop(bool loop) override
        {
            m_state->loop.store(loop);
        }

        void SetMuted(bool muted) override
        {
            m_state->player.muted = muted;
        }

        void SetVolume(double volume) override
        {
            m_state->player.volume = static_cast<float>(volume);
        }

        void Seek(double timeInSeconds) override
        {
            auto state = m_state;
            [m_state->player seekToTime:CMTimeMakeWithSeconds(timeInSeconds, 600)
                        toleranceBefore:kCMTimeZero
                         toleranceAfter:kCMTimeZero
                      completionHandler:^(BOOL) {
                          if (!state->invalid.load() && state->eventCallback)
                          {
                              state->eventCallback(Event::Seeked);
                          }
                      }];
        }

        bool IsReady() const override
        {
            return m_state->ready.load();
        }

        double GetDuration() const override
        {
            if (!m_state->ready.load())
            {
                return 0;
            }
            const CMTime duration = m_state->item.duration;
            return CMTIME_IS_NUMERIC(duration) ? CMTimeGetSeconds(duration) : 0;
        }

        double GetCurrentTime() const override
        {
            const CMTime time = m_state->player.currentTime;
            return CMTIME_IS_NUMERIC(time) ? CMTimeGetSeconds(time) : 0;
        }

        uint32_t GetWidth() const override
        {
            return m_state->width.load();
        }

        uint32_t GetHeight() const override
        {
            return m_state->height.load();
        }

        void UpdateTexture(bgfx::TextureHandle textureHandle) override;

    private:
        std::shared_ptr<State> m_state{};
    };
}

// Observes AVPlayerItem.status so we can report readiness (metadata/dimensions).
@interface VideoPlayerItemObserver : NSObject
- (id)initWithState:(std::shared_ptr<Babylon::Plugins::VideoPlayerApple::State>)state;
- (void)stopObserving;
@end

@implementation VideoPlayerItemObserver
{
    std::shared_ptr<Babylon::Plugins::VideoPlayerApple::State> _state;
    BOOL _observing;
}

- (id)initWithState:(std::shared_ptr<Babylon::Plugins::VideoPlayerApple::State>)state
{
    if (self = [super init])
    {
        _state = std::move(state);
        [_state->item addObserver:self forKeyPath:@"status" options:NSKeyValueObservingOptionNew context:nil];
        _observing = YES;
    }
    return self;
}

- (void)stopObserving
{
    if (_observing)
    {
        [_state->item removeObserver:self forKeyPath:@"status"];
        _observing = NO;
    }
}

- (void)observeValueForKeyPath:(NSString*)keyPath ofObject:(id)__unused object change:(NSDictionary*)__unused change context:(void*)__unused context
{
    if ([keyPath isEqualToString:@"status"] && _state->item.status == AVPlayerItemStatusReadyToPlay && !_state->ready.load())
    {
        const CGSize size = _state->item.presentationSize;
        _state->width.store(static_cast<uint32_t>(size.width));
        _state->height.store(static_cast<uint32_t>(size.height));
        _state->ready.store(true);
        if (!_state->invalid.load() && _state->eventCallback)
        {
            _state->eventCallback(Babylon::Plugins::VideoPlayer::Event::MetadataLoaded);
        }
    }
}
@end

namespace Babylon::Plugins
{
    std::unique_ptr<VideoPlayer> VideoPlayer::Create(Graphics::DeviceContext& deviceContext, const std::string& url, EventCallbackT eventCallback)
    {
        return std::make_unique<VideoPlayerApple>(deviceContext, url, std::move(eventCallback));
    }

    VideoPlayerApple::VideoPlayerApple(Graphics::DeviceContext& deviceContext, const std::string& url, EventCallbackT eventCallback)
        : m_state{std::make_shared<State>()}
    {
        m_state->deviceContext = &deviceContext;
        m_state->eventCallback = std::move(eventCallback);

        NSURL* nsUrl = ResolveUrl(url);
        m_state->item = [AVPlayerItem playerItemWithURL:nsUrl];
        m_state->player = [AVPlayer playerWithPlayerItem:m_state->item];
        m_state->player.actionAtItemEnd = AVPlayerActionAtItemEndNone;

        // Decoded frames as BGRA so they can be blitted straight into the sampled texture.
        NSDictionary* outputAttributes = @{
            (id)kCVPixelBufferPixelFormatTypeKey: @(kCVPixelFormatType_32BGRA),
            (id)kCVPixelBufferMetalCompatibilityKey: @YES,
        };
        m_state->videoOutput = [[AVPlayerItemVideoOutput alloc] initWithPixelBufferAttributes:outputAttributes];
        [m_state->item addOutput:m_state->videoOutput];

        m_state->observer = [[VideoPlayerItemObserver alloc] initWithState:m_state];

        // End-of-playback: loop by seeking back to zero, otherwise report Ended.
        auto state = m_state;
        m_state->endObserverToken = [[NSNotificationCenter defaultCenter] addObserverForName:AVPlayerItemDidPlayToEndTimeNotification
                                                                                      object:m_state->item
                                                                                       queue:nil
                                                                                  usingBlock:^(NSNotification*) {
            if (state->invalid.load())
            {
                return;
            }
            if (state->loop.load())
            {
                [state->player seekToTime:kCMTimeZero toleranceBefore:kCMTimeZero toleranceAfter:kCMTimeZero completionHandler:^(BOOL) {}];
            }
            else
            {
                [state->player pause];
                if (state->eventCallback)
                {
                    state->eventCallback(Event::Ended);
                }
            }
        }];
    }

    VideoPlayerApple::~VideoPlayerApple()
    {
        m_state->invalid.store(true);
        [m_state->observer stopObserving];
        if (m_state->endObserverToken)
        {
            [[NSNotificationCenter defaultCenter] removeObserver:m_state->endObserverToken];
            m_state->endObserverToken = nil;
        }
        [m_state->player pause];
        // In-flight render-thread tasks hold the shared State and finish safely.
    }

    void VideoPlayerApple::UpdateTexture(bgfx::TextureHandle textureHandle)
    {
        if (!m_state->ready.load())
        {
            return;
        }

        // Hook into AfterRender (like CameraDevice::UpdateCameraTexture) so the bgfx
        // handle has been initialized by a frame before we override it, and so Metal
        // work doesn't race the renderer.
        arcana::make_task(m_state->deviceContext->AfterRenderScheduler(), arcana::cancellation::none(), [state = m_state, textureHandle] {
            if (state->invalid.load())
            {
                return;
            }

            // Lazily bind to bgfx's Metal device/queue (render thread only).
            if (state->metalDevice == nil)
            {
                state->metalDevice = (__bridge id<MTLDevice>)bgfx::getInternalData()->context;
                state->commandQueue = (__bridge id<MTLCommandQueue>)bgfx::getInternalData()->commandQueue;
                CVMetalTextureCacheCreate(nullptr, nullptr, state->metalDevice, nullptr, &state->textureCache);
            }

            if (state->bgfxHandle.idx != textureHandle.idx)
            {
                state->bgfxHandle = textureHandle;
                state->refreshBgfxHandle = true;
            }

            const CMTime itemTime = [state->videoOutput itemTimeForHostTime:CACurrentMediaTime()];
            if ([state->videoOutput hasNewPixelBufferForItemTime:itemTime])
            {
                CVPixelBufferRef pixelBuffer = [state->videoOutput copyPixelBufferForItemTime:itemTime itemTimeForDisplay:nil];
                if (pixelBuffer != nullptr)
                {
                    const size_t width = CVPixelBufferGetWidth(pixelBuffer);
                    const size_t height = CVPixelBufferGetHeight(pixelBuffer);
                    state->width.store(static_cast<uint32_t>(width));
                    state->height.store(static_cast<uint32_t>(height));

                    // (Re)create the persistent sampled texture on size change.
                    if (state->textureRGBA == nil || state->textureRGBA.width != width || state->textureRGBA.height != height)
                    {
                        MTLTextureDescriptor* descriptor = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatBGRA8Unorm
                                                                                                               width:width
                                                                                                              height:height
                                                                                                           mipmapped:NO];
                        descriptor.usage = MTLTextureUsageShaderRead;
                        state->textureRGBA = [state->metalDevice newTextureWithDescriptor:descriptor];
                        state->refreshBgfxHandle = true;
                    }

                    // Zero-copy Metal view of the decoded BGRA pixel buffer.
                    CVMetalTextureRef sourceTextureRef{};
                    CVMetalTextureCacheCreateTextureFromImage(nullptr, state->textureCache, pixelBuffer, nullptr,
                        MTLPixelFormatBGRA8Unorm, width, height, 0, &sourceTextureRef);

                    if (sourceTextureRef != nullptr)
                    {
                        id<MTLTexture> sourceTexture = CVMetalTextureGetTexture(sourceTextureRef);
                        if (sourceTexture != nil)
                        {
                            id<MTLCommandBuffer> commandBuffer = [state->commandQueue commandBuffer];
                            commandBuffer.label = @"NativeVideoPlayerBlit";
                            id<MTLBlitCommandEncoder> blit = [commandBuffer blitCommandEncoder];
                            [blit copyFromTexture:sourceTexture
                                      sourceSlice:0
                                      sourceLevel:0
                                     sourceOrigin:MTLOriginMake(0, 0, 0)
                                       sourceSize:MTLSizeMake(width, height, 1)
                                        toTexture:state->textureRGBA
                                 destinationSlice:0
                                 destinationLevel:0
                                destinationOrigin:MTLOriginMake(0, 0, 0)];
                            [blit endEncoding];
                            [commandBuffer addCompletedHandler:^(id<MTLCommandBuffer>) {
                                // Keep the pixel buffer + texture view alive until the GPU is done.
                                CFRelease(sourceTextureRef);
                                CVPixelBufferRelease(pixelBuffer);
                            }];
                            [commandBuffer commit];
                        }
                        else
                        {
                            CFRelease(sourceTextureRef);
                            CVPixelBufferRelease(pixelBuffer);
                        }
                    }
                    else
                    {
                        CVPixelBufferRelease(pixelBuffer);
                    }
                }
            }

            // Wire (or re-wire) Babylon's texture handle to our persistent texture. May
            // fail until the handle has been initialized by bgfx; retried next frame.
            if (state->textureRGBA != nil && state->refreshBgfxHandle && bgfx::isValid(state->bgfxHandle))
            {
                state->refreshBgfxHandle = bgfx::overrideInternal(state->bgfxHandle, reinterpret_cast<uintptr_t>(state->textureRGBA)) == 0;
            }
        });
    }
}
