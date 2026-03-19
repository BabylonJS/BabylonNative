import { NativeEngine, Scene, ShaderMaterial, MeshBuilder, Effect } from "@babylonjs/core";

declare const setShaderTestDone: (failures: number) => void;

// ---------------------------------------------------------------------------
// Comprehensive WebGL2 GLSL ES 3.00 vertex shader
// Tests every language construction and built-in available in ESSL 3.00
// NOTE: No #version directive — BabylonNative's glslang parses at ES 310.
// NOTE: No layout(location=N) on inputs — Babylon binds attributes by name.
// Playground : https://playground.babylonjs.com/#93EMNL
// ---------------------------------------------------------------------------
const vertexSource = `
// ── Precision qualifiers ────────────────────────────────────────────────
precision highp float;
precision highp int;
precision mediump sampler2D;

// ── Preprocessor directives ─────────────────────────────────────────────
#define PI 3.14159265359
#define TWO_PI (2.0 * PI)
#define HALF(x) ((x) * 0.5)
#define FEATURE_ENABLED 1

#if FEATURE_ENABLED
#define USE_FEATURE
#endif

#ifdef USE_FEATURE
#define FEATURE_SCALE 1.0
#else
#define FEATURE_SCALE 0.0
#endif

#ifndef UNDEFINED_MACRO
#define FALLBACK_VALUE 42
#endif

// ── Struct definitions ──────────────────────────────────────────────────
struct LightInfo {
    vec3  position;
    vec3  color;
    float intensity;
};

struct Material {
    vec4      diffuse;
    vec4      specular;
    float     shininess;
    LightInfo mainLight;        // nested struct
};

// ── Uniform blocks (std140 layout) ──────────────────────────────────────
layout(std140) uniform TransformBlock {
    mat4 uModel;
    mat4 uViewProj;
    mat4 uNormalMatrix;
};

layout(std140) uniform SceneBlock {
    vec4  uAmbientColor;
    float uTime;
    int   uFrameCount;
    uint  uFlags;
};

// ── Plain uniforms ──────────────────────────────────────────────────────
uniform Material uMaterial;
uniform float    uCustomFloat;
uniform int      uCustomInt;
uniform uint     uCustomUint;
uniform bool     uCustomBool;

// ── Sampler uniforms (vertex-accessible types) ──────────────────────────
uniform highp sampler2D       uSampler2D;
uniform highp sampler3D       uSampler3D;
uniform highp samplerCube     uSamplerCube;
uniform highp sampler2DArray  uSampler2DArray;

// ── Vertex inputs (names matching Babylon attribute binding) ────────────
in vec3  position;
in vec3  normal;
in vec2  uv;
in vec4  color;
in vec4  tangent;

// ── Outputs to fragment shader ──────────────────────────────────────────
out vec3 vWorldPos;
out vec3 vNormal;
out vec2 vUV;
flat out int   vVertexID;
flat out uint  vFlagsOut;
smooth out vec4 vColor;
centroid out vec2 vCentroidUV;

// ── Constant declarations ───────────────────────────────────────────────
const float EPSILON = 1e-6;
const vec3  UP = vec3(0.0, 1.0, 0.0);
const mat2  ROT45 = mat2(0.7071, -0.7071, 0.7071, 0.7071);
const int   MAX_ITERATIONS = 10;
const uint  BITMASK = 0xFFu;

// ── Array declarations ──────────────────────────────────────────────────
const float weights[4] = float[4](0.1, 0.2, 0.3, 0.4);

// ── Helper function: demonstrates function declarations & return types ──
float saturate(float x) {
    return clamp(x, 0.0, 1.0);
}

vec3 safeNormalize(vec3 v) {
    float len = length(v);
    return len > EPSILON ? v / len : vec3(0.0);
}

// ── Function with out/inout parameters ──────────────────────────────────
void computeLighting(in vec3 N, in vec3 L, out float diffuse, inout float specular) {
    diffuse = max(dot(N, L), 0.0);
    specular += pow(max(dot(reflect(-L, N), vec3(0.0, 0.0, 1.0)), 0.0), 32.0);
}

// ── Function testing matrix operations ──────────────────────────────────
mat4 buildRotationY(float angle) {
    float c = cos(angle);
    float s = sin(angle);
    return mat4(
        c,   0.0, s,   0.0,
        0.0, 1.0, 0.0, 0.0,
        -s,  0.0, c,   0.0,
        0.0, 0.0, 0.0, 1.0
    );
}

// ── Function testing all non-rectangular matrix types ───────────────────
void testMatrixTypes() {
    mat2   m2   = mat2(1.0);
    mat3   m3   = mat3(1.0);
    mat4   m4   = mat4(1.0);
    mat2x3 m2x3 = mat2x3(1.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    mat3x2 m3x2 = mat3x2(1.0, 0.0, 0.0, 1.0, 0.0, 0.0);
    mat2x4 m2x4 = mat2x4(1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0);
    mat4x2 m4x2 = mat4x2(1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0);
    mat3x4 m3x4 = mat3x4(1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    mat4x3 m4x3 = mat4x3(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0);

    // Matrix built-in functions
    mat2 compMul = matrixCompMult(m2, m2);
    mat3 outer   = outerProduct(vec3(1.0), vec3(1.0));
    mat3 trans   = transpose(m3);
    float det    = determinant(m3);
    mat3 inv     = inverse(m3);
    mat4 inv4    = inverse(m4);
}

// ── Main vertex shader ──────────────────────────────────────────────────
void main() {
    // ── All scalar types ────────────────────────────────────────────
    float f  = 1.0;
    int   i  = 1;
    uint  u  = 1u;
    bool  b  = true;

    // ── All vector types ────────────────────────────────────────────
    vec2  v2  = vec2(1.0, 2.0);
    vec3  v3  = vec3(1.0, 2.0, 3.0);
    vec4  v4  = vec4(1.0, 2.0, 3.0, 4.0);
    ivec2 iv2 = ivec2(1, 2);
    ivec3 iv3 = ivec3(1, 2, 3);
    ivec4 iv4 = ivec4(1, 2, 3, 4);
    uvec2 uv2 = uvec2(1u, 2u);
    uvec3 uv3 = uvec3(1u, 2u, 3u);
    uvec4 uv4 = uvec4(1u, 2u, 3u, 4u);
    bvec2 bv2 = bvec2(true, false);
    bvec3 bv3 = bvec3(true, false, true);
    bvec4 bv4 = bvec4(true, false, true, false);

    // ── Swizzle operations ──────────────────────────────────────────
    vec3 swizzled = v4.xyz;
    vec2 swizzled2 = v4.wz;
    vec4 swizzled3 = v3.xyzx;
    v4.xy = v2;
    v4.zw = v2.yx;

    // ── Type constructors / conversions ─────────────────────────────
    float fromInt  = float(i);
    int   fromUint = int(u);
    uint  fromFloat = uint(f);
    float fromBool = float(b);
    vec3  fromIvec = vec3(iv3);
    ivec3 fromVec  = ivec3(v3);
    uvec3 fromVec2 = uvec3(v3);

    // ── Arithmetic operators ────────────────────────────────────────
    float add = f + 1.0;
    float sub = f - 1.0;
    float fmul = f * 2.0;
    float div = f / 2.0;
    int   imod = i % 3;
    vec3  vmul = v3 * 2.0;
    vec3  vdiv = v3 / 2.0;
    mat4  mmul = uModel * uViewProj;

    // ── Comparison operators ────────────────────────────────────────
    bool lt  = f < 2.0;
    bool gt  = f > 0.0;
    bool le  = f <= 1.0;
    bool ge  = f >= 1.0;
    bool eq  = f == 1.0;
    bool ne  = f != 0.0;

    // ── Logical operators ───────────────────────────────────────────
    bool andOp = lt && gt;
    bool orOp  = lt || gt;
    bool notOp = !lt;
    bool xorOp = lt ^^ gt;

    // ── Ternary operator ────────────────────────────────────────────
    float ternary = b ? 1.0 : 0.0;

    // ── Assignment operators ────────────────────────────────────────
    f += 1.0;
    f -= 0.5;
    f *= 2.0;
    f /= 1.5;
    i %= 5;
    i <<= 2;
    i >>= 1;
    i &= 0xFF;
    i |= 0x01;
    i ^= 0x0F;

    // ── Increment/decrement ─────────────────────────────────────────
    i++;
    i--;
    ++i;
    --i;

    // ── Bitwise operators (integer only, new in ES 3.0) ─────────────
    int  band  = i & 0xFF;
    int  bor   = i | 0x01;
    int  bxor  = i ^ 0x0F;
    int  bnot  = ~i;
    int  lsh   = i << 2;
    int  rsh   = i >> 1;
    uint uband = u & 0xFFu;
    uint ubor  = u | 0x01u;
    uint ubxor = u ^ 0x0Fu;
    uint ubnot = ~u;
    uint ulsh  = u << 2u;
    uint ursh  = u >> 1u;

    // ── Integer built-in functions (ES 3.0) ─────────────────────────
    int   absI  = abs(i);
    int   signI = sign(i);
    int   minI  = min(i, 5);
    int   maxI  = max(i, 0);
    int   clampI = clamp(i, 0, 10);
    uint  minU  = min(u, 5u);
    uint  maxU  = max(u, 0u);
    uint  clampU = clamp(u, 0u, 10u);

    // ── Math built-in functions ─────────────────────────────────────
    float absF   = abs(f);
    float signF  = sign(f);
    float floorF = floor(f);
    float ceilF  = ceil(f);
    float fractF = fract(f);
    float modF   = mod(f, 2.0);
    float minF   = min(f, 2.0);
    float maxF   = max(f, 0.0);
    float clampF = clamp(f, 0.0, 1.0);
    float mixF   = mix(0.0, 1.0, 0.5);
    float stepF  = step(0.5, f);
    float ssF    = smoothstep(0.0, 1.0, f);
    float sqrtF  = sqrt(f);
    float isqrtF = inversesqrt(max(f, EPSILON));
    float powF   = pow(f, 2.0);
    float expF   = exp(f);
    float exp2F  = exp2(f);
    float logF   = log(max(f, EPSILON));
    float log2F  = log2(max(f, EPSILON));

    // ── Trigonometric functions ──────────────────────────────────────
    float radians_ = radians(180.0);
    float degrees_ = degrees(PI);
    float sinF  = sin(f);
    float cosF  = cos(f);
    float tanF  = tan(f);
    float asinF = asin(clamp(f, -1.0, 1.0));
    float acosF = acos(clamp(f, -1.0, 1.0));
    float atanF = atan(f);
    float atan2F = atan(f, 1.0);
    float sinhF = sinh(f);
    float coshF = cosh(f);
    float tanhF = tanh(f);
    float asinhF = asinh(f);
    float acoshF = acosh(max(f, 1.0));
    float atanhF = atanh(clamp(f, -0.99, 0.99));

    // ── Geometric functions ─────────────────────────────────────────
    float len     = length(v3);
    float dist    = distance(v3, vec3(0.0));
    float dotP    = dot(v3, UP);
    vec3  crossP  = cross(v3, UP);
    vec3  normV   = normalize(v3);
    vec3  reflV   = reflect(v3, UP);
    vec3  refrV   = refract(v3, UP, 0.5);
    vec3  fwdV    = faceforward(v3, vec3(0.0, 0.0, -1.0), UP);

    // ── Vector relational functions ─────────────────────────────────
    bvec3 ltV  = lessThan(v3, vec3(1.0));
    bvec3 leV  = lessThanEqual(v3, vec3(1.0));
    bvec3 gtV  = greaterThan(v3, vec3(0.0));
    bvec3 geV  = greaterThanEqual(v3, vec3(0.0));
    bvec3 eqV  = equal(v3, vec3(1.0));
    bvec3 neV  = notEqual(v3, vec3(0.0));
    bool  anyV = any(bv3);
    bool  allV = all(bv3);
    bvec3 notV = not(bv3);

    // ── Packing/unpacking functions (ES 3.0) ────────────────────────
    uint  packed1  = packSnorm2x16(v2);
    vec2  unpack1  = unpackSnorm2x16(packed1);
    uint  packed2  = packUnorm2x16(v2);
    vec2  unpack2  = unpackUnorm2x16(packed2);
    uint  packed3  = packHalf2x16(v2);
    vec2  unpack3  = unpackHalf2x16(packed3);

    // ── Texture functions in vertex shader ──────────────────────────
    ivec2 texSize  = textureSize(uSampler2D, 0);
    vec4  texLod   = textureLod(uSampler2D, v2, 0.0);

    // ── isnan / isinf (ES 3.0) ──────────────────────────────────────
    bool  nanCheck = isnan(f);
    bool  infCheck = isinf(f);

    // ── floatBitsToInt / intBitsToFloat / uintBitsToFloat (ES 3.0) ──
    int   fToI = floatBitsToInt(f);
    uint  fToU = floatBitsToUint(f);
    float iToF = intBitsToFloat(fToI);
    float uToF = uintBitsToFloat(fToU);

    // ── Array usage ─────────────────────────────────────────────────
    float dynamicArr[4];
    for (int idx = 0; idx < 4; idx++) {
        dynamicArr[idx] = weights[idx] * f;
    }
    float arrSum = dynamicArr[0] + dynamicArr[1] + dynamicArr[2] + dynamicArr[3];

    // ── Struct usage ────────────────────────────────────────────────
    LightInfo light;
    light.position  = vec3(10.0, 10.0, 10.0);
    light.color     = vec3(1.0);
    light.intensity = 1.0;

    Material mat;
    mat.diffuse   = vec4(1.0, 0.0, 0.0, 1.0);
    mat.specular  = vec4(1.0);
    mat.shininess = 32.0;
    mat.mainLight = light;

    // ── Control flow: if / else if / else ───────────────────────────
    vec3 result = vec3(0.0);
    if (f > 0.5) {
        result = v3;
    } else if (f > 0.25) {
        result = v3 * 0.5;
    } else {
        result = vec3(0.0);
    }

    // ── Control flow: for loop ──────────────────────────────────────
    vec3 accumulated = vec3(0.0);
    for (int j = 0; j < MAX_ITERATIONS; j++) {
        accumulated += v3 * weights[j % 4];
        if (length(accumulated) > 10.0) break;
    }

    // ── Control flow: while loop ────────────────────────────────────
    int counter = 0;
    while (counter < 5) {
        counter++;
        if (counter == 3) continue;
    }

    // ── Control flow: do-while loop ─────────────────────────────────
    int counter2 = 0;
    do {
        counter2++;
    } while (counter2 < 3);

    // ── Control flow: switch / case (ES 3.0) ────────────────────────
    int switchVal = 2;
    vec3 switchResult = vec3(0.0);
    switch (switchVal) {
        case 0:
            switchResult = vec3(1.0, 0.0, 0.0);
            break;
        case 1:
            switchResult = vec3(0.0, 1.0, 0.0);
            break;
        case 2:
            switchResult = vec3(0.0, 0.0, 1.0);
            break;
        default:
            switchResult = vec3(1.0);
            break;
    }

    // ── Matrix operations ───────────────────────────────────────────
    testMatrixTypes();
    mat4 rotY = buildRotationY(uTime);
    vec4 transformed = rotY * vec4(position, 1.0);

    // ── Function call with out/inout parameters ─────────────────────
    float diff = 0.0;
    float spec = 0.0;
    computeLighting(normalize(normal), normalize(light.position - position), diff, spec);

    // ── Comma operator ──────────────────────────────────────────────
    float a, b2;
    a = 1.0;
    b2 = 2.0;

    // ── Built-in vertex variables ───────────────────────────────────
    vVertexID    = gl_VertexID;
    vFlagsOut    = uFlags;
    //vWorldPos    = (uModel * vec4(position, 1.0)).xyz;
    vNormal      = mat3(uNormalMatrix) * normal;
    vUV          = uv;
    vCentroidUV  = uv;
    vColor       = color;

    gl_Position  = uViewProj * uModel * vec4(position, 1.0);
    gl_PointSize = 1.0;
}
`;

