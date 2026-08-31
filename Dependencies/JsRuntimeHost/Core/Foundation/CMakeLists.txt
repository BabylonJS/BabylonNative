set(SOURCES
    "Include/Babylon/Api.h"
    "Include/Babylon/DebugTrace.h"
    "Include/Babylon/PerfTrace.h"
    "Include/Babylon/StandardStreamLogger.h"
    "Source/DebugTrace.cpp"
    "Source/PerfTrace.cpp"
    "Source/StandardStreamLogger.cpp"
    "Source/StandardStreamLoggerPlatform.h")

# .inl bodies are #include'd by the platform TUs (not separate translation units).
# List them in SOURCES only so IDEs/source_group show them next to the .cpp files.
set(STREAM_LOGGER_INCLUDES
    "Source/StandardStreamLogger_Shared.inl")

# Shared tee body (StandardStreamLogger_Shared.inl) + thin OS-ops platform TUs.
# Match AppRuntime: one JSRUNTIMEHOST_PLATFORM implementation is compiled in.
if(JSRUNTIMEHOST_PLATFORM STREQUAL "Win32" OR JSRUNTIMEHOST_PLATFORM STREQUAL "UWP")
    list(APPEND SOURCES "Source/StandardStreamLogger_Windows.cpp")
elseif(JSRUNTIMEHOST_PLATFORM STREQUAL "Android")
    list(APPEND SOURCES "Source/StandardStreamLogger_Android.cpp")
    list(APPEND STREAM_LOGGER_INCLUDES "Source/StandardStreamLogger_PosixOps.inl")
elseif(JSRUNTIMEHOST_PLATFORM STREQUAL "iOS" OR JSRUNTIMEHOST_PLATFORM STREQUAL "macOS")
    list(APPEND SOURCES "Source/StandardStreamLogger_Apple.cpp")
    list(APPEND STREAM_LOGGER_INCLUDES "Source/StandardStreamLogger_PosixOps.inl")
else()
    list(APPEND SOURCES "Source/StandardStreamLogger_Unix.cpp")
endif()

list(APPEND SOURCES ${STREAM_LOGGER_INCLUDES})

add_library(Foundation ${SOURCES})

# Ensure generators never try to compile the .inl include bodies on their own.
set_source_files_properties(${STREAM_LOGGER_INCLUDES} PROPERTIES HEADER_FILE_ONLY TRUE)

target_include_directories(Foundation
    PRIVATE "Include/Babylon"
    PRIVATE "Source"
    INTERFACE "Include")

target_link_libraries(Foundation
    PUBLIC napi
    PRIVATE napi-extensions
    PRIVATE arcana)

if(ANDROID)
    target_link_libraries(Foundation PRIVATE log)
endif()

set_property(TARGET Foundation PROPERTY FOLDER Core)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})