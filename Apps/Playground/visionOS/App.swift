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

struct ImmersiveMetalViewRepresentable: UIViewRepresentable {
  typealias UIViewType = MetalView
  
  func makeUIView(context: Context) -> MetalView {
    let metalView = MetalView(frame: .zero)
    // Ensure the bridge is initialized for immersive rendering
    if let bridge = LibNativeBridge.sharedInstance() {
      // Set a larger viewport for immersive space
      bridge.drawableWillChangeSize(withWidth: 1920, height: 1080)
    }
    return metalView
  }
  
  func updateUIView(_ uiView: MetalView, context: Context) {
    // Trigger a render in immersive mode
    if let bridge = LibNativeBridge.sharedInstance() {
      bridge.render()
    }
  }
}

struct ImmersiveView: View {
  @Binding var immersiveSpaceIsShown: Bool
  @Environment(\.dismissImmersiveSpace) var dismissImmersiveSpace
  @Environment(\.openWindow) private var openWindow
  
  var body: some View {
    ZStack {
      // Use MetalView for 3D rendering in immersive space
      ImmersiveMetalViewRepresentable()
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
      
      VStack {
        Spacer()
        HStack {
          Spacer()
          Button("Exit Immersive Space") {
            Task { @MainActor in
              await dismissImmersiveSpace()
              immersiveSpaceIsShown = false
              openWindow(id: "MainWindow")
              LibNativeBridge.sharedInstance()?.exitImmersiveMode()
              print("📱 Exited immersive space")
            }
          }
          .padding()
          .background(.regularMaterial)
          .cornerRadius(10)
        }
        .padding()
      }
    }
  }
}


struct ContentView: View {
  @Binding var immersiveSpaceIsShown: Bool
  @State private var immersiveSpaceState = ImmersiveSpaceState.closed
  @Environment(\.openImmersiveSpace) var openImmersiveSpace
  @Environment(\.dismissImmersiveSpace) var dismissImmersiveSpace
  @Environment(\.dismissWindow) private var dismissWindow
  
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
              immersiveSpaceIsShown = true
              dismissWindow(id: "MainWindow")
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
              immersiveSpaceIsShown = false
              LibNativeBridge.sharedInstance()?.exitImmersiveMode()
              print("📱 Exited immersive space")
            }
          }
          .padding()
        }
      }
      .padding()
    }
    .onAppear {
      // Auto-enter immersive space for demo
      Task { @MainActor in
        try? await Task.sleep(nanoseconds: 2_000_000_000) // Wait 2 seconds
        print("🚀 Auto-entering immersive space...")
        let result = await openImmersiveSpace(id: "BabylonImmersiveSpace")
        switch result {
        case .opened:
          print("✅ Immersive space opened automatically")
          immersiveSpaceState = .open
          immersiveSpaceIsShown = true
          dismissWindow(id: "MainWindow")
        case .error:
          print("❌ Error opening immersive space automatically")
        case .userCancelled:
          print("🚫 User cancelled immersive space automatically")
        @unknown default:
          print("❓ Unknown result opening immersive space automatically")
        }
      }
    }
  }
}

enum ImmersiveSpaceState {
    case closed
    case open
}

@main
struct ExampleApp: App {
  @State private var immersiveSpaceIsShown = false
  @Environment(\.dismissWindow) private var dismissWindow
  
  var body: some SwiftUI.Scene {
    WindowGroup(id: "MainWindow") {
      ContentView(immersiveSpaceIsShown: $immersiveSpaceIsShown)
    }
    .windowStyle(.plain)
    .windowResizability(.contentSize)
    
    ImmersiveSpace(id: "BabylonImmersiveSpace") {
      ImmersiveView(immersiveSpaceIsShown: $immersiveSpaceIsShown)
    }
    .immersionStyle(selection: .constant(.full), in: .full)
  }
}
