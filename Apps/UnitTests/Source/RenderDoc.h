#pragma once

// To enable RenderDoc captures, uncomment the define below and ensure renderdoc_app.h is
// on the include path (e.g. copy it from the RenderDoc SDK into this directory, or add
// the SDK include directory to UnitTests via CMake).
// #define RENDERDOC

namespace RenderDoc
{
    void Init();
    void StartFrameCapture(void* device = nullptr);
    void StopFrameCapture(void* device = nullptr);
}
