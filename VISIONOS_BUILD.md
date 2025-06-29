# VisionOS Build Instructions

This document explains how to build the BabylonNative Playground app for VisionOS with proper code signing configuration.

## Prerequisites

- Xcode 15.0+ with VisionOS SDK
- Apple Developer Account
- VisionPro device for testing

## Build Commands

### Basic Build (Manual Code Signing)
```bash
cmake -G Xcode -B build_visionos \
  -DCMAKE_SYSTEM_NAME=visionOS \
  -DCMAKE_OSX_DEPLOYMENT_TARGET=1.0 \
  -DCMAKE_XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY="7" \
  -DCMAKE_OSX_ARCHITECTURES=arm64 \
  -DBABYLON_NATIVE_PLUGIN_NATIVECAMERA=OFF
```

### Automatic Code Signing (Recommended)
```bash
cmake -G Xcode -B build_visionos \
  -DCMAKE_SYSTEM_NAME=visionOS \
  -DCMAKE_OSX_DEPLOYMENT_TARGET=1.0 \
  -DCMAKE_XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY="7" \
  -DCMAKE_OSX_ARCHITECTURES=arm64 \
  -DBABYLON_NATIVE_PLUGIN_NATIVECAMERA=OFF \
  -DCMAKE_XCODE_ATTRIBUTE_DEVELOPMENT_TEAM="Andrew Fillebrown" \
  -DCMAKE_XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER="com.yourcompany.BabylonPlayground.visionOS"
```

### Using Default Settings
```bash
cmake -G Xcode -B build_visionos \
  -DCMAKE_SYSTEM_NAME=visionOS \
  -DCMAKE_OSX_DEPLOYMENT_TARGET=1.0 \
  -DCMAKE_XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY="7" \
  -DCMAKE_OSX_ARCHITECTURES=arm64 \
  -DBABYLON_NATIVE_PLUGIN_NATIVECAMERA=OFF
# Uses default team "Andrew Fillebrown" and bundle ID "com.BabylonNative.Playground.visionOS"
```

## Code Signing Variables

| Variable | Description | Default | Example |
|----------|-------------|---------|---------|
| `CMAKE_XCODE_ATTRIBUTE_DEVELOPMENT_TEAM` | Apple Developer Team name or ID | `"Andrew Fillebrown"` | `"Your Team Name"` |
| `CMAKE_XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER` | Unique app bundle identifier | `"com.BabylonNative.Playground.visionOS"` | `"com.yourcompany.app.visionOS"` |

### Finding Your Development Team ID

1. Open Xcode
2. Go to Xcode → Preferences → Accounts
3. Select your Apple ID
4. Find your team name and copy the Team ID (10-character alphanumeric string)

## Build and Run

1. Generate the Xcode project using one of the commands above
2. Open the generated project:
   ```bash
   open build_visionos/BabylonNative.xcodeproj
   ```
3. Select the **Playground** scheme
4. Choose your VisionPro device as the target
5. Build and run (⌘+R)

## Features

- ✅ WebXR immersive-vr session support using CompositorServices
- ✅ Full spatial tracking and immersive experiences
- ✅ Automatic code signing configuration
- ✅ VisionOS-optimized platform implementation

## Troubleshooting

### Code Signing Issues
If you see "Signing for Playground requires a development team":
1. Ensure you provided a valid `CMAKE_XCODE_ATTRIBUTE_DEVELOPMENT_TEAM`
2. Check that your Apple Developer account is properly configured in Xcode
3. Verify your bundle identifier is unique and follows reverse DNS notation

### Camera API Errors
The NativeCamera plugin is automatically disabled for VisionOS builds since camera APIs are not available on the platform.

### Build Errors
1. Clean the build directory: `rm -rf build_visionos`
2. Regenerate with the latest CMake configuration
3. Ensure you're targeting a VisionPro device, not the simulator