#pragma once

// Uncomment this to enable renderdoc captures
// #define RENDERDOC

namespace RenderDoc
{
    void Init();
    void StartFrameCapture(void* device = nullptr);
    void StopFrameCapture(void* device = nullptr);
}
