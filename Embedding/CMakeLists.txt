set(EMBEDDING_SHARED_INCLUDE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/Include/Shared")

set(SOURCES
    "${EMBEDDING_SHARED_INCLUDE_ROOT}/Babylon/Embedding/LogLevel.h"
    "${EMBEDDING_SHARED_INCLUDE_ROOT}/Babylon/Embedding/Runtime.h"
    "${EMBEDDING_SHARED_INCLUDE_ROOT}/Babylon/Embedding/RuntimeOptions.h"
    "${EMBEDDING_SHARED_INCLUDE_ROOT}/Babylon/Embedding/View.h"
    "Source/Runtime.cpp"
    "Source/RuntimeImpl.h"
    "Source/View.cpp")

add_library(Embedding ${SOURCES})

warnings_as_errors(Embedding)

target_include_directories(Embedding PUBLIC "${EMBEDDING_SHARED_INCLUDE_ROOT}")

# Always-on dependencies. The Embedding layer formalizes the canonical
# Babylon Native setup, so the standard polyfills (Blob / Console / File /
# Performance / TextDecoder / XMLHttpRequest) are always linked.
target_link_libraries(Embedding
    PUBLIC napi
    # GraphicsDevice is PUBLIC because View.h (a public header)
    # references `Babylon::Graphics::WindowT` from <Babylon/Graphics/Platform.h>.
    PUBLIC GraphicsDevice
    # GraphicsDeviceContext is PRIVATE — needed so View.cpp can see
    # <Babylon/Graphics/DeviceQueries.h> for the free
    # `Babylon::Graphics::GetDevicePixelRatio(window)` helper used to
    # convert physical → logical pixels on the first `View::Resize`
    # call (before the `Device` exists or while it is still bound to
    # the previous window on a re-attach). Internal Graphics types are
    # never exposed across the Embedding public surface.
    PRIVATE GraphicsDeviceContext
    PRIVATE arcana
    PRIVATE AppRuntime
    PRIVATE ScriptLoader
    PRIVATE Blob
    PRIVATE Console
    PRIVATE File
    PRIVATE Performance
    PRIVATE TextDecoder
    PRIVATE XMLHttpRequest)

# ----- Conditionally-included polyfills -----
#
# Each flag is exposed as a PUBLIC compile definition so the public
# headers (Runtime.h / View.h) and the impl source files can both gate
# on the same value via `#if BABYLON_NATIVE_*`.

if(BABYLON_NATIVE_POLYFILL_WINDOW)
    target_compile_definitions(Embedding PUBLIC BABYLON_NATIVE_POLYFILL_WINDOW=1)
    target_link_libraries(Embedding PRIVATE Window)
endif()

if(BABYLON_NATIVE_POLYFILL_CANVAS)
    target_compile_definitions(Embedding PUBLIC BABYLON_NATIVE_POLYFILL_CANVAS=1)
    target_link_libraries(Embedding PRIVATE Canvas)
endif()

if(BABYLON_NATIVE_POLYFILL_ABORTCONTROLLER)
    target_compile_definitions(Embedding PUBLIC BABYLON_NATIVE_POLYFILL_ABORTCONTROLLER=1)
    target_link_libraries(Embedding PRIVATE AbortController)
endif()

if(BABYLON_NATIVE_POLYFILL_SCHEDULING)
    target_compile_definitions(Embedding PUBLIC BABYLON_NATIVE_POLYFILL_SCHEDULING=1)
    target_link_libraries(Embedding PRIVATE Scheduling)
endif()

if(BABYLON_NATIVE_POLYFILL_URL)
    target_compile_definitions(Embedding PUBLIC BABYLON_NATIVE_POLYFILL_URL=1)
    target_link_libraries(Embedding PRIVATE URL)
endif()

if(BABYLON_NATIVE_POLYFILL_WEBSOCKET)
    target_compile_definitions(Embedding PUBLIC BABYLON_NATIVE_POLYFILL_WEBSOCKET=1)
    target_link_libraries(Embedding PRIVATE WebSocket)
endif()

# ----- Conditionally-included plugins -----

if(BABYLON_NATIVE_PLUGIN_NATIVEENGINE)
    target_compile_definitions(Embedding PUBLIC BABYLON_NATIVE_PLUGIN_NATIVEENGINE=1)
    target_link_libraries(Embedding PRIVATE NativeEngine)
endif()

if(BABYLON_NATIVE_PLUGIN_NATIVEINPUT)
    target_compile_definitions(Embedding PUBLIC BABYLON_NATIVE_PLUGIN_NATIVEINPUT=1)
    target_link_libraries(Embedding PRIVATE NativeInput)
endif()

if(BABYLON_NATIVE_PLUGIN_NATIVECAMERA)
    target_compile_definitions(Embedding PUBLIC BABYLON_NATIVE_PLUGIN_NATIVECAMERA=1)
    target_link_libraries(Embedding PRIVATE NativeCamera)
endif()

if(BABYLON_NATIVE_PLUGIN_NATIVECAPTURE)
    target_compile_definitions(Embedding PUBLIC BABYLON_NATIVE_PLUGIN_NATIVECAPTURE=1)
    target_link_libraries(Embedding PRIVATE NativeCapture)
endif()

if(BABYLON_NATIVE_PLUGIN_NATIVEENCODING)
    target_compile_definitions(Embedding PUBLIC BABYLON_NATIVE_PLUGIN_NATIVEENCODING=1)
    target_link_libraries(Embedding PRIVATE NativeEncoding)
endif()

if(BABYLON_NATIVE_PLUGIN_NATIVEOPTIMIZATIONS)
    target_compile_definitions(Embedding PUBLIC BABYLON_NATIVE_PLUGIN_NATIVEOPTIMIZATIONS=1)
    target_link_libraries(Embedding PRIVATE NativeOptimizations)
endif()

if(BABYLON_NATIVE_PLUGIN_NATIVETRACING)
    target_compile_definitions(Embedding PUBLIC BABYLON_NATIVE_PLUGIN_NATIVETRACING=1)
    target_link_libraries(Embedding PRIVATE NativeTracing)
endif()

if(BABYLON_NATIVE_PLUGIN_NATIVEXR AND TARGET NativeXr)
    # Public surface: Runtime::SetXrWindow / IsXrActive when this flag
    # is set. RuntimeImpl.h includes <Babylon/Plugins/NativeXr.h>, but
    # that's an internal header so the dependency stays PRIVATE here.
    # NativeXr is only compiled on Android/iOS (see Plugins/CMakeLists.txt),
    # so we additionally check `TARGET NativeXr` to skip XR support on
    # platforms where the plugin isn't built even if the flag is ON.
    target_compile_definitions(Embedding PUBLIC BABYLON_NATIVE_PLUGIN_NATIVEXR=1)
    target_link_libraries(Embedding PRIVATE NativeXr)
endif()

if(BABYLON_NATIVE_PLUGIN_SHADERCACHE)
    target_compile_definitions(Embedding PUBLIC BABYLON_NATIVE_PLUGIN_SHADERCACHE=1)
    target_link_libraries(Embedding PRIVATE ShaderCache)
endif()

if(BABYLON_NATIVE_PLUGIN_TESTUTILS)
    target_compile_definitions(Embedding PUBLIC BABYLON_NATIVE_PLUGIN_TESTUTILS=1)
    target_link_libraries(Embedding PRIVATE TestUtils)
endif()

set_property(TARGET Embedding PROPERTY FOLDER Embedding)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

# ----- Per-platform interop layers -----
#
# Each platform's interop layer lives in its own subdirectory and is
# opt-in via its own CMake flag. They depend on the cross-platform
# `Embedding` target above.

if(BABYLON_NATIVE_EMBEDDING_ANDROID)
    add_subdirectory(Android)
endif()

if(BABYLON_NATIVE_EMBEDDING_APPLE)
    add_subdirectory(Apple)
endif()
