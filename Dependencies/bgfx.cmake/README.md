# bgfx.cmake

**NOTE: This port only made to be used as C++ library, some features (such as bindings) might not work! For those features, please use original repo with GENie instead.**

This directory contains the in-tree Babylon Native CMake wrapper for bgfx,
imported with full file history from BabylonJS/bgfx.cmake. See
[UPSTREAM.md](UPSTREAM.md) for provenance, licenses and exact source pins.
Only the wrapper is vendored; bgfx, bx and bimg are still external sources.

## Building

CMake 3.21 or newer is required.

```powershell
git clone https://github.com/BabylonJS/BabylonNative.git
Set-Location BabylonNative
cmake -S Dependencies\bgfx.cmake -B build\bgfx -DBGFX_BUILD_EXAMPLES=OFF
cmake --build build\bgfx --config Release
```

CMake fetches the three full Git repositories at the exact revisions in
`cmake/dependencies.cmake`, without submodules. Default sources are
`${FETCHCONTENT_BASE_DIR}/bgfx.cmake-src/{bgfx,bx,bimg}`. Babylon Native adds
the tracked wrapper locally at the legacy binary root
`${FETCHCONTENT_BASE_DIR}/bgfx.cmake-build`; it never downloads the wrapper.

Source precedence is nonempty `BGFX_DIR`/`BX_DIR`/`BIMG_DIR`, standard
`FETCHCONTENT_SOURCE_DIR_BGFX`/`_BX`/`_BIMG` overrides, valid adjacent bundled
directories, then pinned FetchContent acquisition. Explicit relative roots
are relative to this wrapper, not the outer project. Invalid explicit roots
fail instead of downloading replacements. Parent normal variables take
precedence over cache values. Resolved managed roots are not cached as
explicit overrides; they continue through FetchContent on each configure.
A parent's earlier FetchContent declaration retains its usual precedence.

`FETCHCONTENT_BASE_DIR`, `FETCHCONTENT_UPDATES_DISCONNECTED` (including its
per-source variants) and `FETCHCONTENT_FULLY_DISCONNECTED` keep their CMake
semantics. Fully disconnected builds need populated sources or source
overrides in advance; there is no remote-wrapper fallback.

Babylon Native's `BABYLON_NATIVE_BUILD_SOURCETREE=ON` stages the current
wrapper beside the selected children in `_deps/bgfx.cmake-src`. Existing
packagers can copy this directory, strip Git metadata and configure a fresh
offline build with `FETCHCONTENT_SOURCE_DIR_BGFX.CMAKE` pointing at it.
This legacy wrapper override selects a local `add_subdirectory`, not a fetch.
Untyped relative command-line overrides are normalized against the configure
process's working directory, matching FetchContent.
Custom selected child roots are copied into the package without modifying
the originals.

Versions use the selected bgfx API and its own Git commit count, never an
ancestor repository's history. Source packages snapshot that version in
`bgfx/.bgfx-cmake-version`. Plain archives fall back to their checked-in
`src/version.h` (which may describe a different historical revision), then
revision 0 with a warning if no revision metadata exists. Configuration
never rewrites a source tree's `src/version.h`.

Apple universal builds containing x86 slices require Xcode 14+ (or upstream Clang 14+) for scoped unused-argument handling of the architecture-specific SSE4.2 options.

## How To Use
This project is setup to be included a few different ways. To include bgfx source code in your project simply use add_subdirectory to include this project. To build bgfx binaries build the `INSTALL` target (or `make install`). The installed files will be in the directory specified by `CMAKE_INSTALL_PREFIX` which we recommend you set to `./install` so it will export to your build directory. Note you may want to build install on both `Release` and `Debug` configurations.

## Configuration

See [CMakeLists.txt](CMakeLists.txt) for available settings and defaults. Set them with `-D` or as normal variables in a parent project before `add_subdirectory`; parent normal variables take precedence over cache values. Empty numeric bgfx overrides and per-format bimg parser settings leave the corresponding library defaults unchanged.

Disabling AVIF parsing excludes the bundled libavif/dav1d sources. Enabling HEIF parsing requires externally supplied libheif headers and libraries.

## Features
* No outside dependencies besides bx, bimg, bgfx, and CMake.
* Tested on
    * Windows, OSX, Linux, Android, UWP, Emscripten (experimental)
    * VSCode, Visual Studio, Xcode, gcc, clang.
* Compiles bgfx, tools & examples.
* Detects shader modifications and automatically rebuilds them for all examples.

## Added cmake commands
bgfx.cmake will install `bgfxToolUtils.cmake` which has useful cmake functions for using bgfx's tools:

