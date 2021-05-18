# Frequently asked questions

## Could you explain BabylonNative

[Here is an article that describes the motivation behind Babylon Native.](https://babylonjs.medium.com/babylon-native-821f1694fffc)

## Unable to build for arm64 on Windows

cmake displays such error:
```
C:>cmake -D CMAKE_SYSTEM_NAME=WindowsStore -D CMAKE_SYSTEM_VERSION=10.0 -A arm64 ..
-- Selecting Windows SDK version 10.0.18362.0 to target Windows 10.0.
CMake Error at CMakeLists.txt:5 (project):
  Failed to run MSBuild command:

    C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/MSBuild/Current/Bin/MSBuild.exe
```

Check you have installed MSVC for arm64. 
In VisualStudio, go to menu `Tools` -> `Get Tools and Features`. Then check for arm64

![msvc_arm64](Images/faq/msvc_arm64.png)

## Unable to run BabylonNative on Android Emulator

[A detailed page is avaible here.](AndroidEmulator.md)

## Does Babylon Native support WASM?

The JavaScript virtual machines (JavaScriptCore, V8, Chakra, ...) that we have so far all support WebAssembly automatically.

## How to debug JavaScript ?

### Windows

In VisualStudio, select `Script` in the `Local Windows Debugger`. Then, run your app in debug mode (F5). You can put breakpoints, watch variable, ... in your JS scripts.

### MacOS and iOS

With Safari tools, it's possible to hook any JavaScriptCore instance running locally on your Mac or on your device and simulator. [Follow this steps to set it up.](DebugJavascriptMacIOS.md)
Then you can debug from Safari just like you would debug standard JS.

## How to debug rendering ?

First, if you get rendering issues in Babylon Native but you don't reproduce in BabylonJS (in Playground), [please consider posting the issue on BabylonJS forum.](https://forum.babylonjs.com/)

### Windows and Android

Download and use [RenderDoc.](https://renderdoc.org/) You can find a guide to debug graphics on [Android device at this location.](https://renderdoc.org/docs/how/how_android_capture.html)

### MacOS and iOS

The tool of choice on Apple device is Xcode. It contains tools to capture and inspect shaders, textures and more. A more [detailed documentation is available here.](DebugRenderedFrameMetal.md)