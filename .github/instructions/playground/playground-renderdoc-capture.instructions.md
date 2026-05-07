# Capturing a Frame from Playground with RenderDoc

> Companion to `babylon-native-debugging.instructions.md`. Use when you need
> a `.rdc` file from a Playground visual test to inspect with RenderDoc.

## When to Use

Read this when the user wants to RenderDoc-debug a specific Playground test
(by name or index), or asks "why is this pixel-diff test failing?" after
visual inspection of the result/diff PNGs hasn't been enough. **Don't add
new RenderDoc integration to BN code** -- bgfx's built-in `debug_renderdoc.cpp`
is already wired through `TestUtils.captureNextFrame()`, and the launcher
(`renderdoccmd` / `rdc capture`) handles DLL injection.

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

## How DLL loading works

Playground itself does NOT preload `renderdoc.dll`. bgfx calls
`findModule("renderdoc.dll")` at `bgfx::init`, adopts whatever is already in
the process, and exposes the trigger via `bgfx::renderDocTriggerCapture` /
`TestUtils.captureNextFrame()`. To get the DLL into the process before
`bgfx::init`, launch under `renderdoccmd` or `rdc capture --trigger`. Both
inject the `renderdoc.dll` paired with their own executable into the target
via `CreateRemoteThread + LoadLibrary` before `wWinMain` runs. Because the
DLL comes from the launcher's directory, the version always matches what
`rdc open` will accept -- no PATH-ordering bugs, no version mismatch class.

Verify (while the process is alive):

```powershell
(Get-Process Playground).Modules |
    Where-Object ModuleName -eq 'renderdoc.dll' |
    Select-Object FileName
# -> C:\Users\<you>\Private\util\renderdoc-py\renderdoc.dll
```

## Recipe

### Step 1 -- Launch with `--capture=N` under the launcher

```powershell
$exe   = "<build>\Apps\Playground\Debug\Playground.exe"
$rdcmd = "<renderdoc-py-dir>\renderdoccmd.exe"
Set-Location (Split-Path $exe)

& $rdcmd capture -w $exe `
    --once --include-excluded --test-index=<N> --capture=5 `
    app:///Scripts/validation_native.js
```

Or with the rdc-cli wrapper (same effect, plus `--trigger` skips the
launcher's own auto-capture handshake):

```powershell
& rdc capture --trigger --wait-for-exit -- $exe `
    --once --include-excluded --test-index=<N> --capture=5 `
    app:///Scripts/validation_native.js
```

What the flags do:

| Flag | Purpose |
|---|---|
| `renderdoccmd capture -w` | Inject paired `renderdoc.dll` before `main`; wait for target exit. |
| `rdc capture --trigger -w` | Same, but explicitly inject-only (no launcher-side auto-capture). |
| `--capture=N` | Trigger `TestUtils.captureNextFrame()` on the Nth rendered frame of every executed test. Auto-extends the test's render budget by 5 frames after the trigger so RenderDoc has time to finalize. Pixel comparison still happens at the test's original `renderCount`, so pass/fail is unchanged. |
| `--once`, `--include-excluded`, `--test-index=N` | Standard test selection. |

`--capture=5` is a good default: trigger mid-run, plenty of finalize headroom.

The capture file lands at `<cwd>/temp/bgfx_frame<N>.rdc` (where `<N>` is
bgfx's internal frame counter, NOT the value passed to `--capture` -- both
can be present).

> **Do not** edit `config.json`'s `"capture"` / `"renderCount"` for capture
> work -- `--capture=N` supersedes that legacy path. The config-driven path
> still works (kept for backwards compat) but it triggers at the test's
> last frame and may not finalize.

### Step 2 -- Open + inspect

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

### Step 3 -- Clean up

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
& $rdcmd capture -w .\Playground.exe `
    --once --test-index=286 --include-excluded --capture=5 `
    app:///Scripts/validation_native.js
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
