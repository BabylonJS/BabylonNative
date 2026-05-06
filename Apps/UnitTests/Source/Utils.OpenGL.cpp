#include <gtest/gtest.h>
#include "Utils.h"

#include <X11/Xlib.h>
#include <GL/glx.h>

extern Display* g_display;

Babylon::Graphics::TextureT CreateTestTexture(Babylon::Graphics::DeviceT device, uint32_t width, uint32_t height, uint32_t arraySize)
{
    throw std::runtime_error{"not implemented"};
}

void DestroyTestTexture(Babylon::Graphics::TextureT texture)
{
    throw std::runtime_error{"not implemented"};
}

Babylon::Graphics::DeviceT CreateGraphicsDeviceForTest()
{
    // The bgfx OpenGL backend production path on Linux passes pd.context = nullptr and lets bgfx
    // create its own GLX context bound to the X window. The UpdateDevice cross-platform test
    // exercises the alternate path where the caller owns the GL context, so we construct a
    // GLXContext here against the App layer's X Display.
    if (g_display == nullptr)
    {
        return nullptr;
    }

    static int fbAttribs[] = {
        GLX_X_RENDERABLE, True,
        GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
        GLX_RENDER_TYPE, GLX_RGBA_BIT,
        GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
        GLX_RED_SIZE, 8,
        GLX_GREEN_SIZE, 8,
        GLX_BLUE_SIZE, 8,
        GLX_ALPHA_SIZE, 8,
        GLX_DEPTH_SIZE, 24,
        GLX_STENCIL_SIZE, 8,
        GLX_DOUBLEBUFFER, True,
        None};

    int fbCount = 0;
    GLXFBConfig* fbConfigs = glXChooseFBConfig(g_display, DefaultScreen(g_display), fbAttribs, &fbCount);
    if (fbConfigs == nullptr || fbCount == 0)
    {
        return nullptr;
    }

    GLXContext context = glXCreateNewContext(g_display, fbConfigs[0], GLX_RGBA_TYPE, nullptr, True);
    XFree(fbConfigs);

    return reinterpret_cast<Babylon::Graphics::DeviceT>(context);
}

void DestroyGraphicsDeviceForTest(Babylon::Graphics::DeviceT device)
{
    if (device != nullptr && g_display != nullptr)
    {
        glXDestroyContext(g_display, reinterpret_cast<GLXContext>(device));
    }
}
