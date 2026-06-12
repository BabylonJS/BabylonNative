import UIKit

@UIApplicationMain
class AppDelegate: UIResponder, UIApplicationDelegate {

    var window: UIWindow?

    /// Owned by the app: created in `application(_:didFinishLaunchingWithOptions:)`,
    /// torn down in `applicationWillTerminate`. The `ViewController` borrows
    /// this handle to construct its `BNView`.
    var runtime: BNRuntime?

    func application(_ application: UIApplication, didFinishLaunchingWithOptions launchOptions: [UIApplication.LaunchOptionsKey: Any]?) -> Bool {
        let runtimeOptions = BNRuntimeOptions()
        runtimeOptions.enableDebugger = true
        runtimeOptions.enableDebugTrace = true
        let runtime = BNRuntime(options: runtimeOptions)

        // Queue the Babylon.js bootstrap scripts (shared with the other
        // Playground hosts via Apps/Playground/Shared/PlaygroundScripts.cpp),
        // then the playground experience script. They will run after the
        // first BNView attach completes engine initialization on the JS
        // thread, in submission order.
        PlaygroundBootstrap.loadScripts(runtime)
        runtime.loadScript("app:///Scripts/experience.js")

        self.runtime = runtime
        return true
    }

    func applicationWillResignActive(_ application: UIApplication) {
        runtime?.suspend()
    }

    func applicationDidBecomeActive(_ application: UIApplication) {
        runtime?.resume()
    }

    func applicationWillTerminate(_ application: UIApplication) {
        runtime = nil
    }
}

