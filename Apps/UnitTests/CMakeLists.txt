if((NOT WIN32) OR WINDOWS_STORE)
    message(FATAL_ERROR "Unsupported platform: ${CMAKE_SYSTEM_NAME}")
endif()

include(FetchContent)
FetchContent_Declare(
  googletest
  # Specify the commit you depend on and update it regularly.
  URL https://github.com/google/googletest/archive/23ef29555ef4789f555f1ba8c51b4c52975f0907.zip
)
FetchContent_MakeAvailable(googletest)

add_executable(UnitTests "UnitTests.cpp")

target_link_libraries(UnitTests
    gtest_main
    UrlLib)

add_test(NAME UnitTests COMMAND UnitTests)

set_property(TARGET UnitTests PROPERTY FOLDER Apps)
set_property(TARGET gtest gtest_main gmock gmock_main PROPERTY FOLDER "Dependencies/GoogleTest")