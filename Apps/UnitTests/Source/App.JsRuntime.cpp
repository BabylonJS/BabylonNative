// Host for the unit tests when BABYLON_NATIVE_UNIT_TESTS_GRAPHICS is off, i.e.
// when only the JsRuntimeHost suites are built (iOS, UWP, and any desktop build
// that opts out). There is no window and no graphics device to configure, so
// gtest is driven directly.
//
// The graphics builds use Source/App.cpp plus the per-platform App.Win32.cpp /
// App.Apple.mm / App.X11.cpp instead, which have to stand up a
// Babylon::Graphics::Device on a real surface first.
//
// Android has no main() at all: the Gradle harness in Android/ calls gtest from
// app/src/main/cpp/JNI.cpp.

#include <Babylon/DebugTrace.h>
#include <gtest/gtest.h>

#include <cstdio>
#include <iostream>

#ifdef _WIN32
#include <winapifamily.h>
#include <Windows.h>
#endif

#ifdef __APPLE__
#include <TargetConditionals.h>
#endif

int main(int argc, char* argv[])
{
#ifdef _WIN32
#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
    SetConsoleOutputCP(CP_UTF8);
#endif
#endif

    Babylon::DebugTrace::EnableDebugTrace(true);
    Babylon::DebugTrace::SetTraceOutput([](const char* trace) {
#ifdef _WIN32
        OutputDebugStringA(trace);
        OutputDebugStringA("\n");
#endif
        printf("%s\n", trace);
        fflush(stdout);
    });

    testing::InitGoogleTest(&argc, argv);
    const int exitCode = RUN_ALL_TESTS();

#if defined(__APPLE__) && TARGET_OS_IPHONE
    // `simctl launch` does not surface the app's exit code, so CI reads it back
    // off stderr instead.
    std::cerr << exitCode << std::endl;
    std::cerr.flush();
#endif

    return exitCode;
}
