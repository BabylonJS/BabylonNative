#pragma once
#include <functional>
#include <string>
#include "Babylon/Graphics/FrameBuffer.h"

class nanovg_filterstack
{
public:
    nanovg_filterstack();

    void AddSepia(float strength)
    {
        SepiaElement sepia{ strength };
        stackElements.push_back({ sepia });
    }
    void AddContrast(float strength) {  }
    void AddBlur(int horizontal, int vertical) {  }

    void Render(std::function<void()> element);
    void ParseString(const std::string& string);
    static bool ValidString(const std::string& string);

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
        union
        {
            SepiaElement sepiaElement;
            Contrast contrastElement;
            Blur blurElement;
        };
    };
    std::vector<StackElement> stackElements;

    Babylon::Graphics::FrameBuffer* GetTransientTarget()
    {
        // return a framebuffer that will be reused later as a texture or for another element
        return {};
    }
};