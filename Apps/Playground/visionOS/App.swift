import SwiftUI
import RealityKit

class MetalView: UIView {
  override init(frame: CGRect) {
    super.init(frame: frame)
    self.backgroundColor = .systemGreen // Bright green to be super visible
    print("🎯 MetalView initialized with frame: \(frame)")
    
    // Add a visible label for debugging
    let label = UILabel()
    label.text = "🟢 METAL VIEW IS HERE!"
    label.textColor = .white
    label.backgroundColor = .black
    label.textAlignment = .center
    label.translatesAutoresizingMaskIntoConstraints = false
    self.addSubview(label)
    
    NSLayoutConstraint.activate([
      label.centerXAnchor.constraint(equalTo: self.centerXAnchor),
      label.centerYAnchor.constraint(equalTo: self.centerYAnchor),
      label.widthAnchor.constraint(equalToConstant: 200),
      label.heightAnchor.constraint(equalToConstant: 50)
    ])
  }
  
  required init?(coder: NSCoder) {
    fatalError("init(coder:) has not been implemented")
  }
  
  func setupMetalLayer() {
    print("🔧 MetalView.setupMetalLayer() called")
    guard let bridge = LibNativeBridge.sharedInstance() else { 
      print("❌ Failed to get LibNativeBridge instance")
      return 
    }
    print("✅ Got LibNativeBridge instance")
    
    if bridge.metalLayer != nil {
      print("⚠️ Bridge already has metalLayer, skipping setup")
      return
    }
    print("🎯 Setting up new metal layer...")
    
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
    let width = Int(self.bounds.width * scale)
    let height = Int(self.bounds.height * scale)
    print("🎬 Calling bridge.initialize with size: \(width)x\(height)")
    let success = bridge.initialize(withWidth: width, height: height)
    print("🎭 Bridge initialization result: \(success)")
  }
  
  var metalLayer: CAMetalLayer {
    return layer as! CAMetalLayer
  }
  
  override class var layerClass: AnyClass {
    return CAMetalLayer.self
  }
  
