#pragma once

// Video-file playback backend for NativeVideo (HTMLVideoElement.src support).
//
// Each instance owns a platform media player (AVPlayer on Apple) and a persistent
// GPU texture that receives decoded frames. The texture is wired to Babylon.js's
// VideoTexture with the same bgfx createTexture2D-less overrideInternal pattern
// used by CameraDevice: Babylon creates the InternalTexture (createDynamicTexture)
// and NativeVideo::UpdateTexture hands its bgfx handle here every frame.
//
// Event callbacks may fire on ANY thread — callers must marshal to the JS thread.

#include <bgfx/bgfx.h>
#include <functional>
#include <memory>
#include <string>

namespace Babylon::Graphics
{
    class DeviceContext;
}

namespace Babylon::Plugins
{
    class VideoPlayer
    {
    public:
        enum class Event
        {
            MetadataLoaded, // duration/dimensions known; playback can start
            Ended,          // reached the end (not fired when looping)
            Seeked,         // a Seek() completed
        };

        using EventCallbackT = std::function<void(Event)>;

        // Returns nullptr on platforms without an implementation.
        // Supported url forms: app:///<bundle-relative-path>, absolute file paths, http(s) URLs.
        static std::unique_ptr<VideoPlayer> Create(Graphics::DeviceContext& deviceContext, const std::string& url, EventCallbackT eventCallback);

        virtual ~VideoPlayer() = default;

        virtual void Play() = 0;
        virtual void Pause() = 0;
        virtual void SetLoop(bool loop) = 0;
        virtual void SetMuted(bool muted) = 0;
        virtual void SetVolume(double volume) = 0;
        virtual void Seek(double timeInSeconds) = 0;

        virtual bool IsReady() const = 0;
        virtual double GetDuration() const = 0;
        virtual double GetCurrentTime() const = 0;
        virtual uint32_t GetWidth() const = 0;
        virtual uint32_t GetHeight() const = 0;

        // Pull the latest decoded frame into the persistent texture and (re)wire it to
        // the given bgfx texture handle. Called on the JS thread once per render frame;
        // the GPU work runs on the render thread. Cheap no-op when no new frame exists.
        virtual void UpdateTexture(bgfx::TextureHandle textureHandle) = 0;
    };
}
