# Canvas
Implements parts of the 2D Canvas API using bgfx. Still a very early WIP; many methods are not yet implemented.

# Nanovg Shader Integration in Babylon.js
Nanovg utilizes shaders that are independent of Babylon.js. These shaders are written in the bgfx shading language, which is derived from GLSL, and are compiled into various target languages (SPIR-V, GLSL, Metal, DX, ESSL) during the build process.

## Precompiled Shaders:

A precompiled version of the shaders is provided, eliminating the need to compile shaderc and individual shaders with each application build. This saves time, as shaderc compilation can be time-consuming.

## Shader Modification:

To modify Canvas shaders, enable the CMake options `BGFX_BUILD_TOOLS` and `BGFX_BUILD_TOOLS_SHADER`.
CMake script will add custom steps to build .sc shader files with shaderc executable in that case.
Make the necessary shader changes and build the Canvas polyfill. The shader cache will be automatically generated.
Revert the CMake flags after the modifications are complete. The Playground can then be run with the updated shaders.

## Shader Compilation:

The current forks of glslang in bgfx and BabylonNative make it challenging to consolidate into a single version.
The shader pipeline is currently specific to Canvas. If needed for other projects, a more generic solution will be required.

## Embedding Shaders:

Nanovg shaders, once compiled into a shader language, are converted into code and embedded directly into the application. This process eliminates the need for asset loading.

## Cross-Platform Builds:

The shader cache can be built on Mac, Linux, or Windows systems. For builds targeting Android or iOS, the shaders must first be compiled on one of these systems.
