if(NOT ANDROID)
    add_subdirectory(Playground)
endif()

if(WIN32 OR UNIX AND NOT APPLE AND NOT ANDROID AND NOT WINDOWS_STORE) # TODO: Chakra only for win32?
    add_subdirectory(ValidationTests)
endif()
