import UIKit

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
            guard let deviceSlot = _activeTouchIds.firstIndex(of: -1) else { continue }
            _activeTouchIds[deviceSlot] = touch.hash
            let loc = touch.location(in: view)
            
            _onTouchDown(Int32(deviceSlot), Int32(loc.x), Int32(loc.y))
        }
    }
    
    override func touchesMoved(_ touches: Set<UITouch>, with event: UIEvent) {
        super.touchesMoved(touches, with: event)
        
        for touch in touches {
            guard let deviceSlot = _activeTouchIds.firstIndex(of: touch.hash) else { continue }
            let loc = touch.location(in: view)
            
            _onTouchMove(Int32(deviceSlot), Int32(loc.x), Int32(loc.y))
        }
    }
    
    override func touchesEnded(_ touches: Set<UITouch>, with event: UIEvent) {
        super.touchesEnded(touches, with: event)
        
        for touch in touches {
            guard let deviceSlot = _activeTouchIds.firstIndex(of: touch.hash) else { continue }
            let loc = touch.location(in: view)
            
            _onTouchUp(Int32(deviceSlot), Int32(loc.x), Int32(loc.y))
            _activeTouchIds[deviceSlot] = -1
        }
    }
}
