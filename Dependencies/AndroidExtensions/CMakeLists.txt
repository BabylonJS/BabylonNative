set(SOURCES
    "Include/AndroidExtensions/Globals.h"
    "Include/AndroidExtensions/JavaWrappers.h"
    "Include/AndroidExtensions/OpenGLHelpers.h"
    "Include/AndroidExtensions/Permissions.h"
    "Source/Globals.cpp"
    "Source/JavaWrappers.cpp"
    "Source/OpenGLHelpers.cpp"
    "Source/Permissions.cpp")

add_library(AndroidExtensions ${SOURCES})
warnings_as_errors(AndroidExtensions)

target_link_to_dependencies(AndroidExtensions 
    PUBLIC arcana)

target_include_directories(AndroidExtensions PUBLIC "Include")
