// Minimal test to debug immersive space issue
import SwiftUI
import RealityKit

@main
struct TestApp: App {
    var body: some Scene {
        WindowGroup {
            VStack {
                Text("BabylonNative Test")
                TestContentView()
            }
        }
        .windowStyle(.volumetric)
        .defaultSize(width: 1.0, height: 1.0, depth: 1.0, in: .meters)
        
        ImmersiveSpace(id: "TestImmersiveSpace") {
            RealityView { content in
                let entity = Entity()
                content.add(entity)
            }
        }
        .immersionStyle(selection: .constant(.full), in: .full)
    }
}

struct TestContentView: View {
    @State private var isImmersive = false
    @Environment(\.openImmersiveSpace) var openImmersiveSpace
    @Environment(\.dismissImmersiveSpace) var dismissImmersiveSpace
    
    var body: some View {
        Button("Test Immersive Space") {
            Task { @MainActor in
                if !isImmersive {
                    switch await openImmersiveSpace(id: "TestImmersiveSpace") {
                    case .opened:
                        isImmersive = true
                    case .error, .userCancelled:
                        print("Failed to open immersive space")
                    @unknown default:
                        print("Unknown result")
                    }
                } else {
                    await dismissImmersiveSpace()
                    isImmersive = false
                }
            }
        }
    }
}