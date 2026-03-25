#include "RenderDoc.h"

#ifdef RENDERDOC

#ifdef _WIN32
#include <Windows.h>
#include "C:\Program Files\RenderDoc\renderdoc_app.h"
#elif defined(__linux__)
#include <dlfcn.h>
#include "renderdoc_app.h"
#endif

namespace
{
    RENDERDOC_API_1_1_2* rdoc_api = nullptr;
}

#endif

void RenderDoc::Init()
{
#ifdef RENDERDOC
#ifdef _WIN32
    if (HMODULE mod = GetModuleHandleA("renderdoc.dll"))
    {
        pRENDERDOC_GetAPI RENDERDOC_GetAPI = (pRENDERDOC_GetAPI)GetProcAddress(mod, "RENDERDOC_GetAPI");
        int ret = RENDERDOC_GetAPI(eRENDERDOC_API_Version_1_1_2, (void**)&rdoc_api);
        (void)ret;
    }
#elif defined(__linux__)
    if (void* mod = dlopen("librenderdoc.so", RTLD_NOW | RTLD_NOLOAD))
    {
        pRENDERDOC_GetAPI RENDERDOC_GetAPI = (pRENDERDOC_GetAPI)dlsym(mod, "RENDERDOC_GetAPI");
        int ret = RENDERDOC_GetAPI(eRENDERDOC_API_Version_1_1_2, (void**)&rdoc_api);
        (void)ret;
    }
#endif
#endif
}

void RenderDoc::StartFrameCapture(void* device)
{
#ifdef RENDERDOC
    if (rdoc_api)
    {
        rdoc_api->StartFrameCapture(device, nullptr);
    }
#else
    (void)device;
#endif
}

void RenderDoc::StopFrameCapture(void* device)
{
#ifdef RENDERDOC
    if (rdoc_api)
    {
        rdoc_api->EndFrameCapture(device, nullptr);
    }
#else
    (void)device;
#endif
}
