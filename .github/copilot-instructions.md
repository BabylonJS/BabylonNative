# RenderDoc + GitHub Copilot GPU Debugging

> Based on [rudybear/renderdoc-skill](https://github.com/rudybear/renderdoc-skill) — adapted for GitHub Copilot.

## Project Overview

This project provides Copilot with instructions for GPU frame capture, inspection, and debugging using `rdc-cli` (a 66-command CLI wrapping RenderDoc's Python API). Works with Vulkan, D3D11, D3D12, and OpenGL applications.

## Tools

- **rdc-cli** (`rdc`): CLI wrapping RenderDoc's Python API. Install via `pip install rdc-cli`.
- **renderdoc.pyd**: Python module from RenderDoc (built from source or from an official install).

## Environment

- `RENDERDOC_PYTHON_PATH=<path-to-renderdoc-module>` — rdc-cli uses this to find renderdoc.pyd
- Captures go in `./captures/` (or any directory you choose)
- Analysis exports (PNGs, meshes, buffers) go in `./captures/analysis/`

## Your Application (customize this section)

<!-- Replace with your application details -->
- **Executable**: `/path/to/your/app`
- **CWD requirement**: Set CWD to your project root if the app resolves assets relative to CWD
- **Debug markers**: List any debug markers your app emits (e.g., "Shadow Pass", "GBuffer Pass")
- **Scenes / modes**: List any flags or scenes relevant to your app

## Capture Requirements

- **Vulkan layer**: Must be registered (see `.github/instructions/renderdoc-gpu-debug.instructions.md` for setup details)
- **Environment**: `ENABLE_VULKAN_RENDERDOC_CAPTURE=1` must be set for Vulkan apps
- **Swapchain**: The app must present frames (headless apps without `vkQueuePresentKHR` cannot be captured)
- **Capture script**: Use `rdc capture` or the RenderDoc Python API for programmatic capture

## Quick Start

```bash
# Check setup
rdc doctor

# Capture a frame
rdc capture -o ./captures/my_capture.rdc -- /path/to/app [args...]

# Inspect
rdc open ./captures/my_capture.rdc
rdc info --json
rdc draws --limit 20
rdc close
```

## GPU Debugging Instructions

See `.github/instructions/renderdoc-gpu-debug.instructions.md` for the full GPU debugging instructions with workflows, recipes, and command reference.
