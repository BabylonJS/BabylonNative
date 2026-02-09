# CanvasWgpu (femtovg-backed) capability notes

This directory tracks the wgpu-backed Canvas 2D migration using `femtovg`.

Goal: preserve or expand the currently exposed Canvas API behavior, and only
mark APIs as unsupported when `femtovg` is genuinely missing the capability.

## Current policy

- Do not preserve historical NanoVG/bgfx limitations if `femtovg` supports the
  feature.
- Keep parity with existing JS API surface (`Canvas`, `Context`, `Image`,
  `Path2D`, `CanvasGradient`, `ImageData`) while mapping to `femtovg`/wgpu.
- Clearly comment known non-CTS areas at call sites.

## femtovg vs current inline NanoVG fork

From upstream `femtovg` docs and source:

- `femtovg` supports: path fill/stroke, line caps/joins/miter, global alpha,
  composite operations, scissor/intersect scissor, transforms, linear/radial/box
  gradients, text fill/stroke, shaping, letter spacing, and a wgpu renderer.
- `femtovg` does **not** support: stroke dashing, path scissoring, custom shaders,
  and 3D transforms.

Compared to the current Canvas implementation in `Polyfills/Canvas`:

- `setLineDash` is currently not implemented already.
- Shadow accessors are currently not implemented already.
- `putImageData` is currently not implemented already.
- `getImageData().data` currently returns zeroed data (stubbed).

Potential gaps to handle explicitly during port:

- Current NanoVG fork has custom `nvgRoundedRectElliptic`; `femtovg` has
  rounded rect + varying corner radii but no direct elliptical-per-corner API.
  If needed, emulate via explicit path commands.
- Current NanoVG fork has custom CSS-like `filter` parsing (currently blur-only).
  `femtovg` has image filtering primitives, but no direct CSS filter parser.

## Current backend status (this branch)

- `Image.src` decode is implemented without `bimg/bgfx` by routing encoded bytes
  through the Rust backend (`image` crate) into RGBA pixels.
- `drawImage(...)` paths use decoded RGBA via `nvgCreateImageRGBA`.
- `context.filter = "blur(...)"` is parsed and executed in backend draw paths
  using multi-pass weighted offsets (approximate gaussian blur). This remains
  explicit non-CTS behavior until BabylonJS CTS coverage is fully integrated.
- `getCanvasTexture()` returns an opaque native interop handle (texture/device/
  queue + dimensions) used by NativeWebGPU debug texture import.
- The legacy `Graphics::Texture*` pointer contract is still not exposed by
  `CanvasWgpu`; integration currently uses the WebGPU-native debug bridge.

## Implementation intent

CanvasWgpu should expose all features that `femtovg` can represent directly.
Only genuine `femtovg` limitations should remain as explicit not-implemented
paths.
