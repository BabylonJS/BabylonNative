# Add NativeCapture
add_subdirectory(NativeCapture)

# Add NativeInput
add_subdirectory(NativeInput)

# Add NativeEngine
add_subdirectory(NativeEngine)

# Add NativeXr
# Dependencies: NativeEngine
add_subdirectory(NativeXr)

if (APPLE OR ANDROID OR WIN32)
    add_subdirectory(NativeCamera)
endif()
