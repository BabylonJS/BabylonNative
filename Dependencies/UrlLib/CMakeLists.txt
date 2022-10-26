if(ANDROID)
    set(ADDITIONAL_SOURCES
        "Source/UrlRequest_Android.cpp")
    set(ADDITIONAL_LIBRARIES
        PRIVATE AndroidExtensions)
elseif(APPLE)
    set(ADDITIONAL_SOURCES
        "Source/UrlRequest_Apple.mm")
elseif(UNIX)
    set(ADDITIONAL_SOURCES
        "Source/UrlRequest_Unix.cpp")
    set(ADDITIONAL_LIBRARIES
        PRIVATE curl)
elseif(WIN32)
    set(ADDITIONAL_SOURCES
        "Source/UrlRequest_Windows.cpp")
    if(NOT WINDOWS_STORE)
        set(ADDITIONAL_LIBRARIES
            PUBLIC onecoreuap.lib)
    endif()
else()
    message(FATAL_ERROR "Unrecognized platform: ${CMAKE_SYSTEM_NAME}")
endif()

set(SOURCES
    "Include/UrlLib/UrlLib.h"
    "Source/UrlRequest_Base.h"
    "Source/UrlRequest_Shared.h"
    ${ADDITIONAL_SOURCES})

add_library(UrlLib ${SOURCES})
warnings_as_errors(UrlLib)

target_include_directories(UrlLib PUBLIC "Include")
target_include_directories(UrlLib PRIVATE "Source")

target_link_libraries(UrlLib
    PUBLIC arcana
    ${ADDITIONAL_LIBRARIES})
    
if(APPLE)
    set_target_properties(UrlLib PROPERTIES
        XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC YES)
    set_property(TARGET UrlLib PROPERTY UNITY_BUILD false)
endif()

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})
