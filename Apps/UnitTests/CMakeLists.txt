if((NOT WIN32) OR WINDOWS_STORE)
    message(FATAL_ERROR "Unsupported platform: ${CMAKE_SYSTEM_NAME}")
endif()

include(FetchContent)
FetchContent_Declare(
  googletest
  # Specify the commit you depend on and update it regularly.
  URL https://github.com/google/googletest/archive/23ef29555ef4789f555f1ba8c51b4c52975f0907.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(UnitTests "UnitTests.cpp")

target_compile_definitions(UnitTests PRIVATE UNICODE)
target_compile_definitions(UnitTests PRIVATE _UNICODE)

target_include_directories(UnitTests PRIVATE "Source" ".")

target_link_libraries(UnitTests
    gtest_main
    UrlLib)


add_test(NAME UnitTests COMMAND UnitTests)

set_property(TARGET UnitTests PROPERTY FOLDER Apps)