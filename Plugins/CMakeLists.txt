# Add NativeCapture
add_subdirectory(NativeCapture)

# Add NativeInput
add_subdirectory(NativeInput)

# Add NativeEngine
add_subdirectory(NativeEngine)

# Add NativeXr
# Dependencies: NativeEngine
add_subdirectory(NativeXr)

if (APPLE OR ANDROID OR (WIN32 AND NOT WINDOWS_STORE))
    add_subdirectory(NativeCamera)
endif()
