#include "nanovg_filterstack.h"
#include <regex>
#include <cassert>

#include <bgfx/bgfx.h>
#include <bgfx/embedded_shader.h>

std::regex blurRegex(R"(blur\((\d*\.?\d+)(px|rem)?\)|blur\(\))");
std::regex noneRegex(R"(^\s*none\s*$)");

#define BLUR_ITERATIONS 3

#include "Shaders/dx11/vs_fspass.h"
#include "Shaders/dx11/fs_fspass.h"
#include "Shaders/metal/vs_fspass.h"
#include "Shaders/metal/fs_fspass.h"
#include "Shaders/glsl/vs_fspass.h"
#include "Shaders/glsl/fs_fspass.h"
#include "Shaders/essl/vs_fspass.h"
#include "Shaders/essl/fs_fspass.h"
#include "Shaders/spirv/vs_fspass.h"
#include "Shaders/spirv/fs_fspass.h"

#include "Shaders/dx11/fs_gaussblur.h"
#include "Shaders/metal/fs_gaussblur.h"
#include "Shaders/glsl/fs_gaussblur.h"
#include "Shaders/essl/fs_gaussblur.h"
#include "Shaders/spirv/fs_gaussblur.h"

static const bgfx::EmbeddedShader s_embeddedShadersFilterStack[] =
{
    BGFX_EMBEDDED_SHADER(vs_fspass),
    BGFX_EMBEDDED_SHADER(fs_fspass),
    BGFX_EMBEDDED_SHADER(fs_gaussblur),
    BGFX_EMBEDDED_SHADER_END()
};

nanovg_filterstack::nanovg_filterstack()
{
}

void nanovg_filterstack::InitBgfx()
{
    m_uniforms.u_strength = bgfx::createUniform("u_strength", bgfx::UniformType::Vec4);
    m_uniforms.u_direction = bgfx::createUniform("u_direction", bgfx::UniformType::Vec4);

    // create shaders used by the different elements
    bgfx::RendererType::Enum type = bgfx::getRendererType();
    fspassProg = bgfx::createProgram(
        bgfx::createEmbeddedShader(s_embeddedShadersFilterStack, type, "vs_fspass")
        , bgfx::createEmbeddedShader(s_embeddedShadersFilterStack, type, "fs_fspass")
        , true
    );
    blurProg = bgfx::createProgram(
        bgfx::createEmbeddedShader(s_embeddedShadersFilterStack, type, "vs_fspass")
        , bgfx::createEmbeddedShader(s_embeddedShadersFilterStack, type, "fs_gaussblur")
        , true
    );
}

void nanovg_filterstack::DisposeBgfx()
{
    // check if uniforms + programs are valid before destroying
    if (m_uniforms.u_strength.idx != bgfx::kInvalidHandle)
        bgfx::destroy(m_uniforms.u_strength);
    if (m_uniforms.u_direction.idx != bgfx::kInvalidHandle)
        bgfx::destroy(m_uniforms.u_direction);
    if (fspassProg.idx != bgfx::kInvalidHandle)
        bgfx::destroy(fspassProg);
    if (blurProg.idx != bgfx::kInvalidHandle)
        bgfx::destroy(blurProg);
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
        if ( match.size() > 1 && match[1].matched)
        {
            int radius = std::stoi(match[1].str());
            std::string unit = match[2].matched ? match[2].str() : "px";
            std::transform(unit.begin(), unit.end(), unit.begin(), ::tolower);

            if (unit != "px")
            {
                // TODO: convert non-px radius
            }

            for (int i = 0; i < BLUR_ITERATIONS; i++)
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

void nanovg_filterstack::Render(std::function<void()> element)
{
    element();
}

void nanovg_filterstack::Render(
    bgfx::ProgramHandle firstProg,
    std::function<void(bgfx::UniformHandle, const void *value)> setUniform,
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
                float horizontal[4] = {element.blurElement.horizontal * 1.f, 0.f, 0.f, 0.f}; // scale by element.blurElement.horizontal
                setUniform(m_uniforms.u_direction, horizontal);

                nextBuf = acquire();
                filterPass(blurProg, prevBuf, nextBuf);
                release(prevBuf);
                prevBuf = nextBuf;
                nextBuf = nullptr;

                // Vertical Pass
                float vertical[4] = {0.f, element.blurElement.vertical * 1.f, 0.f, 0.f}; // scale by element.blurElement.vertical
                setUniform(m_uniforms.u_direction, vertical);
                if (last)
                {
                    lastProg = blurProg;
                    break; // last pass will write to finalFrameBuffer
                }
                nextBuf = acquire();
                filterPass(blurProg, prevBuf, nextBuf);
                release(prevBuf);
                prevBuf = nextBuf;
                nextBuf = nullptr;
            }
            i++;
        }

        assert(prevBuf != nullptr);
        assert(nextBuf == nullptr);

        filterPass(lastProg, prevBuf, finalFrameBuffer);
        release(prevBuf);
    }
}