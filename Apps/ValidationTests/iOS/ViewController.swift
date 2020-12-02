import UIKit
import MetalKit

class ViewController: UIViewController {
    
    var mtkView: MTKView!
    
    override func viewDidLoad() {
        super.viewDidLoad()
    }
    
    override func viewDidAppear(_ animated: Bool) {
        super.viewDidAppear(animated)
        let appDelegate = UIApplication.shared.delegate as? AppDelegate
        if appDelegate != nil {
            mtkView = MTKView()
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
            
            let mainView: UnsafeMutableRawPointer = Unmanaged.passUnretained(mtkView).toOpaque()
            
            appDelegate!._bridge!.init(mainView, width:Int32(width), height:Int32(height))
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
}

