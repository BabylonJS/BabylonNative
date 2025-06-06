#pragma once

#include <d3d11.h>

// Uncomment this to enable renderdoc captures
//#define RENDERDOC

namespace RenderDoc
{
    void Init();
    void StartFrameCapture(ID3D11Device* d3dDevice);
    void StopFrameCapture(ID3D11Device* d3dDevice);
}
