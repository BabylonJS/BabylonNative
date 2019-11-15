#include "ShaderCompiler.h"
#include "ResourceLimits.h"
#include <arcana/experimental/array.h>
#include <glslang/Public/ShaderLang.h>
#include <SPIRV/GlslangToSpv.h>
#include <spirv_parser.hpp>
#include <spirv_glsl.hpp>
#include "Console.h"

namespace babylon
{
    extern const TBuiltInResource DefaultTBuiltInResource;

    namespace
    {
        void AddShader(glslang::TProgram& program, glslang::TShader& shader, std::string_view source)
        {
            const std::array<const char*, 1> sources{ source.data() };
            shader.setStrings(sources.data(), gsl::narrow_cast<int>(sources.size()));
            shader.setEnvInput(glslang::EShSourceGlsl, shader.getStage(), glslang::EShClientOpenGL, 100);
            shader.setEnvClient(glslang::EShClientOpenGL, glslang::EShTargetOpenGL_450);
            shader.setEnvTarget(glslang::EShTargetSpv, glslang::EShTargetSpv_1_0);

            if (!shader.parse(&DefaultTBuiltInResource, 430, false, EShMsgDefault))
            {
                throw std::exception();
            }

            program.addShader(&shader);
        }

        std::unique_ptr<spirv_cross::Compiler> CompileShader(glslang::TProgram& program, EShLanguage stage, std::string& glsl)
        {
            std::vector<uint32_t> spirv;
            glslang::GlslangToSpv(*program.getIntermediate(stage), spirv);

            spirv_cross::Parser parser{ std::move(spirv) };
            parser.parse();

            auto compiler = std::make_unique<spirv_cross::CompilerGLSL>(parser.get_parsed_ir());

            // remove "uniform Frame { .... }" . Keep the uniforms
            const std::string frame = "uniform Frame {";
            const std::string frameEnd = "};";
            size_t pos = glsl.find(frame);
            if (pos != std::string::npos)
            {
                glsl.replace(pos, frame.size(), "");
                pos = glsl.find(frameEnd, pos);
                if (pos != std::string::npos)
                {
                    glsl.replace(pos, frameEnd.size(), "");
                }
            }
#ifdef ANDROID
            // remove location
            static const std::string locations[] = {"layout(location=0)", "layout(location=1)", "layout(location=2)", "layout(location=3)", "layout(location=4)","layout(location=5)"};
            for (auto location : locations)
            {
                pos = glsl.find(location);
                while (pos != std::string::npos)
                {
                    glsl.replace(pos, location.size(), "");
                    pos = glsl.find(location);
                }
            }

            // frag def
            static const std::string fragDef = "out vec4 glFragColor;";
            pos = glsl.find(fragDef);
            if (pos != std::string::npos)
            {
                glsl.replace(pos, fragDef.size(), "");
            }

            // frag
            static const std::string fragColor = "glFragColor";
            pos = glsl.find(fragColor);
            if (pos != std::string::npos)
            {
                glsl.replace(pos, fragColor.size(), "gl_FragColor");
            }
#else 
            glsl = std::string("#version 430\n") + glsl;
#endif
            
            return std::move(compiler);
        }
    }

    ShaderCompiler::ShaderCompiler()
    {
        glslang::InitializeProcess();
    }

    ShaderCompiler::~ShaderCompiler()
    {
        glslang::FinalizeProcess();
    }

