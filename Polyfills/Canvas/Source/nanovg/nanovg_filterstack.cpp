#include "nanovg_filterstack.h"
#include <regex>
#include <cassert>

#include <bgfx/bgfx.h>
#include <bgfx/embedded_shader.h>

std::regex blurRegex(R"(blur\((\d*\.?\d+)(px|rem)?\)|blur\(\))");
std::regex noneRegex(R"(^\s*none\s*$)");

#include "Shaders/dx11/vs_fspass.h"
#include "Shaders/metal/vs_fspass.h"
#include "Shaders/glsl/vs_fspass.h"
#include "Shaders/essl/vs_fspass.h"
#include "Shaders/spirv/vs_fspass.h"

#include "Shaders/dx11/fs_gaussblur.h"
#include "Shaders/metal/fs_gaussblur.h"
#include "Shaders/glsl/fs_gaussblur.h"
#include "Shaders/essl/fs_gaussblur.h"
#include "Shaders/spirv/fs_gaussblur.h"

#include "Shaders/dx11/fs_boxblur.h"
#include "Shaders/metal/fs_boxblur.h"
#include "Shaders/glsl/fs_boxblur.h"
#include "Shaders/essl/fs_boxblur.h"
#include "Shaders/spirv/fs_boxblur.h"

#define BLUR_TAPS 9
#define BLUR_UNIFORM_SIZE 3 // fit into vec4: ceil(BLUR_TAPS / 4)

static const bgfx::EmbeddedShader s_embeddedShadersFilterStack[] =
{
    BGFX_EMBEDDED_SHADER(vs_fspass),
    BGFX_EMBEDDED_SHADER(fs_gaussblur),
    BGFX_EMBEDDED_SHADER(fs_boxblur),
    BGFX_EMBEDDED_SHADER_END()
};

nanovg_filterstack::nanovg_filterstack()
{
}

void nanovg_filterstack::InitBgfx()
{
    m_uniforms.u_strength = bgfx::createUniform("u_strength", bgfx::UniformType::Vec4);
    m_uniforms.u_direction = bgfx::createUniform("u_direction", bgfx::UniformType::Vec4);
    m_uniforms.u_weights = bgfx::createUniform("u_weights", bgfx::UniformType::Vec4, BLUR_UNIFORM_SIZE);

    // create shaders used by the different elements
    bgfx::RendererType::Enum type = bgfx::getRendererType();
    gaussBlurProg = bgfx::createProgram(
        bgfx::createEmbeddedShader(s_embeddedShadersFilterStack, type, "vs_fspass")
        , bgfx::createEmbeddedShader(s_embeddedShadersFilterStack, type, "fs_gaussblur")
        , true
    );
    boxBlurProg = bgfx::createProgram(
        bgfx::createEmbeddedShader(s_embeddedShadersFilterStack, type, "vs_fspass"),
        bgfx::createEmbeddedShader(s_embeddedShadersFilterStack, type, "fs_boxblur"),
        true);
}

void nanovg_filterstack::DisposeBgfx()
{
    // check if uniforms + programs are valid before destroying
    if (m_uniforms.u_strength.idx != bgfx::kInvalidHandle)
        bgfx::destroy(m_uniforms.u_strength);
    if (m_uniforms.u_direction.idx != bgfx::kInvalidHandle)
        bgfx::destroy(m_uniforms.u_direction);
    if (m_uniforms.u_weights.idx != bgfx::kInvalidHandle)
        bgfx::destroy(m_uniforms.u_weights);
    if (gaussBlurProg.idx != bgfx::kInvalidHandle)
        bgfx::destroy(gaussBlurProg);
    if (boxBlurProg.idx != bgfx::kInvalidHandle)
        bgfx::destroy(boxBlurProg);
}

bool nanovg_filterstack::ValidString(const std::string& string)
{
    std::smatch match;
    return std::regex_match(string, match, noneRegex) || std::regex_match(string, match, blurRegex);
}

void nanovg_filterstack::ParseString(const std::string& string)
{
    stackElements.clear();

    std::smatch match;
    if (std::regex_match(string, match, blurRegex))
    {
        if (match.size() > 1 && match[1].matched)
        {
            float radius = std::stof(match[1].str());
            std::string unit = match[2].matched ? match[2].str() : "px";
            std::transform(unit.begin(), unit.end(), unit.begin(), ::tolower);

            if (unit != "px")
            {
                // TODO: convert non-px radius
            }

            if (radius > 0)
            {
                StackElement element = {};
                element.type = SE_BLUR;
                element.blurElement = {radius, radius};
                stackElements.push_back(element);
            }
        }
        else
        {
            // defaults to blur(0), which is no blur
        }
    }
}

