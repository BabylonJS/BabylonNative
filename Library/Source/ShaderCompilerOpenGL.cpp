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

            // TODO: Do this to avoid the work around for dFdy?
            //shader->setInvertY(true);

            if (!shader.parse(&DefaultTBuiltInResource, 430, false, EShMsgDefault))
            {
                Console::Error(shader.getInfoDebugLog());
                throw std::exception();
            }

            program.addShader(&shader);
        }

        std::unique_ptr<spirv_cross::Compiler> CompileShader(glslang::TProgram& program, EShLanguage stage, /*gsl::span<const spirv_cross::HLSLVertexAttributeRemap> attributes, ID3DBlob** blob*/std::string& glsl)
        {
            std::vector<uint32_t> spirv;
            glslang::GlslangToSpv(*program.getIntermediate(stage), spirv);

            spirv_cross::Parser parser{ std::move(spirv) };
            parser.parse();




            auto compiler = std::make_unique<spirv_cross::CompilerGLSL>(parser.get_parsed_ir());

            /*
                        auto options = compiler->get_common_options();
            	options.version = 430;
				options.es = false;
                options.flatten_multidimensional_arrays = true;
                options.enable_420pack_extension = false;
                options.emit_uniform_buffer_as_plain_uniforms = true;
                compiler->set_common_options(options);
                */


            //compiler->set_hlsl_options({ 40 });

            /*for (const auto& attribute : attributes)
            {
                compiler->add_vertex_attribute_remap(attribute);
            }
            */
            /*std::string*/ glsl = compiler->compile();

            //Microsoft::WRL::ComPtr<ID3DBlob> errorMsgs;
            //const char* target = stage == EShLangVertex ? "vs_4_0" : "ps_4_0";

            //UINT flags = 0;
            /*
#ifdef _DEBUG
            flags |= D3DCOMPILE_DEBUG;
#endif

            if (FAILED(D3DCompile(hlsl.data(), hlsl.size(), nullptr, nullptr, nullptr, "main", target, flags, 0, blob, &errorMsgs)))
            {
                throw std::exception(static_cast<const char*>(errorMsgs->GetBufferPointer()));
            }
            */
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


        std::string _vs = R"(
#define DIFFUSEDIRECTUV 0
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

layout(location=0) out vec3 vPositionW;
layout(location=1) out vec3 vNormalW;




#define CUSTOM_VERTEX_DEFINITIONS
void main(void) {
#define CUSTOM_VERTEX_MAIN_BEGIN
vec3 positionUpdated=a_position;
vec3 normalUpdated=a_normal;
#define CUSTOM_VERTEX_UPDATE_POSITION
#define CUSTOM_VERTEX_UPDATE_NORMAL
mat4 finalWorld=world;
gl_Position=viewProjection*finalWorld*vec4(positionUpdated,1.0);
vec4 worldPos=finalWorld*vec4(positionUpdated,1.0);
vPositionW=vec3(worldPos);
mat3 normalWorld=mat3(finalWorld);
vNormalW=normalize(normalWorld*normalUpdated);
vec2 uvUpdated=vec2(0.,0.);
vec2 uv2=vec2(0.,0.);
#define CUSTOM_VERTEX_MAIN_END
}
)";
std::string _fs = R"(
#define DIFFUSEDIRECTUV 0
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

#define SHADER_NAME fragment:default
precision highp float;
             
uniform vec4 vDiffuseColor;
uniform vec4 vSpecularColor;
uniform vec4 vEmissiveColor;
uniform vec4 visibility;
uniform vec4 vEyePosition;
uniform vec4 vAmbientColor;
uniform vec4 vLightData0;
uniform vec4 vLightDiffuse0;
uniform vec4 vLightSpecular0;
uniform vec4 vLightGround0;
  



#define CUSTOM_FRAGMENT_BEGIN
#define RECIPROCAL_PI2 0.15915494


layout(location=0) in vec3 vPositionW;
layout(location=1) in vec3 vNormalW;
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