    void ShaderCompiler::Compile(std::string_view vertexSource, std::string_view fragmentSource, std::function<void(ShaderInfo, ShaderInfo)> onCompiled)
    {
        glslang::TProgram program;

        glslang::TShader vertexShader{ EShLangVertex };
        AddShader(program, vertexShader, vertexSource);

        glslang::TShader fragmentShader{ EShLangFragment };
        AddShader(program, fragmentShader, fragmentSource);

        if (!program.link(EShMsgDefault))
        {
            throw std::exception();
        }

        std::string vertexGLSL(vertexSource.data(), vertexSource.size());
        auto vertexCompiler = CompileShader(program, EShLangVertex, vertexGLSL);
/*
vertexGLSL = R"(#define DIFFUSEDIRECTUV 0
    #define AMBIENTDIRECTUV 0
    #define OPACITYDIRECTUV 0
    #define EMISSIVEDIRECTUV 0
    #define SPECULARDIRECTUV 0
    #define BUMPDIRECTUV 0
    #define SPECULARTERM
    #define NORMAL
    #define NUM_BONE_INFLUENCERS 0
    #define BonesPerMesh 0
    #define LIGHTMAPDIRECTUV 0
    #define NUM_MORPH_INFLUENCERS 0
    #define VIGNETTEBLENDMODEMULTIPLY
    #define SAMPLER3DGREENDEPTH
    #define SAMPLER3DBGRMAP
    #define LIGHT0
    #define HEMILIGHT0
    
    #define SHADER_NAME vertex:default
    precision highp float;

    uniform mat4 viewProjection;
    uniform mat4 view;
    uniform mat4 world;
    uniform vec4 vLightData0;
    uniform vec4 vLightDiffuse0;
    uniform vec4 vLightSpecular0;
    uniform vec4 vLightGround0;

#define CUSTOM_VERTEX_BEGIN
layout(location=0) in vec3 a_position;
layout(location=1) in vec3 a_normal;
const float PI=3.1415926535897932384626433832795;
const float LinearEncodePowerApprox=2.2;
const float GammaEncodePowerApprox=1.0/LinearEncodePowerApprox;
const vec3 LuminanceEncodeApprox=vec3(0.2126,0.7152,0.0722);
const float Epsilon=0.0000001;
#define saturate(x) clamp(x,0.0,1.0)
#define absEps(x) abs(x)+Epsilon
#define maxEps(x) max(x,Epsilon)
#define saturateEps(x) clamp(x,Epsilon,1.0)
mat3 transposeMat3(mat3 inMatrix) {
vec3 i0=inMatrix[0];
vec3 i1=inMatrix[1];
vec3 i2=inMatrix[2];
mat3 outMatrix=mat3(
vec3(i0.x,i1.x,i2.x),
vec3(i0.y,i1.y,i2.y),
vec3(i0.z,i1.z,i2.z)
);
return outMatrix;
}
mat3 inverseMat3(mat3 inMatrix) {
float a00=inMatrix[0][0],a01=inMatrix[0][1],a02=inMatrix[0][2];
float a10=inMatrix[1][0],a11=inMatrix[1][1],a12=inMatrix[1][2];
float a20=inMatrix[2][0],a21=inMatrix[2][1],a22=inMatrix[2][2];
float b01=a22*a11-a12*a21;
float b11=-a22*a10+a12*a20;
float b21=a21*a10-a11*a20;
float det=a00*b01+a01*b11+a02*b21;
return mat3(b01,(-a22*a01+a02*a21),(a12*a01-a02*a11),
b11,(a22*a00-a02*a20),(-a12*a00+a02*a10),
b21,(-a21*a00+a01*a20),(a11*a00-a01*a10))/det;
}
vec3 toLinearSpace(vec3 color)
{
return pow(color,vec3(LinearEncodePowerApprox));
}
vec3 toGammaSpace(vec3 color)
{
return pow(color,vec3(GammaEncodePowerApprox));
}
float toGammaSpace(float color)
{
return pow(color,GammaEncodePowerApprox);
}
float square(float value)
{
return value*value;
}
float pow5(float value) {
float sq=value*value;
return sq*sq*value;
}
float getLuminance(vec3 color)
{
return clamp(dot(color,LuminanceEncodeApprox),0.,1.);
}
float getRand(vec2 seed) {
return fract(sin(dot(seed.xy ,vec2(12.9898,78.233)))*43758.5453);
}
float dither(vec2 seed,float varianceAmount) {
float rand=getRand(seed);
float dither=mix(-varianceAmount/255.0,varianceAmount/255.0,rand);
return dither;
}
const float rgbdMaxRange=255.0;
vec4 toRGBD(vec3 color) {
float maxRGB=maxEps(max(color.r,max(color.g,color.b)));
float D=max(rgbdMaxRange/maxRGB,1.);
D=clamp(floor(D)/255.0,0.,1.);
vec3 rgb=color.rgb*D;
rgb=toGammaSpace(rgb);
return vec4(rgb,D);
}
vec3 fromRGBD(vec4 rgbd) {
rgbd.rgb=toLinearSpace(rgbd.rgb);
return rgbd.rgb/rgbd.a;
}

out vec3 vPositionW;
out vec3 vNormalW;




#define CUSTOM_VERTEX_DEFINITIONS
void main(void) {
#define CUSTOM_VERTEX_MAIN_BEGIN
vec3 positionUpdated=a_position;
vec3 normalUpdated=a_normal;
#define CUSTOM_VERTEX_UPDATE_POSITION
#define CUSTOM_VERTEX_UPDATE_NORMAL
mat4 finalWorld=world;
vec4 worldPos=finalWorld*vec4(positionUpdated,1.0);
gl_Position=viewProjection*worldPos;
vPositionW=vec3(worldPos);
mat3 normalWorld=mat3(finalWorld);
vNormalW=normalize(normalWorld*normalUpdated);
vec2 uvUpdated=vec2(0.,0.);
vec2 uv2=vec2(0.,0.);
#define CUSTOM_VERTEX_MAIN_END
})";
*/
        std::string fragmentGLSL(fragmentSource.data(), fragmentSource.size());
        auto fragmentCompiler = CompileShader(program, EShLangFragment, fragmentGLSL);

        uint8_t* strVertex = (uint8_t*)vertexGLSL.data();
        uint8_t* strFragment = (uint8_t*)fragmentGLSL.data();
        onCompiled
        (
            { std::move(vertexCompiler), gsl::make_span(strVertex, vertexGLSL.size()) },
            { std::move(fragmentCompiler), gsl::make_span(strFragment, fragmentGLSL.size()) }
        );
    }
}
