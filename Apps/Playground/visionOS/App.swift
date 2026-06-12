// App.swift — visionOS Playground entry point.
//
// Built on the Babylon::Embedding Apple interop layer (BNRuntime /
// BNView), the same one the iOS Playground uses. visionOS differs
// from iOS in two small ways:
//   1. SwiftUI app lifecycle (`@main App`) instead of
//      `UIApplicationDelegate` — we own the BNRuntime as a top-level
//      `@StateObject` and observe `scenePhase` for suspend/resume.
//   2. No storyboards. `BabylonView` is a `UIViewRepresentable`
//      wrapping an `MTKView`; BNView auto-installs its render delegate.

import SwiftUI
import MetalKit

/// Owns the `BNRuntime` for the app's lifetime. Constructed once on
/// app launch (so the JS engine + thread start immediately) and torn
/// down with the app. Bootstrap scripts and the playground experience
/// are queued here; they run after the first BNView attach inside
/// `BabylonView` completes engine initialization on the JS thread.
@MainActor
final class BabylonRuntime: ObservableObject {
    let bnRuntime: BNRuntime

    init() {
        let options = BNRuntimeOptions()
        options.enableDebugger = true
        options.enableDebugTrace = true
        bnRuntime = BNRuntime(options: options)

        PlaygroundBootstrap.loadScripts(bnRuntime)
        bnRuntime.loadScript("app:///Scripts/experience.js")
    }
}

/// SwiftUI wrapper around the `MTKView` + `BNView` pair. BNView's
/// auto-installed `BNViewDelegate` drives per-frame render; resize is
/// driven from the MTKView's `layoutSubviews` (SwiftUI has no
/// view-controller layout hook). We keep BNView alive for the lifetime
/// of the MTKView (held in the Coordinator).
struct BabylonView: UIViewRepresentable {
    let runtime: BNRuntime

    func makeCoordinator() -> Coordinator {
        Coordinator()
    }

    func makeUIView(context: Context) -> MTKView {
        let mtkView = BabylonMTKView()
        let coordinator = context.coordinator

        // First BNView attach on this runtime triggers GPU device
        // construction + plugin initialization on the JS thread +
        // queued-script flush.
        coordinator.bnView = BNView(runtime: runtime, view: mtkView)

        // Babylon Native owns the drawable size (BNView sets
        // autoResizeDrawable = NO), so MTKView no longer reports size
        // changes via its delegate. Drive resize from layout, passing
        // logical points; BNView applies the device-pixel-ratio internally.
        mtkView.onLayout = { [weak coordinator] size in
            guard let bnView = coordinator?.bnView, size.width > 0, size.height > 0 else { return }
            bnView.resize(width: UInt(size.width), height: UInt(size.height))
        }

        // Simple gesture recognizer: forwards touches to BNView. Same
        // wiring as the iOS Playground.
        let recognizer = UIBabylonGestureRecognizer(
            target: coordinator,
            onTouchDown: { [weak coordinator] (id, x, y) in
                coordinator?.bnView?.pointerDown(id: Int(id), x: CGFloat(x), y: CGFloat(y))
            },
            onTouchMove: { [weak coordinator] (id, x, y) in
                coordinator?.bnView?.pointerMove(id: Int(id), x: CGFloat(x), y: CGFloat(y))
            },
            onTouchUp: { [weak coordinator] (id, x, y) in
                coordinator?.bnView?.pointerUp(id: Int(id), x: CGFloat(x), y: CGFloat(y))
            }
        )
        mtkView.addGestureRecognizer(recognizer)

        return mtkView
    }

    func updateUIView(_ uiView: MTKView, context: Context) {}

    /// Holds the BNView strongly so it outlives `makeUIView`'s scope.
    class Coordinator {
        var bnView: BNView?
    }
}

/// MTKView subclass that reports layout changes. SwiftUI has no
/// view-controller layout hook, so we observe layout on the view itself
/// to drive BNView resize — the same role `viewDidLayoutSubviews` plays
/// on iOS and `viewDidLayout` on macOS.
final class BabylonMTKView: MTKView {
    var onLayout: ((CGSize) -> Void)?

    override func layoutSubviews() {
        super.layoutSubviews()
        onLayout?(bounds.size)
    }
}

@main
struct ExampleApp: App {
    @StateObject private var runtime = BabylonRuntime()
    @Environment(\.scenePhase) private var scenePhase

    var body: some Scene {
        WindowGroup {
            BabylonView(runtime: runtime.bnRuntime)
                .frame(maxWidth: .infinity, maxHeight: .infinity)
                .onChange(of: scenePhase) { _, newPhase in
                    // BNRuntime suspend/resume is refcounted, so this
                    // composes safely with any other code that may
                    // suspend the runtime (e.g. modal dialogs).
                    switch newPhase {
                    case .active:
                        runtime.bnRuntime.resume()
                    case .inactive, .background:
                        runtime.bnRuntime.suspend()
                    @unknown default:
                        break
                    }
                }
        }
    }
}
