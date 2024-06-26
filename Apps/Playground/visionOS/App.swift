import SwiftUI
import CompositorServices

struct ContentStageConfiguration: CompositorLayerConfiguration {
  func makeConfiguration(capabilities: LayerRenderer.Capabilities, configuration: inout LayerRenderer.Configuration) {
    configuration.depthFormat = .depth32Float
    configuration.colorFormat = .bgra8Unorm_srgb

    let foveationEnabled = capabilities.supportsFoveation
    configuration.isFoveationEnabled = foveationEnabled

    let options: LayerRenderer.Capabilities.SupportedLayoutsOptions = foveationEnabled ? [.foveationEnabled] : []
    let supportedLayouts = capabilities.supportedLayouts(options: options)

    configuration.layout = supportedLayouts.contains(.layered) ? .layered : .dedicated
  }
}

class Renderer {
  let layerRenderer: LayerRenderer
  var libNativeBridge: LibNativeBridge

  init(_ layerRenderer: LayerRenderer) {
    self.layerRenderer = layerRenderer
    self.libNativeBridge = LibNativeBridge(layerRenderer)
  }

  func startRenderLoop() {
      let renderThread = Thread {
        self.renderLoop()
      }
      renderThread.name = "Render Thread"
      renderThread.start()
  }


  func renderLoop() {
    while true {
      if layerRenderer.state == .invalidated {
        print("Layer is invalidated")

        libNativeBridge.shutdown()
        return
      } else if layerRenderer.state == .paused {
        layerRenderer.waitUntilRunning()
        continue
      } else {
        autoreleasepool {
          libNativeBridge.initialize()
          libNativeBridge.render()
        }
      }
    }
  }
}


@main
struct ExampleApp: App {
  @State private var showImmersiveSpace = false
  @State private var immersiveSpaceIsShown = false

  @Environment(\.openImmersiveSpace) var openImmersiveSpace
  @Environment(\.dismissImmersiveSpace) var dismissImmersiveSpace

  var body: some Scene {
    WindowGroup {
      VStack {
        Toggle("Show Immersive Space", isOn: $showImmersiveSpace)
          .toggleStyle(.button)
          .padding(.top, 50)
      }
      .onChange(of: showImmersiveSpace) { _, newValue in
        Task {
          if newValue {
            switch await openImmersiveSpace(id: "ImmersiveSpace") {
            case .opened:
              immersiveSpaceIsShown = true
            case .error, .userCancelled:
              fallthrough
            @unknown default:
              immersiveSpaceIsShown = false
              showImmersiveSpace = false
            }
          } else if immersiveSpaceIsShown {
            await dismissImmersiveSpace()
            immersiveSpaceIsShown = false
          }
        }
      }

    }
    ImmersiveSpace(id: "ImmersiveSpace") {
      CompositorLayer(configuration: ContentStageConfiguration()) { layerRenderer in
        let renderer = Renderer(layerRenderer)
        renderer.startRenderLoop()
      }
    }.immersionStyle(selection: .constant(.full), in: .full)
  }
}
