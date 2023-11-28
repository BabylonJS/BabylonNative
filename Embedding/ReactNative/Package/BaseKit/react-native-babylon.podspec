require "json"

package = JSON.parse(File.read(File.join(__dir__, "package.json")))

Pod::Spec.new do |s|
  s.name         = "react-native-babylon"
  s.version      = package["version"]
  s.summary      = package["description"]
  s.homepage     = package["homepage"]
  s.license      = package["license"]
  s.authors      = package["author"]

  s.platforms    = { :ios => "12.0" }
  s.source       = { :git => package["repository"]["url"], :tag => s.version }

  s.source_files = "ios/**/*.{h,m,mm}"
  s.requires_arc = true
  s.xcconfig     = { 'USER_HEADER_SEARCH_PATHS' => '$(inherited) ${PODS_TARGET_SRCROOT}/shared ${PODS_TARGET_SRCROOT}/../react-native/shared' }

  s.vendored_libraries = 'ios/libs/libastc-encoder.a',
    'ios/libs/libBabylonNative.a',
    'ios/libs/libbgfx.a',
    'ios/libs/libbimg.a',
    'ios/libs/libbx.a',
    'ios/libs/libCanvas.a',
    'ios/libs/libedtaa3.a',
    'ios/libs/libetc1.a',
    'ios/libs/libetc2.a',
    'ios/libs/libGenericCodeGen.a',
    'ios/libs/libglslang-default-resource-limits.a',
    'ios/libs/libglslang.a',
    'ios/libs/libGraphics.a',
    'ios/libs/libiqa.a',
    'ios/libs/libJsRuntime.a',
    'ios/libs/libMachineIndependent.a',
    'ios/libs/libnapi.a',
    'ios/libs/libNativeCapture.a',
    'ios/libs/libNativeEngine.a',
    'ios/libs/libNativeInput.a',
    'ios/libs/libNativeOptimizations.a',
    'ios/libs/libNativeTracing.a',
    'ios/libs/libnvtt.a',
    'ios/libs/libOGLCompiler.a',
    'ios/libs/libOSDependent.a',
    'ios/libs/libpvrtc.a',
    'ios/libs/libspirv-cross-core.a',
    'ios/libs/libspirv-cross-glsl.a',
    'ios/libs/libspirv-cross-msl.a',
    'ios/libs/libSPIRV.a',
    'ios/libs/libsquish.a',
    'ios/libs/libtinyexr.a',
    'ios/libs/libUrlLib.a',
    'ios/libs/libWindow.a',
    'ios/libs/libXMLHttpRequest.a'

  s.frameworks = "MetalKit"

  s.dependency "React"
end