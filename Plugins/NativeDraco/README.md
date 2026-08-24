# NativeDraco

> ⚠️ **This plugin is experimental and subject to change.**

The NativeDraco plugin provides native [Draco](https://github.com/google/draco) geometry decompression to Babylon, so `KHR_draco_mesh_compression` glTF assets can be decoded without shipping and instantiating the Draco WebAssembly module.

The plugin is **off by default**. Enable it with `-D BABYLON_NATIVE_PLUGIN_NATIVEDRACO=ON`.

## Limitations

- **Decode only.** There is no encoder. This mirrors Babylon.js, whose default configuration loads `draco_decoder_gltf.wasm`; its encoder lives in a separate module (`draco_encoder.wasm`) that `DracoEncoder` fetches on demand. Linking Draco's encoder here would add roughly 1.2 MB to every binary to serve an authoring path Babylon Native does not exercise.
- **glTF bitstream only.** Draco is built with `DRACO_GLTF_BITSTREAM=ON`, matching the `draco3dgltf` package Babylon.js decodes with. That enables mesh compression, normal encoding and the standard edgebreaker, and excludes point clouds, the predictive edgebreaker, backwards compatibility with pre-1.0 streams, and attribute deduplication. A point-cloud stream decodes to an error rather than geometry.
- **No consumer yet.** Nothing in the pinned `babylonjs` package calls `_native.DracoCodec`. The grouping and the entry-point names have therefore not faced a real consumer and may still move.

## Design

The API is exposed as a single `DracoCodec` object on the `_native` global rather than as free functions, so that:

1. **One feature probe.** JavaScript checks for the object once instead of once per entry point.
2. **The object can carry a version.** Draco's bitstream is versioned; a caller holding a stream this build is too old to read otherwise has no way to find out ahead of time. A bare function name cannot express that.

```typescript
interface INative {
  DracoCodec: {
    Decode: (
      data: ArrayBufferView,
      attributeIds?: { [kind: string]: number }
    ) => {
      indices: Uint32Array | null;
      attributes: Array<{
        kind: string;
        data: Float32Array | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array;
        size: number;
        byteOffset: number;
        byteStride: number;
        normalized: boolean;
      }>;
      totalVertices: number;
    };
    Version: string;
  };
}
```

`attributeIds` is the glTF path: a map of Babylon vertex-buffer kind to Draco unique attribute id, taken from the `KHR_draco_mesh_compression` extension. When it is omitted, attributes are identified by their Draco geometry type instead.

`indices` is `null` for geometry that decodes to a point cloud rather than a triangular mesh.

## Notes

`Decode` reads the keys of `attributeIds` via `Object.keys` obtained from the global object, rather than `Napi::Object::GetPropertyNames()`. `GetPropertyNames()` is unusable on JavaScriptCore — the default engine on macOS and iOS — and the enumeration semantics differ across the non-V8 backends generally. See the comment in `Source/NativeDraco.cpp` and [JsRuntimeHost#216](https://github.com/BabylonJS/JsRuntimeHost/issues/216).
