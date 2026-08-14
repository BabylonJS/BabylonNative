# NativeMeshopt

> ⚠️ **This plugin is experimental and subject to change.**

The NativeMeshopt plugin provides native [meshoptimizer](https://github.com/zeux/meshoptimizer) vertex and index buffer decompression to Babylon, so `EXT_meshopt_compression` glTF assets can be decoded without shipping and instantiating the meshoptimizer WebAssembly module.

The plugin is **off by default**. Enable it with `-D BABYLON_NATIVE_PLUGIN_NATIVEMESHOPT=ON`.

## Limitations

- **Decode only.** Encoding is an authoring-time concern that Babylon Native does not exercise.
- **No consumer yet.** Nothing in the pinned `babylonjs` package calls `_native.MeshoptCodec`. The grouping and the entry-point names have therefore not faced a real consumer and may still move.

## Design

The API is exposed as a single `MeshoptCodec` object on the `_native` global, for the same reasons as `DracoCodec`. Publishing the version matters more here: meshoptimizer stores its codec version in the first header byte, and a decoder **rejects** streams newer than it understands — returning an error rather than degraded output. Exposing the version lets the JavaScript side fall back before it tries.

```typescript
interface INative {
  MeshoptCodec: {
    Decode: (
      source: ArrayBufferView,
      count: number,
      stride: number,
      mode: "ATTRIBUTES" | "TRIANGLES" | "INDICES",
      filter?: "NONE" | "OCTAHEDRAL" | "QUATERNION" | "EXPONENTIAL"
    ) => Uint8Array;
    Version: string;
  };
}
```

`Decode` mirrors the reference `meshopt_decoder.js` `decode()` helper exactly: it decodes into a buffer sized for `count` rounded up to a multiple of 4, applies the filter in place over that rounded count, then returns the first `count * stride` bytes.

## Notes

meshoptimizer validates `count` and `stride` with `assert()`, which compiles out in release builds — out-of-range values would be undefined behavior rather than a thrown error. The plugin therefore range-checks both before calling into the library:

- `count` must not be negative.
- `stride` must be in `[1, 256]`.
- `ATTRIBUTES` requires a stride that is a multiple of 4.
- `TRIANGLES` and `INDICES` require a stride of exactly 2 or 4.
