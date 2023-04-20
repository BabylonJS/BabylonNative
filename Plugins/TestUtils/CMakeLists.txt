if(APPLE)
    set(PLATFORM Apple)
else()
    set(PLATFORM ${BABYLON_NATIVE_PLATFORM})
endif()

set(SOURCES
    "Include/Babylon/Plugins/TestUtils.h"
    "Source/TestUtils.h"
    "Source/TestUtils.cpp"
    "Source/TestUtilsImplData.h"
    "Source/${PLATFORM}/TestUtilsImpl.${BABYLON_NATIVE_PLATFORM_IMPL_EXT}")

add_library(TestUtils ${SOURCES})
warnings_as_errors(TestUtils)

target_include_directories(TestUtils PUBLIC
    "Include")

target_link_to_dependencies(TestUtils
    PUBLIC napi
    PRIVATE JsRuntimeInternal
    PRIVATE GraphicsDevice
    PRIVATE GraphicsDeviceContext)

set_property(TARGET TestUtils PROPERTY FOLDER Plugins)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})
