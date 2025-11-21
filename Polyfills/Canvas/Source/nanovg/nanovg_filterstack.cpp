#include "nanovg_filterstack.h"
#include <regex>
#include <cassert>
#include <cmath>

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

#define BLUR_MAX_PX 1000
#define BLUR_TAPS 13
#define BLUR_UNIFORM_SIZE 5 // fit into vec4: ceil(BLUR_TAPS / 4)

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
    s_gaussBlurProg = bgfx::createProgram(
        bgfx::createEmbeddedShader(s_embeddedShadersFilterStack, type, "vs_fspass")
        , bgfx::createEmbeddedShader(s_embeddedShadersFilterStack, type, "fs_gaussblur")
        , true
    );
    s_boxBlurProg = bgfx::createProgram(
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
    if (s_gaussBlurProg.idx != bgfx::kInvalidHandle)
        bgfx::destroy(s_gaussBlurProg);
    if (s_boxBlurProg.idx != bgfx::kInvalidHandle)
        bgfx::destroy(s_boxBlurProg);
}

bool nanovg_filterstack::ValidString(const std::string& string)
{
    std::smatch match;
    return std::regex_match(string, match, noneRegex) || std::regex_match(string, match, blurRegex);
}

void nanovg_filterstack::ParseString(const std::string& string)
{
    stackElementCount = 0;

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

            assert(stackElementCount < MAX_STACK_SIZE);
            if (radius > 0 && stackElementCount < MAX_STACK_SIZE)
            {
                StackElement& element = stackElements[stackElementCount++];
                element.type = SE_BLUR;
                element.blurElement = {radius, radius};
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

std::array<float, 2> nanovg_filterstack::CalculateBoxKernel(float sigma)
{
    if (sigma > BLUR_MAX_PX)
        sigma = BLUR_MAX_PX;
    float d = std::floor(1.879971f * sigma + 0.5f); // d = floor(s * (3 * sqrt(2 * pi) / 4) + 0.5)
    std::array<float, 2> kernel = {d, std::floor(d / 2.0f)}; // kernel size, kernel radius
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
    std::function<void(bgfx::ProgramHandle, Babylon::Graphics::FrameBuffer*, Babylon::Graphics::FrameBuffer*)> finalPass,
    Babylon::Graphics::FrameBuffer* finalFrameBuffer,
    std::function<Babylon::Graphics::FrameBuffer*()> acquire,
    std::function<void(Babylon::Graphics::FrameBuffer*)> release
)
{
    if (!stackElementCount)
    {
        // no filter, render straight into final framebuffer
        firstPass(firstProg, finalFrameBuffer);
    }
    else
    {
        Babylon::Graphics::FrameBuffer* prevBuf = nullptr;
        Babylon::Graphics::FrameBuffer* nextBuf = acquire();
        bgfx::ProgramHandle lastProg = firstProg;

        // first pass
        firstPass(firstProg, nextBuf);
        prevBuf = nextBuf;
        nextBuf = nullptr;

        int i = 0;
        for (int iStackElement = 0; iStackElement < stackElementCount; iStackElement++)
        {
            auto& element = stackElements[iStackElement];
            assert(prevBuf != nullptr);
            assert(nextBuf == nullptr);

            const bool lastElement = (i == stackElementCount - 1);

            if (element.type == SE_BLUR)
            {
                static const std::array<std::array<float, 4>, 2> directions = {
                    std::array<float, 4>{1.f, 0.f, 0.f, 0.f}, // horizontal
                    std::array<float, 4>{0.f, 1.f, 0.f, 0.f}  // vertical
                };

                for (int i = 0; i < 2; i++)
                {
                    const std::array<float, 4>& direction = directions[i];
                    bool last = lastElement && i == 1;
                    float sigma = i == 0 ? element.blurElement.horizontal : element.blurElement.vertical;

                    // use gaussian blur for sigma < 2, box blur for sigma >= 2
                    if (sigma < 2)
                    {
                        std::vector<float> kernel = CalculateGaussianKernel(sigma, BLUR_TAPS);
                        setUniform(m_uniforms.u_direction, &direction, 1);
                        setUniform(m_uniforms.u_weights, kernel.data(), BLUR_UNIFORM_SIZE);

                        if (last)
                        {
                            lastProg = s_gaussBlurProg;
                            break; // last pass will write to finalFrameBuffer
                        }
                        nextBuf = acquire();
                        filterPass(s_gaussBlurProg, prevBuf, nextBuf);
                        release(prevBuf);
                        prevBuf = nextBuf;
                        nextBuf = nullptr;
                    }
                    else
                    {
                        std::array<float, 2> kernel = CalculateBoxKernel(sigma);
                        bool isOdd = static_cast<int>(kernel[0]) % 2 == 1;

                        std::array<std::array<float, 4>, 3> kernelsOdd = {
                            std::array<float, 4>{kernel[0], kernel[1], 0.f, 0.f},
                            std::array<float, 4>{kernel[0], kernel[1], 0.f, 0.f},
                            std::array<float, 4>{kernel[0], kernel[1], 0.f, 0.f},
                        };
                        float xOffsetL = direction[0] > 0 ? -0.5f : 0.f;
                        float xOffsetR = direction[0] > 0 ? 0.5f : 0.f;
                        float yOffsetL = direction[1] > 0 ? -0.5f : 0.f;
                        float yOffsetR = direction[1] > 0 ? 0.5f : 0.f;
                        std::array<std::array<float, 4>, 3> kernelsEven = {
                            std::array<float, 4>{kernel[0], kernel[1], xOffsetL, yOffsetL},
                            std::array<float, 4>{kernel[0], kernel[1], xOffsetR, yOffsetR},
                            std::array<float, 4>{kernel[0] + 1, kernel[1], 0.f, 0.f},
                        };

                        // 3 pass box blur for s >= 2
                        for (int i = 0; i < 3; i++)
                        {
                            setUniform(m_uniforms.u_direction, &direction, 1);
                            setUniform(m_uniforms.u_weights, isOdd ? kernelsOdd[i].data() : kernelsEven[i].data(), 1);

                            if (last && i == 2)
                            {
                                lastProg = s_boxBlurProg;
                                break; // last pass will write to finalFrameBuffer
                            }
                            nextBuf = acquire();
                            filterPass(s_boxBlurProg, prevBuf, nextBuf);
                            release(prevBuf);
                            prevBuf = nextBuf;
                            nextBuf = nullptr;
                        }
                    }
                }
            }
            i++;
        }

        assert(prevBuf != nullptr);
        assert(nextBuf == nullptr);

        finalPass(lastProg, prevBuf, finalFrameBuffer);
        release(prevBuf);
    }
}