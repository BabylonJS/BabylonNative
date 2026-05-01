# Capturing a Frame from Playground with RenderDoc

> Companion to `babylon-native-debugging.instructions.md`. Use when you need
> a `.rdc` file from a Playground visual test to inspect with RenderDoc.

## When to Use

Read this when the user wants to RenderDoc-debug a specific Playground test
(by name or index), or asks "why is this pixel-diff test failing?" after
visual inspection of the result/diff PNGs hasn't been enough. **Don't add
new RenderDoc integration to BN code** -- bgfx's built-in `debug_renderdoc.cpp`
is already wired through `TestUtils.captureNextFrame()`.

For generic `rdc-cli` usage and inspection workflows, see
`../renderdoc/renderdoc-gpu-debug.instructions.md` and
`../renderdoc/rdc-commands-reference.instructions.md`.

## Pre-flight: rule out non-pixel-diff failures first

RenderDoc can only capture a frame if the test actually produces one. Run
the test in baseline mode first (no `--capture`) and look for these
disqualifying patterns in stdout -- none of them are pixel-diff bugs and a
`.rdc` capture would either be empty or never finalize:

| Pattern in stdout | Diagnosis | Don't capture; instead |
|---|---|---|
| `MISSING_REFERENCE_IMAGE: Test 'X' ...` | Reference PNG not on disk. Test never rendered. | Generate the ref (`--generate-references`) or fix `referenceImage` in `config.json`. |
| `--- BN: UNCAUGHT JS ERROR ---` banner with `at createScene (eval ...)` | JS scene-builder threw. No GPU work was queued. | Look at the JS callstack frame -- that's the line that threw. Check for ChakraCore-unsupported syntax (`?.`, `??`), undefined globals (e.g. `ADDONS`), or missing engine caps. |
| `--- BN: JS CONSOLE ERROR ---` with `Cannot read properties of undefined` etc. | Babylon code path threw recoverable error. May or may not have rendered. | Check whether `Result/<name>.png` was written. If not, treat as JS error. |
| No `First pixel off at ...` line, no `Result/<name>.png` produced | Render loop never reached the screenshot frame. | Check `renderCount`, see whether a JS error fired silently. |
| `Run complete. ran=0 ...` | Filter matched nothing. | Fix the `--test` substring or `--test-index`. |

**Only proceed to `--capture=N` if you see `First pixel off at ...` and
`Pixel difference: N pixels.` in stdout.** That's the signature of a real
GPU-side mismatch worth a RenderDoc capture session.

## Why `rdc capture` (the launcher) doesn't work for Playground

The `rdc capture` mode launches the target executable under a RenderDoc
injector and waits for swapchain Present calls. It fails for Playground in
two common modes:

| Setup | Why it fails |
|---|---|
| `--headless` | No swapchain -> no Present calls -> RenderDoc has no frame boundary to capture. |
| Windowed `--once` | App exits before the RenderDoc child-injection handshake completes. Error: "failed to connect to target". |

Use bgfx's **in-process** capture path instead -- it doesn't need an injector
and works in headless mode too because the `TriggerCapture` API records the
in-flight D3D command stream regardless of whether a real Present happens.

## Recipe (works for both windowed and headless)

### Step 1 -- Pick which RenderDoc DLL to load

`rdc-cli` ships with `renderdoc-py` whose `renderdoc.pyd` exposes a specific
RenderDoc replay version. The capture must be made by a `renderdoc.dll` of
the **same** RenderDoc version, otherwise `rdc open` rejects it with:

```
OpenCapture failed: Captured API data was made on a newer incompatible
version of RenderDoc: D3D11 capture is incompatible version 20, newest
supported by this build of RenderDoc is 19
```

The DLL paired with `rdc-cli` lives next to `renderdoc.pyd` in
`renderdoc-py`'s install dir. Verify with `rdc doctor` -- both
`renderdoc-module` and `win-renderdoc-install` rows should show the same
version (e.g. 1.41).

Playground accepts the DLL location from any of these (first match wins,
top-down):

