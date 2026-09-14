# Canvas
Implements parts of the 2D Canvas API using bgfx. Still a very early WIP; many methods are not yet implemented.

## Readback and text metrics

`toDataURL()` encodes PNG from the rendered canvas. Empty, case-variant, and
unsupported media types use the same PNG fallback; JPEG/WebP encoding is not
implemented. `drawImage(canvas, ...)` snapshots rendered source pixels, not only
the CPU pixel mirror.

`measureText()` uses em-scaled font sizes and glyph ink bounds. Atlas SDF padding,
the interpolation border, and fontstash blur padding are excluded from those
bounds. The no-font fallback exposes the same metric properties, but its values
are estimates rather than measurements of a loaded face.

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
