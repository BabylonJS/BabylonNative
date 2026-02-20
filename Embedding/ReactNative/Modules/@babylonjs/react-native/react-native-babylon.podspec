require "json"

package = JSON.parse(File.read(File.join(__dir__, "package.json")))

# This Podspec is used for local development

podspec_dir = File.dirname(__FILE__)
base_path = File.expand_path('../../../node_modules/@babylonjs/react-native/Build/iOS', podspec_dir)
base_path_symlink = File.expand_path('../../../../../Modules/@babylonjs/react-native/Build/iOS', podspec_dir)
base = "$(inherited) #{base_path}/\${CONFIGURATION}-\${PLATFORM_NAME} #{base_path_symlink}/\${CONFIGURATION}-\${PLATFORM_NAME}"

items = ['/shared/BabylonNative/Repo/Polyfills/Canvas', 
  '/shared/BabylonNative/Repo/Polyfills/Window', 
  '/shared/BabylonNative/Repo/Plugins/ExternalTexture', 
  '/shared/BabylonNative/Repo/Plugins/NativeCamera', 
  '/shared/BabylonNative/Repo/Plugins/NativeCapture', 
  '/shared/BabylonNative/Repo/Plugins/NativeEngine',
  '/shared/BabylonNative/Repo/Plugins/NativeInput', 
  '/shared/BabylonNative/Repo/Plugins/NativeOptimizations', 
  '/shared/BabylonNative/Repo/Plugins/NativeTracing', 
  '/shared/BabylonNative/Repo/Plugins/NativeXr', 
  '/shared/BabylonNative/Repo/Plugins/ShaderCache', 
  '/shared/BabylonNative/Repo/Plugins/ShaderCompiler',
  '/shared/BabylonNative/Repo/Core/Graphics',
  '/shared/BabylonNative/Repo/Dependencies/xr', 
  '/_deps/bgfx.cmake-build/cmake/bgfx',
  '/_deps/bgfx.cmake-build/cmake/bimg',
  '/_deps/bgfx.cmake-build/cmake/bx',
  '/_deps/glslang-build/glslang',
  '/_deps/glslang-build/glslang/OSDependent/Unix',
  '/_deps/glslang-build/OGLCompilersDLL',
  '/_deps/glslang-build/SPIRV',
  '/_deps/urllib-build',
  '/_deps/libwebp-build',
  '/_deps/jsruntimehost-build/Polyfills/XMLHttpRequest',
  '/_deps/jsruntimehost-build/Polyfills/Scheduling',
  '/_deps/jsruntimehost-build/Core/JsRuntime',
  '/_deps/jsruntimehost-build/Core/Node-API-JSI',
  '/_deps/jsruntimehost-build/Core/Foundation',
  '/_deps/spirv-cross-build']


all_items = items.map { |item| "#{base_path}#{item}/\${CONFIGURATION}-\${PLATFORM_NAME}" } +
            items.map { |itemb| "#{base_path_symlink}#{itemb}/\${CONFIGURATION}-\${PLATFORM_NAME}" }

result = base + " " + all_items.join(" ")

Pod::Spec.new do |s|
  s.name         = "react-native-babylon"
  s.version      = package["version"]
  s.summary      = package["description"]
  s.homepage     = package["homepage"]
  s.license      = package["license"]
  s.authors      = package["author"]

  s.platforms    = { :ios => "12.0" }
  s.source       = { :git => package["repository"]["url"], :tag => s.version }

  s.source_files = "ios/*.{h,m,mm}"
  s.requires_arc = true
  s.xcconfig     = { 'USER_HEADER_SEARCH_PATHS' => '$(inherited) ${PODS_TARGET_SRCROOT}/shared ${PODS_TARGET_SRCROOT}/../react-native/shared',
  'LIBRARY_SEARCH_PATHS'=>  result}

  s.vendored_frameworks = "ios/libs/*.xcframework"

  frameworks = ['MetalKit', 'AVFoundation', 'CoreMedia']

  if ENV['BABYLON_NATIVE_PLUGIN_NATIVEXR'] != '0'
    frameworks << 'ARKit'
  end
  s.frameworks = frameworks

  libs = ['BabylonNative',
          'bgfx',
          'bimg',
          'bimg_encode',
          'bimg_decode',
          'minz',
          'Scheduling',
          'bx',
          'Canvas',
          'GenericCodeGen',
          'glslang',
          'glslang-default-resource-limits',
          'Graphics',
          'jsRuntime',
          'OGLCompiler',
          'OSDependent',
          'MachineIndependent',
          'napi',
          'NativeCapture',
          'NativeEngine',
          'NativeInput',
          'NativeOptimizations',
          'NativeTracing',
          'Foundation',
          'SPIRV',
          'spirv-cross-core',
          'spirv-cross-msl',
          'UrlLib',
          'Window',
          'XMLHttpRequest',
          'ShaderCache',
          'ShaderCompiler',
          'webp',
          'sharpyuv']

  if ENV['BABYLON_NATIVE_PLUGIN_NATIVECAMERA'] != '0'
    libs << 'NativeCamera'
  end
  if ENV['BABYLON_NATIVE_PLUGIN_NATIVEXR'] != '0'
    libs << 'NativeXR'
    libs << 'xr'
  end
  s.libraries = libs

  # install_modules_dependencies has been defined in RN 0.70
  # This check ensure that the library can work on older versions of RN
  if defined?(install_modules_dependencies)
    install_modules_dependencies(s)
  else
    s.dependency "React-Core"

    # Don't install the dependencies when we run `pod install` in the old architecture.
    if new_arch_enabled then
      s.compiler_flags = folly_compiler_flags + " -DRCT_NEW_ARCH_ENABLED=1"
      s.pod_target_xcconfig    = {
        "HEADER_SEARCH_PATHS" => "\"$(PODS_ROOT)/boost\"",
        "OTHER_CPLUSPLUSFLAGS" => "-DFOLLY_NO_CONFIG -DFOLLY_MOBILE=1 -DFOLLY_USE_LIBCPP=1",
        "CLANG_CXX_LANGUAGE_STANDARD" => "c++17"
      }
      s.dependency "React-Codegen"
      s.dependency "RCT-Folly"
      s.dependency "RCTRequired"
      s.dependency "RCTTypeSafety"
      s.dependency "ReactCommon/turbomodule/core"
      s.dependency "React-RCTFabric"
    end
  end
end

