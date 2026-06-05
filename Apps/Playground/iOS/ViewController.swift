import UIKit
import MetalKit

class ViewController: UIViewController {

    var mtkView: MTKView!
    var xrView: MTKView!

    override func viewDidLoad() {
        super.viewDidLoad()
    }

    override func viewDidAppear(_ animated: Bool) {
        super.viewDidAppear(animated)
        guard
            let appDelegate = UIApplication.shared.delegate as? AppDelegate,
            let bridge = appDelegate._bridge
        else { return }
        
        setupViews()
        
        let device = MTLCreateSystemDefaultDevice()
        mtkView.device = device
        
        mtkView.colorPixelFormat = .bgra8Unorm_srgb
        mtkView.depthStencilPixelFormat = .depth32Float
        
        // Simple gesture recognizer, just provides platform to handle input events
        let gesture = UIBabylonGestureRecognizer(
            target: self,
            onTouchDown: bridge.setTouchDown,
            onTouchMove: bridge.setTouchMove,
            onTouchUp: bridge.setTouchUp
        )
        mtkView.addGestureRecognizer(gesture)
        
        // Force a layout pass so mtkView (and its backing CAMetalLayer) is laid
        // out and gets a valid drawable size before bgfx is initialized and
        // before the first draw(in:) callback. mtkView is added via Auto Layout
        // and is otherwise still 0x0 at this point, which would make bgfx render
        // into a 0x0 layer and trigger the Metal validation error
        // "No output textures defined for the render pass".
        view.layoutIfNeeded()

        let scale  = view.contentScaleFactor
        let width  = view.bounds.size.width
        let height = view.bounds.size.height
        
        bridge.init(
            mtkView,
            screenScale:Float(UIScreen.main.scale),
            width:Int32(width * scale),
            height:Int32(height * scale),
            xrView:Unmanaged.passUnretained(xrView).toOpaque()
        )
    }
  
    func setupViews() {
        mtkView = MTKView()
        mtkView.delegate = self
        mtkView.translatesAutoresizingMaskIntoConstraints = false
        view.addSubview(mtkView)
        view.addConstraints(NSLayoutConstraint.constraints(withVisualFormat: "|[mtkView]|", options: [], metrics: nil, views: ["mtkView" : mtkView]))
        view.addConstraints(NSLayoutConstraint.constraints(withVisualFormat: "V:|[mtkView]|", options: [], metrics: nil, views: ["mtkView" : mtkView]))
        
        xrView = MTKView()
        xrView.translatesAutoresizingMaskIntoConstraints = false
        xrView.isUserInteractionEnabled = false
        xrView.isHidden = true
        view.addSubview(xrView)
        view.addConstraints(NSLayoutConstraint.constraints(withVisualFormat: "|[xrView]|", options: [], metrics: nil, views: ["xrView" : xrView]))
        view.addConstraints(NSLayoutConstraint.constraints(withVisualFormat: "V:|[xrView]|", options: [], metrics: nil, views: ["xrView" : xrView]))
    }
}

// MARK: MTKViewDelegate
extension ViewController: MTKViewDelegate {
    func draw(in view: MTKView) {
        // Skip rendering until the layer has a valid drawable size, otherwise
        // bgfx cannot acquire a drawable and Metal validation reports a render
        // pass with no output textures.
        guard view.drawableSize.width > 0, view.drawableSize.height > 0 else { return }
        guard let appDelegate = UIApplication.shared.delegate as? AppDelegate else { return }
        xrView.isHidden = !(appDelegate._bridge?.isXRActive() ?? false)
        appDelegate._bridge?.render()
    }
    
    func mtkView(_ view: MTKView, drawableSizeWillChange size: CGSize) {
        guard size.width > 0, size.height > 0 else { return }
        guard let appDelegate = UIApplication.shared.delegate as? AppDelegate else { return }
        appDelegate._bridge?.resize(Int32(size.width), height: Int32(size.height))
    }
}

