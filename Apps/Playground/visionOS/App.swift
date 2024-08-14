import SwiftUI

class Renderer {
  static var shared = Renderer()
  
  var displayLink: CADisplayLink? = nil
  var libNativeBridge: LibNativeBridge? = nil
  
  var metalView: UIView? {
    didSet {
      libNativeBridge = LibNativeBridge(metalLayer: metalView?.layer as? CAMetalLayer)
      let scale = UITraitCollection.current.displayScale
      libNativeBridge?.initialize(withWidth: Int((metalView?.bounds.width ?? 0) * scale), height: Int((metalView?.bounds.height ?? 0) * scale))
      self.displayLink = CADisplayLink(target: self, selector: #selector(self.renderMetalLoop))
      self.displayLink?.add(to: .main, forMode: .common)
    }
  }
  
  init() {}
  
  @objc func renderMetalLoop() {
    libNativeBridge?.render()
  }
  
  func drawableWillChangeSize(width: Int, height: Int) {
    libNativeBridge?.drawableWillChangeSize(withWidth: Int(width), height: Int(height))
  }
  
  func setTouchMove(pointerId: Int32, x: Int32, y: Int32) {
    libNativeBridge?.setTouchMoveWithPointerId(Int(pointerId), x: Int(x), y: Int(y))
  }
  
  func setTouchDown(pointerId: Int32, x: Int32, y: Int32) {
    libNativeBridge?.setTouchDownWithPointerId(Int(pointerId), x: Int(x), y: Int(y))
  }
  
  func setTouchUp(pointerId: Int32, x: Int32, y: Int32) {
    libNativeBridge?.setTouchUpWithPointerId(Int(pointerId), x: Int(x), y: Int(y))
  }
}

class MetalView: UIView {
  
  override init(frame: CGRect) {
    super.init(frame: frame)
    self.backgroundColor = .clear
  }
  
  required init?(coder: NSCoder) {
    fatalError("init(coder:) has not been implemented")
  }
  
  func setupMetalLayer() {
    if Renderer.shared.metalView != nil {
      return
    }
    
    self.addGestureRecognizer(
      UIBabylonGestureRecognizer(
        target: self,
        onTouchDown: Renderer.shared.setTouchDown,
        onTouchMove: Renderer.shared.setTouchMove,
        onTouchUp: Renderer.shared.setTouchUp
      )
    )
    metalLayer.pixelFormat = .bgra8Unorm
    metalLayer.framebufferOnly = true
    // Assign metalView to renderer
    Renderer.shared.metalView = self
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
    Renderer.shared.drawableWillChangeSize(width: Int(bounds.width * scale), height: Int(bounds.height * scale))
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


@main
struct ExampleApp: App {
  var body: some Scene {
    WindowGroup {
      MetalViewRepresentable()
        .frame(maxWidth: .infinity, maxHeight: .infinity)
    }
  }
}
