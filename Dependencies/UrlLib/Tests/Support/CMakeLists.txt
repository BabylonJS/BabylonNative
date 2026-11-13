# Loopback HTTP server shared by the UrlLib and JsRuntimeHost unit tests. It is not part of the
# UrlLib library and is only built when a test target links it.
add_library(UrlLibTestSupport STATIC EXCLUDE_FROM_ALL
    "Include/UrlLib/TestSupport/LoopbackHttpServer.h"
    "Source/LoopbackHttpServer.cpp")

target_include_directories(UrlLibTestSupport PUBLIC "Include")

if(WIN32)
    target_link_libraries(UrlLibTestSupport PUBLIC ws2_32)
endif()

set_property(TARGET UrlLibTestSupport PROPERTY FOLDER Tests)
