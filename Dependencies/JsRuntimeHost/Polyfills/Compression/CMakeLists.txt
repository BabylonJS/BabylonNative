if(APPLE)
    # FindZLIB exposes the SDK's usr/include as an explicit system include on
    # recent Xcode versions. That breaks libc++'s include_next ordering. The
    # header and library are both SDK-provided, so only add the link library --
    # by name, not via find_library: a configure-time path points into the
    # device SDK, which the simulator link then rejects (libz.tbd "missing
    # required architecture"). "-lz" resolves against whichever SDK is active.
    set(JSRUNTIMEHOST_COMPRESSION_ZLIB_TARGET z)
elseif(TARGET ZLIB::ZLIB)
    set(JSRUNTIMEHOST_COMPRESSION_ZLIB_TARGET ZLIB::ZLIB)
elseif(TARGET zlibstatic)
    set(JSRUNTIMEHOST_COMPRESSION_ZLIB_TARGET zlibstatic)
elseif(TARGET zlib)
    set(JSRUNTIMEHOST_COMPRESSION_ZLIB_TARGET zlib)
else()
    find_package(ZLIB QUIET)
    if(ZLIB_FOUND)
        set(JSRUNTIMEHOST_COMPRESSION_ZLIB_TARGET ZLIB::ZLIB)
    else()
        set(ZLIB_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
        set(SKIP_INSTALL_ALL ON CACHE BOOL "" FORCE)
        FetchContent_Declare(jsruntimehost_zlib
            GIT_REPOSITORY https://github.com/madler/zlib.git
            GIT_TAG 51b7f2abdade71cd9bb0e7a373ef2610ec6f9daf
            EXCLUDE_FROM_ALL)
        FetchContent_MakeAvailable_With_Message(jsruntimehost_zlib)
        set(JSRUNTIMEHOST_COMPRESSION_ZLIB_TARGET zlibstatic)
        set_property(TARGET zlibstatic PROPERTY FOLDER Dependencies)
    endif()
endif()

set(COMPRESSION_POLYFILL_FILE "${CMAKE_CURRENT_SOURCE_DIR}/Source/CompressionPolyfill.js")
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${COMPRESSION_POLYFILL_FILE}")
file(READ "${COMPRESSION_POLYFILL_FILE}" COMPRESSION_POLYFILL_SOURCE)
configure_file(
    "Source/CompressionScripts.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/Generated/CompressionScripts.h"
    @ONLY)

set(SOURCES
    "Include/Babylon/Polyfills/Compression.h"
    "README.md"
    "Source/Compression.cpp"
    "Source/CompressionPolyfill.js"
    "Source/CompressionScripts.h.in"
    "ThirdParty/zlib/LICENSE")

add_library(Compression ${SOURCES})
warnings_as_errors(Compression)

target_include_directories(Compression
    PUBLIC "Include"
    PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/Generated")

target_link_libraries(Compression
    PUBLIC JsRuntime
    PUBLIC Streams
    PRIVATE ${JSRUNTIMEHOST_COMPRESSION_ZLIB_TARGET})

set_property(TARGET Compression PROPERTY FOLDER Polyfills)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})