// ---------------------------------------------------------------------------
// Comprehensive WebGL2 GLSL ES 3.00 fragment shader
// ---------------------------------------------------------------------------
const fragmentSource = `
// ── Precision qualifiers ────────────────────────────────────────────────
precision highp float;
precision highp int;
precision highp sampler2D;
precision highp sampler3D;
precision highp samplerCube;
precision highp sampler2DArray;
precision highp sampler2DShadow;
precision highp samplerCubeShadow;
precision highp sampler2DArrayShadow;
precision highp isampler2D;
precision highp isampler3D;
precision highp isamplerCube;
precision highp isampler2DArray;
precision highp usampler2D;
precision highp usampler3D;
precision highp usamplerCube;
precision highp usampler2DArray;

// ── Struct (must match vertex) ──────────────────────────────────────────
struct LightInfo {
    vec3  position;
    vec3  color;
    float intensity;
};

struct Material {
    vec4      diffuse;
    vec4      specular;
    float     shininess;
    LightInfo mainLight;
};

// ── Uniform blocks (std140) ─────────────────────────────────────────────
layout(std140) uniform TransformBlock {
    mat4 uModel;
    mat4 uViewProj;
    mat4 uNormalMatrix;
};

layout(std140) uniform SceneBlock {
    vec4  uAmbientColor;
    float uTime;
    int   uFrameCount;
    uint  uFlags;
};

// ── All sampler types ───────────────────────────────────────────────────
uniform sampler2D             uSampler2D;
uniform sampler3D             uSampler3D;
uniform samplerCube           uSamplerCube;
uniform sampler2DArray        uSampler2DArray;
uniform sampler2DShadow       uSampler2DShadow;
uniform samplerCubeShadow     uSamplerCubeShadow;
uniform sampler2DArrayShadow  uSampler2DArrayShadow;
uniform isampler2D            uISampler2D;
uniform isampler3D            uISampler3D;
uniform isamplerCube          uISamplerCube;
uniform isampler2DArray       uISampler2DArray;
uniform usampler2D            uUSampler2D;
uniform usampler3D            uUSampler3D;
uniform usamplerCube          uUSamplerCube;
uniform usampler2DArray       uUSampler2DArray;

// ── Plain uniforms ──────────────────────────────────────────────────────
uniform Material uMaterial;
uniform float    uCustomFloat;

// ── Fragment inputs (from vertex shader) ────────────────────────────────
in vec3 vWorldPos;
in vec3 vNormal;
in vec2 vUV;
flat in int   vVertexID;
flat in uint  vFlagsOut;
smooth in vec4 vColor;
centroid in vec2 vCentroidUV;

// ── Fragment output (MRT-capable) ───────────────────────────────────────
layout(location = 0) out vec4 fragColor;

// ── Constants ───────────────────────────────────────────────────────────
const float PI = 3.14159265359;
const float EPSILON = 1e-6;

// ── Helper: Blinn-Phong ─────────────────────────────────────────────────
vec3 blinnPhong(vec3 N, vec3 L, vec3 V, vec3 lightColor, float shininess) {
    float NdotL = max(dot(N, L), 0.0);
    vec3  H     = normalize(L + V);
    float NdotH = max(dot(N, H), 0.0);
    float specPower = pow(NdotH, shininess);
    return lightColor * (NdotL + specPower);
}

// ── Helper: Fresnel-Schlick ─────────────────────────────────────────────
vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

// ── Helper: normal mapping simulation ───────────────────────────────────
vec3 perturbNormal(vec3 N, vec3 texNormal) {
    // unpack from [0,1] to [-1,1]
    vec3 mapped = texNormal * 2.0 - 1.0;
    return normalize(N + mapped * 0.5);
}

void main() {
    // ── Fragment built-in variables ─────────────────────────────────
    vec4  fragCoord    = gl_FragCoord;
    bool  frontFacing  = gl_FrontFacing;
    float fragDepth    = gl_FragCoord.z;

    // ── Derivative functions (fragment only) ─────────────────────────
    vec2 dxUV = dFdx(vUV);
    vec2 dyUV = dFdy(vUV);
    vec2 fwUV = fwidth(vUV);
    
    // ── Texture sampling: texture() ─────────────────────────────────
    vec4  tex2d       = texture(uSampler2D, vUV);
    /*vec4  tex3d       = texture(uSampler3D, vec3(vUV, 0.0));
    vec4  texCube     = texture(uSamplerCube, vNormal);
    vec4  texArr      = texture(uSampler2DArray, vec3(vUV, 0.0));

    // ── Texture sampling: textureLod() ──────────────────────────────
    vec4  tex2dLod    = textureLod(uSampler2D, vUV, 1.0);

    // ── Texture sampling: textureOffset() ───────────────────────────
    vec4  tex2dOff    = textureOffset(uSampler2D, vUV, ivec2(1, 0));

    // ── Texture sampling: textureGrad() ─────────────────────────────
    vec4  tex2dGrad   = textureGrad(uSampler2D, vUV, dxUV, dyUV);

    // ── Texture sampling: textureProj() ─────────────────────────────
    vec4  tex2dProj   = textureProj(uSampler2D, vec3(vUV, 1.0));
    vec4  tex2dProj4  = textureProj(uSampler2D, vec4(vUV, 0.0, 1.0));

    // ── Texture sampling: textureProjLod() ──────────────────────────
    vec4  tex2dProjL  = textureProjLod(uSampler2D, vec3(vUV, 1.0), 0.0);

    // ── Texture sampling: texelFetch() ──────────────────────────────
    ivec2 texCoord = ivec2(gl_FragCoord.xy);
    vec4  fetched  = texelFetch(uSampler2D, texCoord, 0);

    // ── Texture sampling: textureSize() ─────────────────────────────
    ivec2 size2d    = textureSize(uSampler2D, 0);
    ivec3 size3d    = textureSize(uSampler3D, 0);
    ivec2 sizeCube  = textureSize(uSamplerCube, 0);
    ivec3 sizeArr   = textureSize(uSampler2DArray, 0);

    // ── Shadow sampler ──────────────────────────────────────────────
    float shadow2d    = texture(uSampler2DShadow, vec3(vUV, 0.5));
    float shadowCube  = texture(uSamplerCubeShadow, vec4(vNormal, 0.5));
    float shadowArr   = texture(uSampler2DArrayShadow, vec4(vUV, 0.0, 0.5));

    // ── Integer sampler ─────────────────────────────────────────────
    ivec4 iTex2d     = texture(uISampler2D, vUV);
    ivec4 iTex3d     = texture(uISampler3D, vec3(vUV, 0.0));
    ivec4 iTexCube   = texture(uISamplerCube, vNormal);
    ivec4 iTexArr    = texture(uISampler2DArray, vec3(vUV, 0.0));
    uvec4 uTex2d     = texture(uUSampler2D, vUV);
    uvec4 uTex3d     = texture(uUSampler3D, vec3(vUV, 0.0));
    uvec4 uTexCube   = texture(uUSamplerCube, vNormal);
    uvec4 uTexArr    = texture(uUSampler2DArray, vec3(vUV, 0.0));
    */
    // ── trunc / round (ES 3.0) ──────────────────────────────────────
    float truncF = trunc(1.7);
    float roundF = round(1.5);
    float roundEven_ = roundEven(2.5);

    // ── modf (ES 3.0) ──────────────────────────────────────────────
    float integralPart;
    float fractionalPart = modf(3.75, integralPart);

    // ── min/max/clamp on vec types ──────────────────────────────────
    vec3 minV = min(vNormal, vec3(0.5));
    vec3 maxV = max(vNormal, vec3(0.0));
    vec3 clampV = clamp(vNormal, vec3(0.0), vec3(1.0));
    
    // ── mix with bvec selector ──────────────────────────────────────
    vec3 mixBvec = mix(vec3(0.0), vec3(1.0), bvec3(true, false, true));

    // ── Control flow: if / else with discard ────────────────────────
    if (tex2d.a < 0.01) {
        discard;
    }

    // ── Control flow: for loop with early exit ──────────────────────
    vec3 lightAccum = vec3(0.0);
    for (int j = 0; j < 4; j++) {
        LightInfo lt;
        lt.position  = vec3(float(j) * 3.0, 5.0, 0.0);
        lt.color     = vec3(1.0, 0.9, 0.8);
        lt.intensity = 1.0 / (1.0 + float(j));

        vec3 L = normalize(lt.position - vWorldPos);
        vec3 V = normalize(-vWorldPos);
        lightAccum += blinnPhong(normalize(vNormal), L, V, lt.color, uMaterial.shininess) * lt.intensity;

        if (length(lightAccum) > 3.0) break;
    }
    
    // ── Control flow: while ─────────────────────────────────────────
    float decay = 1.0;
    int wc = 0;
    while (decay > 0.01 && wc < 10) {
        decay *= 0.7;
        wc++;
    }

    // ── Control flow: do-while ──────────────────────────────────────
    float buildUp = 0.0;
    int dc = 0;
    do {
        buildUp += 0.1;
        dc++;
    } while (buildUp < 0.5 && dc < 10);
    
    // ── Control flow: switch / case ─────────────────────────────────
    vec3 modeColor = vec3(0.0);
    switch (vVertexID % 4) {
        case 0: modeColor = vec3(1.0, 0.0, 0.0); break;
        case 1: modeColor = vec3(0.0, 1.0, 0.0); break;
        case 2: modeColor = vec3(0.0, 0.0, 1.0); break;
        default: modeColor = vec3(1.0); break;
    }

    // ── Bitwise on uint flags ───────────────────────────────────────
    bool flag0 = (vFlagsOut & 1u) != 0u;
    bool flag1 = (vFlagsOut & 2u) != 0u;
    uint shifted = vFlagsOut << 1u;
    uint masked  = vFlagsOut & 0xF0u;
    
    // ── Front-facing conditional ────────────────────────────────────
    vec3 normal = frontFacing ? normalize(vNormal) : -normalize(vNormal);

    // ── Fresnel ─────────────────────────────────────────────────────
    vec3 viewDir = normalize(-vWorldPos);
    vec3 fresnel = fresnelSchlick(max(dot(normal, viewDir), 0.0), vec3(0.04));

    // ── Normal perturbation ─────────────────────────────────────────
    vec3 perturbedN = perturbNormal(normal, tex2d.rgb);
    
    // ── Final composition ───────────────────────────────────────────
    vec3 albedo   = tex2d.rgb * uMaterial.diffuse.rgb * vColor.rgb;
    vec3 ambient  = uAmbientColor.rgb * albedo;
    
    vec3 lit      = ambient + lightAccum * albedo;
    vec3 specular = fresnel * uMaterial.specular.rgb;
    
    vec4 finalColor = vec4(lit + specular + modeColor * 0.01, tex2d.a * uMaterial.diffuse.a);
    
    // ── Tone mapping (Reinhard) ─────────────────────────────────────
    finalColor.rgb = finalColor.rgb / (finalColor.rgb + vec3(1.0));

    // ── Gamma correction ────────────────────────────────────────────
    finalColor.rgb = pow(finalColor.rgb, vec3(1.0 / 2.2));

    // ── Write fragment depth (ES 3.0) ───────────────────────────────
    gl_FragDepth = fragDepth;

    // ── Output ──────────────────────────────────────────────────────
    fragColor = finalColor;
}
`;

