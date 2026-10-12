# Shader Tool

A cross-platform console tool that compiles GLSL shaders using the ShaderCompiler component and saves the compiled results using ShaderCache. This tool is designed to be run as part of the build process.

## Overview

This tool takes vertex and fragment shader source files (in GLSL format) as input, compiles them for the target graphics API using Babylon Native's ShaderCompiler, and saves the compiled shader data to an output file using the ShaderCache component. Multiple shader pairs can be compiled into a single cache file. It can also precompile the shaders NativeEngine generates at run time for WebGL2 transform feedback programs and for divisor-driven instanced attributes.

Shader sources must exactly match the strings Babylon Native receives at run time (`createProgram`); CR/LF differences are ignored.

## Usage

```
ShaderTool -o <output_file> [-v <vertex> <varyings>]... [[-i <attributes>] <vertex1> <fragment1> ...]
```

### Options

| Option | Description |
|--------|-------------|
| `-o <output_file>` | Path to the output compiled shader cache file (required) |
| `-v <vertex> <varyings>` | Transform feedback vertex shader and its comma-separated captured varyings (interleaved, in capture order); may be repeated |
| `-i <attributes>` | Comma-separated attributes the next vertex/fragment pair reads with a vertex divisor; caches that instanced variant as well |

### Arguments

| Argument | Description |
|----------|-------------|
| `<vertex> <fragment>` | Pairs of vertex and fragment shader source files (GLSL) |

### Examples

Single shader pair:
```bash
ShaderTool -o cache.bin vertex.glsl fragment.glsl
```

Multiple shader pairs:
```bash
ShaderTool -o cache.bin v1.glsl f1.glsl v2.glsl f2.glsl v3.glsl f3.glsl
```

Transform feedback update shader plus an instanced render pair:
```bash
ShaderTool -o cache.bin -v update.glsl outPosition,outAge -i position,age v1.glsl f1.glsl
```

## Build Integration

This tool can be integrated into your build process using CMake custom commands:

### Single Shader Pair

```cmake
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/shaders.cache
    COMMAND ShaderTool
        -o ${CMAKE_CURRENT_BINARY_DIR}/shaders.cache
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders/vertex.glsl
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders/fragment.glsl
    DEPENDS
        ShaderTool
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders/vertex.glsl
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders/fragment.glsl
    COMMENT "Compiling shaders..."
)
```

### Multiple Shader Pairs

```cmake
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/shaders.cache
    COMMAND ShaderTool
        -o ${CMAKE_CURRENT_BINARY_DIR}/shaders.cache
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders/basic.vert
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders/basic.frag
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders/pbr.vert
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders/pbr.frag
    DEPENDS
        ShaderTool
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders/basic.vert
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders/basic.frag
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders/pbr.vert
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders/pbr.frag
    COMMENT "Compiling shaders..."
)
```

## Building

This tool is built as part of the Babylon Native build system when `BABYLON_NATIVE_PLUGIN_SHADERTOOL` is enabled.