struct lightingInfo
{
vec3 diffuse;
vec3 specular;
};
lightingInfo computeLighting(vec3 viewDirectionW,vec3 vNormal,vec4 lightData,vec3 diffuseColor,vec3 specularColor,float range,float glossiness) {
lightingInfo result;
vec3 lightVectorW;
float attenuation=1.0;
if (lightData.w == 0.)
{
vec3 direction=lightData.xyz-vPositionW;
attenuation=max(0.,1.0-length(direction)/range);
lightVectorW=normalize(direction);
}
else
{
lightVectorW=normalize(-lightData.xyz);
}
float ndl=max(0.,dot(vNormal,lightVectorW));
result.diffuse=ndl*diffuseColor*attenuation;
vec3 angleW=normalize(viewDirectionW+lightVectorW);
float specComp=max(0.,dot(vNormal,angleW));
specComp=pow(specComp,max(1.,glossiness));
result.specular=specComp*specularColor*attenuation;
return result;
}
lightingInfo computeSpotLighting(vec3 viewDirectionW,vec3 vNormal,vec4 lightData,vec4 lightDirection,vec3 diffuseColor,vec3 specularColor,float range,float glossiness) {
lightingInfo result;
vec3 direction=lightData.xyz-vPositionW;
vec3 lightVectorW=normalize(direction);
float attenuation=max(0.,1.0-length(direction)/range);
float cosAngle=max(0.,dot(lightDirection.xyz,-lightVectorW));
if (cosAngle>=lightDirection.w)
{
cosAngle=max(0.,pow(cosAngle,lightData.w));
attenuation*=cosAngle;
float ndl=max(0.,dot(vNormal,lightVectorW));
result.diffuse=ndl*diffuseColor*attenuation;
vec3 angleW=normalize(viewDirectionW+lightVectorW);
float specComp=max(0.,dot(vNormal,angleW));
specComp=pow(specComp,max(1.,glossiness));
result.specular=specComp*specularColor*attenuation;
return result;
}
result.diffuse=vec3(0.);
result.specular=vec3(0.);
return result;
}
lightingInfo computeHemisphericLighting(vec3 viewDirectionW,vec3 vNormal,vec4 lightData,vec3 diffuseColor,vec3 specularColor,vec3 groundColor,float glossiness) {
lightingInfo result;
float ndl=dot(vNormal,lightData.xyz)*0.5+0.5;
result.diffuse=mix(groundColor,diffuseColor,ndl);
vec3 angleW=normalize(viewDirectionW+lightData.xyz);
float specComp=max(0.,dot(vNormal,angleW));
specComp=pow(specComp,max(1.,glossiness));
result.specular=specComp*specularColor;
return result;
}
vec3 computeProjectionTextureDiffuseLighting(sampler2D projectionLightSampler,mat4 textureProjectionMatrix){
vec4 strq=textureProjectionMatrix*vec4(vPositionW,1.0);
strq/=strq.w;
vec3 textureColor=texture(projectionLightSampler,strq.xy).rgb;
return textureColor;
}
vec4 applyImageProcessing(vec4 result) {
result.rgb=toGammaSpace(result.rgb);
result.rgb=saturate(result.rgb);
return result;
}
#define CUSTOM_FRAGMENT_DEFINITIONS
layout(location=0) out vec4 glFragColor;
void main(void) {
#define CUSTOM_FRAGMENT_MAIN_BEGIN
vec3 viewDirectionW=normalize(vEyePosition.xyz-vPositionW);
vec4 baseColor=vec4(1.,1.,1.,1.);
vec3 diffuseColor=vDiffuseColor.rgb;
float alpha=vDiffuseColor.a;
vec3 normalW=normalize(vNormalW);
vec2 uvOffset=vec2(0.0,0.0);
#define CUSTOM_FRAGMENT_UPDATE_DIFFUSE
vec3 baseAmbientColor=vec3(1.,1.,1.);
#define CUSTOM_FRAGMENT_BEFORE_LIGHTS
float glossiness=vSpecularColor.a;
vec3 specularColor=vSpecularColor.rgb;
vec3 diffuseBase=vec3(0.,0.,0.);
lightingInfo info;
vec3 specularBase=vec3(0.,0.,0.);
float shadow=1.;
info=computeHemisphericLighting(viewDirectionW,normalW,vLightData0,vLightDiffuse0.rgb,vLightSpecular0.xyz,vLightGround0.xyz,glossiness);
shadow=1.;
diffuseBase+=info.diffuse*shadow;
specularBase+=info.specular*shadow;
vec3 refractionColor=vec3(0.,0.,0.);
vec3 reflectionColor=vec3(0.,0.,0.);
vec3 emissiveColor=vEmissiveColor.xyz;
vec3 finalDiffuse=clamp(diffuseBase*diffuseColor+emissiveColor+vAmbientColor.xyz,0.0,1.0)*baseColor.rgb;
vec3 finalSpecular=specularBase*specularColor;
vec4 color=vec4(finalDiffuse*baseAmbientColor+finalSpecular+reflectionColor+refractionColor,alpha);
#define CUSTOM_FRAGMENT_BEFORE_FOG
color.rgb=max(color.rgb,0.);
color.a*=visibility.x;
#define CUSTOM_FRAGMENT_BEFORE_FRAGCOLOR
glFragColor=color;
}
)";


        glslang::TShader vertexShader{ EShLangVertex };
        AddShader(program, vertexShader, vertexSource);

        glslang::TShader fragmentShader{ EShLangFragment };
        AddShader(program, fragmentShader, fragmentSource);

        if (!program.link(EShMsgDefault))
        {
            Console::Error(program.getInfoDebugLog());
            throw std::exception();
        }
        /*
        static const spirv_cross::HLSLVertexAttributeRemap attributes[] =
        {
            { bgfx::Attrib::Position,   "POSITION"      },
            { bgfx::Attrib::Normal,     "NORMAL"        },
            { bgfx::Attrib::Tangent,    "TANGENT"       },
            { bgfx::Attrib::Color0,     "COLOR"         },
            { bgfx::Attrib::Indices,    "BLENDINDICES"  },
            { bgfx::Attrib::Weight,     "BLENDWEIGHT"   },
            { bgfx::Attrib::TexCoord0,  "TEXCOORD0"     },
            { bgfx::Attrib::TexCoord1,  "TEXCOORD1"     },
            { bgfx::Attrib::TexCoord2,  "TEXCOORD2"     },
            { bgfx::Attrib::TexCoord3,  "TEXCOORD3"     },
            { bgfx::Attrib::TexCoord4,  "TEXCOORD4"     },
            { bgfx::Attrib::TexCoord5,  "TEXCOORD5"     },
            { bgfx::Attrib::TexCoord6,  "TEXCOORD6"     },
            { bgfx::Attrib::TexCoord7,  "TEXCOORD7"     },
        };
        */
       // Microsoft::WRL::ComPtr<ID3DBlob> vertexBlob;
        std::string vertexGLSL;
        auto vertexCompiler = CompileShader(program, EShLangVertex, vertexGLSL);
        //vertexGLSL[0]='/';
        //vertexGLSL[1]='/';
        //Microsoft::WRL::ComPtr<ID3DBlob> fragmentBlob;

        std::string fragmentGLSL;
        auto fragmentCompiler = CompileShader(program, EShLangFragment, fragmentGLSL);
        //fragmentGLSL[0]='/';
        //fragmentGLSL[1]='/';


        std::string tmpVS;// = "#version 430\n";
        tmpVS += _vs;
        std::string tmpFS;// = "#version 430\n";
        tmpFS += _fs;

        uint8_t* strVertex = (uint8_t*)tmpVS.data();
        uint8_t* strFragment = (uint8_t*)tmpFS.data();
        onCompiled
        (
            { std::move(vertexCompiler), gsl::make_span(strVertex, tmpVS.size()) },
            { std::move(fragmentCompiler), gsl::make_span(strFragment, tmpFS.size()) }
        );
    }
}
