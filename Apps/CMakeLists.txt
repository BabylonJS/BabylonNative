if((WIN32 AND NOT WINDOWS_STORE) AND GRAPHICS_API STREQUAL D3D11)
    add_subdirectory(HeadlessScreenshotApp)
    add_subdirectory(StyleTransferApp)
endif()

if(NOT ANDROID)
    add_subdirectory(Playground)
endif()

if((WIN32 AND NOT WINDOWS_STORE) OR (APPLE AND NOT IOS AND NOT VISIONOS) OR (UNIX AND NOT ANDROID AND NOT APPLE))
    add_subdirectory(UnitTests)
endif()

npm(install --silent)
