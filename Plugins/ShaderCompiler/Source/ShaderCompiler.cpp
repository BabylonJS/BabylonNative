#include <Babylon/Plugins/ShaderCompilerInternal.h>
#include <bgfx/bgfx.h>

namespace
{
    // Patching shader code to append clip space coordinates for the current rendering API.
    // Can be done with glslang shader traversal. Done with string patching for now.
    std::string_view ProcessShaderCoordinates(std::string_view vertexSource, std::string& workingBuffer)
    {
        if (!bgfx::getCaps()->homogeneousDepth)
        {
            const auto lastClosingCurly = vertexSource.find_last_of('}');
            workingBuffer = vertexSource.substr(0, lastClosingCurly);
            workingBuffer += "gl_Position.z = (gl_Position.z + gl_Position.w) / 2.0; }";
            return workingBuffer;
        }

        return vertexSource;
    }

    std::string_view ProcessSamplerFlip(std::string_view vertexSource, std::string& workingBuffer)
    {
        // for d3d, vulkan, metal, flip the texture sampling on vertical axis
        if (!bgfx::getCaps()->originBottomLeft)
        {
            static const std::string shaderNameDefineStr = "#define SHADER_NAME";
            const auto shaderNameDefine = vertexSource.find(shaderNameDefineStr);
            if (shaderNameDefine != std::string::npos)
            {
                static const auto textureSamplerFunctions = R"(
                    highp vec2 flip(highp vec2 uv)
                    {
                        return vec2(uv.x, 1. - uv.y);
                    }
                    highp vec3 flip(highp vec3 uv)
                    {
                        return uv;
                    }
                    #define texture(x,y) texture(x, flip(y))
                    #define textureLod(x,y,z) textureLod(x, flip(y), z)
                    #define SHADER_NAME)";

                workingBuffer = vertexSource;
                workingBuffer.replace(shaderNameDefine, shaderNameDefineStr.length(), textureSamplerFunctions);
                return workingBuffer;
            }
        }

        return vertexSource;
    }
}

namespace Babylon::Plugins
{
    Graphics::BgfxShaderInfo ShaderCompiler::Compile(std::string_view vertexSource, std::string_view fragmentSource)
    {
        std::string vertexWorkingBuffer;
        std::string fragmentWorkingBuffer;
        return CompileInternal(
            ProcessShaderCoordinates(vertexSource, vertexWorkingBuffer),
            ProcessSamplerFlip(fragmentSource, fragmentWorkingBuffer));
    }
}
