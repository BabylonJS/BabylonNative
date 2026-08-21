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
/*!*************************************************!*\
  !*** ./src/tests.externalTexture.layerCount.ts ***!
  \*************************************************/
__webpack_require__.r(__webpack_exports__);
/* harmony import */ var _babylonjs_core__WEBPACK_IMPORTED_MODULE_0__ = __webpack_require__(/*! @babylonjs/core */ "@babylonjs/core");
/* harmony import */ var _babylonjs_core__WEBPACK_IMPORTED_MODULE_0___default = /*#__PURE__*/__webpack_require__.n(_babylonjs_core__WEBPACK_IMPORTED_MODULE_0__);
// JS-side scaffolding for Tests.ExternalTexture.LayerCount.cpp.
//
// Verifies that wrapNativeTexture auto-populates is2DArray/depth/baseDepth on the wrapped
// InternalTexture from the native layer count:
//   - BN exposes getTextureLayerCount(handle) (BabylonJS/BabylonNative#1733), reporting the *bound*
//     layer count: 1 for a single-slice view (layerIndex set), else the underlying array size.
//   - BJS wrapNativeTexture consumes it (BabylonJS/Babylon.js#18535): layers > 1 -> is2DArray=true,
//     baseDepth=depth=layers; layers == 1 -> left as a plain 2D texture.
// C++ wraps whole-array, single-slice, and single-layer native textures and asserts the values
// reported back here.




var engine;

function getEngine() {
  if (!engine) {
    engine = new _babylonjs_core__WEBPACK_IMPORTED_MODULE_0__.NativeEngine();
    // This test never renders; don't wait on async shader compilation.
    delete engine.getCaps().parallelShaderCompile;
  }
  return engine;
}









// Wraps the given native texture and returns the layer-related InternalTexture fields, plus the raw
// native layer count so C++ can tell "binding missing" / "BJS too old" from a real failure.
function inspectWrappedTexture(nativeTexture) {
  var e = getEngine();
  // getTextureLayerCount is a binding on the raw native engine (_engine), which is private and not
  // yet in the shipped @babylonjs/core types; wrapNativeTexture reads it internally via this._engine.
  var nativeEngine = e._engine;
  var hasBinding = !!nativeEngine && typeof nativeEngine.getTextureLayerCount === "function";
  var rawLayerCount = hasBinding ? nativeEngine.getTextureLayerCount(nativeTexture) : -1;
  var internalTexture = e.wrapNativeTexture(nativeTexture);
  return {
    hasBinding: hasBinding,
    rawLayerCount: rawLayerCount,
    is2DArray: !!internalTexture.is2DArray,
    depth: internalTexture.depth,
    baseDepth: internalTexture.baseDepth
  };
}

globalThis.inspectWrappedTexture = inspectWrappedTexture;
})();

/******/ })()
;