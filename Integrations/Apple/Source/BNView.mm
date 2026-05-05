// BNView.mm — Obj-C++ implementation bridging BNView to
// Babylon::Integrations::View.

#import "BNRuntimeInternal.h"
#import <BabylonNativeIntegrations/BNView.h>

#import <QuartzCore/CAMetalLayer.h>

#include <Babylon/Integrations/View.h>
#include <Babylon/Integrations/ViewDescriptor.h>

#include <cstdint>
#include <memory>

namespace
{
    // Read physical-pixel dimensions + DPR from a CAMetalLayer and
    // Build a ViewDescriptor from a CAMetalLayer. drawableSize is in
    // physical pixels (= surface backing buffer size), which is what
    // ViewDescriptor wants directly. The Device queries the screen
    // contentsScale itself for any DPR-based math; the host does not
    // need to compute or pass it.
    Babylon::Integrations::ViewDescriptor MakeViewDescriptor(CAMetalLayer* layer)
    {
        Babylon::Integrations::ViewDescriptor descriptor{};
        descriptor.nativeWindow = (__bridge CA::MetalLayer*)layer;
        descriptor.width = static_cast<uint32_t>(layer.drawableSize.width);
        descriptor.height = static_cast<uint32_t>(layer.drawableSize.height);
        return descriptor;
    }
}

@implementation BNView
{
    std::unique_ptr<Babylon::Integrations::View> _view;
}

- (instancetype)initWithRuntime:(BNRuntime*)runtime layer:(CAMetalLayer*)layer
{
    if (runtime == nil || layer == nil)
    {
        return nil;
    }
    if ((self = [super init]))
    {
        // First attach on this runtime triggers GPU device construction
        // + plugin initialization + queued-script flush.
        _view = Babylon::Integrations::View::Attach(*runtime.nativeRuntime, MakeViewDescriptor(layer));
        if (!_view)
        {
            return nil;
        }
    }
    return self;
}

- (void)renderFrame
{
    if (_view)
    {
        _view->RenderFrame();
    }
}

- (void)resizeForLayer:(CAMetalLayer*)layer
{
    if (!_view || layer == nil)
    {
        return;
    }
    const auto descriptor = MakeViewDescriptor(layer);
    _view->Resize(descriptor.width, descriptor.height);
}

#if BABYLON_NATIVE_PLUGIN_NATIVEINPUT

- (void)pointerDown:(NSInteger)pointerId atX:(CGFloat)x y:(CGFloat)y
{
    if (_view)
    {
        _view->OnPointerDown(static_cast<int32_t>(pointerId),
                              static_cast<float>(x),
                              static_cast<float>(y));
    }
}

- (void)pointerMove:(NSInteger)pointerId atX:(CGFloat)x y:(CGFloat)y
{
    if (_view)
    {
        _view->OnPointerMove(static_cast<int32_t>(pointerId),
                              static_cast<float>(x),
                              static_cast<float>(y));
    }
}

- (void)pointerUp:(NSInteger)pointerId atX:(CGFloat)x y:(CGFloat)y
{
    if (_view)
    {
        _view->OnPointerUp(static_cast<int32_t>(pointerId),
                            static_cast<float>(x),
                            static_cast<float>(y));
    }
}

#else

// When NATIVEINPUT is disabled at native build time, the methods are
// still declared on the public BNView header for binary stability;
// they become no-ops here.
- (void)pointerDown:(NSInteger)pointerId atX:(CGFloat)x y:(CGFloat)y { (void)pointerId; (void)x; (void)y; }
- (void)pointerMove:(NSInteger)pointerId atX:(CGFloat)x y:(CGFloat)y { (void)pointerId; (void)x; (void)y; }
- (void)pointerUp:(NSInteger)pointerId atX:(CGFloat)x y:(CGFloat)y   { (void)pointerId; (void)x; (void)y; }

#endif

@end
