import UIKit
import MetalKit

class ViewController: UIViewController, MTKViewDelegate {

    var mtkView: MTKView!
    var xrView: MTKView!

    override func viewDidLoad() {
        super.viewDidLoad()
    }

    override func viewDidAppear(_ animated: Bool) {
        super.viewDidAppear(animated)
        let appDelegate = UIApplication.shared.delegate as? AppDelegate
        if appDelegate != nil {
            mtkView = MTKView()
            mtkView.delegate = self
            mtkView.translatesAutoresizingMaskIntoConstraints = false
            view.addSubview(mtkView)
            view.addConstraints(NSLayoutConstraint.constraints(withVisualFormat: "|[mtkView]|", options: [], metrics: nil, views: ["mtkView" : mtkView]))
            view.addConstraints(NSLayoutConstraint.constraints(withVisualFormat: "V:|[mtkView]|", options: [], metrics: nil, views: ["mtkView" : mtkView]))

            let device = MTLCreateSystemDefaultDevice()!
            mtkView.device = device

            mtkView.colorPixelFormat = .bgra8Unorm_srgb
            mtkView.depthStencilPixelFormat = .depth32Float

            let gesture = UILongPressGestureRecognizer(target: self, action: #selector(self.pressGesture))
            gesture.minimumPressDuration = 0;
            mtkView.addGestureRecognizer(gesture)

            let scale = view.contentScaleFactor
            let width = view.bounds.size.width
            let height = view.bounds.size.height

            xrView = MTKView()
            xrView.translatesAutoresizingMaskIntoConstraints = false
            xrView.isUserInteractionEnabled = false
            xrView.isHidden = true
            view.addSubview(xrView)
            view.addConstraints(NSLayoutConstraint.constraints(withVisualFormat: "|[xrView]|", options: [], metrics: nil, views: ["xrView" : xrView]))
            view.addConstraints(NSLayoutConstraint.constraints(withVisualFormat: "V:|[xrView]|", options: [], metrics: nil, views: ["xrView" : xrView]))

            appDelegate!._bridge!.init(mtkView, width:Int32(width * scale), height:Int32(height * scale), xrView:Unmanaged.passUnretained(xrView).toOpaque())
        }
    }

    func draw(in view: MTKView) {
        let appDelegate = UIApplication.shared.delegate as? AppDelegate
        if appDelegate != nil {
            xrView.isHidden = !appDelegate!._bridge!.isXRActive()
            appDelegate!._bridge!.render()
        }
    }

    func mtkView(_ view: MTKView, drawableSizeWillChange size: CGSize) {
        let appDelegate = UIApplication.shared.delegate as? AppDelegate
        if appDelegate != nil {
            appDelegate!._bridge!.resize(Int32(size.width), height: Int32(size.height))
        }
    }

    @objc func pressGesture(sender: UILongPressGestureRecognizer) {
        let appDelegate = UIApplication.shared.delegate as? AppDelegate
        let loc = mtkView.superview?.convert(sender.location(in: mtkView), to: nil)
        
        if (appDelegate != nil && loc != nil) {
            let state = sender.state;
            
            if (state == UIGestureRecognizerState.began) {
                appDelegate!._bridge!.setTouchDown(Int32(loc!.x * UIScreen.main.scale), y:Int32(loc!.y * UIScreen.main.scale))
            }
            else if (state == UIGestureRecognizerState.changed) {
                appDelegate!._bridge!.setTouchMove(Int32(loc!.x * UIScreen.main.scale), y:Int32(loc!.y * UIScreen.main.scale))
            }
            else if (state == UIGestureRecognizerState.ended) {
                appDelegate!._bridge!.setTouchUp(Int32(loc!.x * UIScreen.main.scale), y:Int32(loc!.y * UIScreen.main.scale))
            }
        }
    }
}

