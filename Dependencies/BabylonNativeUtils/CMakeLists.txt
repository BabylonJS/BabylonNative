if(APPLE)
    set(SOURCES
        "Include/Babylon/NetworkUtils.h"
        "Include/Babylon/TicketedCollection.h"
        "Source/NetworkUtils.mm")
else()
    set(SOURCES
        "Include/Babylon/NetworkUtils.h"
        "Include/Babylon/TicketedCollection.h"
        "Source/NetworkUtils.cpp")
endif()

add_library(BabylonNativeUtils ${SOURCES})

target_include_directories(BabylonNativeUtils PRIVATE "Include/Babylon")
target_include_directories(BabylonNativeUtils INTERFACE "Include")

if(WIN32 OR WINDOWS_STORE)
    target_link_libraries(BabylonNativeUtils
        PUBLIC arcana
        PRIVATE libcurl)
else()
    target_link_libraries(BabylonNativeUtils
        PUBLIC arcana)
endif()

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})
