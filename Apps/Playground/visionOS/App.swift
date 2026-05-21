// App.swift — visionOS Playground entry point.
//
// Built on the Babylon::Integrations Apple interop layer (BNRuntime /
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
/// auto-installed `BNViewDelegate` drives per-frame render and resize
/// callbacks; we just have to keep BNView alive for the lifetime of
/// the MTKView (held in the Coordinator).
struct BabylonView: UIViewRepresentable {
    let runtime: BNRuntime

    func makeCoordinator() -> Coordinator {
        Coordinator()
    }

    func makeUIView(context: Context) -> MTKView {
        let mtkView = MTKView()

        // First BNView attach on this runtime triggers GPU device
        // construction + plugin initialization on the JS thread +
        // queued-script flush.
        context.coordinator.bnView = BNView(runtime: runtime, view: mtkView)

        // Simple gesture recognizer: forwards touches to BNView. Same
        // wiring as the iOS Playground.
        let coordinator = context.coordinator
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