### `bgfx_compile_binary_to_header`
Add a build rule for a binary file to the generated build system using bin2c.
```cmake
bgfx_compile_binary_to_header(
	INPUT_FILE filename
	OUTPUT_FILE filename
	ARRAY_NAME name
)
```
This defines a bin2c command to generate a specified `OUTPUT_FILE` header with an array `ARRAY_NAME` with the binary representation of a `INPUT_FILE` file.

Adding these `INPUT_FILE` as source files to a target will run `bin2c` at build time and they will rebuild if either the contents of the `INPUT_FILE` change.

#### Examples: Generating an image as a header
```cmake
bgfx_compile_binary_to_header(
  INPUT_FILE ${CMAKE_CURRENT_SOURCE_DIR}/image.png
  OUTPUT_FILE ${CMAKE_BINARY_DIR}/include/generated/images/image.png.h
  ARRAY_NAME image_bytes
)
add_library(myLib image.png)
target_include_directories(myLib ${CMAKE_BINARY_DIR}/include/generated/images)
```

```cpp
// main.cpp
#include <image.png.h>
// You now have access to a static const uint8_t or char array named image_bytes
```

### `bgfx_compile_texture`
Add a build rule for a texture to the generated build system be compiled using texturec.
```cmake
bgfx_compile_texture(
     FILE filename
     OUTPUT filename
     [FORMAT format]
     [QUALITY default|fastest|highest]
     [MIPS]
     [MIPSKIP N]
     [NORMALMAP]
     [EQUIRECT]
     [STRIP]
     [SDF]
     [REF alpha]
     [IQA]
     [PMA]
     [LINEAR]
     [MAX max size]
     [RADIANCE model]
     [AS extension]
)
```

### `bgfx_compile_shaders`
Add a build rule for a `*.sc` shader to the generated build system using shaderc.
```cmake
bgfx_compile_shaders(
	TYPE VERTEX|FRAGMENT|COMPUTE
	SHADERS filenames
	VARYING_DEF filename
	OUTPUT_DIR directory
	[AS_HEADERS]
)
```
This defines a shaderc command to generate binaries or headers for a number of `TYPE` shaders with `SHADERS` files and `VARYING_DEF` file in the `OUTPUT_DIR` directory. There will be one generated shader for each supported rendering API on this current platform according to the `BGFX_EMBEDDED_SHADER` macro in `bgfx/embedded_shader.h` for headers and in the directory expected by `load_shader` in `bgfx_utils.h`.

The generated headers will have names in the format of `${RENDERING_API}/${SHADERS}.bin[.h]` where `RENDERING_API` can be `glsl`, `essl`, `spirv`, `dx11` and `metal` depending on the availability of the platform.

Adding these `SHADERS` as source files to a target will run `shaderc` at build time and they will rebuild if either the contents of the `SHADERS` or the `VARYING_DEF` change.

#### Examples: Generating shaders as headers
```cmake
bgfx_compile_shaders(
  TYPE VERTEX
  SHADERS vs.sc
  VARYING_DEF varying.def.sc
  OUTPUT_DIR ${CMAKE_BINARY_DIR}/include/generated/shaders
  AS_HEADERS
)
bgfx_compile_shaders(
  TYPE FRAGMENT
  SHADERS fs.sc
  VARYING_DEF ${CMAKE_SOURCE_DIR}/varying.def.sc
  OUTPUT_DIR ${CMAKE_BINARY_DIR}/include/generated/shaders
  AS_HEADERS
)

add_library(myLib main.cpp vs.sc fs.sc)
target_include_directories(myLib ${CMAKE_BINARY_DIR}/include/generated/shaders)
```

```cpp
// main.cpp
#include <glsl/vs.sc.bin.h>
#include <essl/vs.sc.bin.h>
#include <spirv/vs.sc.bin.h>
#include <glsl/fs.sc.bin.h>
#include <essl/fs.sc.bin.h>
#include <spirv/fs.sc.bin.h>
#if defined(_WIN32)
#include <dx11/vs.sc.bin.h>
#include <dx11/fs.sc.bin.h>
#endif //  defined(_WIN32)
#if __APPLE__
#include <metal/vs.sc.bin.h>
#include <metal/fs.sc.bin.h>
#endif // __APPLE__

const bgfx::EmbeddedShader k_vs = BGFX_EMBEDDED_SHADER(vs);
const bgfx::EmbeddedShader k_fs = BGFX_EMBEDDED_SHADER(fs);
```

## Source updates

Source updates are deliberate pin changes, not implicit upgrades. Reconfigure
managed sources after changing a pin; explicit external trees remain untouched.
Keep wrapper updates and source updates separate as described in `UPSTREAM.md`.