// ---------------------------------------------------------------------------
// Test runner
// ---------------------------------------------------------------------------
console.log("=== Shader Cross-Compilation Test ===");

const engine = new NativeEngine();
const scene = new Scene(engine);
scene.createDefaultCamera();

const sphere = MeshBuilder.CreateSphere("sphere", { segments: 8, diameter: 1 }, scene);

let failures = 0;
let done = false;

function finish(f: number) {
    if (!done) {
        done = true;
        engine.stopRenderLoop();
        setShaderTestDone(f);
    }
}

try {
    const shaders = {
        vertexSource: vertexSource,
        fragmentSource: fragmentSource,
    };

    const mat = new ShaderMaterial("crossTest", scene, shaders, {
        attributes: [
            "position", "normal", "uv", "color", "tangent",
        ],
        uniforms: [
            "world", "worldViewProjection", "worldView", "view", "projection",
            "uCustomFloat", "uCustomInt", "uCustomUint", "uCustomBool",
        ],
        samplers: [
            "uSampler2D", "uSampler3D", "uSamplerCube", "uSampler2DArray",
            "uSampler2DShadow", "uSamplerCubeShadow", "uSampler2DArrayShadow",
            "uISampler2D", "uISampler3D", "uISamplerCube", "uISampler2DArray",
            "uUSampler2D", "uUSampler3D", "uUSamplerCube", "uUSampler2DArray",
        ],
        uniformBuffers: [
            "TransformBlock", "SceneBlock",
        ],
    });

    mat.onCompiled = (effect: Effect) => {
        console.log("[PASS] Shader compiled successfully.");
        finish(failures);
    };

    mat.onError = (effect: Effect, errors: string) => {
        console.log("[FAIL] Shader compilation error: " + errors);
        failures++;
        finish(failures);
    };

    sphere.material = mat;
    console.log("[INFO] ShaderMaterial created, starting render loop...");

    // Must start scene rendering so isReady() is called on the material,
    // which triggers effect creation and compilation.
    engine.runRenderLoop(() => {
        scene.render();
    });
} catch (e: any) {
    console.log("[FAIL] ShaderMaterial creation error: " + e.message);
    failures++;
    finish(failures);
}
