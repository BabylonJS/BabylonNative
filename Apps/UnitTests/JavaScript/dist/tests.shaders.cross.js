/******/ (() => { // webpackBootstrap
/******/ 	"use strict";
/******/ 	var __webpack_modules__ = ({

/***/ "@babylonjs/core"
/*!**************************!*\
  !*** external "BABYLON" ***!
  \**************************/
(module) {

module.exports = BABYLON;

/***/ }

/******/ 	});
/************************************************************************/
/******/ 	// The module cache
/******/ 	var __webpack_module_cache__ = {};
/******/ 	
/******/ 	// The require function
/******/ 	function __webpack_require__(moduleId) {
/******/ 		// Check if module is in cache
/******/ 		var cachedModule = __webpack_module_cache__[moduleId];
/******/ 		if (cachedModule !== undefined) {
/******/ 			return cachedModule.exports;
/******/ 		}
/******/ 		// Check if module exists (development only)
/******/ 		if (__webpack_modules__[moduleId] === undefined) {
/******/ 			var e = new Error("Cannot find module '" + moduleId + "'");
/******/ 			e.code = 'MODULE_NOT_FOUND';
/******/ 			throw e;
/******/ 		}
/******/ 		// Create a new module (and put it into the cache)
/******/ 		var module = __webpack_module_cache__[moduleId] = {
/******/ 			// no module.id needed
/******/ 			// no module.loaded needed
/******/ 			exports: {}
/******/ 		};
/******/ 	
/******/ 		// Execute the module function
/******/ 		__webpack_modules__[moduleId](module, module.exports, __webpack_require__);
/******/ 	
/******/ 		// Return the exports of the module
/******/ 		return module.exports;
/******/ 	}
/******/ 	
/************************************************************************/
/******/ 	/* webpack/runtime/compat get default export */
/******/ 	(() => {
/******/ 		// getDefaultExport function for compatibility with non-harmony modules
/******/ 		__webpack_require__.n = (module) => {
/******/ 			var getter = module && module.__esModule ?
/******/ 				() => (module['default']) :
/******/ 				() => (module);
/******/ 			__webpack_require__.d(getter, { a: getter });
/******/ 			return getter;
/******/ 		};
/******/ 	})();
/******/ 	
/******/ 	/* webpack/runtime/define property getters */
/******/ 	(() => {
/******/ 		// define getter functions for harmony exports
/******/ 		__webpack_require__.d = (exports, definition) => {
/******/ 			for(var key in definition) {
/******/ 				if(__webpack_require__.o(definition, key) && !__webpack_require__.o(exports, key)) {
/******/ 					Object.defineProperty(exports, key, { enumerable: true, get: definition[key] });
/******/ 				}
/******/ 			}
/******/ 		};
/******/ 	})();
/******/ 	
/******/ 	/* webpack/runtime/hasOwnProperty shorthand */
/******/ 	(() => {
/******/ 		__webpack_require__.o = (obj, prop) => (Object.prototype.hasOwnProperty.call(obj, prop))
/******/ 	})();
/******/ 	
/******/ 	/* webpack/runtime/make namespace object */
/******/ 	(() => {
/******/ 		// define __esModule on exports
/******/ 		__webpack_require__.r = (exports) => {
/******/ 			if(typeof Symbol !== 'undefined' && Symbol.toStringTag) {
/******/ 				Object.defineProperty(exports, Symbol.toStringTag, { value: 'Module' });
/******/ 			}
/******/ 			Object.defineProperty(exports, '__esModule', { value: true });
/******/ 		};
/******/ 	})();
/******/ 	
/************************************************************************/
var __webpack_exports__ = {};
// This entry needs to be wrapped in an IIFE because it needs to be isolated against other modules in the chunk.
(() => {
/*!************************************!*\
  !*** ./src/tests.shaders.cross.ts ***!
  \************************************/
__webpack_require__.r(__webpack_exports__);
/* harmony import */ var _babylonjs_core__WEBPACK_IMPORTED_MODULE_0__ = __webpack_require__(/*! @babylonjs/core */ "@babylonjs/core");
/* harmony import */ var _babylonjs_core__WEBPACK_IMPORTED_MODULE_0___default = /*#__PURE__*/__webpack_require__.n(_babylonjs_core__WEBPACK_IMPORTED_MODULE_0__);




// ---------------------------------------------------------------------------
// Comprehensive WebGL2 GLSL ES 3.00 vertex shader
// Tests every language construction and built-in available in ESSL 3.00
// NOTE: No #version directive — BabylonNative's glslang parses at ES 310.
// NOTE: No layout(location=N) on inputs — Babylon binds attributes by name.
// Playground : https://playground.babylonjs.com/#93EMNL
// ---------------------------------------------------------------------------
var vertexSource = "\n// \u2500\u2500 Precision qualifiers \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\nprecision highp float;\nprecision highp int;\nprecision mediump sampler2D;\n\n// \u2500\u2500 Preprocessor directives \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\n#define PI 3.14159265359\n#define TWO_PI (2.0 * PI)\n#define HALF(x) ((x) * 0.5)\n#define FEATURE_ENABLED 1\n\n#if FEATURE_ENABLED\n#define USE_FEATURE\n#endif\n\n#ifdef USE_FEATURE\n#define FEATURE_SCALE 1.0\n#else\n#define FEATURE_SCALE 0.0\n#endif\n\n#ifndef UNDEFINED_MACRO\n#define FALLBACK_VALUE 42\n#endif\n\n// \u2500\u2500 Struct definitions \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\nstruct LightInfo {\n    vec3  position;\n    vec3  color;\n    float intensity;\n};\n\nstruct Material {\n    vec4      diffuse;\n    vec4      specular;\n    float     shininess;\n    LightInfo mainLight;        // nested struct\n};\n\n// \u2500\u2500 Uniform blocks (std140 layout) \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\nlayout(std140) uniform TransformBlock {\n    mat4 uModel;\n    mat4 uViewProj;\n    mat4 uNormalMatrix;\n};\n\nlayout(std140) uniform SceneBlock {\n    vec4  uAmbientColor;\n    float uTime;\n    int   uFrameCount;\n    uint  uFlags;\n};\n\n// \u2500\u2500 Plain uniforms \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\nuniform Material uMaterial;\nuniform float    uCustomFloat;\nuniform int      uCustomInt;\nuniform uint     uCustomUint;\nuniform bool     uCustomBool;\n\n// \u2500\u2500 Sampler uniforms (vertex-accessible types) \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\nuniform highp sampler2D       uSampler2D;\nuniform highp sampler3D       uSampler3D;\nuniform highp samplerCube     uSamplerCube;\nuniform highp sampler2DArray  uSampler2DArray;\n\n// \u2500\u2500 Vertex inputs (names matching Babylon attribute binding) \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\nin vec3  position;\nin vec3  normal;\nin vec2  uv;\nin vec4  color;\nin vec4  tangent;\n\n// \u2500\u2500 Outputs to fragment shader \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\nout vec3 vWorldPos;\nout vec3 vNormal;\nout vec2 vUV;\nflat out int   vVertexID;\nflat out uint  vFlagsOut;\nsmooth out vec4 vColor;\ncentroid out vec2 vCentroidUV;\n\n// \u2500\u2500 Constant declarations \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\nconst float EPSILON = 1e-6;\nconst vec3  UP = vec3(0.0, 1.0, 0.0);\nconst mat2  ROT45 = mat2(0.7071, -0.7071, 0.7071, 0.7071);\nconst int   MAX_ITERATIONS = 10;\nconst uint  BITMASK = 0xFFu;\n\n// \u2500\u2500 Array declarations \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\nconst float weights[4] = float[4](0.1, 0.2, 0.3, 0.4);\n\n// \u2500\u2500 Helper function: demonstrates function declarations & return types \u2500\u2500\nfloat saturate(float x) {\n    return clamp(x, 0.0, 1.0);\n}\n\nvec3 safeNormalize(vec3 v) {\n    float len = length(v);\n    return len > EPSILON ? v / len : vec3(0.0);\n}\n\n// \u2500\u2500 Function with out/inout parameters \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\nvoid computeLighting(in vec3 N, in vec3 L, out float diffuse, inout float specular) {\n    diffuse = max(dot(N, L), 0.0);\n    specular += pow(max(dot(reflect(-L, N), vec3(0.0, 0.0, 1.0)), 0.0), 32.0);\n}\n\n// \u2500\u2500 Function testing matrix operations \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\nmat4 buildRotationY(float angle) {\n    float c = cos(angle);\n    float s = sin(angle);\n    return mat4(\n        c,   0.0, s,   0.0,\n        0.0, 1.0, 0.0, 0.0,\n        -s,  0.0, c,   0.0,\n        0.0, 0.0, 0.0, 1.0\n    );\n}\n\n// \u2500\u2500 Function testing all non-rectangular matrix types \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\nvoid testMatrixTypes() {\n    mat2   m2   = mat2(1.0);\n    mat3   m3   = mat3(1.0);\n    mat4   m4   = mat4(1.0);\n    mat2x3 m2x3 = mat2x3(1.0, 0.0, 0.0, 0.0, 1.0, 0.0);\n    mat3x2 m3x2 = mat3x2(1.0, 0.0, 0.0, 1.0, 0.0, 0.0);\n    mat2x4 m2x4 = mat2x4(1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0);\n    mat4x2 m4x2 = mat4x2(1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0);\n    mat3x4 m3x4 = mat3x4(1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);\n    mat4x3 m4x3 = mat4x3(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0);\n\n    // Matrix built-in functions\n    mat2 compMul = matrixCompMult(m2, m2);\n    mat3 outer   = outerProduct(vec3(1.0), vec3(1.0));\n    mat3 trans   = transpose(m3);\n    float det    = determinant(m3);\n    mat3 inv     = inverse(m3);\n    mat4 inv4    = inverse(m4);\n}\n\n// \u2500\u2500 Main vertex shader \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\nvoid main() {\n    // \u2500\u2500 All scalar types \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\n    float f  = 1.0;\n    int   i  = 1;\n    uint  u  = 1u;\n    bool  b  = true;\n\n    // \u2500\u2500 All vector types \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\n    vec2  v2  = vec2(1.0, 2.0);\n    vec3  v3  = vec3(1.0, 2.0, 3.0);\n    vec4  v4  = vec4(1.0, 2.0, 3.0, 4.0);\n    ivec2 iv2 = ivec2(1, 2);\n    ivec3 iv3 = ivec3(1, 2, 3);\n    ivec4 iv4 = ivec4(1, 2, 3, 4);\n    uvec2 uv2 = uvec2(1u, 2u);\n    uvec3 uv3 = uvec3(1u, 2u, 3u);\n    uvec4 uv4 = uvec4(1u, 2u, 3u, 4u);\n    bvec2 bv2 = bvec2(true, false);\n    bvec3 bv3 = bvec3(true, false, true);\n    bvec4 bv4 = bvec4(true, false, true, false);\n\n    // \u2500\u2500 Swizzle operations \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\n    vec3 swizzled = v4.xyz;\n    vec2 swizzled2 = v4.wz;\n    vec4 swizzled3 = v3.xyzx;\n    v4.xy = v2;\n    v4.zw = v2.yx;\n\n    // \u2500\u2500 Type constructors / conversions \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\n    float fromInt  = float(i);\n    int   fromUint = int(u);\n    uint  fromFloat = uint(f);\n    float fromBool = float(b);\n    vec3  fromIvec = vec3(iv3);\n    ivec3 fromVec  = ivec3(v3);\n    uvec3 fromVec2 = uvec3(v3);\n\n    // \u2500\u2500 Arithmetic operators \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\n    float add = f + 1.0;\n    float sub = f - 1.0;\n    float fmul = f * 2.0;\n    float div = f / 2.0;\n    int   imod = i % 3;\n    vec3  vmul = v3 * 2.0;\n    vec3  vdiv = v3 / 2.0;\n    mat4  mmul = uModel * uViewProj;\n\n    // \u2500\u2500 Comparison operators \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\n    bool lt  = f < 2.0;\n    bool gt  = f > 0.0;\n    bool le  = f <= 1.0;\n    bool ge  = f >= 1.0;\n    bool eq  = f == 1.0;\n    bool ne  = f != 0.0;\n\n    // \u2500\u2500 Logical operators \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\n    bool andOp = lt && gt;\n    bool orOp  = lt || gt;\n    bool notOp = !lt;\n    bool xorOp = lt ^^ gt;\n\n    // \u2500\u2500 Ternary operator \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\n    float ternary = b ? 1.0 : 0.0;\n\n    // \u2500\u2500 Assignment operators \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\n    f += 1.0;\n    f -= 0.5;\n    f *= 2.0;\n    f /= 1.5;\n    i %= 5;\n    i <<= 2;\n    i >>= 1;\n    i &= 0xFF;\n    i |= 0x01;\n    i ^= 0x0F;\n\n    // \u2500\u2500 Increment/decrement \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\n    i++;\n    i--;\n    ++i;\n    --i;\n\n    // \u2500\u2500 Bitwise operators (integer only, new in ES 3.0) \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\n    int  band  = i & 0xFF;\n    int  bor   = i | 0x01;\n    int  bxor  = i ^ 0x0F;\n    int  bnot  = ~i;\n    int  lsh   = i << 2;\n    int  rsh   = i >> 1;\n    uint uband = u & 0xFFu;\n    uint ubor  = u | 0x01u;\n    uint ubxor = u ^ 0x0Fu;\n    uint ubnot = ~u;\n    uint ulsh  = u << 2u;\n    uint ursh  = u >> 1u;\n\n    // \u2500\u2500 Integer built-in functions (ES 3.0) \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\n    int   absI  = abs(i);\n    int   signI = sign(i);\n    int   minI  = min(i, 5);\n    int   maxI  = max(i, 0);\n    int   clampI = clamp(i, 0, 10);\n    uint  minU  = min(u, 5u);\n    uint  maxU  = max(u, 0u);\n    uint  clampU = clamp(u, 0u, 10u);\n\n    // \u2500\u2500 Math built-in functions \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\n    float absF   = abs(f);\n    float signF  = sign(f);\n    float floorF = floor(f);\n    float ceilF  = ceil(f);\n    float fractF = fract(f);\n    float modF   = mod(f, 2.0);\n    float minF   = min(f, 2.0);\n    float maxF   = max(f, 0.0);\n    float clampF = clamp(f, 0.0, 1.0);\n    float mixF   = mix(0.0, 1.0, 0.5);\n    float stepF  = step(0.5, f);\n    float ssF    = smoothstep(0.0, 1.0, f);\n    float sqrtF  = sqrt(f);\n    float isqrtF = inversesqrt(max(f, EPSILON));\n    float powF   = pow(f, 2.0);\n    float expF   = exp(f);\n    float exp2F  = exp2(f);\n    float logF   = log(max(f, EPSILON));\n    float log2F  = log2(max(f, EPSILON));\n\n    // \u2500\u2500 Trigonometric functions \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\n    float radians_ = radians(180.0);\n    float degrees_ = degrees(PI);\n    float sinF  = sin(f);\n    float cosF  = cos(f);\n    float tanF  = tan(f);\n    float asinF = asin(clamp(f, -1.0, 1.0));\n    float acosF = acos(clamp(f, -1.0, 1.0));\n    float atanF = atan(f);\n    float atan2F = atan(f, 1.0);\n    float sinhF = sinh(f);\n    float coshF = cosh(f);\n    float tanhF = tanh(f);\n    float asinhF = asinh(f);\n    float acoshF = acosh(max(f, 1.0));\n    float atanhF = atanh(clamp(f, -0.99, 0.99));\n\n    // \u2500\u2500 Geometric functions \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\n    float len     = length(v3);\n    float dist    = distance(v3, vec3(0.0));\n    float dotP    = dot(v3, UP);\n    vec3  crossP  = cross(v3, UP);\n    vec3  normV   = normalize(v3);\n    vec3  reflV   = reflect(v3, UP);\n    vec3  refrV   = refract(v3, UP, 0.5);\n    vec3  fwdV    = faceforward(v3, vec3(0.0, 0.0, -1.0), UP);\n\n    // \u2500\u2500 Vector relational functions \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\n    bvec3 ltV  = lessThan(v3, vec3(1.0));\n    bvec3 leV  = lessThanEqual(v3, vec3(1.0));\n    bvec3 gtV  = greaterThan(v3, vec3(0.0));\n    bvec3 geV  = greaterThanEqual(v3, vec3(0.0));\n    bvec3 eqV  = equal(v3, vec3(1.0));\n    bvec3 neV  = notEqual(v3, vec3(0.0));\n    bool  anyV = any(bv3);\n    bool  allV = all(bv3);\n    bvec3 notV = not(bv3);\n\n    // \u2500\u2500 Packing/unpacking functions (ES 3.0) \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\n    uint  packed1  = packSnorm2x16(v2);\n    vec2  unpack1  = unpackSnorm2x16(packed1);\n    uint  packed2  = packUnorm2x16(v2);\n    vec2  unpack2  = unpackUnorm2x16(packed2);\n    uint  packed3  = packHalf2x16(v2);\n    vec2  unpack3  = unpackHalf2x16(packed3);\n\n    // \u2500\u2500 Texture functions in vertex shader \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\n    ivec2 texSize  = textureSize(uSampler2D, 0);\n    vec4  texLod   = textureLod(uSampler2D, v2, 0.0);\n\n    // \u2500\u2500 isnan / isinf (ES 3.0) \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\n    bool  nanCheck = isnan(f);\n    bool  infCheck = isinf(f);\n\n    // \u2500\u2500 floatBitsToInt / intBitsToFloat / uintBitsToFloat (ES 3.0) \u2500\u2500\n    int   fToI = floatBitsToInt(f);\n    uint  fToU = floatBitsToUint(f);\n    float iToF = intBitsToFloat(fToI);\n    float uToF = uintBitsToFloat(fToU);\n\n    // \u2500\u2500 Array usage \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\n    float dynamicArr[4];\n    for (int idx = 0; idx < 4; idx++) {\n        dynamicArr[idx] = weights[idx] * f;\n    }\n    float arrSum = dynamicArr[0] + dynamicArr[1] + dynamicArr[2] + dynamicArr[3];\n\n    // \u2500\u2500 Struct usage \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\n    LightInfo light;\n    light.position  = vec3(10.0, 10.0, 10.0);\n    light.color     = vec3(1.0);\n    light.intensity = 1.0;\n\n    Material mat;\n    mat.diffuse   = vec4(1.0, 0.0, 0.0, 1.0);\n    mat.specular  = vec4(1.0);\n    mat.shininess = 32.0;\n    mat.mainLight = light;\n\n    // \u2500\u2500 Control flow: if / else if / else \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\n    vec3 result = vec3(0.0);\n    if (f > 0.5) {\n        result = v3;\n    } else if (f > 0.25) {\n        result = v3 * 0.5;\n    } else {\n        result = vec3(0.0);\n    }\n\n    // \u2500\u2500 Control flow: for loop \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\n    vec3 accumulated = vec3(0.0);\n    for (int j = 0; j < MAX_ITERATIONS; j++) {\n        accumulated += v3 * weights[j % 4];\n        if (length(accumulated) > 10.0) break;\n    }\n\n    // \u2500\u2500 Control flow: while loop \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\n    int counter = 0;\n    while (counter < 5) {\n        counter++;\n        if (counter == 3) continue;\n    }\n\n    // \u2500\u2500 Control flow: do-while loop \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\n    int counter2 = 0;\n    do {\n        counter2++;\n    } while (counter2 < 3);\n\n    // \u2500\u2500 Control flow: switch / case (ES 3.0) \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\n    int switchVal = 2;\n    vec3 switchResult = vec3(0.0);\n    switch (switchVal) {\n        case 0:\n            switchResult = vec3(1.0, 0.0, 0.0);\n            break;\n        case 1:\n            switchResult = vec3(0.0, 1.0, 0.0);\n            break;\n        case 2:\n            switchResult = vec3(0.0, 0.0, 1.0);\n            break;\n        default:\n            switchResult = vec3(1.0);\n            break;\n    }\n\n    // \u2500\u2500 Matrix operations \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\n    testMatrixTypes();\n    mat4 rotY = buildRotationY(uTime);\n    vec4 transformed = rotY * vec4(position, 1.0);\n\n    // \u2500\u2500 Function call with out/inout parameters \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\n    float diff = 0.0;\n    float spec = 0.0;\n    computeLighting(normalize(normal), normalize(light.position - position), diff, spec);\n\n    // \u2500\u2500 Comma operator \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\n    float a, b2;\n    a = 1.0;\n    b2 = 2.0;\n\n    // \u2500\u2500 Built-in vertex variables \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\n    vVertexID    = gl_VertexID;\n    vFlagsOut    = uFlags;\n    //vWorldPos    = (uModel * vec4(position, 1.0)).xyz;\n    vNormal      = mat3(uNormalMatrix) * normal;\n    vUV          = uv;\n    vCentroidUV  = uv;\n    vColor       = color;\n\n    gl_Position  = uViewProj * uModel * vec4(position, 1.0);\n    gl_PointSize = 1.0;\n}\n";









































































































































































































































































































































































































































// ---------------------------------------------------------------------------
// Comprehensive WebGL2 GLSL ES 3.00 fragment shader
// ---------------------------------------------------------------------------
var fragmentSource = "\n// \u2500\u2500 Precision qualifiers \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\nprecision highp float;\nprecision highp int;\nprecision highp sampler2D;\nprecision highp sampler3D;\nprecision highp samplerCube;\nprecision highp sampler2DArray;\nprecision highp sampler2DShadow;\nprecision highp samplerCubeShadow;\nprecision highp sampler2DArrayShadow;\nprecision highp isampler2D;\nprecision highp isampler3D;\nprecision highp isamplerCube;\nprecision highp isampler2DArray;\nprecision highp usampler2D;\nprecision highp usampler3D;\nprecision highp usamplerCube;\nprecision highp usampler2DArray;\n\n// \u2500\u2500 Struct (must match vertex) \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\nstruct LightInfo {\n    vec3  position;\n    vec3  color;\n    float intensity;\n};\n\nstruct Material {\n    vec4      diffuse;\n    vec4      specular;\n    float     shininess;\n    LightInfo mainLight;\n};\n\n// \u2500\u2500 Uniform blocks (std140) \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\nlayout(std140) uniform TransformBlock {\n    mat4 uModel;\n    mat4 uViewProj;\n    mat4 uNormalMatrix;\n};\n\nlayout(std140) uniform SceneBlock {\n    vec4  uAmbientColor;\n    float uTime;\n    int   uFrameCount;\n    uint  uFlags;\n};\n\n// \u2500\u2500 All sampler types \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\nuniform sampler2D             uSampler2D;\nuniform sampler3D             uSampler3D;\nuniform samplerCube           uSamplerCube;\nuniform sampler2DArray        uSampler2DArray;\nuniform sampler2DShadow       uSampler2DShadow;\nuniform samplerCubeShadow     uSamplerCubeShadow;\nuniform sampler2DArrayShadow  uSampler2DArrayShadow;\nuniform isampler2D            uISampler2D;\nuniform isampler3D            uISampler3D;\nuniform isamplerCube          uISamplerCube;\nuniform isampler2DArray       uISampler2DArray;\nuniform usampler2D            uUSampler2D;\nuniform usampler3D            uUSampler3D;\nuniform usamplerCube          uUSamplerCube;\nuniform usampler2DArray       uUSampler2DArray;\n\n// \u2500\u2500 Plain uniforms \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\nuniform Material uMaterial;\nuniform float    uCustomFloat;\n\n// \u2500\u2500 Fragment inputs (from vertex shader) \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\nin vec3 vWorldPos;\nin vec3 vNormal;\nin vec2 vUV;\nflat in int   vVertexID;\nflat in uint  vFlagsOut;\nsmooth in vec4 vColor;\ncentroid in vec2 vCentroidUV;\n\n// \u2500\u2500 Fragment output (MRT-capable) \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\nlayout(location = 0) out vec4 fragColor;\n\n// \u2500\u2500 Constants \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\nconst float PI = 3.14159265359;\nconst float EPSILON = 1e-6;\n\n// \u2500\u2500 Helper: Blinn-Phong \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\nvec3 blinnPhong(vec3 N, vec3 L, vec3 V, vec3 lightColor, float shininess) {\n    float NdotL = max(dot(N, L), 0.0);\n    vec3  H     = normalize(L + V);\n    float NdotH = max(dot(N, H), 0.0);\n    float specPower = pow(NdotH, shininess);\n    return lightColor * (NdotL + specPower);\n}\n\n// \u2500\u2500 Helper: Fresnel-Schlick \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\nvec3 fresnelSchlick(float cosTheta, vec3 F0) {\n    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);\n}\n\n// \u2500\u2500 Helper: normal mapping simulation \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\nvec3 perturbNormal(vec3 N, vec3 texNormal) {\n    // unpack from [0,1] to [-1,1]\n    vec3 mapped = texNormal * 2.0 - 1.0;\n    return normalize(N + mapped * 0.5);\n}\n\nvoid main() {\n    // \u2500\u2500 Fragment built-in variables \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\n    vec4  fragCoord    = gl_FragCoord;\n    bool  frontFacing  = gl_FrontFacing;\n    float fragDepth    = gl_FragCoord.z;\n\n    // \u2500\u2500 Derivative functions (fragment only) \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\n    vec2 dxUV = dFdx(vUV);\n    vec2 dyUV = dFdy(vUV);\n    vec2 fwUV = fwidth(vUV);\n    \n    // \u2500\u2500 Texture sampling: texture() \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\n    vec4  tex2d       = texture(uSampler2D, vUV);\n    /*vec4  tex3d       = texture(uSampler3D, vec3(vUV, 0.0));\n    vec4  texCube     = texture(uSamplerCube, vNormal);\n    vec4  texArr      = texture(uSampler2DArray, vec3(vUV, 0.0));\n\n    // \u2500\u2500 Texture sampling: textureLod() \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\n    vec4  tex2dLod    = textureLod(uSampler2D, vUV, 1.0);\n\n    // \u2500\u2500 Texture sampling: textureOffset() \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\n    vec4  tex2dOff    = textureOffset(uSampler2D, vUV, ivec2(1, 0));\n\n    // \u2500\u2500 Texture sampling: textureGrad() \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\n    vec4  tex2dGrad   = textureGrad(uSampler2D, vUV, dxUV, dyUV);\n\n    // \u2500\u2500 Texture sampling: textureProj() \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\n    vec4  tex2dProj   = textureProj(uSampler2D, vec3(vUV, 1.0));\n    vec4  tex2dProj4  = textureProj(uSampler2D, vec4(vUV, 0.0, 1.0));\n\n    // \u2500\u2500 Texture sampling: textureProjLod() \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\n    vec4  tex2dProjL  = textureProjLod(uSampler2D, vec3(vUV, 1.0), 0.0);\n\n    // \u2500\u2500 Texture sampling: texelFetch() \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\n    ivec2 texCoord = ivec2(gl_FragCoord.xy);\n    vec4  fetched  = texelFetch(uSampler2D, texCoord, 0);\n\n    // \u2500\u2500 Texture sampling: textureSize() \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\n    ivec2 size2d    = textureSize(uSampler2D, 0);\n    ivec3 size3d    = textureSize(uSampler3D, 0);\n    ivec2 sizeCube  = textureSize(uSamplerCube, 0);\n    ivec3 sizeArr   = textureSize(uSampler2DArray, 0);\n\n    // \u2500\u2500 Shadow sampler \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\n    float shadow2d    = texture(uSampler2DShadow, vec3(vUV, 0.5));\n    float shadowCube  = texture(uSamplerCubeShadow, vec4(vNormal, 0.5));\n    float shadowArr   = texture(uSampler2DArrayShadow, vec4(vUV, 0.0, 0.5));\n\n    // \u2500\u2500 Integer sampler \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\n    ivec4 iTex2d     = texture(uISampler2D, vUV);\n    ivec4 iTex3d     = texture(uISampler3D, vec3(vUV, 0.0));\n    ivec4 iTexCube   = texture(uISamplerCube, vNormal);\n    ivec4 iTexArr    = texture(uISampler2DArray, vec3(vUV, 0.0));\n    uvec4 uTex2d     = texture(uUSampler2D, vUV);\n    uvec4 uTex3d     = texture(uUSampler3D, vec3(vUV, 0.0));\n    uvec4 uTexCube   = texture(uUSamplerCube, vNormal);\n    uvec4 uTexArr    = texture(uUSampler2DArray, vec3(vUV, 0.0));\n    */\n    // \u2500\u2500 trunc / round (ES 3.0) \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\n    float truncF = trunc(1.7);\n    float roundF = round(1.5);\n    float roundEven_ = roundEven(2.5);\n\n    // \u2500\u2500 modf (ES 3.0) \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\n    float integralPart;\n    float fractionalPart = modf(3.75, integralPart);\n\n    // \u2500\u2500 min/max/clamp on vec types \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\n    vec3 minV = min(vNormal, vec3(0.5));\n    vec3 maxV = max(vNormal, vec3(0.0));\n    vec3 clampV = clamp(vNormal, vec3(0.0), vec3(1.0));\n    \n    // \u2500\u2500 mix with bvec selector \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\n    vec3 mixBvec = mix(vec3(0.0), vec3(1.0), bvec3(true, false, true));\n\n    // \u2500\u2500 Control flow: if / else with discard \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\n    if (tex2d.a < 0.01) {\n        discard;\n    }\n\n    // \u2500\u2500 Control flow: for loop with early exit \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\n    vec3 lightAccum = vec3(0.0);\n    for (int j = 0; j < 4; j++) {\n        LightInfo lt;\n        lt.position  = vec3(float(j) * 3.0, 5.0, 0.0);\n        lt.color     = vec3(1.0, 0.9, 0.8);\n        lt.intensity = 1.0 / (1.0 + float(j));\n\n        vec3 L = normalize(lt.position - vWorldPos);\n        vec3 V = normalize(-vWorldPos);\n        lightAccum += blinnPhong(normalize(vNormal), L, V, lt.color, uMaterial.shininess) * lt.intensity;\n\n        if (length(lightAccum) > 3.0) break;\n    }\n    \n    // \u2500\u2500 Control flow: while \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\n    float decay = 1.0;\n    int wc = 0;\n    while (decay > 0.01 && wc < 10) {\n        decay *= 0.7;\n        wc++;\n    }\n\n    // \u2500\u2500 Control flow: do-while \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\n    float buildUp = 0.0;\n    int dc = 0;\n    do {\n        buildUp += 0.1;\n        dc++;\n    } while (buildUp < 0.5 && dc < 10);\n    \n    // \u2500\u2500 Control flow: switch / case \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\n    vec3 modeColor = vec3(0.0);\n    switch (vVertexID % 4) {\n        case 0: modeColor = vec3(1.0, 0.0, 0.0); break;\n        case 1: modeColor = vec3(0.0, 1.0, 0.0); break;\n        case 2: modeColor = vec3(0.0, 0.0, 1.0); break;\n        default: modeColor = vec3(1.0); break;\n    }\n\n    // \u2500\u2500 Bitwise on uint flags \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\n    bool flag0 = (vFlagsOut & 1u) != 0u;\n    bool flag1 = (vFlagsOut & 2u) != 0u;\n    uint shifted = vFlagsOut << 1u;\n    uint masked  = vFlagsOut & 0xF0u;\n    \n    // \u2500\u2500 Front-facing conditional \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\n    vec3 normal = frontFacing ? normalize(vNormal) : -normalize(vNormal);\n\n    // \u2500\u2500 Fresnel \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\n    vec3 viewDir = normalize(-vWorldPos);\n    vec3 fresnel = fresnelSchlick(max(dot(normal, viewDir), 0.0), vec3(0.04));\n\n    // \u2500\u2500 Normal perturbation \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\n    vec3 perturbedN = perturbNormal(normal, tex2d.rgb);\n    \n    // \u2500\u2500 Final composition \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\n    vec3 albedo   = tex2d.rgb * uMaterial.diffuse.rgb * vColor.rgb;\n    vec3 ambient  = uAmbientColor.rgb * albedo;\n    \n    vec3 lit      = ambient + lightAccum * albedo;\n    vec3 specular = fresnel * uMaterial.specular.rgb;\n    \n    vec4 finalColor = vec4(lit + specular + modeColor * 0.01, tex2d.a * uMaterial.diffuse.a);\n    \n    // \u2500\u2500 Tone mapping (Reinhard) \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\n    finalColor.rgb = finalColor.rgb / (finalColor.rgb + vec3(1.0));\n\n    // \u2500\u2500 Gamma correction \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\n    finalColor.rgb = pow(finalColor.rgb, vec3(1.0 / 2.2));\n\n    // \u2500\u2500 Write fragment depth (ES 3.0) \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\n    gl_FragDepth = fragDepth;\n\n    // \u2500\u2500 Output \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\n    fragColor = finalColor;\n}\n";








































































































































































































































































// ---------------------------------------------------------------------------
// Test runner
// ---------------------------------------------------------------------------
console.log("=== Shader Cross-Compilation Test ===");

var engine = new _babylonjs_core__WEBPACK_IMPORTED_MODULE_0__.NativeEngine();
var scene = new _babylonjs_core__WEBPACK_IMPORTED_MODULE_0__.Scene(engine);
scene.createDefaultCamera();

var sphere = _babylonjs_core__WEBPACK_IMPORTED_MODULE_0__.MeshBuilder.CreateSphere("sphere", { segments: 8, diameter: 1 }, scene);

var failures = 0;
var done = false;

function finish(f) {
  if (!done) {
    done = true;
    engine.stopRenderLoop();
    setShaderTestDone(f);
  }
}

try {
  var shaders = {
    vertexSource: vertexSource,
    fragmentSource: fragmentSource
  };

  var mat = new _babylonjs_core__WEBPACK_IMPORTED_MODULE_0__.ShaderMaterial("crossTest", scene, shaders, {
    attributes: [
    "position", "normal", "uv", "color", "tangent"],

    uniforms: [
    "world", "worldViewProjection", "worldView", "view", "projection",
    "uCustomFloat", "uCustomInt", "uCustomUint", "uCustomBool"],

    samplers: [
    "uSampler2D", "uSampler3D", "uSamplerCube", "uSampler2DArray",
    "uSampler2DShadow", "uSamplerCubeShadow", "uSampler2DArrayShadow",
    "uISampler2D", "uISampler3D", "uISamplerCube", "uISampler2DArray",
    "uUSampler2D", "uUSampler3D", "uUSamplerCube", "uUSampler2DArray"],

    uniformBuffers: [
    "TransformBlock", "SceneBlock"]

  });

  mat.onCompiled = function (effect) {
    console.log("[PASS] Shader compiled successfully.");
    finish(failures);
  };

  mat.onError = function (effect, errors) {
    console.log("[INFO] Shader compilation error (expected for validation testing): " + errors);
    failures++;
    finish(failures);
  };

  sphere.material = mat;
  console.log("[INFO] ShaderMaterial created, starting render loop...");

  // Must start scene rendering so isReady() is called on the material,
  // which triggers effect creation and compilation.
  engine.runRenderLoop(function () {
    scene.render();
  });
} catch (e) {
  console.log("[FAIL] ShaderMaterial creation error: " + e.message);
  failures++;
  finish(failures);
}
})();

/******/ })()
;