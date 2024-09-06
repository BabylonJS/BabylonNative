set(HEADER "Include/Babylon/Plugins/NativeXr.h")
FILE(GLOB SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/Source/*")

add_library(NativeXr ${SOURCES} ${HEADER})
warnings_as_errors(NativeXr)

target_include_directories(NativeXr
    PUBLIC "Include")

target_link_libraries(NativeXr
    PUBLIC napi
    PRIVATE arcana
    PRIVATE bgfx
    PRIVATE GraphicsDeviceContext
    PRIVATE JsRuntimeInternal
    PRIVATE xr)

set_property(TARGET NativeXr PROPERTY FOLDER Plugins)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

if(WINDOWS_STORE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
endif()
