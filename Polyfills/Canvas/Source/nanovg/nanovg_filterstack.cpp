#include "nanovg_filterstack.h"
#include <regex>
#include <cassert>

#include <bgfx/bgfx.h>
#include <bgfx/embedded_shader.h>

std::regex blurRegex(R"(blur\((\d*\.?\d+)(px|rem)?\)|blur\(\))");
std::regex noneRegex(R"(^\s*none\s*$)");

// TODO: move fspass code from nanovg_babylon.cpp into nanovg_filterstack.cpp
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

static const bgfx::EmbeddedShader s_embeddedShadersFilterStack[] =
{
    BGFX_EMBEDDED_SHADER(vs_fspass),
    BGFX_EMBEDDED_SHADER(fs_fspass),
    BGFX_EMBEDDED_SHADER_END()
};

bgfx::ProgramHandle blurProg;

nanovg_filterstack::nanovg_filterstack()
{
    // create shaders used by the different elements
    // TODO: Implement seperable blur
    bgfx::RendererType::Enum type = bgfx::getRendererType();
	blurProg = bgfx::createProgram(
		bgfx::createEmbeddedShader(s_embeddedShadersFilterStack, type, "vs_fspass")
		, bgfx::createEmbeddedShader(s_embeddedShadersFilterStack, type, "fs_fspass")
		, true
	);
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

            StackElement element = {};
            element.type = SE_BLUR;
            element.blurElement = {radius, radius};
            stackElements.push_back(element);
        }
        else
        {
            // defaults to blur(0)
            StackElement element = {};
            element.type = SE_BLUR;
            element.blurElement = {0, 0};
            stackElements.push_back(element);
        }
    }
}

void nanovg_filterstack::Render(std::function<void()> element)
{
    element();
}

void nanovg_filterstack::Render(
    bgfx::ProgramHandle firstProg,
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
        firstPass(firstProg, finalFrameBuffer); // TODO: filterPass(firstProg, nullptr, finalFrameBuffer);
    }
    else
    {
        Babylon::Graphics::FrameBuffer* prevBuf = nullptr;
        Babylon::Graphics::FrameBuffer* nextBuf = acquire();
        bgfx::ProgramHandle lastProg = firstProg; // TODO: should be able to just rely on this for single pass cases

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
                if (last)
                {
                    lastProg = blurProg;
                    break;
                }

                // HACK: temporary single pass (unseparated)
                nextBuf = acquire();
                filterPass(blurProg, prevBuf, nextBuf);
                release(prevBuf);
                prevBuf = nextBuf;
                nextBuf = nullptr;

                // TODO: seperable blur filter (1x vertical, 1x horizontal)
                /*
                // TODO: vec2 direction uniforms change
                nextBuf = acquire();
                filterPass(seperateBlurProg, prevBuf, nextBuf);
                release(prevBuf);
                prevBuf = nextBuf;
                nextBuf = nullptr;
                */
            }
            i++;
        }

        assert(prevBuf != nullptr);
        assert(nextBuf == nullptr);

        filterPass(lastProg, prevBuf, finalFrameBuffer);
        release(prevBuf);
    }
}