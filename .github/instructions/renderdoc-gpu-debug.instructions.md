---
applyTo: "**/*.{rdc,frag,vert,comp,glsl,hlsl,spv}"
---

# RenderDoc GPU Debugging Instructions

> Adapted from [rudybear/renderdoc-skill](https://github.com/rudybear/renderdoc-skill) for GitHub Copilot.

## When to Use

Use these instructions when the user mentions:
GPU debugging, .rdc files, RenderDoc, shader issues, pipeline state, rendering artifacts,
frame capture, shadow problems, visual glitches, draw calls, render targets, pixel history,
texture inspection, depth buffer, blend state, rasterizer state, vertex shader, fragment shader,
pixel shader, compute shader, mesh output, GPU performance, overdraw, bandwidth, shadow map,
PCF filtering, screen-space artifacts, z-fighting, alpha blending, stencil, MSAA,
"why does this look wrong", "debug this pixel", "what's drawing here", "capture a frame",
"inspect the pipeline", "export render target", Vulkan debugging, D3D debugging, GL debugging.

DO NOT use for: CSS rendering, React rendering, server-side rendering, HTML layout,
browser DevTools, web performance, canvas 2D, SVG rendering.

## Overview

This provides GPU frame capture, inspection, and debugging using `rdc-cli`, a 66-command CLI wrapping RenderDoc's Python API. It works with Vulkan, D3D11, D3D12, and OpenGL applications.

### Prerequisites

Before any GPU debugging, verify the environment:

```bash
rdc doctor
```

All checks should pass. If `rdc doctor` fails, check:
- `RENDERDOC_PYTHON_PATH` is set to the directory containing `renderdoc.pyd`
- `renderdoc.dll` is in the same directory as `renderdoc.pyd`
- `renderdoccmd.exe` is on PATH or in the module directory

## 1. Session Lifecycle

Every inspection session follows open-work-close:

```bash
rdc open path/to/capture.rdc   # Start daemon, load capture
# ... inspection commands ...
rdc close                       # Release resources, stop daemon
```

Check session state with `rdc status`. Only one capture can be open at a time.

**IMPORTANT**: Always close sessions when done. Leaked daemon processes consume GPU memory.

## 2. Capture Workflow

### Capture from an executable

```bash
rdc capture -o output.rdc -- /path/to/app [app-args...]
```

Key options:
- `-o, --output`: Output .rdc file path
- `--frame N`: Capture frame N (default: first presented frame)
- `--timeout S`: Capture timeout in seconds (default: 60)
- `--api-validation`: Enable API validation layer
- `--ref-all-resources`: Reference all resources (larger capture, more complete)
- `--wait-for-exit`: Wait for the app to exit after capture

### CWD matters

Many applications resolve assets relative to CWD. Always set CWD appropriately:

```bash
cd /path/to/app/root && rdc capture -o /output/path/capture.rdc -- ./app
```

### Vulkan layer requirement

For Vulkan apps, the RenderDoc Vulkan layer must be registered:
- **Windows**: Registry `HKCU\SOFTWARE\Khronos\Vulkan\ImplicitLayers` must contain `renderdoc.json` (DWORD 0)
- **Linux**: Copy `renderdoc.json` to `~/.local/share/vulkan/implicit_layer.d/`
- Environment: `ENABLE_VULKAN_RENDERDOC_CAPTURE=1` must be set

### If headless capture fails

Headless Vulkan apps (no swapchain / no `vkQueuePresentKHR`) cannot be captured by RenderDoc since frame boundaries are defined by present calls. Options:
1. If possible, run the app in a non-headless mode that opens a window with a swapchain
2. Use the RenderDoc Python API for reliable programmatic capture (see [RenderDoc documentation](https://renderdoc.org/docs/in_application_api.html))
3. Use `--trigger` mode (inject without auto-capture, then `rdc capture-trigger`)
4. Fall back to `renderdoccmd capture` directly

## 3. Frame Exploration

After opening a capture, start with high-level overview:

```bash
rdc info --json          # API, GPU, driver, resolution, frame number
rdc stats --json         # Per-pass breakdown, top draws, largest resources
rdc passes               # List render passes (debug markers / implicit passes)
rdc draws --limit 20     # First 20 draw calls
rdc count draws          # Total draw call count
```

### Navigate by pass

```bash
rdc draws --pass "Shadow Pass" --json    # Draws in a specific pass
rdc draws --pass "GBuffer" --limit 10    # First 10 GBuffer draws
```

### Navigate by event

```bash
rdc events --limit 50                    # All API events (not just draws)
rdc events --type DrawIndexed --json     # Filter by event type
rdc event 42 --json                      # Detail for one event
```

### VFS browsing

The virtual filesystem provides a structured view of the capture:

```bash
rdc ls /                      # Root directories
rdc ls /textures -l           # List textures with metadata
rdc tree /pipelines --depth 2 # Pipeline tree
rdc cat /events/42            # Event detail
```

## 4. Pipeline State Inspection

Get full pipeline state at any draw call:

```bash
rdc pipeline EID --json              # Full pipeline state
rdc pipeline EID vs --json           # Vertex shader section only
rdc pipeline EID ps --json           # Pixel/fragment shader section only
rdc pipeline EID om --json           # Output merger (blend, depth, stencil)
rdc pipeline EID rs --json           # Rasterizer state (culling, viewport)
rdc pipeline EID ia --json           # Input assembler (vertex buffers, topology)
rdc pipeline EID ds --json           # Depth-stencil state
```

### Bound resources

```bash
rdc bindings EID --json              # All bindings for all stages
rdc bindings EID --set 0 --json      # Descriptor set 0 only
```

## 5. Shader Inspection

### View shader metadata and source

```bash
rdc shader EID vs --json             # Vertex shader metadata
rdc shader EID ps --json             # Fragment/pixel shader metadata
rdc shader EID ps --source           # Debug source code (if available)
rdc shader EID ps --reflect --json   # Reflection: inputs, outputs, cbuffers
rdc shader EID ps --constants --json # Constant buffer values at this draw
```

### Disassembly targets

```bash
rdc shader EID ps --targets          # List available disassembly formats
rdc shader EID ps --target spirv     # SPIR-V disassembly
rdc shader EID ps --target glsl      # GLSL (cross-compiled, if available)
```

### Search shaders

```bash
rdc search "shadow" --json           # Search all shader disassembly for "shadow"
rdc search "main" --stage ps         # Search only pixel shaders
rdc shaders --stage ps --json        # List all unique pixel shaders
rdc shader-map --json                # EID-to-shader mapping
```

## 6. Visual Inspection: Export-View-Analyze

This is the core pattern for visual debugging. Export to PNG, then view the image to diagnose issues.

### Export render targets

```bash
rdc rt EID -o ./captures/analysis/render_target.png
rdc rt EID --target 1 -o ./captures/analysis/rt_target1.png  # MRT target 1
```

### Export textures

```bash
rdc texture RESID -o ./captures/analysis/texture.png
rdc texture RESID --mip 2 -o ./captures/analysis/mip2.png
```

### Export thumbnail

```bash
rdc thumbnail -o ./captures/analysis/thumb.png
```

### View exported images

After exporting a PNG, open it in the editor or image viewer to inspect visually.

### Analyze

Correlate what you see in the image with pipeline state data:
1. Export the render target at the suspicious draw
2. View the exported image
3. Check pipeline state (`rdc pipeline EID --json`)
4. Check shader constants (`rdc shader EID ps --constants --json`)
5. Form hypothesis and verify

## 7. Pixel Debugging

### Pixel history

Find all draws that wrote to a pixel:

```bash
rdc pixel X Y --json                 # Full pixel history at current event
rdc pixel X Y EID --json             # Pixel history up to EID
```

### Pick pixel color

Read the current color at a pixel:

```bash
rdc pick-pixel X Y --json            # Color at (X,Y) from current render target
rdc pick-pixel X Y EID --json        # Color at specific event
```

### Debug pixel shader

Step through shader execution for a specific pixel:

```bash
rdc debug pixel EID X Y --json           # Shader inputs/outputs summary
rdc debug pixel EID X Y --trace          # Full execution trace (every line)
rdc debug pixel EID X Y --dump-at 42     # Variable snapshot at line 42
```

### Debug vertex shader

```bash
rdc debug vertex EID VTXID --json        # Vertex shader debug for vertex ID
rdc debug vertex EID VTXID --trace       # Full trace
```

### Debug compute shader

```bash
rdc debug thread EID GX GY GZ TX TY TZ --json   # Compute thread debug
```

## 8. Shader Edit-Replay

Modify shaders without recompiling the application:

```bash
# 1. Check available encodings
rdc shader-encodings --json

# 2. Extract shader source
rdc shader EID ps --source -o ./captures/analysis/shader.frag

# 3. Edit the shader file

# 4. Build the modified shader
rdc shader-build ./captures/analysis/shader.frag --encoding GLSL --stage ps --json

# 5. Replace (use the shader_id from build output)
rdc shader-replace EID ps --with SHADER_ID --json

# 6. Export to verify the change
rdc rt EID -o ./captures/analysis/after_edit.png

# 7. Restore when done
rdc shader-restore EID ps
# or: rdc shader-restore-all
```

## 9. Frame Comparison

Compare two captures:

```bash
# Quick summary of differences
rdc diff capture_a.rdc capture_b.rdc --shortstat

# Detailed comparisons
rdc diff capture_a.rdc capture_b.rdc --draws --json       # Draw call differences
rdc diff capture_a.rdc capture_b.rdc --resources --json   # Resource differences
rdc diff capture_a.rdc capture_b.rdc --passes --json      # Pass structure differences
rdc diff capture_a.rdc capture_b.rdc --framebuffer --json  # Final framebuffer diff

# Visual diff
rdc diff capture_a.rdc capture_b.rdc --framebuffer --diff-output ./captures/analysis/diff.png
```

## 10. Debugging Recipes

### Recipe: Object is invisible

```bash
# 1. Find the draw call that should render the object
rdc draws --json | jq '.[] | select(.name | contains("ObjectName"))'
# or search by pass:
rdc draws --pass "Main Pass" --json

# 2. Check if it's being culled
rdc pipeline EID rs --json   # Look at CullMode, FrontFace

# 3. Check depth state
rdc pipeline EID ds --json   # DepthEnable, DepthFunc, DepthWriteMask

# 4. Check blend state (maybe alpha is 0)
rdc pipeline EID om --json   # BlendEnable, SrcBlend, DestBlend

# 5. Check vertex transform
rdc debug vertex EID 0 --json   # Is the position off-screen or behind camera?

# 6. Check if the draw is even issuing primitives
rdc draw EID --json   # VertexCount, InstanceCount, IndexCount
```

### Recipe: Colors are wrong

```bash
# 1. Export the render target to see what's there
rdc rt EID -o ./captures/analysis/wrong_color.png

# 2. Pick the problematic pixel
rdc pick-pixel X Y EID --json

# 3. Check texture bindings — is the right texture bound?
rdc bindings EID --json

# 4. Export the bound texture
rdc texture RESID -o ./captures/analysis/bound_texture.png

# 5. Check shader constants — wrong material colors?
rdc shader EID ps --constants --json

# 6. Check blend state — additive when it should be alpha?
rdc pipeline EID om --json

# 7. Debug the pixel shader to trace the calculation
rdc debug pixel EID X Y --trace
```

### Recipe: Shadows are broken

```bash
# 1. Find the shadow pass
rdc passes --json
rdc draws --pass "Shadow*" --json

# 2. Export the shadow map
SHADOW_EID=$(rdc draws --pass "Shadow*" -q | tail -1)
rdc rt $SHADOW_EID -o ./captures/analysis/shadow_map.png

# 3. Check shadow map resolution (is it too small?)
rdc bindings $SHADOW_EID --json   # Look at render target dimensions

# 4. Check depth bias
rdc pipeline $SHADOW_EID rs --json   # DepthBias, SlopeScaledDepthBias

# 5. Find the lighting pass that reads the shadow map
rdc draws --pass "Raster*" --json
LIGHT_EID=$(rdc draws --pass "Raster*" -q | head -1)

# 6. Check how the shadow map is sampled
rdc shader $LIGHT_EID ps --source   # Look for shadow sampling code
rdc shader $LIGHT_EID ps --constants --json   # Light matrices, bias values

# 7. Debug a shadowed pixel
rdc debug pixel $LIGHT_EID X Y --trace
```

### Recipe: Performance is bad

```bash
# 1. Get frame overview
rdc stats --json

# 2. Count draws per pass
rdc passes --json   # Look for passes with excessive draw counts

# 3. Look for redundant state changes
rdc events --limit 500 --json | jq 'group_by(.type) | map({type: .[0].type, count: length}) | sort_by(-.count)'

# 4. Check for large resources
rdc resources --sort size --json | jq '.[-10:]'   # Top 10 largest resources

# 5. Look for overdraw (if GPU counters available)
rdc counters --list
rdc counters --name "overdraw" --json

# 6. Export with wireframe overlay to visualize overdraw
rdc rt EID --overlay wireframe -o ./captures/analysis/wireframe.png
```

### Recipe: What changed between two frames

```bash
# Quick diff
rdc diff before.rdc after.rdc --shortstat

# Detailed diffs
rdc diff before.rdc after.rdc --draws --json
rdc diff before.rdc after.rdc --framebuffer --diff-output ./captures/analysis/frame_diff.png

# Compare pipeline state at specific draw
rdc diff before.rdc after.rdc --pipeline EID --json
```

### Recipe: Debug this pixel

```bash
# 1. Get pixel history — which draws touched this pixel?
rdc pixel X Y --json

# 2. Pick the draw that produced the final color
# (usually the last non-failing entry in pixel history)

# 3. Get the pixel's current value
rdc pick-pixel X Y EID --json

# 4. Debug the shader at that pixel
rdc debug pixel EID X Y --json

# 5. Get full execution trace if needed
rdc debug pixel EID X Y --trace

# 6. Check variable values at a specific shader line
rdc debug pixel EID X Y --dump-at LINE_NUMBER
```

## 11. Output Size Management

GPU captures can produce enormous output. Follow these rules:

1. **Always use `--limit` for exploration**: `rdc draws --limit 20`, `rdc events --limit 50`
2. **Filter by pass**: `rdc draws --pass "Shadow Pass"` instead of all draws
3. **Use `-q` for ID lists**: `rdc draws -q` returns only EIDs, not full details
4. **Use `--json` selectively**: JSON is verbose; use TSV default for scanning
5. **Pipeline sections**: `rdc pipeline EID vs` instead of full `rdc pipeline EID`
6. **Limit search results**: `rdc search "pattern" --limit 10`

### Size estimates
- `rdc info`: ~20 lines
- `rdc draws --limit 20`: ~25 lines
- `rdc pipeline EID --json`: ~200-500 lines (full), ~50 lines (per section)
- `rdc shader EID ps --source`: ~50-500 lines depending on shader
- `rdc debug pixel EID X Y --trace`: ~100-1000 lines depending on shader complexity

## 12. Error Handling

### rdc not found
```bash
pip install rdc-cli
```

### rdc doctor fails
Check `RENDERDOC_PYTHON_PATH` points to directory containing `renderdoc.pyd` and `renderdoc.dll`.

### Capture fails with "no swapchain"
The app may not present frames in the expected way. Try:
```bash
rdc capture --trigger -- /path/to/app   # Inject-only mode
# Then manually trigger: rdc capture-trigger
```

### Daemon not responding
```bash
rdc status          # Check if daemon is alive
rdc close           # Force close
rdc open capture.rdc  # Re-open
```

### GPU feature not supported
Some features (GPU counters, pixel history) depend on the GPU and driver. Check:
```bash
rdc gpus --json     # GPU capabilities
rdc counters --list # Available counters (empty if not supported)
```

## Command Reference

For the complete list of all 66 commands with arguments, options, types, and defaults, see `rdc-commands-reference.instructions.md`.

For extended debugging recipes with expected output shapes, see `debugging-recipes.instructions.md`.
