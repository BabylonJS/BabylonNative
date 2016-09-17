bgfx.cmake
===================

This repo contains a bunch of cmake files that can be used to build bgfx with CMake. To get started, clone the repo and run these commands from within the root directory of the repo:

```
git submodule init
git submodule update
mkdir build
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=./install
```

If downloading via zip (instead of using git submodules) manually download bx and bgfx and copy them into the root directory, or locate them via BX_DIR and BGFX_DIR CMake variables.

How To Use
-------------
This project is setup to be included a few different ways. To include bgfx source code in your project simply use add_subdirectory to include this project. To build bgfx binaries build the INSTALL target (or "make install"). The installed files will be in the directory specified by CMAKE_INSTALL_PREFIX which I recommend you set to "./install" so it will export to your build directory. Note you may want to build install on both Release and Debug configurations.

Features
-------------
* No outside dependencies besides bx, bgfx, and CMake.
* Tested on Visual Studio 2015 and Xcode.
* Compiles bgfx, tools & examples.
* Detects shader modifications and automatically rebuilds them for all examples.

Todo
-------------
* Support Linux.
* Support Android.
* Support Native Client.
* Support Windows Phone.
* Build texturec and texturev.
* More configuration.
