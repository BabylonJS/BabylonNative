# Canvas
Implements parts of the 2D Canvas API using bgfx. Still a very early WIP; many methods are not yet implemented.

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
