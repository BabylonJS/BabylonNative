# ShaderCacheGenerator

Headless Win32 + OpenGL tool that builds a GPU shader cache for a Babylon.js
scene. It hosts the scene through the `Babylon::Embedding` Runtime/View API on a
hidden window (ANGLE OpenGL backend), renders frames until the scene's shaders
are compiled, then writes the `ShaderCache` to a file.

Command-line parsing, process diagnostics (console/crash/exit handling) and the
log callback are reused from `Apps/Playground/Shared`.

## Usage

```
ShaderCacheGenerator <scene.js> <output.bin> [shared options]
```

| Argument       | Description                            |
| -------------- | -------------------------------------- |
| `<scene.js>`   | Scene script to load (required).       |
| `<output.bin>` | Shader cache file to write (required). |

`[shared options]` are the Playground command-line flags (run with `--help` to
list them); e.g. `--debug-trace=true`.

Exit codes: `0` success (>=1 shader written), `2` command-line / input error,
`1` runtime failure, `3` hard crash.

## Build

Configure BabylonNative for Win32 with the ANGLE OpenGL backend, then build the
target:

```
cmake -B build -G "Visual Studio 18 2026" -A x64 -D GRAPHICS_API=OpenGLWindowsDevOnly -D BABYLON_NATIVE_BUILD_APPS=ON
cmake --build build --target ShaderCacheGenerator --config Release
```

Requires an installed Edge/Chrome (for `libEGL.dll` / `libGLESv2.dll`). The
cache is GL-specific, so it is compatible with the Android OpenGL backend.

## "Scene ready" detection

There is no fixed frame count. The tool renders until the shader-cache entry
count stops growing for a number of consecutive checks -- i.e. the scene has
finished compiling every effect it uses. This is the meaningful signal for a
cache generator and works for any scene without it exposing globals. A frame cap
is applied only as a safety net.
