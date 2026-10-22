FetchContent_Declare(googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0)

if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
endif()
FetchContent_MakeAvailable(googletest)
set_property(TARGET gtest PROPERTY FOLDER Dependencies)
set_property(TARGET gtest_main PROPERTY FOLDER Dependencies)

if(ANDROID)
    add_library(UrlLibTests SHARED UrlRequestErrorReporting.cpp SchemeResolver.cpp)
else()
    add_executable(UrlLibTests UrlRequestErrorReporting.cpp SchemeResolver.cpp)
endif()

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

if(ANDROID OR IOS)
    include("${CMAKE_CURRENT_LIST_DIR}/../../../Apps/UnitTests/Mobile/Packaging.cmake")
    package_mobile_unit_tests(UrlLibTests com.urllib.unittests)
else()
    add_test(NAME UrlLibTests COMMAND UrlLibTests)
endif()
