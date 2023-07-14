# bgfx.cmake
[![Build Status](https://github.com/bkaradzic/bgfx.cmake/workflows/Release/badge.svg)](https://github.com/bkaradzic/bgfx.cmake/workflows/Release/badge.svg)

**NOTE: This port only made to be used as C++ library, some features (such as bindings) might not work! For those features, please use original repo with GENie instead.**

This repo contains cmake configuration files that can be used to build bgfx with CMake.

## Building

```bash
git clone https://github.com/bkaradzic/bgfx.cmake.git
cd bgfx.cmake
git submodule init
git submodule update
cmake -S. -Bcmake-build # $CMakeOptions
cmake --build cmake-build
```

If downloading via zip (instead of using git submodules) manually download bx, bimg and bgfx and copy them into the root directory, or locate them via `BX_DIR`, `BIMG_DIR` and `BGFX_DIR` CMake variables.

## How To Use
This project is setup to be included a few different ways. To include bgfx source code in your project simply use add_subdirectory to include this project. To build bgfx binaries build the `INSTALL` target (or `make install`). The installed files will be in the directory specified by `CMAKE_INSTALL_PREFIX` which we recommend you set to `./install` so it will export to your build directory. Note you may want to build install on both `Release` and `Debug` configurations.

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

### `bgfx_compile_shader_to_header`
Add a build rule for a `*.sc` shader to the generated build system using shaderc.
```cmake
bgfx_compile_shader_to_header(
	TYPE VERTEX|FRAGMENT|COMPUTE
	SHADERS filenames
	VARYING_DEF filename
	OUTPUT_DIR directory
)
```
This defines a shaderc command to generate headers for a number of `TYPE` shaders with `SHADERS` files and `VARYING_DEF` file in the `OUTPUT_DIR` directory. There will be one generated shader for each supported rendering API on this current platform according to the `BGFX_EMBEDDED_SHADER` macro in `bgfx/embedded_shader.h`.

The generated headers will have names in the format of `${SHADERS}.${RENDERING_API}.bin.h` where `RENDERING_API` can be `glsl`, `essl`, `spv`, `dx9`, `dx11` and `mtl` depending on the availability of the platform.

Adding these `SHADERS` as source files to a target will run `shaderc` at build time and they will rebuild if either the contents of the `SHADERS` or the `VARYING_DEF` change.

#### Examples: Generating shaders as headers
```cmake
bgfx_compile_shader_to_header(
  TYPE VERTEX
  SHADERS vs.sc
  VARYING_DEF varying.def.sc
  OUTPUT_DIR ${CMAKE_BINARY_DIR}/include/generated/shaders
)
bgfx_compile_shader_to_header(
  TYPE FRAGMENT
  SHADERS fs.sc
  VARYING_DEF ${CMAKE_SOURCE_DIR}/varying.def.sc
  OUTPUT_DIR ${CMAKE_BINARY_DIR}/include/generated/shaders
)

add_library(myLib main.cpp vs.sc fs.sc)
target_include_directories(myLib ${CMAKE_BINARY_DIR}/include/generated/shaders)
```

```cpp
// main.cpp
#include <vs.sc.glsl.bin.h>
#include <vs.sc.essl.bin.h>
#include <vs.sc.spv.bin.h>
#include <fs.sc.glsl.bin.h>
#include <fs.sc.essl.bin.h>
#include <fs.sc.spv.bin.h>
#if defined(_WIN32)
#include <vs.sc.dx9.bin.h>
#include <vs.sc.dx11.bin.h>
#include <fs.sc.dx9.bin.h>
#include <fs.sc.dx11.bin.h>
#endif //  defined(_WIN32)
#if __APPLE__
#include <vs.sc.mtl.bin.h>
#include <fs.sc.mtl.bin.h>
#endif // __APPLE__

const bgfx::EmbeddedShader k_vs = BGFX_EMBEDDED_SHADER(vs);
const bgfx::EmbeddedShader k_fs = BGFX_EMBEDDED_SHADER(fs);
```

## Does this work with latest bx/bgfx/bimg?
Probably! This project needs to be updated if a dependency is added or the bgfx build system changes. The bgfx repository is very active but these types of changes are rare. New examples have to be added manually as well, but not doing so will merely result in that example not showing up and won't break anything else. If pulling latest causes issues, be sure to manually reconfigure CMake as the glob patterns may need to be refreshed (the use of glob patterns in CMake is generally discouraged but in this project it helps to ensure upwards compatibilty with newer bgfx updates).
