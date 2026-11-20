# Experimental Windows visual-test timing

This opt-in job measures a fixed 305-scene workload on Windows, first in one process and then in two disjoint workers. It does not establish complete-suite coverage or replace legacy validation.

The native prototype is pinned to `397d5c09ec01fce62b206ba4b68b2fc189b4f744`; the shared scene setup, catalog, NativePage adapter and original web references are pinned to Babylon.js `d09f642c3813c8df178215acadc27f649510675a`. The native prototype's package lock pins Babylon.js and its extensions to 9.15.0. The build uses Release, V8, D3D11, Draco and Meshopt, with debug tracing and bx debug checks off. Each worker keeps one Playground process and creates/disposes a fresh NativeEngine for every scene.

`workload.json` pins scene metadata, the disjoint partitions and SHA-256 hashes of the original reference images. Framebuffer size is 600 by 400; Playwright threshold is 0.035 and the permitted pixel ratio is the shared scene's `errorRatio` or 1.1 percent. Snapshot updates and retries are disabled. All 305 scenes are attempted even if Windows produces image mismatches.

The extraction of shared functions matches the compared workload, including skipping `applyStates()` during native disposal because NativeEngine does not expose that method. No mobile transport or credentials are required.

Run the workflow on a draft pull request changing this directory or its workflow. Artifacts include every scene's outcome and timings, image differences, complete Playwright process wall time, runner hardware, executable hash and package versions. Build/setup and test execution are recorded separately. Different host processors and graphics devices limit hardware-controlled comparisons.
