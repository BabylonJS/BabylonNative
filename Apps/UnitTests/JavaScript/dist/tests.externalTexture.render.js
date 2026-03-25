/******/ (() => { // webpackBootstrap
/******/ 	var __webpack_modules__ = ({

/***/ "@babylonjs/core"
/*!**************************!*\
  !*** external "BABYLON" ***!
  \**************************/
(module) {

"use strict";
module.exports = BABYLON;

/***/ },

/***/ "../../node_modules/@babel/runtime/helpers/OverloadYield.js"
/*!******************************************************************!*\
  !*** ../../node_modules/@babel/runtime/helpers/OverloadYield.js ***!
  \******************************************************************/
(module) {

function _OverloadYield(e, d) {
  this.v = e, this.k = d;
}
module.exports = _OverloadYield, module.exports.__esModule = true, module.exports["default"] = module.exports;

/***/ },

/***/ "../../node_modules/@babel/runtime/helpers/regenerator.js"
/*!****************************************************************!*\
  !*** ../../node_modules/@babel/runtime/helpers/regenerator.js ***!
  \****************************************************************/
(module, __unused_webpack_exports, __webpack_require__) {

var regeneratorDefine = __webpack_require__(/*! ./regeneratorDefine.js */ "../../node_modules/@babel/runtime/helpers/regeneratorDefine.js");
function _regenerator() {
  /*! regenerator-runtime -- Copyright (c) 2014-present, Facebook, Inc. -- license (MIT): https://github.com/babel/babel/blob/main/packages/babel-helpers/LICENSE */
  var e,
    t,
    r = "function" == typeof Symbol ? Symbol : {},
    n = r.iterator || "@@iterator",
    o = r.toStringTag || "@@toStringTag";
  function i(r, n, o, i) {
    var c = n && n.prototype instanceof Generator ? n : Generator,
      u = Object.create(c.prototype);
    return regeneratorDefine(u, "_invoke", function (r, n, o) {
      var i,
        c,
        u,
        f = 0,
        p = o || [],
        y = !1,
        G = {
          p: 0,
          n: 0,
          v: e,
          a: d,
          f: d.bind(e, 4),
          d: function d(t, r) {
            return i = t, c = 0, u = e, G.n = r, a;
          }
        };
      function d(r, n) {
        for (c = r, u = n, t = 0; !y && f && !o && t < p.length; t++) {
          var o,
            i = p[t],
            d = G.p,
            l = i[2];
          r > 3 ? (o = l === n) && (u = i[(c = i[4]) ? 5 : (c = 3, 3)], i[4] = i[5] = e) : i[0] <= d && ((o = r < 2 && d < i[1]) ? (c = 0, G.v = n, G.n = i[1]) : d < l && (o = r < 3 || i[0] > n || n > l) && (i[4] = r, i[5] = n, G.n = l, c = 0));
        }
        if (o || r > 1) return a;
        throw y = !0, n;
      }
      return function (o, p, l) {
        if (f > 1) throw TypeError("Generator is already running");
        for (y && 1 === p && d(p, l), c = p, u = l; (t = c < 2 ? e : u) || !y;) {
          i || (c ? c < 3 ? (c > 1 && (G.n = -1), d(c, u)) : G.n = u : G.v = u);
          try {
            if (f = 2, i) {
              if (c || (o = "next"), t = i[o]) {
                if (!(t = t.call(i, u))) throw TypeError("iterator result is not an object");
                if (!t.done) return t;
                u = t.value, c < 2 && (c = 0);
              } else 1 === c && (t = i["return"]) && t.call(i), c < 2 && (u = TypeError("The iterator does not provide a '" + o + "' method"), c = 1);
              i = e;
            } else if ((t = (y = G.n < 0) ? u : r.call(n, G)) !== a) break;
          } catch (t) {
            i = e, c = 1, u = t;
          } finally {
            f = 1;
          }
        }
        return {
          value: t,
          done: y
        };
      };
    }(r, o, i), !0), u;
  }
  var a = {};
  function Generator() {}
  function GeneratorFunction() {}
  function GeneratorFunctionPrototype() {}
  t = Object.getPrototypeOf;
  var c = [][n] ? t(t([][n]())) : (regeneratorDefine(t = {}, n, function () {
      return this;
    }), t),
    u = GeneratorFunctionPrototype.prototype = Generator.prototype = Object.create(c);
  function f(e) {
    return Object.setPrototypeOf ? Object.setPrototypeOf(e, GeneratorFunctionPrototype) : (e.__proto__ = GeneratorFunctionPrototype, regeneratorDefine(e, o, "GeneratorFunction")), e.prototype = Object.create(u), e;
  }
  return GeneratorFunction.prototype = GeneratorFunctionPrototype, regeneratorDefine(u, "constructor", GeneratorFunctionPrototype), regeneratorDefine(GeneratorFunctionPrototype, "constructor", GeneratorFunction), GeneratorFunction.displayName = "GeneratorFunction", regeneratorDefine(GeneratorFunctionPrototype, o, "GeneratorFunction"), regeneratorDefine(u), regeneratorDefine(u, o, "Generator"), regeneratorDefine(u, n, function () {
    return this;
  }), regeneratorDefine(u, "toString", function () {
    return "[object Generator]";
  }), (module.exports = _regenerator = function _regenerator() {
    return {
      w: i,
      m: f
    };
  }, module.exports.__esModule = true, module.exports["default"] = module.exports)();
}
module.exports = _regenerator, module.exports.__esModule = true, module.exports["default"] = module.exports;

/***/ },

/***/ "../../node_modules/@babel/runtime/helpers/regeneratorAsync.js"
/*!*********************************************************************!*\
  !*** ../../node_modules/@babel/runtime/helpers/regeneratorAsync.js ***!
  \*********************************************************************/
(module, __unused_webpack_exports, __webpack_require__) {

var regeneratorAsyncGen = __webpack_require__(/*! ./regeneratorAsyncGen.js */ "../../node_modules/@babel/runtime/helpers/regeneratorAsyncGen.js");
function _regeneratorAsync(n, e, r, t, o) {
  var a = regeneratorAsyncGen(n, e, r, t, o);
  return a.next().then(function (n) {
    return n.done ? n.value : a.next();
  });
}
module.exports = _regeneratorAsync, module.exports.__esModule = true, module.exports["default"] = module.exports;

/***/ },

/***/ "../../node_modules/@babel/runtime/helpers/regeneratorAsyncGen.js"
/*!************************************************************************!*\
  !*** ../../node_modules/@babel/runtime/helpers/regeneratorAsyncGen.js ***!
  \************************************************************************/
(module, __unused_webpack_exports, __webpack_require__) {

var regenerator = __webpack_require__(/*! ./regenerator.js */ "../../node_modules/@babel/runtime/helpers/regenerator.js");
var regeneratorAsyncIterator = __webpack_require__(/*! ./regeneratorAsyncIterator.js */ "../../node_modules/@babel/runtime/helpers/regeneratorAsyncIterator.js");
function _regeneratorAsyncGen(r, e, t, o, n) {
  return new regeneratorAsyncIterator(regenerator().w(r, e, t, o), n || Promise);
}
module.exports = _regeneratorAsyncGen, module.exports.__esModule = true, module.exports["default"] = module.exports;

/***/ },

/***/ "../../node_modules/@babel/runtime/helpers/regeneratorAsyncIterator.js"
/*!*****************************************************************************!*\
  !*** ../../node_modules/@babel/runtime/helpers/regeneratorAsyncIterator.js ***!
  \*****************************************************************************/
(module, __unused_webpack_exports, __webpack_require__) {

var OverloadYield = __webpack_require__(/*! ./OverloadYield.js */ "../../node_modules/@babel/runtime/helpers/OverloadYield.js");
var regeneratorDefine = __webpack_require__(/*! ./regeneratorDefine.js */ "../../node_modules/@babel/runtime/helpers/regeneratorDefine.js");
function AsyncIterator(t, e) {
  function n(r, o, i, f) {
    try {
      var c = t[r](o),
        u = c.value;
      return u instanceof OverloadYield ? e.resolve(u.v).then(function (t) {
        n("next", t, i, f);
      }, function (t) {
        n("throw", t, i, f);
      }) : e.resolve(u).then(function (t) {
        c.value = t, i(c);
      }, function (t) {
        return n("throw", t, i, f);
      });
    } catch (t) {
      f(t);
    }
  }
  var r;
  this.next || (regeneratorDefine(AsyncIterator.prototype), regeneratorDefine(AsyncIterator.prototype, "function" == typeof Symbol && Symbol.asyncIterator || "@asyncIterator", function () {
    return this;
  })), regeneratorDefine(this, "_invoke", function (t, o, i) {
    function f() {
      return new e(function (e, r) {
        n(t, i, e, r);
      });
    }
    return r = r ? r.then(f, f) : f();
  }, !0);
}
module.exports = AsyncIterator, module.exports.__esModule = true, module.exports["default"] = module.exports;

/***/ },

/***/ "../../node_modules/@babel/runtime/helpers/regeneratorDefine.js"
/*!**********************************************************************!*\
  !*** ../../node_modules/@babel/runtime/helpers/regeneratorDefine.js ***!
  \**********************************************************************/
(module) {

function _regeneratorDefine(e, r, n, t) {
  var i = Object.defineProperty;
  try {
    i({}, "", {});
  } catch (e) {
    i = 0;
  }
  module.exports = _regeneratorDefine = function regeneratorDefine(e, r, n, t) {
    function o(r, n) {
      _regeneratorDefine(e, r, function (e) {
        return this._invoke(r, n, e);
      });
    }
    r ? i ? i(e, r, {
      value: n,
      enumerable: !t,
      configurable: !t,
      writable: !t
    }) : e[r] = n : (o("next", 0), o("throw", 1), o("return", 2));
  }, module.exports.__esModule = true, module.exports["default"] = module.exports, _regeneratorDefine(e, r, n, t);
}
module.exports = _regeneratorDefine, module.exports.__esModule = true, module.exports["default"] = module.exports;

/***/ },

/***/ "../../node_modules/@babel/runtime/helpers/regeneratorKeys.js"
/*!********************************************************************!*\
  !*** ../../node_modules/@babel/runtime/helpers/regeneratorKeys.js ***!
  \********************************************************************/
(module) {

function _regeneratorKeys(e) {
  var n = Object(e),
    r = [];
  for (var t in n) r.unshift(t);
  return function e() {
    for (; r.length;) if ((t = r.pop()) in n) return e.value = t, e.done = !1, e;
    return e.done = !0, e;
  };
}
module.exports = _regeneratorKeys, module.exports.__esModule = true, module.exports["default"] = module.exports;

/***/ },

/***/ "../../node_modules/@babel/runtime/helpers/regeneratorRuntime.js"
/*!***********************************************************************!*\
  !*** ../../node_modules/@babel/runtime/helpers/regeneratorRuntime.js ***!
  \***********************************************************************/
(module, __unused_webpack_exports, __webpack_require__) {

var OverloadYield = __webpack_require__(/*! ./OverloadYield.js */ "../../node_modules/@babel/runtime/helpers/OverloadYield.js");
var regenerator = __webpack_require__(/*! ./regenerator.js */ "../../node_modules/@babel/runtime/helpers/regenerator.js");
var regeneratorAsync = __webpack_require__(/*! ./regeneratorAsync.js */ "../../node_modules/@babel/runtime/helpers/regeneratorAsync.js");
var regeneratorAsyncGen = __webpack_require__(/*! ./regeneratorAsyncGen.js */ "../../node_modules/@babel/runtime/helpers/regeneratorAsyncGen.js");
var regeneratorAsyncIterator = __webpack_require__(/*! ./regeneratorAsyncIterator.js */ "../../node_modules/@babel/runtime/helpers/regeneratorAsyncIterator.js");
var regeneratorKeys = __webpack_require__(/*! ./regeneratorKeys.js */ "../../node_modules/@babel/runtime/helpers/regeneratorKeys.js");
var regeneratorValues = __webpack_require__(/*! ./regeneratorValues.js */ "../../node_modules/@babel/runtime/helpers/regeneratorValues.js");
function _regeneratorRuntime() {
  "use strict";

  var r = regenerator(),
    e = r.m(_regeneratorRuntime),
    t = (Object.getPrototypeOf ? Object.getPrototypeOf(e) : e.__proto__).constructor;
  function n(r) {
    var e = "function" == typeof r && r.constructor;
    return !!e && (e === t || "GeneratorFunction" === (e.displayName || e.name));
  }
  var o = {
    "throw": 1,
    "return": 2,
    "break": 3,
    "continue": 3
  };
  function a(r) {
    var e, t;
    return function (n) {
      e || (e = {
        stop: function stop() {
          return t(n.a, 2);
        },
        "catch": function _catch() {
          return n.v;
        },
        abrupt: function abrupt(r, e) {
          return t(n.a, o[r], e);
        },
        delegateYield: function delegateYield(r, o, a) {
          return e.resultName = o, t(n.d, regeneratorValues(r), a);
        },
        finish: function finish(r) {
          return t(n.f, r);
        }
      }, t = function t(r, _t, o) {
        n.p = e.prev, n.n = e.next;
        try {
          return r(_t, o);
        } finally {
          e.next = n.n;
        }
      }), e.resultName && (e[e.resultName] = n.v, e.resultName = void 0), e.sent = n.v, e.next = n.n;
      try {
        return r.call(this, e);
      } finally {
        n.p = e.prev, n.n = e.next;
      }
    };
  }
  return (module.exports = _regeneratorRuntime = function _regeneratorRuntime() {
    return {
      wrap: function wrap(e, t, n, o) {
        return r.w(a(e), t, n, o && o.reverse());
      },
      isGeneratorFunction: n,
      mark: r.m,
      awrap: function awrap(r, e) {
        return new OverloadYield(r, e);
      },
      AsyncIterator: regeneratorAsyncIterator,
      async: function async(r, e, t, o, u) {
        return (n(e) ? regeneratorAsyncGen : regeneratorAsync)(a(r), e, t, o, u);
      },
      keys: regeneratorKeys,
      values: regeneratorValues
    };
  }, module.exports.__esModule = true, module.exports["default"] = module.exports)();
}
module.exports = _regeneratorRuntime, module.exports.__esModule = true, module.exports["default"] = module.exports;

/***/ },

/***/ "../../node_modules/@babel/runtime/helpers/regeneratorValues.js"
/*!**********************************************************************!*\
  !*** ../../node_modules/@babel/runtime/helpers/regeneratorValues.js ***!
  \**********************************************************************/
(module, __unused_webpack_exports, __webpack_require__) {

var _typeof = (__webpack_require__(/*! ./typeof.js */ "../../node_modules/@babel/runtime/helpers/typeof.js")["default"]);
function _regeneratorValues(e) {
  if (null != e) {
    var t = e["function" == typeof Symbol && Symbol.iterator || "@@iterator"],
      r = 0;
    if (t) return t.call(e);
    if ("function" == typeof e.next) return e;
    if (!isNaN(e.length)) return {
      next: function next() {
        return e && r >= e.length && (e = void 0), {
          value: e && e[r++],
          done: !e
        };
      }
    };
  }
  throw new TypeError(_typeof(e) + " is not iterable");
}
module.exports = _regeneratorValues, module.exports.__esModule = true, module.exports["default"] = module.exports;

/***/ },

/***/ "../../node_modules/@babel/runtime/helpers/typeof.js"
/*!***********************************************************!*\
  !*** ../../node_modules/@babel/runtime/helpers/typeof.js ***!
  \***********************************************************/
(module) {

function _typeof(o) {
  "@babel/helpers - typeof";

  return module.exports = _typeof = "function" == typeof Symbol && "symbol" == typeof Symbol.iterator ? function (o) {
    return typeof o;
  } : function (o) {
    return o && "function" == typeof Symbol && o.constructor === Symbol && o !== Symbol.prototype ? "symbol" : typeof o;
  }, module.exports.__esModule = true, module.exports["default"] = module.exports, _typeof(o);
}
module.exports = _typeof, module.exports.__esModule = true, module.exports["default"] = module.exports;

/***/ },

/***/ "../../node_modules/@babel/runtime/regenerator/index.js"
/*!**************************************************************!*\
  !*** ../../node_modules/@babel/runtime/regenerator/index.js ***!
  \**************************************************************/
(module, __unused_webpack_exports, __webpack_require__) {

// TODO(Babel 8): Remove this file.

var runtime = __webpack_require__(/*! ../helpers/regeneratorRuntime */ "../../node_modules/@babel/runtime/helpers/regeneratorRuntime.js")();
module.exports = runtime;

// Copied from https://github.com/facebook/regenerator/blob/main/packages/runtime/runtime.js#L736=
try {
  regeneratorRuntime = runtime;
} catch (accidentalStrictMode) {
  if (typeof globalThis === "object") {
    globalThis.regeneratorRuntime = runtime;
  } else {
    Function("r", "regeneratorRuntime = r")(runtime);
  }
}


/***/ },

/***/ "../../node_modules/@babel/runtime/helpers/esm/asyncToGenerator.js"
/*!*************************************************************************!*\
  !*** ../../node_modules/@babel/runtime/helpers/esm/asyncToGenerator.js ***!
  \*************************************************************************/
(__unused_webpack___webpack_module__, __webpack_exports__, __webpack_require__) {

"use strict";
__webpack_require__.r(__webpack_exports__);
/* harmony export */ __webpack_require__.d(__webpack_exports__, {
/* harmony export */   "default": () => (/* binding */ _asyncToGenerator)
/* harmony export */ });
function asyncGeneratorStep(n, t, e, r, o, a, c) {
  try {
    var i = n[a](c),
      u = i.value;
  } catch (n) {
    return void e(n);
  }
  i.done ? t(u) : Promise.resolve(u).then(r, o);
}
function _asyncToGenerator(n) {
  return function () {
    var t = this,
      e = arguments;
    return new Promise(function (r, o) {
      var a = n.apply(t, e);
      function _next(n) {
        asyncGeneratorStep(a, r, o, _next, _throw, "next", n);
      }
      function _throw(n) {
        asyncGeneratorStep(a, r, o, _next, _throw, "throw", n);
      }
      _next(void 0);
    });
  };
}


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
// This entry needs to be wrapped in an IIFE because it needs to be in strict mode.
(() => {
"use strict";
/*!*********************************************!*\
  !*** ./src/tests.externalTexture.render.ts ***!
  \*********************************************/
__webpack_require__.r(__webpack_exports__);
/* harmony import */ var _babel_runtime_helpers_asyncToGenerator__WEBPACK_IMPORTED_MODULE_0__ = __webpack_require__(/*! @babel/runtime/helpers/asyncToGenerator */ "../../node_modules/@babel/runtime/helpers/esm/asyncToGenerator.js");
/* harmony import */ var _babel_runtime_regenerator__WEBPACK_IMPORTED_MODULE_1__ = __webpack_require__(/*! @babel/runtime/regenerator */ "../../node_modules/@babel/runtime/regenerator/index.js");
/* harmony import */ var _babel_runtime_regenerator__WEBPACK_IMPORTED_MODULE_1___default = /*#__PURE__*/__webpack_require__.n(_babel_runtime_regenerator__WEBPACK_IMPORTED_MODULE_1__);
/* harmony import */ var _babylonjs_core__WEBPACK_IMPORTED_MODULE_2__ = __webpack_require__(/*! @babylonjs/core */ "@babylonjs/core");
/* harmony import */ var _babylonjs_core__WEBPACK_IMPORTED_MODULE_2___default = /*#__PURE__*/__webpack_require__.n(_babylonjs_core__WEBPACK_IMPORTED_MODULE_2__);


var vertexShader = "\n    precision highp float;\n    attribute vec3 position;\n    attribute vec2 uv;\n    uniform mat4 worldViewProjection;\n    varying vec2 vUV;\n    void main(void) {\n        gl_Position = worldViewProjection * vec4(position, 1.0);\n        vUV = uv;\n    }\n";











var fragmentShader = "\n    precision highp float;\n    precision highp sampler2DArray;\n    uniform sampler2DArray textureArraySampler;\n    uniform float sliceIndex;\n    varying vec2 vUV;\n    void main(void) {\n        gl_FragColor = texture(textureArraySampler, vec3(vUV, sliceIndex));\n    }\n";










var engine;
var scene;
var material;

function startup(
inputNativeTexture,
outputNativeTexture,
width,
height)
{
  engine = new _babylonjs_core__WEBPACK_IMPORTED_MODULE_2__.NativeEngine();
  delete engine.getCaps().parallelShaderCompile;
  scene = new _babylonjs_core__WEBPACK_IMPORTED_MODULE_2__.Scene(engine);

  // Wrap the output texture as a render target.
  var outputTexture = new _babylonjs_core__WEBPACK_IMPORTED_MODULE_2__.RenderTargetTexture(
    "output",
    { width: width, height: height },
    scene,
    {
      colorAttachment: engine.wrapNativeTexture(outputNativeTexture),
      generateDepthBuffer: true,
      generateStencilBuffer: false
    }
  );

  // Orthographic camera filling the viewport exactly.
  var camera = new _babylonjs_core__WEBPACK_IMPORTED_MODULE_2__.FreeCamera("camera", new _babylonjs_core__WEBPACK_IMPORTED_MODULE_2__.Vector3(0, 0, -1), scene);
  camera.setTarget(_babylonjs_core__WEBPACK_IMPORTED_MODULE_2__.Vector3.Zero());
  camera.mode = _babylonjs_core__WEBPACK_IMPORTED_MODULE_2__.Camera.ORTHOGRAPHIC_CAMERA;
  camera.orthoTop = 1;
  camera.orthoBottom = -1;
  camera.orthoLeft = -1;
  camera.orthoRight = 1;
  camera.outputRenderTarget = outputTexture;

  // Fullscreen quad.
  var quad = _babylonjs_core__WEBPACK_IMPORTED_MODULE_2__.MeshBuilder.CreatePlane("quad", { size: 2 }, scene);

  // Shader material that samples from a texture array.
  material = new _babylonjs_core__WEBPACK_IMPORTED_MODULE_2__.ShaderMaterial(
    "textureArrayShader",
    scene,
    { vertexSource: vertexShader, fragmentSource: fragmentShader },
    {
      attributes: ["position", "uv"],
      uniforms: ["worldViewProjection", "sliceIndex"],
      samplers: ["textureArraySampler"]
    }
  );

  material.onError = function (_effect, errors) {
    console.error("ShaderMaterial compilation error: " + errors);
  };

  material.backFaceCulling = false;
  material.depthFunction = _babylonjs_core__WEBPACK_IMPORTED_MODULE_2__.Constants.ALWAYS;

  // Wrap the input texture array and bind it to the shader.
  var internalTex = engine.wrapNativeTexture(inputNativeTexture);
  var inputTexWrapper = new _babylonjs_core__WEBPACK_IMPORTED_MODULE_2__.Texture(null, scene);
  inputTexWrapper._texture = internalTex;
  material.setTexture("textureArraySampler", inputTexWrapper);
  material.setFloat("sliceIndex", 0);

  quad.material = material;
}function

renderSlice(_x) {return _renderSlice.apply(this, arguments);}function _renderSlice() {_renderSlice = (0,_babel_runtime_helpers_asyncToGenerator__WEBPACK_IMPORTED_MODULE_0__["default"])(/*#__PURE__*/_babel_runtime_regenerator__WEBPACK_IMPORTED_MODULE_1___default().mark(function _callee(sliceIndex) {return _babel_runtime_regenerator__WEBPACK_IMPORTED_MODULE_1___default().wrap(function (_context) {while (1) switch (_context.prev = _context.next) {case 0:
          material.setFloat("sliceIndex", sliceIndex);_context.next = 1;return (
            scene.whenReadyAsync());case 1:
          scene.render();case 2:case "end":return _context.stop();}}, _callee);}));return _renderSlice.apply(this, arguments);}


globalThis.startup = startup;
globalThis.renderSlice = renderSlice;
})();

/******/ })()
;