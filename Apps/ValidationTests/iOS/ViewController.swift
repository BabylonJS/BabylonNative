import UIKit
import MetalKit

class ViewController: UIViewController, MTKViewDelegate {
    
    var mtkView: MTKView!
    
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

            let width = 600
            let height = 400
            
            appDelegate!._bridge!.init(mtkView, width:Int32(width), height:Int32(height))
        }
    }

    override func viewWillLayoutSubviews() {
        super.viewWillLayoutSubviews()
        let appDelegate = UIApplication.shared.delegate as? AppDelegate
        if mtkView == nil {
            return
        }
        if appDelegate != nil {
            appDelegate!._bridge!.resize(Int32(600), height: Int32(400))
        }
    }
    
    func mtkView(_ view: MTKView, drawableSizeWillChange size: CGSize) {
        // no resize here. keep the 600x400 size for validation tests.
    }

    func draw(in view: MTKView) {
        let appDelegate = UIApplication.shared.delegate as? AppDelegate
        if appDelegate != nil {
            appDelegate!._bridge!.render()
        }
    }
}

