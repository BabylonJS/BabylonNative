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

import { NativeEngine } from "@babylonjs/core";
import type { InternalTexture } from "@babylonjs/core";

let engine: NativeEngine | undefined;

function getEngine(): NativeEngine {
    if (!engine) {
        engine = new NativeEngine();
        // This test never renders; don't wait on async shader compilation.
        delete engine.getCaps().parallelShaderCompile;
    }
    return engine;
}

interface WrappedTextureInfo {
    hasBinding: boolean;
    rawLayerCount: number;
    is2DArray: boolean;
    depth: number;
    baseDepth: number;
}

// Wraps the given native texture and returns the layer-related InternalTexture fields, plus the raw
// native layer count so C++ can tell "binding missing" / "BJS too old" from a real failure.
function inspectWrappedTexture(nativeTexture: unknown): WrappedTextureInfo {
    const e = getEngine();
    // getTextureLayerCount is a binding on the raw native engine (_engine), which is private and not
    // yet in the shipped @babylonjs/core types; wrapNativeTexture reads it internally via this._engine.
    const nativeEngine = (e as any)._engine;
    const hasBinding = !!nativeEngine && typeof nativeEngine.getTextureLayerCount === "function";
    const rawLayerCount = hasBinding ? nativeEngine.getTextureLayerCount(nativeTexture) : -1;
    const internalTexture: InternalTexture = e.wrapNativeTexture(nativeTexture);
    return {
        hasBinding,
        rawLayerCount,
        is2DArray: !!internalTexture.is2DArray,
        depth: internalTexture.depth,
        baseDepth: internalTexture.baseDepth,
    };
}

(globalThis as any).inspectWrappedTexture = inspectWrappedTexture;
