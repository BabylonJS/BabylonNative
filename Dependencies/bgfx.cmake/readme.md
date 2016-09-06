bgfx.cmake
===================

This repo contains a bunch of cmake files that can be used to build bgfx with CMake. To get started, pull down the repo and run these commands from within the root directory of the repo:

```
git clone git@github.com:bkaradzic/bx.git
git clone git@github.com:bkaradzic/bgfx.git
mkdir build
cd build
cmake ..
```

Features
-------------
* No outside dependencies besides bx, bgfx, and CMake.
* Tested on Visual Studio 2015 and Xcode.
* Compiles bgfx, tools & examples.
* Detects shader modifications and automatically rebuilds them.
* Uses CMake interface libraries (linking against bgfx is one line of cmake code via target_link_libraries).

License
-------------
The license is BSD with no clauses meaning you can use this software however you want but I cannot be held liable for damages.
