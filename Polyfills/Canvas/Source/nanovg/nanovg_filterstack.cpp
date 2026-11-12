#include "nanovg_filterstack.h"
#include <regex>
#include <cassert>
#include <cmath>

#include <bgfx/bgfx.h>
#include <bgfx/embedded_shader.h>

const std::regex blurRegex(R"(blur\((\d*\.?\d+)(px|rem)?\)|blur\(\))");
const std::regex noneRegex(R"(^\s*none\s*$)");

#include "Shaders/dxbc/vs_fspass.h"
#include "Shaders/dxil/vs_fspass.h"
#include "Shaders/metal/vs_fspass.h"
#include "Shaders/glsl/vs_fspass.h"
#include "Shaders/essl/vs_fspass.h"
#include "Shaders/spirv/vs_fspass.h"

#include "Shaders/dxbc/fs_gaussblur.h"
#include "Shaders/dxil/fs_gaussblur.h"
#include "Shaders/metal/fs_gaussblur.h"
#include "Shaders/glsl/fs_gaussblur.h"
#include "Shaders/essl/fs_gaussblur.h"
#include "Shaders/spirv/fs_gaussblur.h"

#include "Shaders/dxbc/fs_boxblur.h"
#include "Shaders/dxil/fs_boxblur.h"
#include "Shaders/metal/fs_boxblur.h"
#include "Shaders/glsl/fs_boxblur.h"
#include "Shaders/essl/fs_boxblur.h"
#include "Shaders/spirv/fs_boxblur.h"

namespace
{
    constexpr float BLUR_MAX_PX = 1000.f;
    constexpr int BLUR_TAPS = 13;
    constexpr int BLUR_UNIFORM_SIZE = 5; // Matches u_weights[5] in fs_gaussblur.sc.
    constexpr size_t UNIFORM_VECTOR_COMPONENT_COUNT = 4;
    constexpr size_t BLUR_DIRECTION_COUNT = 2;
    constexpr size_t BOX_BLUR_PASS_COUNT = 3;
    constexpr float GAUSSIAN_BLUR_SIGMA_THRESHOLD = 2.f;
    constexpr float BOX_KERNEL_SCALE = 1.879971f;

    enum class BoxKernelComponent : size_t
    {
        Size,
        Radius,
    };
}

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
    if (s_bgfxRefCount++ > 0)
    {
        return;
    }

    m_uniforms.u_strength = bgfx::createUniform("u_strength", bgfx::UniformType::Vec4);
    m_uniforms.u_direction = bgfx::createUniform("u_direction", bgfx::UniformType::Vec4);
    m_uniforms.u_weights = bgfx::createUniform("u_weights", bgfx::UniformType::Vec4, BLUR_UNIFORM_SIZE);

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
    if (s_bgfxRefCount <= 0)
    {
        return;
    }
    if (--s_bgfxRefCount > 0)
    {
        return;
    }

    // Destroy only on the last live canvas/NVG context, then clear handles so a
    // later InitBgfx starts clean and stale handles are never double-destroyed.
    if (bgfx::isValid(m_uniforms.u_strength))
    {
        bgfx::destroy(m_uniforms.u_strength);
        m_uniforms.u_strength = BGFX_INVALID_HANDLE;
    }
    if (bgfx::isValid(m_uniforms.u_direction))
    {
        bgfx::destroy(m_uniforms.u_direction);
        m_uniforms.u_direction = BGFX_INVALID_HANDLE;
    }
    if (bgfx::isValid(m_uniforms.u_weights))
    {
        bgfx::destroy(m_uniforms.u_weights);
        m_uniforms.u_weights = BGFX_INVALID_HANDLE;
    }
    if (bgfx::isValid(s_gaussBlurProg))
    {
        bgfx::destroy(s_gaussBlurProg);
        s_gaussBlurProg = BGFX_INVALID_HANDLE;
    }
    if (bgfx::isValid(s_boxBlurProg))
    {
        bgfx::destroy(s_boxBlurProg);
        s_boxBlurProg = BGFX_INVALID_HANDLE;
    }
}

