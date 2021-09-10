if(NOT ANDROID)
    add_subdirectory(Playground)
    add_subdirectory(ValidationTests)
endif()

if((WIN32 AND NOT WINDOWS_STORE) OR (APPLE AND NOT IOS))
    add_subdirectory(UnitTests)
endif()