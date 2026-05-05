import UIKit
import MetalKit

class ViewController: UIViewController {

    var mtkView: MTKView!
    var xrView: MTKView!
    var bnView: BNView?

    override func viewDidAppear(_ animated: Bool) {
        super.viewDidAppear(animated)
        guard
            let appDelegate = UIApplication.shared.delegate as? AppDelegate,
            let runtime = appDelegate.runtime
        else { return }

        setupViews()

        // Hand the runtime a reference to the XR overlay so NativeXr
        // can render its content into a separate transparent layer
        // when an XR session is active. The runtime keeps the
        // overlay's visibility in sync with the XR session state on
        // its own — the host doesn't need to toggle anything.
        runtime.setXrView(xrView)

        // Attach BNView to the main MTKView. Because mtkView's delegate
        // is still nil at this point, BNView auto-installs a managed
        // `BNViewDelegate` that drives the per-frame render and resize
        // callbacks. First attach on this runtime triggers GPU device
        // construction + plugin initialization on the JS thread +
        // queued-script flush.
        bnView = BNView(runtime: runtime, view: mtkView)

        // Simple gesture recognizer: forwards touches to BNView.
        let recognizer = UIBabylonGestureRecognizer(
            target: self,
            onTouchDown: { [weak self] (id, x, y) in self?.bnView?.pointerDown(id: Int(id), x: CGFloat(x), y: CGFloat(y)) },
            onTouchMove: { [weak self] (id, x, y) in self?.bnView?.pointerMove(id: Int(id), x: CGFloat(x), y: CGFloat(y)) },
            onTouchUp:   { [weak self] (id, x, y) in self?.bnView?.pointerUp  (id: Int(id), x: CGFloat(x), y: CGFloat(y)) }
        )
        mtkView.addGestureRecognizer(recognizer)
    }

    func setupViews() {
        mtkView = MTKView()
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

