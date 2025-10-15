#include "RenderDoc.h"
#include <Windows.h>
#include <cassert>

#ifdef RENDERDOC

#include "C:\\Program Files\\RenderDoc\\renderdoc_app.h"

namespace
{
    RENDERDOC_API_1_1_2* rdoc_api = nullptr;
}

#endif

void RenderDoc::Init()
{
#ifdef RENDERDOC
    if (HMODULE mod = GetModuleHandleA("renderdoc.dll"))
    {
        pRENDERDOC_GetAPI RENDERDOC_GetAPI = (pRENDERDOC_GetAPI)GetProcAddress(mod, "RENDERDOC_GetAPI");
        int ret = RENDERDOC_GetAPI(eRENDERDOC_API_Version_1_1_2, (void **)&rdoc_api);
        assert(ret == 1);
    }
#endif
}

void RenderDoc::StartFrameCapture(ID3D11Device* d3dDevice)
{
#ifdef RENDERDOC
    if (rdoc_api)
    {
        rdoc_api->StartFrameCapture(d3dDevice, nullptr);
    }
#endif
}

void RenderDoc::StopFrameCapture(ID3D11Device* d3dDevice)
{
#ifdef RENDERDOC
    if (rdoc_api)
    {
        rdoc_api->EndFrameCapture(d3dDevice, nullptr);
    }
#endif
}
