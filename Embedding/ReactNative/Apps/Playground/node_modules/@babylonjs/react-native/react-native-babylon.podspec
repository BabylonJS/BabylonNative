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
  s.exclude_files = "ios/BabylonNative.h"
  s.requires_arc = true

  s.libraries = 'astc-codec',
                'astc',
                'BabylonNative',
                'bgfx',
                'bimg',
                'bx',
                'GenericCodeGen',
                'glslang',
                'jsRuntime',
                'OGLCompiler',
                'OSDependent',
                'MachineIndependent',
                'napi',
                'NativeEngine',
                'NativeInput',
                'NativeWindow',
                'NativeXR',
                'SPIRV',
                'spirv-cross-core',
                'spirv-cross-glsl',
                'spirv-cross-hlsl',
                'spirv-cross-msl',
                'Window',
                'xr'

  s.frameworks = "MetalKit", "ARKit"

  s.dependency "React"
end

