bgfx.cmake
===================
[![Build Status](https://github.com/bkaradzic/bgfx.cmake/workflows/Release/badge.svg)](https://github.com/bkaradzic/bgfx.cmake/workflows/Release/badge.svg)

**NOTE: This port only made to be used as C++ library, some features (such as bindings, examples) might not work! Please use original repo with GENie instead.**

This repo contains a bunch of cmake files that can be used to build bgfx with CMake.

Building
-------------

```bash
git clone https://github.com/bkaradzic/bgfx.cmake.git
cd bgfx.cmake
git submodule init
git submodule update
mkdir build
cd build
cmake ..
```

If downloading via zip (instead of using git submodules) manually download bx, bimg and bgfx and copy them into the root directory, or locate them via `BX_DIR`, `BIMG_DIR` and `BGFX_DIR` CMake variables.

How To Use
-------------
This project is setup to be included a few different ways. To include bgfx source code in your project simply use add_subdirectory to include this project. To build bgfx binaries build the `INSTALL` target (or `make install`). The installed files will be in the directory specified by `CMAKE_INSTALL_PREFIX` which I recommend you set to `./install` so it will export to your build directory. Note you may want to build install on both `Release` and `Debug` configurations.

Features
-------------
* No outside dependencies besides bx, bimg, bgfx, and CMake.
* Tested on Visual Studio 2015, Xcode, gcc 5.4, clang 3.8.
* Compiles bgfx, tools & examples.
* Detects shader modifications and automatically rebuilds them for all examples.

Does this work with latest bx/bgfx/bimg?
-------------
Probably! This project needs to be updated if a dependency is added or the bgfx build system changes. The bgfx repository is very active but these types of changes are rare. New examples have to be added manually as well, but not doing so will merely result in that example not showing up and won't break anything else. If pulling latest causes issues, be sure to manually reconfigure CMake as the glob patterns may need to be refreshed (the use of glob patterns in CMake is generally discouraged but in this project it helps to ensure upwards compatibilty with newer bgfx updates).
