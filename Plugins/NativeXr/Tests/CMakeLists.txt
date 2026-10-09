cmake_minimum_required(VERSION 3.21)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    project(NativeXrShutdownTests LANGUAGES CXX)
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    if(POLICY CMP0135)
        cmake_policy(SET CMP0135 NEW)
    endif()
    include(FetchContent)
    FetchContent_Declare(arcana.cpp
        GIT_REPOSITORY https://github.com/microsoft/arcana.cpp.git
        GIT_TAG fe41b17dedea47b5dead98f0271ca284e94ac6a8)
    FetchContent_Declare(googletest
        URL "https://github.com/google/googletest/archive/refs/tags/v1.17.0.tar.gz")
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(arcana.cpp googletest)
    enable_testing()
    add_executable(NativeXrShutdownTests "Tests.Shutdown.cpp")
    add_test(NAME NativeXrShutdownTests COMMAND NativeXrShutdownTests)
    set(TEST_TARGET NativeXrShutdownTests)
else()
    target_sources(UnitTests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/Tests.Shutdown.cpp")
    set(TEST_TARGET UnitTests)
endif()

# Compile the production lifecycle methods with a deterministic backend, without XR hardware.
set(IMPL_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/../Source/NativeXrImpl.cpp")
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${IMPL_SOURCE}")
file(READ "${IMPL_SOURCE}" IMPL)
string(FIND "${IMPL}" "arcana::task<void, std::exception_ptr> NativeXr::Impl::BeginSessionAsync()" BEGIN_OFFSET)
string(FIND "${IMPL}" "void NativeXr::Impl::ScheduleFrame(" END_OFFSET)
if(BEGIN_OFFSET LESS 0 OR END_OFFSET LESS BEGIN_OFFSET)
    message(FATAL_ERROR "Cannot locate NativeXr session lifecycle methods")
endif()
math(EXPR METHODS_LENGTH "${END_OFFSET} - ${BEGIN_OFFSET}")
string(SUBSTRING "${IMPL}" ${BEGIN_OFFSET} ${METHODS_LENGTH} SESSION_METHODS)
file(CONFIGURE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/NativeXrSessionMethods.inc"
    CONTENT "@SESSION_METHODS@" @ONLY)

target_include_directories(${TEST_TARGET} PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
target_link_libraries(${TEST_TARGET} PRIVATE arcana gtest_main)
