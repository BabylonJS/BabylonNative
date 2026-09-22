# Canvas
Implements parts of the 2D Canvas API using bgfx. Still a very early WIP; many methods are not yet implemented.

## Readback and text metrics

`getImageData()` and `toDataURL()` flush pending draws and read the rendered
framebuffer, including transforms, clips, alpha, filters, and GPU sampling.
An untouched Canvas with a 2D context lazily creates a transparent render target
on its first readback; no resize or drawing call is required.
`getImageData()` returns straight-alpha RGBA8 and transparent black outside the
canvas; it does not use a CPU drawing mirror. `drawImage(canvas, ...)` uses the
same rendered source snapshot.

`toDataURL()` always supports PNG encoding, independently of
`BABYLON_NATIVE_PLUGIN_NATIVEENGINE_LOAD_IMAGES`, which controls input-image
loading. Empty, case-variant, and unsupported media types use the same PNG
fallback; JPEG/WebP encoding is not implemented.
The data URL contains only the written PNG bytes through `IEND`, not unused
encoding-buffer capacity.

`drawImage()` validates its arity and normalizes/clips its geometry before
readback or image upload. Calls that draw nothing do not create temporary images;
images referenced by queued draws remain alive until the NanoVG flush.
The source kind is retained before numeric argument coercion, even if that
coercion changes its prototype or properties. Source dimensions are read after
coercion so a resize takes effect; changed ImageBitmap data is validated before use.

`measureText()` uses em-scaled font sizes and glyph ink bounds. Atlas SDF padding,
the interpolation border, and fontstash blur padding are excluded from those
bounds. The no-font fallback exposes the same metric properties, but its values
are estimates rather than measurements of a loaded face.

The Canvas pixel regressions in `JavaScript.All` require a rendering GPU backend.
The test host sets `hasGpuRendering` to false only for the explicit no-op Metal
test configuration (`BABYLON_NATIVE_TESTS_USE_NOOP_METAL_DEVICE=ON`), using the
same `USE_NOOP_METAL_DEVICE` definition as the Apple test host. This keeps the
shared tests compatible with installed public headers without exposing bgfx.
Only the pixel regressions are reported as pending in that configuration; API,
PNG fallback, and text metrics tests still run. Configure that option `OFF` on a
Metal-capable Mac to run the pixel regressions as well. Windows and Linux retain
the pixel assertions.

## Native Canvas visual baseline

The `native-canvas.png` reference for Playground `#TKVFSA#8` is a Native regression
baseline, not a claim of browser Canvas parity. The em-size correction is
incremental, and the current reference also records these known divergences:

- Linear-gradient colors repeat past their end instead of clamping to the final
  stop as Babylon.js/browser Canvas does.
- The projected canvas texture has an additional dark perimeter.
- The Native path uses Droid Sans, so the font inputs are not matched with the
  browser path.

Follow-up work must isolate gradient extension and texture-edge sampling/alpha,
and run both paths with the same font bytes, dimensions, device-pixel ratio, and
scene inputs. Compare each intended correction against Babylon.js before updating
its reference. Do not regenerate a golden from current Native output merely to
make the comparison pass. The readback/rectangle and text-padding fixes do not
regenerate this baseline.

# Nanovg
This project contains a fork of Nanovg code and shaders found in bgfx repo. This fork features new filters stack to allow shadow, blur to be enabled in nanovg rendering (nanovg_filterstack.*).
Also, the rendering backend of Nanovg is defined in nanovg_babylon.*. It implements nanovg rendering using bgfx with an extension to allow blending of 2 textures (used for gradient mixing) whereas default implementation only allow 1 texture. Shaders are modified accordingly.

# Nanovg Shader Integration in Babylon.js
Nanovg utilizes shaders that are independent of Babylon.js. These shaders are written in the bgfx shading language, which is derived from GLSL, and are compiled into various target languages (SPIR-V, GLSL, Metal, DX, ESSL) during the build process.

## Precompiled Shaders:

A set of precompiled shaders is included, so you don’t need to build shaderc or recompile individual shaders every time you build the application. This significantly reduces build time, since compiling shaders with shaderc can be quite slow.

## Shader Modification and Compilation

If you modify any shader, you’ll need shaderc — the bgfx shader compiler — to rebuild them. Shaderc can generate shaders targeting MSL, GLSL, HLSL, or SPIR-V.

To obtain shaderc:
- Clone bgfx.cmake and generate and build the project 
  Or follow these instructions: https://bkaradzic.github.io/bgfx/build.html
- Once built, locate the `shaderc` executable in the output directory.
- Set the full path to this executable in CMake using the variable `SHADERC_PATH`.

If `SHADERC_PATH` is set but does not point to a valid executable, CMake will display a fatal error during project generation.
When correctly configured, the BabylonNative project will automatically use shaderc to compile all .sc shader files.

## Embedding Shaders:

Nanovg shaders, once compiled into a shader language, are converted into code and embedded directly into the application. This process eliminates the need for asset loading.

## Cross-Platform Builds:

The shader cache can be built on Mac, Linux, or Windows systems. For builds targeting Android or iOS, the shaders must first be compiled on one of these systems.
