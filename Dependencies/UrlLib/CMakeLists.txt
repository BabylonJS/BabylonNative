if(ANDROID)
    set(PLATFORM "Android")
elseif(APPLE)
    set(PLATFORM "Apple")
elseif(WIN32)
    set(PLATFORM "Windows")
    if(NOT WINDOWS_STORE)
        set(ADDITIONAL_LIBRARIES PUBLIC onecoreuap.lib)
    endif()
else()
    message(FATAL_ERROR "Unrecognized platform: ${CMAKE_SYSTEM_NAME}")
endif()

set(SOURCES
    "Include/UrlLib/UrlLib.h"
    "Source/Shared/UrlRequest.h"
    "Source/${PLATFORM}/UrlLib.cpp")

add_library(UrlLib ${SOURCES})

target_include_directories(UrlLib PRIVATE "Include")
target_include_directories(UrlLib PRIVATE "Source")
target_include_directories(UrlLib INTERFACE "Include")

target_link_libraries(UrlLib
    PUBLIC arcana
    ${ADDITIONAL_LIBRARIES})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})
