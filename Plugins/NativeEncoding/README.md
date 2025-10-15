# NativeEncoding

> ⚠️ **This plugin is experimental and subject to change.**

The NativeEncoding plugin provides native image encoding capabilities to Babylon, allowing raw pixel data to be encoded into standard image formats (PNG, JPEG, WebP, etc.).

## Design 

Unlike a traditional polyfill which would implement Canvas's `toBlob()` or `toDataURL()` methods, NativeEncoding exists as a plugin because:
1. **No standard Web API exists** for general-purpose image encoding separate from Canvas
2. **Simplicity** - Exposes only what Babylon actually needs: direct pixel-to-bytes encoding
3. **Efficiency** - Avoids extra routing through the Canvas API via intermediate data structures
4. **Modularity** - Image encoding is a separate concern from 2D canvas rendering
5. **Extensibility** - New codecs can be added in the future without bloating other components

The encoding functions are exposed on the `_native` global object, similar to NativeOptimizations. They're designed to be wrapped by higher-level Babylon.js APIs (e.g., DumpTools) for common workflows like asset exports and screenshots.

## Limitations

Currently, **only PNG encoding** is supported.
