# WebGPU Backend

This repository now uses a WebGPU native backend.

## What this backend path does
- Routes `Core` to `Core/GraphicsWgpu`.
- Uses a Rust static library (`Core/GraphicsWgpu/Rust`) based on `wgpu`.
- Preserves the `Graphics::Device` lifecycle shape (`EnableRendering`, `StartRenderingCurrentFrame`, `FinishRenderingCurrentFrame`, `DisableRendering`).

## Current intentional limits
- Rendering is currently a bootstrap validation path, not full feature parity yet.
- Screenshot callback currently returns a black RGBA buffer placeholder.

## Build prerequisites
- Rust toolchain with `cargo` available in `PATH`.
- Existing C/C++ toolchain requirements for Babylon Native.

## Notes on offline configure
If network fetches are blocked, pre-seed `FetchContent` source dirs (for example from an existing `_deps` cache), then configure.
