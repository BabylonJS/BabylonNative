import SwiftUI
import RealityKit

class MetalView: UIView {
  override init(frame: CGRect) {
    super.init(frame: frame)
    self.backgroundColor = .clear
  }
  
  required init?(coder: NSCoder) {
    fatalError("init(coder:) has not been implemented")
  }
  
  func setupMetalLayer() {
    guard let bridge = LibNativeBridge.sharedInstance() else { return }
    
    if bridge.metalLayer != nil {
      return
    }
    
    self.addGestureRecognizer(
      UIBabylonGestureRecognizer(
        target: self,
        onTouchDown: bridge.setTouchDown,
        onTouchMove: bridge.setTouchMove,
        onTouchUp: bridge.setTouchUp
      )
    )
    metalLayer.pixelFormat = .bgra8Unorm
    metalLayer.framebufferOnly = true
    
    bridge.metalLayer = self.metalLayer
    
    let scale = UITraitCollection.current.displayScale
    bridge.initialize(withWidth: Int(self.bounds.width * scale), height: Int(self.bounds.height * scale))
  }
  
  var metalLayer: CAMetalLayer {
    return layer as! CAMetalLayer
  }
  
  override class var layerClass: AnyClass {
    return CAMetalLayer.self
  }
  
  override func layoutSubviews() {
    super.layoutSubviews()
    setupMetalLayer()
    updateDrawableSize()
  }
  
  private func updateDrawableSize() {
    let scale = UITraitCollection.current.displayScale
    LibNativeBridge.sharedInstance().drawableWillChangeSize(withWidth: Int(bounds.width * scale), height: Int(bounds.height * scale))
    metalLayer.drawableSize = CGSize(width: bounds.width * scale, height: bounds.height * scale)
  }
}

struct MetalViewRepresentable: UIViewRepresentable {
  typealias UIViewType = MetalView
  
  func makeUIView(context: Context) -> MetalView {
    MetalView(frame: .zero)
  }
  
  func updateUIView(_ uiView: MetalView, context: Context) {}
}

struct ImmersiveView: View {
  var body: some View {
    RealityView { content in
      // Create a simple entity as a placeholder
      let entity = Entity()
      content.add(entity)
    }
    .onAppear {
      // Initialize immersive mode when the view appears
      if let bridge = LibNativeBridge.sharedInstance() {
        bridge.initializeImmersiveMode()
      }
    }
    .onDisappear {
      // Exit immersive mode when view disappears
      if let bridge = LibNativeBridge.sharedInstance() {
        bridge.exitImmersiveMode()
      }
    }
  }
}


struct ContentView: View {
  @State private var immersiveSpaceState = ImmersiveSpaceState.closed
  @Environment(\.openImmersiveSpace) var openImmersiveSpace
  @Environment(\.dismissImmersiveSpace) var dismissImmersiveSpace
  
  var body: some View {
    VStack {
      MetalViewRepresentable()
        .frame(maxWidth: .infinity, maxHeight: .infinity)
      
      HStack {
        Button("Enter Immersive Space") {
          Task { @MainActor in
            print("🚀 Attempting to open immersive space...")
            let result = await openImmersiveSpace(id: "BabylonImmersiveSpace")
            switch result {
            case .opened:
              print("✅ Immersive space opened successfully")
              immersiveSpaceState = .open
            case .error:
              print("❌ Error opening immersive space")
            case .userCancelled:
              print("🚫 User cancelled immersive space")
            @unknown default:
              print("❓ Unknown result opening immersive space")
            }
          }
        }
        .disabled(immersiveSpaceState == .open)
        .padding()
        
        if immersiveSpaceState == .open {
          Button("Exit Immersive Space") {
            Task { @MainActor in
              await dismissImmersiveSpace()
              immersiveSpaceState = .closed
              LibNativeBridge.sharedInstance()?.exitImmersiveMode()
              print("📱 Exited immersive space")
            }
          }
          .padding()
        }
      }
      .padding()
    }
  }
}

enum ImmersiveSpaceState {
    case closed
    case open
}

@main
struct ExampleApp: App {
  var body: some SwiftUI.Scene {
    WindowGroup {
      ContentView()
    }
    
    ImmersiveSpace(id: "BabylonImmersiveSpace") {
      ImmersiveView()
    }
    .immersionStyle(selection: .constant(.full), in: .full)
  }
}