  override func layoutSubviews() {
    super.layoutSubviews()
    print("🔧 MetalView.layoutSubviews called, bounds: \(bounds)")
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
  private var displayLink: CADisplayLink?
  
  override init(frame: CGRect) {
    super.init(frame: frame)
    setupImmersiveView()
  }
  
  required init?(coder: NSCoder) {
    super.init(coder: coder)
    setupImmersiveView()
  }
  
  deinit {
    displayLink?.invalidate()
  }
  
  private func setupImmersiveView() {
    // Set background to blue for debugging - should be visible if MetalView is working
    backgroundColor = .systemBlue
    
    // CRITICAL: Ensure the metal layer is properly configured for drawable presentation
    metalLayer.device = MTLCreateSystemDefaultDevice()
    metalLayer.pixelFormat = .bgra8Unorm
    metalLayer.framebufferOnly = true
    metalLayer.isOpaque = true
    metalLayer.contentsScale = UITraitCollection.current.displayScale
    metalLayer.presentsWithTransaction = false
    metalLayer.needsDisplayOnBoundsChange = true
    
    print("🎨 ImmersiveMetalView configured:")
    print("  - Device: \(metalLayer.device != nil ? "YES" : "NO")")
    print("  - Scale: \(metalLayer.contentsScale)")
    print("  - Pixel Format: bgra8Unorm")
    print("  - Opaque: \(metalLayer.isOpaque)")
    
    // Add a debug label to immersive view
    let label = UILabel()
    label.text = "🌌 IMMERSIVE METAL VIEW"
    label.textColor = .white
    label.backgroundColor = .red
    label.textAlignment = .center
    label.translatesAutoresizingMaskIntoConstraints = false
    self.addSubview(label)
    
    NSLayoutConstraint.activate([
      label.centerXAnchor.constraint(equalTo: self.centerXAnchor),
      label.topAnchor.constraint(equalTo: self.topAnchor, constant: 100),
      label.widthAnchor.constraint(equalToConstant: 300),
      label.heightAnchor.constraint(equalToConstant: 50)
    ])
    
    // Get the shared bridge instance - don't reinitialize
    let bridge = LibNativeBridge.sharedInstance()
    
    // IMPORTANT: Set the metal layer FIRST
    bridge?.metalLayer = self.metalLayer
    
    // Update the viewport for immersive space
    let scale = UITraitCollection.current.displayScale
    let width = 1920 * scale
    let height = 1080 * scale
    
    // CRITICAL: Set the drawable size BEFORE setting the layer
    metalLayer.drawableSize = CGSize(width: width, height: height)
    metalLayer.contentsScale = scale
    
    // Notify bridge about size change
    bridge?.drawableWillChangeSize(withWidth: Int(width), height: Int(height))
    
    // Set up the immersive metal layer with proper error handling
    DispatchQueue.main.asyncAfter(deadline: .now() + 0.5) {
      guard let bridge = LibNativeBridge.sharedInstance() else {
        print("❌ LibNativeBridge instance is nil")
        return
      }
      
      print("🌌 FIXED APPROACH: Setting up immersive mode properly 🌌")
      
      // Get the current scale and calculate proper dimensions
      let scale = UITraitCollection.current.displayScale
      let width = Int(1920 * scale)
      let height = Int(1080 * scale)
      
      print("🔧 Setting up immersive layer: size \(width)x\(height)")
      
      // CRITICAL: Switch the graphics device to render to THIS metal layer
      let success = bridge.switch(toImmersiveLayer: self.metalLayer, withWidth: width, height: height)
      
      if success {
        print("✅ Successfully switched to immersive metal layer")
        
        // Now initialize immersive mode
        let immersiveSuccess = bridge.initializeImmersiveMode()
        if immersiveSuccess {
          print("✅ Successfully initialized immersive mode")
        } else {
          print("❌ Failed to initialize immersive mode")
        }
      } else {
        print("❌ Failed to switch to immersive metal layer")
      }
    }
    
    // DISABLED: Don't set up a separate display link - rely on the main render loop
    // setupDisplayLink()
    print("🎮 Immersive view configured - using main render loop")
  }
  
  private func setupDisplayLink() {
    // DISABLED: Avoiding display link conflicts
    // displayLink = CADisplayLink(target: self, selector: #selector(renderLoop))
    // displayLink?.add(to: .current, forMode: .default)
    // print("🎮 Immersive display link started")
  }
  
  @objc private func renderLoop() {
    // DISABLED: Using main render loop instead
    // LibNativeBridge.sharedInstance()?.render()
  }
  
  override func layoutSubviews() {
    super.layoutSubviews()
    
    // CRITICAL: Update drawable size when bounds change
    if bounds.size.width > 0 && bounds.size.height > 0 {
      let scale = UITraitCollection.current.displayScale
      metalLayer.drawableSize = CGSize(width: bounds.width * scale, height: bounds.height * scale)
      print("🎨 ImmersiveMetalView layoutSubviews - drawable size: \(metalLayer.drawableSize)")
    }
  }
  
  var metalLayer: CAMetalLayer {
    return layer as! CAMetalLayer
  }
  
  override class var layerClass: AnyClass {
    return CAMetalLayer.self
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
  
  var body: some View {
    ZStack {
      // Use MetalView for 3D rendering in immersive space
      ImmersiveMetalViewRepresentable()
        .frame(maxWidth: .infinity, maxHeight: .infinity)
        .ignoresSafeArea()
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
  
  var body: some View {
    VStack {
      Text("🎯 BABYLON NATIVE PLAYGROUND")
        .font(.title)
        .foregroundColor(.white)
        .padding()
        .background(Color.black)
        .cornerRadius(10)
      
      MetalViewRepresentable()
        .frame(maxWidth: .infinity, maxHeight: .infinity)
        .background(Color.red) // Temporary: red background to make it super visible
      
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
      print("🎯 ContentView onAppear called - main window is visible!")
      print("📱 Main window appeared - setting up immersive space trigger")
      
      // AUTO-TEST: Automatically enter immersive mode immediately
      Task { @MainActor in
        print("🚀 AUTO-TEST: Main window appeared, triggering immersive space immediately")
        let result = await openImmersiveSpace(id: "BabylonImmersiveSpace")
        switch result {
        case .opened:
          print("✅ AUTO-TEST: Successfully entered immersive space")
          immersiveSpaceState = .open
          immersiveSpaceIsShown = true
        case .error:
          print("❌ AUTO-TEST: Error opening immersive space")
        case .userCancelled:
          print("🚫 AUTO-TEST: User cancelled immersive space")
        @unknown default:
          print("❓ AUTO-TEST: Unknown result opening immersive space")
        }
      }
      
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
  
  var body: some SwiftUI.Scene {
    WindowGroup {
      ContentView(immersiveSpaceIsShown: $immersiveSpaceIsShown)
        .frame(minWidth: 800, idealWidth: 800, maxWidth: 1200,
               minHeight: 600, idealHeight: 600, maxHeight: 900)
    }
    .defaultSize(width: 800, height: 600)
    
    ImmersiveSpace(id: "BabylonImmersiveSpace") {
      ImmersiveView(immersiveSpaceIsShown: $immersiveSpaceIsShown)
    }
    .immersionStyle(selection: .constant(.full), in: .full)
  }
}
