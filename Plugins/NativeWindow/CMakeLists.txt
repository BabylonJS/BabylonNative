set(SOURCES
    "Source/NativeWindow.cpp")

add_library(NativeWindow 
    "Include/Babylon/NativeWindow.h"
    "Source/NativeWindow.cpp")

target_include_directories(NativeWindow PRIVATE "Include/Babylon")
target_include_directories(NativeWindow INTERFACE "Include")

target_link_libraries(NativeWindow 
    PRIVATE base-n
    PUBLIC Library)

set_property(TARGET NativeWindow PROPERTY FOLDER Plugins/Babylon)