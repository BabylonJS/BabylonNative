import UIKit
import MetalKit

class ViewController: UIViewController {

    var mtkView: MTKView!
    var xrView: MTKView!
    var bnView: BNView?

    override func viewDidLoad() {
        super.viewDidLoad()
    }

    override func viewDidAppear(_ animated: Bool) {
        super.viewDidAppear(animated)
        guard
            let appDelegate = UIApplication.shared.delegate as? AppDelegate,
            let runtime = appDelegate.runtime
        else { return }

        setupViews()

        let device = MTLCreateSystemDefaultDevice()
        mtkView.device = device

        mtkView.colorPixelFormat = .bgra8Unorm_srgb
        mtkView.depthStencilPixelFormat = .depth32Float

        // Hand the runtime a reference to the XR overlay so NativeXr can
        // render its content into a separate transparent layer when an
        // XR session is active.
        runtime.setXrView(xrView)

        // Construct the BNView against the main MetalLayer. First attach
        // on this runtime triggers GPU device construction + plugin
        // initialization on the JS thread + queued-script flush.
        if let layer = mtkView.layer as? CAMetalLayer {
            bnView = BNView(runtime: runtime, layer: layer)
        }

        // Simple gesture recognizer: forwards touches to BNView.
        let recognizer = UIBabylonGestureRecognizer(
            target: self,
            onTouchDown: { [weak self] (id, x, y) in self?.bnView?.pointerDown(Int(id), atX: CGFloat(x), y: CGFloat(y)) },
            onTouchMove: { [weak self] (id, x, y) in self?.bnView?.pointerMove(Int(id), atX: CGFloat(x), y: CGFloat(y)) },
            onTouchUp:   { [weak self] (id, x, y) in self?.bnView?.pointerUp(Int(id), atX: CGFloat(x), y: CGFloat(y)) }
        )
        mtkView.addGestureRecognizer(recognizer)
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
        guard let appDelegate = UIApplication.shared.delegate as? AppDelegate else { return }
        xrView.isHidden = !(appDelegate.runtime?.isXRActive ?? false)
        bnView?.renderFrame()
    }

    func mtkView(_ view: MTKView, drawableSizeWillChange size: CGSize) {
        bnView?.resize(withWidth: UInt(size.width), height: UInt(size.height))
    }
}

