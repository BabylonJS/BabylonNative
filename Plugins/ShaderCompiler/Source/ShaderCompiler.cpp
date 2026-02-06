#include <Babylon/Plugins/ShaderCompilerInternal.h>

namespace
{
#ifndef OPENGL
    // Patching shader code to append clip space coordinates for the current rendering API.
    // Can be done with glslang shader traversal. Done with string patching for now.
    std::string_view ProcessShaderCoordinates(std::string_view source, std::string& workingBuffer)
    {
        workingBuffer = source.substr(0, source.find_last_of('}'));
        workingBuffer += "gl_Position.z = (gl_Position.z + gl_Position.w) / 2.0; }";
        return workingBuffer;
    }

    std::string_view ProcessSamplerFlip(std::string_view source, std::string& workingBuffer)
    {
        static const std::string shaderNameDefineStr = "#define SHADER_NAME";
        const auto shaderNameDefine = source.find(shaderNameDefineStr);
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

            workingBuffer = source;
            return workingBuffer.replace(shaderNameDefine, shaderNameDefineStr.length(), textureSamplerFunctions);
        }

        return source;
    }
#endif
}

namespace Babylon::Plugins
{
    Graphics::BgfxShaderInfo ShaderCompiler::Compile(std::string_view vertexSource, std::string_view fragmentSource)
    {
#ifndef OPENGL
        std::string vertexWorkingBuffer;
        vertexSource = ProcessSamplerFlip(ProcessShaderCoordinates(vertexSource, vertexWorkingBuffer), vertexWorkingBuffer);

        std::string fragmentWorkingBuffer;
        fragmentSource = ProcessSamplerFlip(fragmentSource, fragmentWorkingBuffer);
#endif

        return CompileInternal(vertexSource, fragmentSource);
    }
}
