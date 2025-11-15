if(BABYLON_NATIVE_PLUGIN_EXTERNALTEXTURE)
    add_subdirectory(ExternalTexture)
endif()

if(BABYLON_NATIVE_PLUGIN_NATIVECAMERA)
    add_subdirectory(NativeCamera)
endif()

if(BABYLON_NATIVE_PLUGIN_NATIVECAPTURE)
    add_subdirectory(NativeCapture)
endif()

if(BABYLON_NATIVE_PLUGIN_NATIVEENGINE)
    add_subdirectory(NativeEngine)
endif()

if(BABYLON_NATIVE_PLUGIN_NATIVEINPUT)
    add_subdirectory(NativeInput)
endif()

if(BABYLON_NATIVE_PLUGIN_NATIVEOPTIMIZATIONS)
    add_subdirectory(NativeOptimizations)
endif()

if(BABYLON_NATIVE_PLUGIN_NATIVETRACING)
    add_subdirectory(NativeTracing)
endif()

if(BABYLON_NATIVE_PLUGIN_TESTUTILS)
    add_subdirectory(TestUtils)
endif()

if(BABYLON_NATIVE_PLUGIN_NATIVEXR AND (ANDROID OR IOS))
    add_subdirectory(NativeXr)
endif()

if(BABYLON_NATIVE_PLUGIN_NATIVEENCODING)
    add_subdirectory(NativeEncoding)
endif()

if(BABYLON_NATIVE_PLUGIN_DATASTREAM)
    add_subdirectory(DataStream)
endif()