bool nanovg_filterstack::ValidString(const std::string& string)
{
    return std::regex_match(string, noneRegex) || std::regex_match(string, blurRegex);
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
            assert(stackElementCount < MAX_STACK_SIZE);
            if (radius > 0 && stackElementCount < MAX_STACK_SIZE)
            {
                stackElements[stackElementCount++] = {radius, radius};
            }
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

std::array<float, nanovg_filterstack::BOX_KERNEL_COMPONENT_COUNT> nanovg_filterstack::CalculateBoxKernel(float sigma)
{
    if (sigma > BLUR_MAX_PX)
        sigma = BLUR_MAX_PX;
    float d = std::floor(BOX_KERNEL_SCALE * sigma + 0.5f);
    std::array<float, BOX_KERNEL_COMPONENT_COUNT> kernel = {d, std::floor(d / 2.0f)};
    return kernel;
}

void nanovg_filterstack::Render(
    bgfx::ProgramHandle firstProg,
    std::function<void(bgfx::UniformHandle, const void *value, const uint16_t num)> setUniform,
    std::function<void(bgfx::ProgramHandle, Babylon::Graphics::FrameBuffer*)> firstPass,
    std::function<void(bgfx::ProgramHandle, Babylon::Graphics::FrameBuffer*, Babylon::Graphics::FrameBuffer*)> filterPass,
    std::function<void(bgfx::ProgramHandle, Babylon::Graphics::FrameBuffer*, Babylon::Graphics::FrameBuffer*)> finalPass,
    Babylon::Graphics::FrameBuffer* finalFrameBuffer,
    std::function<Babylon::Graphics::FrameBuffer*()> acquire,
    std::function<void(Babylon::Graphics::FrameBuffer*)> release,
    bool separateComposite
)
{
    if (!stackElementCount)
    {
        firstPass(firstProg, finalFrameBuffer);
        return;
    }

    Babylon::Graphics::FrameBuffer* prevBuf = acquire();
    Babylon::Graphics::FrameBuffer* nextBuf = nullptr;
    bgfx::ProgramHandle lastProg = firstProg;
    firstPass(firstProg, prevBuf);

    static const std::array<std::array<float, UNIFORM_VECTOR_COMPONENT_COUNT>, BLUR_DIRECTION_COUNT> directions = {
        std::array<float, UNIFORM_VECTOR_COMPONENT_COUNT>{1.f, 0.f, 0.f, 0.f},
        std::array<float, UNIFORM_VECTOR_COMPONENT_COUNT>{0.f, 1.f, 0.f, 0.f}
    };

    for (int elementIndex = 0; elementIndex < stackElementCount; ++elementIndex)
    {
        const auto& blur = stackElements[elementIndex];
        const bool lastElement = elementIndex == stackElementCount - 1;
        assert(prevBuf != nullptr);
        assert(nextBuf == nullptr);

        for (size_t directionIndex = 0; directionIndex < BLUR_DIRECTION_COUNT; ++directionIndex)
        {
            const auto& direction = directions[directionIndex];
            const bool last = lastElement && directionIndex == BLUR_DIRECTION_COUNT - 1 && !separateComposite;
            const float sigma = directionIndex == 0 ? blur.horizontal : blur.vertical;

            if (sigma < GAUSSIAN_BLUR_SIGMA_THRESHOLD)
            {
                std::vector<float> kernel = CalculateGaussianKernel(sigma, BLUR_TAPS);
                kernel.resize(BLUR_UNIFORM_SIZE * UNIFORM_VECTOR_COMPONENT_COUNT, 0.0f);
                setUniform(m_uniforms.u_direction, &direction, 1);
                setUniform(m_uniforms.u_weights, kernel.data(), BLUR_UNIFORM_SIZE);

                if (last)
                {
                    lastProg = s_gaussBlurProg;
                    break;
                }
                nextBuf = acquire();
                filterPass(s_gaussBlurProg, prevBuf, nextBuf);
                release(prevBuf);
                prevBuf = nextBuf;
                nextBuf = nullptr;
                continue;
            }

            const std::array<float, BOX_KERNEL_COMPONENT_COUNT> kernel = CalculateBoxKernel(sigma);
            const auto kernelSize = kernel[static_cast<size_t>(BoxKernelComponent::Size)];
            const auto kernelRadius = kernel[static_cast<size_t>(BoxKernelComponent::Radius)];
            const bool isOdd = static_cast<int>(kernelSize) % 2 == 1;
            const float xOffset = direction[0] > 0 ? 0.5f : 0.f;
            const float yOffset = direction[1] > 0 ? 0.5f : 0.f;
            const std::array<std::array<float, UNIFORM_VECTOR_COMPONENT_COUNT>, BOX_BLUR_PASS_COUNT> kernelsOdd = {
                std::array<float, UNIFORM_VECTOR_COMPONENT_COUNT>{kernelSize, kernelRadius, 0.f, 0.f},
                std::array<float, UNIFORM_VECTOR_COMPONENT_COUNT>{kernelSize, kernelRadius, 0.f, 0.f},
                std::array<float, UNIFORM_VECTOR_COMPONENT_COUNT>{kernelSize, kernelRadius, 0.f, 0.f},
            };
            const std::array<std::array<float, UNIFORM_VECTOR_COMPONENT_COUNT>, BOX_BLUR_PASS_COUNT> kernelsEven = {
                std::array<float, UNIFORM_VECTOR_COMPONENT_COUNT>{kernelSize, kernelRadius, -xOffset, -yOffset},
                std::array<float, UNIFORM_VECTOR_COMPONENT_COUNT>{kernelSize, kernelRadius, xOffset, yOffset},
                std::array<float, UNIFORM_VECTOR_COMPONENT_COUNT>{kernelSize + 1, kernelRadius, 0.f, 0.f},
            };

            for (size_t pass = 0; pass < BOX_BLUR_PASS_COUNT; ++pass)
            {
                setUniform(m_uniforms.u_direction, &direction, 1);
                setUniform(m_uniforms.u_weights, isOdd ? kernelsOdd[pass].data() : kernelsEven[pass].data(), 1);

                if (last && pass == BOX_BLUR_PASS_COUNT - 1)
                {
                    lastProg = s_boxBlurProg;
                    break;
                }
                nextBuf = acquire();
                filterPass(s_boxBlurProg, prevBuf, nextBuf);
                release(prevBuf);
                prevBuf = nextBuf;
                nextBuf = nullptr;
            }
        }
    }

    assert(prevBuf != nullptr);
    assert(nextBuf == nullptr);
    finalPass(lastProg, prevBuf, finalFrameBuffer);
    release(prevBuf);
}