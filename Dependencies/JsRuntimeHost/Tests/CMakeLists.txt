if (NOT WINDOWS_STORE)
    add_subdirectory(UnitTests)
    npm(install --silent)
endif()