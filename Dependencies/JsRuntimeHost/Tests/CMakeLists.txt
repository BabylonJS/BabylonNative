if (NOT WINDOWS_STORE)
    add_subdirectory(UnitTests)
    npm(install ${CMAKE_CURRENT_SOURCE_DIR} "UnitTests")
endif()