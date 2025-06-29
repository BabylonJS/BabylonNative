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

class ImmersiveMetalView: UIView {
  override init(frame: CGRect) {
    super.init(frame: frame)
    setupImmersiveView()
  }
  
  required init?(coder: NSCoder) {
    super.init(coder: coder)
    setupImmersiveView()
  }
  
  private func setupImmersiveView() {
    // Set background to clear for immersive space
    backgroundColor = .clear
    
    // Setup metal layer properties  
    metalLayer.pixelFormat = .bgra8Unorm
    metalLayer.framebufferOnly = true
    
    // Get the shared bridge instance - don't reinitialize
    let bridge = LibNativeBridge.sharedInstance()
    
    // IMPORTANT: Set the metal layer FIRST
    bridge?.metalLayer = self.metalLayer
    
    // Update the viewport for immersive space
    let scale = UITraitCollection.current.displayScale
    let width = 1920 * scale
    let height = 1080 * scale
    bridge?.drawableWillChangeSize(withWidth: Int(width), height: Int(height))
    metalLayer.drawableSize = CGSize(width: width, height: height)
    
    // Set up the immersive metal layer for proper rendering
    DispatchQueue.main.asyncAfter(deadline: .now() + 0.1) {
      let bridge = LibNativeBridge.sharedInstance()
      
      // Update the bridge to use this immersive metal layer
      bridge?.metalLayer = self.metalLayer
      
      // Trigger immersive mode setup
      bridge?.initializeImmersiveMode()
      print("🎯 ImmersiveMetalView configured for immersive space")
    }
  }
  
  var metalLayer: CAMetalLayer {
    return layer as! CAMetalLayer
  }
  
  override class var layerClass: AnyClass {
    return CAMetalLayer.self
  }
  
  override func layoutSubviews() {
    super.layoutSubviews()
    // Update drawable size if bounds change
    let bridge = LibNativeBridge.sharedInstance()
    let scale = UITraitCollection.current.displayScale
    bridge?.drawableWillChangeSize(withWidth: Int(bounds.width * scale), height: Int(bounds.height * scale))
    metalLayer.drawableSize = CGSize(width: bounds.width * scale, height: bounds.height * scale)
  }
}

struct ImmersiveMetalViewRepresentable: UIViewRepresentable {
  typealias UIViewType = ImmersiveMetalView
  
  func makeUIView(context: Context) -> ImmersiveMetalView {
    ImmersiveMetalView(frame: .zero)
  }
  
  func updateUIView(_ uiView: ImmersiveMetalView, context: Context) {}
}

struct ImmersiveView: View {
  @Binding var immersiveSpaceIsShown: Bool
  @Environment(\.dismissImmersiveSpace) var dismissImmersiveSpace
  @Environment(\.openWindow) private var openWindow
  
  var body: some View {
    ZStack {
      // Use MetalView for 3D rendering in immersive space
      ImmersiveMetalViewRepresentable()
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
      print("📱 Main window appeared - setting up immersive space trigger")
      
      // Listen for immersive space trigger
      NotificationCenter.default.addObserver(
        forName: NSNotification.Name("TriggerImmersiveSpace"),
        object: nil,
        queue: .main
      ) { _ in
        Task { @MainActor in
          print("🚀 Received immersive space trigger - transitioning now")
          let result = await openImmersiveSpace(id: "BabylonImmersiveSpace")
          switch result {
          case .opened:
            print("✅ Successfully entered immersive space")
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
