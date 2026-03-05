# PrecompiledShaderTest

A cross-platform headless app that validates the **ShaderTool → ShaderCache →
NativeEngine** pipeline by rendering a scene using pre-compiled shaders and
saving the result as a PNG screenshot.

## Overview

The app has two parts: a **JavaScript** scene that describes what to render, and
a **C++** host that loads pre-compiled shaders, creates a graphics device,
executes the scene script, and saves the rendered frame as a PNG.

### Pipeline

1. `npm run build` in `JavaScript/` bundles the scene script and extracts
   preprocessed shader source files.
2. At CMake build time, **ShaderTool** compiles the shader source files into a
   binary cache (`shaders.bin`).
3. At run time, the app loads `shaders.bin` into **ShaderCache**, renders the
   scene to an offscreen texture, and writes `output.png`.

## Building

### 1. Prepare the JavaScript assets

```bash
cd JavaScript
npm install
npm run build
```

This produces:
- `JavaScript/dist/index.js` — bundled scene script
- `JavaScript/dist/shaders/<name>/vertex.fx` — preprocessed vertex shaders
- `JavaScript/dist/shaders/<name>/fragment.fx` — preprocessed fragment shaders

### 2. Build with CMake

Configure and build the `PrecompiledShaderTest` target as part of the normal
BabylonNative CMake build. The `CMakeLists.txt` invokes **ShaderTool** to
compile the shader files into `shaders.bin` and copies all assets next to
the executable.

