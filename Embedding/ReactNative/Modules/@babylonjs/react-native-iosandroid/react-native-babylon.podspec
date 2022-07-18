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

  s.libraries = 'astc-codec',
                'astc',
                'etc1',
                'etc2',
                'nvtt',
                'squish',
                'pvrtc',
                'iqa',
                'edtaa3',
                'BabylonNative',
                'bgfx',
                'bimg',
                'bx',
                'Canvas',
                'GenericCodeGen',
                'glslang',
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
                'NativeXR',
                'SPIRV',
                'spirv-cross-core',
                'spirv-cross-glsl',
                'spirv-cross-msl',
                'tinyexr',
                'UrlLib',
                'Window',
                'XMLHttpRequest',
                'xr'

  s.frameworks = "MetalKit", "ARKit"

  s.dependency "React"
end

