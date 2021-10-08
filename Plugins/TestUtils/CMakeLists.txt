set(SOURCES "Include/Babylon/Plugins/TestUtils.h"
            "Source/TestUtils.h"
            "Source/TestUtils.cpp"
            "Source/TestUtilsImplData.h")

if(APPLE)
    set(SOURCES ${SOURCES} "Source/${BABYLON_NATIVE_PLATFORM}/TestUtilsImpl.mm")
else()
    set(SOURCES ${SOURCES} "Source/${BABYLON_NATIVE_PLATFORM}/TestUtilsImpl.cpp")
endif()

add_library(TestUtils ${SOURCES})
warnings_as_errors(TestUtils)

if(WINDOWS_STORE)
    target_compile_options(TestUtils PRIVATE /ZW)
    target_compile_options(TestUtils PRIVATE /await)
endif()

target_include_directories(TestUtils INTERFACE "Include")
target_include_directories(TestUtils PRIVATE "Source")

target_link_to_dependencies(TestUtils
    PUBLIC JsRuntime
    PRIVATE GraphicsInternal)

set_property(TARGET TestUtils PROPERTY FOLDER Plugins)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})
