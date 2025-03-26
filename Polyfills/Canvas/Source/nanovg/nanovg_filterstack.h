#pragma once
#include <functional>
#include <string>
#include "Babylon/Graphics/FrameBuffer.h"

class nanovg_filterstack
{
public:
    nanovg_filterstack();

    static void InitBgfx();
    static void DisposeBgfx();
    inline static bgfx::ProgramHandle fspassProg;
    inline static bgfx::ProgramHandle blurProg;
    inline struct Uniforms
    {
        bgfx::UniformHandle u_direction;
    } static m_uniforms;

    // TODO: not necessary?
    void AddSepia(float strength) {}
    void AddContrast(float strength) {}
    void AddBlur(int horizontal, int vertical) {}

    void Render(
        bgfx::ProgramHandle firstProg,
        std::function<void(bgfx::UniformHandle, const void *value)> setUniform,
        std::function<void(bgfx::ProgramHandle firstProg, Babylon::Graphics::FrameBuffer *outBuffer)> firstPass,
        std::function<void(bgfx::ProgramHandle firstProg, Babylon::Graphics::FrameBuffer *inBuffer, Babylon::Graphics::FrameBuffer *outBuffer)> filterPass,
        Babylon::Graphics::FrameBuffer* finalFrameBuffer,
        std::function<Babylon::Graphics::FrameBuffer*()> acquire,
        std::function<void(Babylon::Graphics::FrameBuffer*)> release
    );

    // HACK: remove this once I implement for both text + shapes...
    void Render(std::function<void()> element);

    void ParseString(const std::string& string);
    static bool ValidString(const std::string& string);

    // NOTE: This currently happens in Context.cpp
    void SetSize(int width, int height)
    {
        // flush pool
        // keep size for new pool item creation
    }
    void AddDropShadow()
    {
        // break down shadow as blur + color
    }
    void Clear() { stackElements.clear(); }
protected:

    enum StackElementTypes
    {
        SE_SEPIA = 0,
        SE_CONTRAST = 1,
        SE_BLUR = 2,
    };

    struct SepiaElement
    {
        float strength;
    };
    struct Contrast
    {
        float strength;
    };
    struct Blur
    {
        int horizontal, vertical;
    };
    struct StackElement
    {
        StackElementTypes type;
        union
        {
            SepiaElement sepiaElement;
            Contrast contrastElement;
            Blur blurElement;
        };
    };

    std::vector<StackElement> stackElements;

    // NOTE: Does it make sense to have this called here? Or find just to pass std::function from nanovg_babylon.cpp
    Babylon::Graphics::FrameBuffer* GetTransientTarget()
    {
        // return a framebuffer that will be reused later as a texture or for another element
        return {};
    }
};