| Source | Notes |
|---|---|
| `--renderdoc-dll=<file-or-dir>` | **Preferred.** Parse-time validated (exit 2 if path missing or directory has no `renderdoc.dll`). Works without `--capture` (lets you confirm what would load). Survives PATH ordering. |
| `BN_RENDERDOC_DLL` env var | File or directory. Only consulted when `--capture` is set (so a stale env var doesn't silently load RenderDoc into every Playground run). |
| `RENDERDOC_PYTHON_PATH` env var | Same var rdc-cli already reads -- set it once, both rdc-cli and Playground pick the matching DLL. Only consulted when `--capture` is set. `\renderdoc.dll` is appended automatically. |
| PATH `LoadLibrary("renderdoc.dll")` | Fallback. Whichever directory PATH resolves first wins. Fragile -- easy to load the wrong version. |

Always confirm in stdout:
```
RenderDoc: C:\Users\...\renderdoc-py\renderdoc.dll (API 1.6.0, FileVersion 1.41.0.0)
```
That line appears whenever any of the above resolved. Possible warnings:

| Line prefix | Meaning |
|---|---|
| `RenderDoc: WARNING: --capture requested but renderdoc.dll could not be loaded.` | None of the four sources found a DLL. Pin one with `--renderdoc-dll=...`. |
| `RenderDoc: WARNING: loaded DLL differs from RENDERDOC_PYTHON_PATH pair.` | A DLL was loaded, but it doesn't match what rdc-cli expects -> `rdc open` will likely reject the capture. The warning prints the exact `--renderdoc-dll=...` argument to add for a fix. |
| `RenderDoc: ERROR: --renderdoc-dll=X could not be honored.` | Something (e.g. an injector) had already loaded a different `renderdoc.dll` before `wWinMain` ran. Use the path printed alongside, or relaunch outside the injector. |

**Legacy PATH-based recipe** (still works):
```powershell
$env:Path = "<dir-with-matching-renderdoc.dll>;$env:Path"
```
Use only when you can't use `--renderdoc-dll`. Subject to PATH-ordering bugs.

### Step 2 -- Launch with `--capture=N`

The `--capture=N` CLI flag does all the wiring for you:

- Triggers `TestUtils.captureNextFrame()` on the Nth rendered frame of
  every executed test.
- Auto-extends each test's render budget by 5 frames after the trigger so
  RenderDoc has time to finalize the `.rdc`. (Single-frame default tests
  with `renderCount: 1` would otherwise exit before finalize -- that was
  the historical reason for hand-editing config.json.)
- Pixel comparison still happens at the test's original `renderCount`, so
  pass/fail is unchanged. You can compare the run output with-and-without
  `--capture` and the "First pixel off / Pixel difference" lines should be
  identical.

```powershell
$exe = "<build>\Apps\Playground\Debug\Playground.exe"
$wd  = "<build>\Apps\Playground\Debug"
Set-Location $wd

& $exe --headless --once --include-excluded --test-index=<N> `
       --capture=5 --renderdoc-dll="<dir-with-matching-renderdoc.dll>" `
       app:///Scripts/validation_native.js
```

`--capture=5` is a good default: trigger mid-run, plenty of finalize headroom.
Use a different N if you specifically need a different frame.

Watch stdout for the diagnostic line:
```
RenderDoc: <full-path-to-loaded-renderdoc.dll> (API X.Y.Z, FileVersion ...)
BGFX Loading RenderDoc...     # only if bgfx hadn't seen it; with --renderdoc-dll
                              # it usually prints "RenderDoc is already loaded."
BGFX [ ] Capture
```
The `RenderDoc:` line confirms the DLL load and version. The `BGFX [ ] Capture`
line confirms the trigger fired. If you see neither, check the DLL location --
the resolution order is documented in Step 1.

The capture file lands at:
```
<cwd>/temp/bgfx_frame<N>.rdc
```
(`temp/bgfx` is the default `BGFX_CONFIG_RENDERDOC_LOG_FILEPATH`, relative
to the working directory. The `<N>` here is bgfx's internal frame counter,
NOT the value you passed to `--capture` -- both can be present.)

> **Do not** edit `config.json`'s `"capture"` / `"renderCount"` for capture
> work -- `--capture=N` supersedes that legacy path. The config-driven path
> still works (kept for backwards compat) but it triggers at the test's
> last frame and may not finalize.

### Step 3 -- Open + inspect

```powershell
$env:RENDERDOC_PYTHON_PATH = "<renderdoc-py dir>"
rdc.exe open <cwd>\temp\bgfx_frame<N>.rdc
rdc.exe info
rdc.exe events
rdc.exe draws
rdc.exe event <eid> --json   # full API call args incl. IndexCountPerInstance
rdc.exe pipeline <eid> vs    # bound shaders
rdc.exe cat /shaders/<id>/disasm   # DXBC + HLSL source comments
rdc.exe buffer <id> -o file.bin    # raw buffer bytes
rdc.exe mesh <eid> --stage vs-out --json   # post-VS positions
```

### Step 4 -- Clean up

`--capture` and the rendered extra frames don't persist -- nothing to revert.
Just delete `<cwd>/temp/` if you want to free disk.

## What the rdc CLI **cannot** do (drop to renderdoc-py)

- Show the IA input element layout (no `pipeline EID ia` section in
  rdc 0.5.x with renderdoc-py 1.41).
- Identify which buffer slot/offset feeds which shader semantic.
- Show *all* components of VS output (only first 4 are dumped to JSON).
- Debug shader execution (`rdc debug vertex/pixel` may crash the daemon
  on some captures).

For these, write a small Python script:

```python
import sys
sys.path.insert(0, RENDERDOC_PY_DIR)   # same as RENDERDOC_PYTHON_PATH
import renderdoc as rd

cap = rd.OpenCaptureFile()
cap.OpenFile(rdc_path, "rdc", None)
result, ctrl = cap.OpenCapture(rd.ReplayOptions(), None)
ctrl.SetFrameEvent(eid, True)

pipe = ctrl.GetPipelineState()

# IA layout: which VB / offset / format / perInstance feeds each semantic
for a in pipe.GetVertexInputs():
    print(a.name, a.vertexBuffer, a.byteOffset,
          a.format.Name(), a.perInstance, a.used)

# Bound vertex buffers (slot, resourceId, stride, offset)
for i, vb in enumerate(pipe.GetVBuffers()):
    print(i, int(vb.resourceId), vb.byteStride, vb.byteOffset)

# Post-VS data for instance/view, including non-position varyings
post = ctrl.GetPostVSData(instanceIdx, viewIdx, rd.MeshDataStage.VSOut)
data = ctrl.GetBufferData(post.vertexResourceId,
                          post.vertexByteOffset,
                          post.vertexByteStride)
# ... unpack as floats per VS output signature

# Find an action by EID via GetRootActions() walk; ReplayController has no
# GetAction(eid) on this version.
```

## Worked example -- test 286 "Instances with color buffer"

Symptom: pixel `(13,13,15)` instead of expected `(255,0,0)`. Both cubes
render black; reference shows red+green from per-instance color buffer.

```powershell
.\Playground.exe --headless --once --test-index=286 --include-excluded `
    --capture=5 --renderdoc-dll="C:\Users\bkaradzic\Private\util\renderdoc-py" `
    app:///Scripts/validation_native.js
# -> RenderDoc: C:\...\renderdoc-py\renderdoc.dll (API 1.6.0, FileVersion 1.41.0.0)
# -> "First pixel off at 286108: Value: (13, 13, 15) - Expected: (255, 0, 0)"
# -> "Pixel difference: 53296 pixels."
# -> exit -1
# -> temp\bgfx_frame5.rdc (~= 154 KB)
```

Inspecting via the renderdoc-py script revealed the IA input layout had
`COLOR` declared on slot 0 (per-vertex VB) at offset 0, overlapping POSITION
as `R8G8B8A8_UINT` -- `perInstance=False`. The actual per-instance color
attribute lives on slot 1 at offset 64 but was labeled `TEXCOORD3`, which
the shader doesn't read (`used=False`). Net effect: VS reads garbage uint
bytes from the position float as "color", PS multiplies by near-zero
ambient, output ~= black.

That's exactly the kind of bug visual diffs cannot point to but a 30-line
renderdoc-py script can identify in one capture. Cite this as the canonical
example when documenting "pixel diff with no obvious cause" workflows.
