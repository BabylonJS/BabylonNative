FetchContent_Declare(googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0)

set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
set_property(TARGET gtest PROPERTY FOLDER Dependencies)
set_property(TARGET gtest_main PROPERTY FOLDER Dependencies)

add_executable(UrlLibTests UrlRequestErrorReporting.cpp)

target_link_libraries(UrlLibTests
    PRIVATE UrlLib
    PRIVATE GTest::gtest_main)

if(APPLE)
    # The UrlLib static library's NSURLSession-based backend needs Foundation at link time.
    target_link_libraries(UrlLibTests PRIVATE "-framework Foundation")
elseif(WIN32)
    # The RefusingPort test fixture uses Winsock directly.
    target_link_libraries(UrlLibTests PRIVATE ws2_32)
endif()

set_property(TARGET UrlLibTests PROPERTY FOLDER Tests)

add_test(NAME UrlLibTests COMMAND UrlLibTests)
