if(NOT ANDROID)
    add_subdirectory(Playground)
    add_subdirectory(ValidationTests)
    add_subdirectory(NativeTextureApp)
endif()

if((WIN32 AND NOT WINDOWS_STORE) OR (APPLE AND NOT IOS) OR (UNIX AND NOT ANDROID))
    add_subdirectory(UnitTests)
endif()