std::vector<float> nanovg_filterstack::CalculateGaussianKernel(float sigma, int kernelSize)
{
    assert(kernelSize % 2 == 1); // kernel size must be odd
    std::vector<float> kernel(kernelSize);
    int halfSize = kernelSize / 2;
    float sum = 0.0f;

    // calculate weights
    for (int i = -halfSize; i <= halfSize; ++i)
    {
        float weight = std::exp(-0.5f * (i * i) / (sigma * sigma));
        kernel[i + halfSize] = weight;
        sum += weight;
    }

    // normalize kernel
    for (float& weight : kernel)
    {
        weight /= sum;
    }

    return kernel;
}

std::vector<float> nanovg_filterstack::CalculateBoxKernel(float sigma)
{
    // box blur uses equally weighted kernel
    int d = static_cast<int>(float(1.879971f * sigma + 0.5f)); // d = floor(s * (3 * sqrt(2 * pi) / 4) + 0.5)
    std::vector<float> kernel(d, 1.0f / d);

    return kernel;
}

void nanovg_filterstack::Render(std::function<void()> element)
{
    element();
}

void nanovg_filterstack::Render(
    bgfx::ProgramHandle firstProg,
    std::function<void(bgfx::UniformHandle, const void *value, const uint16_t num)> setUniform,
    std::function<void(bgfx::ProgramHandle, Babylon::Graphics::FrameBuffer*)> firstPass,
    std::function<void(bgfx::ProgramHandle, Babylon::Graphics::FrameBuffer*, Babylon::Graphics::FrameBuffer*)> filterPass,
    Babylon::Graphics::FrameBuffer* finalFrameBuffer,
    std::function<Babylon::Graphics::FrameBuffer*()> acquire,
    std::function<void(Babylon::Graphics::FrameBuffer*)> release
)
{
    if (stackElements.empty())
    {
        // no filter, render straight into final framebuffer
        firstPass(firstProg, finalFrameBuffer);
    }
    else
    {
        assert(stackElements.size() > 0);

        Babylon::Graphics::FrameBuffer* prevBuf = nullptr;
        Babylon::Graphics::FrameBuffer* nextBuf = acquire();
        bgfx::ProgramHandle lastProg = firstProg;

        // first pass
        firstPass(firstProg, nextBuf);
        prevBuf = nextBuf;
        nextBuf = nullptr;

        int i = 0;
        for (auto& element : stackElements)
        {
            assert(prevBuf != nullptr);
            assert(nextBuf == nullptr);

            const bool last = (i == stackElements.size() - 1);

            if (element.type == SE_BLUR)
            {
                // Horizontal Pass
                if (element.blurElement.horizontal < 2)
                {
                    std::vector<float> kernel = CalculateGaussianKernel(element.blurElement.horizontal, BLUR_TAPS);
                    float horizontal[4] = {1.f, 0.f, 0.f, 0.f};
                    setUniform(m_uniforms.u_direction, horizontal, 1);
                    setUniform(m_uniforms.u_weights, kernel.data(), BLUR_UNIFORM_SIZE);

                    nextBuf = acquire();
                    filterPass(gaussBlurProg, prevBuf, nextBuf);
                    release(prevBuf);
                    prevBuf = nextBuf;
                    nextBuf = nullptr;
                }
                // 3 pass box blur for s >= 2
                else
                {
                    std::vector<float> kernel = CalculateBoxKernel(element.blurElement.horizontal);
                    // TODO: 3 pass box blur
                }

                // Vertical Pass
                if (element.blurElement.vertical < 2)
                {
                    std::vector<float> kernel = CalculateGaussianKernel(element.blurElement.vertical, BLUR_TAPS);
                    float vertical[4] = {0.f, 1.f, 0.f, 0.f};
                    setUniform(m_uniforms.u_direction, vertical, 1);
                    setUniform(m_uniforms.u_weights, kernel.data(), 1);
                    if (last)
                    {
                        lastProg = gaussBlurProg;
                        break; // last pass will write to finalFrameBuffer
                    }
                    nextBuf = acquire();
                    filterPass(gaussBlurProg, prevBuf, nextBuf);
                    release(prevBuf);
                    prevBuf = nextBuf;
                    nextBuf = nullptr;
                }
                // 3 pass box blur if s >= 2
                else
                {
                    std::vector<float> kernel = CalculateBoxKernel(element.blurElement.vertical);
                    // TODO: 3 pass box blur
                    // TODO: last pass will write to finalFrameBuffer
                }
            }
            i++;
        }

        assert(prevBuf != nullptr);
        assert(nextBuf == nullptr);

        filterPass(lastProg, prevBuf, finalFrameBuffer);
        release(prevBuf);
    }
}