import UIKit
import MetalKit

import UIKit
import UIKit.UIGestureRecognizerSubclass

/**
  * A very simple gesture recognizer.  All that it does is to emulate the functionality found in other platforms
  */
class UIBabylonGestureRecognizer: UIGestureRecognizer {
    // Callback for touch down events
    private let _onTouchDown: (Int32, Int32, Int32)->Void
    // Callback for touch movement events
    private let _onTouchMove: (Int32, Int32, Int32)->Void
    // Callback for touch up events
    private let _onTouchUp: (Int32, Int32, Int32)->Void
    // Table to track hashes of active touches
    private var _activeTouchIds: Array<Int> = [-1, -1, -1, -1, -1, -1, -1, -1, -1, -1]
    
    public init(target: Any?, onTouchDown: @escaping(Int32, Int32, Int32)->Void, onTouchMove: @escaping(Int32, Int32, Int32)->Void, onTouchUp: @escaping(Int32, Int32, Int32)->Void) {
        _onTouchDown = onTouchDown
        _onTouchMove = onTouchMove
        _onTouchUp = onTouchUp
        
        super.init(target: target, action: nil)
    }
    
    override func touchesBegan(_ touches: Set<UITouch>, with event: UIEvent) {
        super.touchesBegan(touches, with: event)
        
        for touch in touches {
            let deviceSlot = _activeTouchIds.firstIndex(of: -1)
            
            if (deviceSlot != nil) {
                _activeTouchIds[deviceSlot!] = touch.hash
                let loc = touch.location(in: view)
                
                _onTouchDown(Int32(deviceSlot!), Int32(loc.x), Int32(loc.y))
            }
        }
    }
    
    override func touchesMoved(_ touches: Set<UITouch>, with event: UIEvent) {
        super.touchesMoved(touches, with: event)
        
        for touch in touches {
            let deviceSlot = _activeTouchIds.firstIndex(of: touch.hash)
            if (deviceSlot != nil) {
                let loc = touch.location(in: view)
                
                _onTouchMove(Int32(deviceSlot!), Int32(loc.x), Int32(loc.y))
            }
        }
    }
    
    override func touchesEnded(_ touches: Set<UITouch>, with event: UIEvent) {
        super.touchesEnded(touches, with: event)
        
        for touch in touches {
            let deviceSlot = _activeTouchIds.firstIndex(of: touch.hash)
            if (deviceSlot != nil) {
                let loc = touch.location(in: view)
                
                _onTouchUp(Int32(deviceSlot!), Int32(loc.x), Int32(loc.y))
                _activeTouchIds[deviceSlot!] = -1
            }
        }
    }
}

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

            // Simple gesture recognizer, just provides platform to handle input events
            let gesture = UIBabylonGestureRecognizer(target: self, onTouchDown: appDelegate!._bridge!.setTouchDown, onTouchMove: appDelegate!._bridge!.setTouchMove, onTouchUp: appDelegate!._bridge!.setTouchUp)
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

            appDelegate!._bridge!.init(mtkView, screenScale:Float(UIScreen.main.scale), width:Int32(width * scale), height:Int32(height * scale), xrView:Unmanaged.passUnretained(xrView).toOpaque())
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
}

