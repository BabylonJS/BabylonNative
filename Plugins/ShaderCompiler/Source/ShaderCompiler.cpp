#include <Babylon/Plugins/ShaderCompilerInternal.h>

#include <sstream>

namespace
{
    void NormalizeLineEndings(std::string& source)
    {
        std::string temp;

        for (char ch : source)
        {
            if (ch != '\r')
            {
                temp.push_back(ch);
            }
        }

        source.swap(temp);
    }

#ifndef OPENGL
    // Patching shader code to append clip space coordinates for the current rendering API.
    // Can be done with glslang shader traversal. Done with string patching for now.
    void ProcessShaderCoordinates(std::string& vertexSource)
    {
        vertexSource = vertexSource.substr(0, vertexSource.find_last_of('}')) + "gl_Position.z = (gl_Position.z + gl_Position.w) / 2.0; }";
    }

    void ProcessSamplerFlip(std::string& fragmentSource)
    {
        static const std::string shaderNameDefineStr = "#define SHADER_NAME";
        const auto shaderNameDefine = fragmentSource.find(shaderNameDefineStr);
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

            fragmentSource.replace(shaderNameDefine, shaderNameDefineStr.length(), textureSamplerFunctions);
        }
    }
#endif
}

namespace Babylon::Plugins
{
    Graphics::BgfxShaderInfo ShaderCompiler::Compile(std::string vertexSource, std::string fragmentSource)
    {
        NormalizeLineEndings(vertexSource);
        NormalizeLineEndings(fragmentSource);

#ifndef OPENGL
        ProcessShaderCoordinates(vertexSource);
        ProcessSamplerFlip(fragmentSource);
#endif

        return CompileInternal(std::move(vertexSource), std::move(fragmentSource));
    }
}
