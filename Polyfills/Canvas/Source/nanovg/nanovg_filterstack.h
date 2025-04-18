#pragma once
#include <functional>
#include <string>
#include <vector>
#include "Babylon/Graphics/FrameBuffer.h"

class nanovg_filterstack
{
public:
    nanovg_filterstack();

    static void InitBgfx();
    static void DisposeBgfx();
    inline static bgfx::ProgramHandle fspassProg;
    inline static bgfx::ProgramHandle gaussBlurProg;
    inline static bgfx::ProgramHandle boxBlurProg;
    inline struct Uniforms
    {
        bgfx::UniformHandle u_strength;
        bgfx::UniformHandle u_direction;
        bgfx::UniformHandle u_weights;
    } static m_uniforms;

    void AddSepia(float strength) {}
    void AddContrast(float strength) {}
    void AddBlur(int horizontal, int vertical) {}

    void Render(
        bgfx::ProgramHandle firstProg,
        std::function<void(bgfx::UniformHandle, const void *value, const uint16_t num)> setUniform,
        std::function<void(bgfx::ProgramHandle firstProg, Babylon::Graphics::FrameBuffer *outBuffer)> firstPass,
        std::function<void(bgfx::ProgramHandle firstProg, Babylon::Graphics::FrameBuffer *inBuffer, Babylon::Graphics::FrameBuffer *outBuffer)> filterPass,
        Babylon::Graphics::FrameBuffer* finalFrameBuffer,
        std::function<Babylon::Graphics::FrameBuffer*()> acquire,
        std::function<void(Babylon::Graphics::FrameBuffer*)> release
    );
    void Render(std::function<void()> element);

    void ParseString(const std::string& string);
    static bool ValidString(const std::string& string);

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
        float horizontal, vertical; // blur strength (standard deviation px)
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

private:
    // calculate Gaussian kernel
    std::vector<float> CalculateGaussianKernel(float sigma, int kernelSize);
    std::vector<float> CalculateBoxKernel(float sigma);
};