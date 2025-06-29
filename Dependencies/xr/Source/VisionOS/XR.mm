//
// VisionOS XR Implementation
// Minimal stub - disables XR for VisionOS until full implementation
//

#if ! __has_feature(objc_arc)
#error "ARC is off"
#endif

// Empty implementation - this effectively disables XR for VisionOS
// The absence of any implementation will cause the NativeXr plugin to not initialize XR
// This allows the build to complete while we work on CompositorServices integration