# Babylon Native -- Debuggability Reference

Read this **before** debugging Babylon Native crashes, hangs, asserts, or visual
test failures. The Playground app has dedicated diagnostic infrastructure
that turns common silent failures into searchable banners with stack traces.
Don't add new ad-hoc logging or `MessageBox` prompts when the existing facilities
already cover the case.

## When to Use

Read this when the user mentions: Playground, validation tests, BabylonNative
crash, headless tests, `BX_ASSERT`, "MS C++ runtime dialog", call stack,
`config.json` test, `excludeFromAutomaticTesting`, pixel diff failure,
`captureNextFrame`, RenderDoc capture from Playground, or asks to debug a
specific BN test by name or index.

For pure RenderDoc CLI usage (any app, not BN-specific), see
`renderdoc/renderdoc-gpu-debug.instructions.md` and
`renderdoc/rdc-commands-reference.instructions.md`.

## Layout

| File / Dir | Role |
|---|---|
| `Apps/Playground/Shared/CommandLine.{h,cpp}` | Argument parser. Single source of truth for supported flags. |
| `Apps/Playground/Shared/Diagnostics.{h,cpp}` | Crash handler, `DumpFailure`, finish-line, exit-code tracking. |
| `Apps/Playground/Shared/AppContext.cpp` | Wires `UnhandledExceptionHandler` + `console.error` into `DumpFailure`; injects `_playgroundOptions` into JS. |
| `Apps/Playground/Scripts/validation_native.js` | Test runner. Reads `_playgroundOptions`, picks tests, calls `TestUtils.captureNextFrame()`, monkey-patches `console.error` to always include a JS stack. |
| `Apps/Playground/Scripts/config.json` | Test catalog. Each entry has `title`, `playgroundId`/`scriptToRun`, `referenceImage`, optional `excludeFromAutomaticTesting`/`reason`/`onlyVisual`/`renderCount`/`capture`/`threshold`/`errorRatio`. |
| `Plugins/TestUtils/Source/TestUtils.cpp` | Native side of `TestUtils.captureNextFrame()` -- calls `m_deviceContext.RequestCaptureNextFrame()`. |
| `Core/Graphics/Source/BgfxCallback.cpp` | bgfx trace/fatal sink. Routes bgfx output to stdout in headless mode and to `OutputDebugString` always. |

## Built-in features -- do NOT re-implement

These are already wired up. Use them; don't add parallel mechanisms.

### 1. Crash + assert handler with native call stacks
`Diagnostics::InstallCrashHandler()` runs from `Diagnostics::Initialize()`,
which is called at the **top** of `wWinMain` / `main`. It:
- installs `bx::installExceptionHandler()` (SEH on Windows, signals elsewhere)
  so unhandled exceptions print a banner + native callstack to **stderr**
  before terminating with exit code **3**;
- on Windows: suppresses the modal "Microsoft Visual C++ Runtime Library"
  assertion dialog, the `abort()` retry dialog, and the invalid-parameter
  dialog -- all are routed through `DumpFailure` instead;
- only calls `bx::debugBreak()` when a debugger is attached, so the breakpoint
  doesn't escape and corrupt the exit code in CI;
- maps `BX_ASSERT` / `BX_CHECK` failures into the same banner path (the bx
  default assert handler delegates to `DumpFailure`).

### 2. `DumpFailure(category, file, line, skipFrames, fmt, ...)`
Reusable, formatted banner emitter. Output looks like:
```
--- <CATEGORY> ---
<file>(<line>): <printf-formatted message body>

Callstack (N):
     #: File ---           Line  PC ---            Function ---
     0: foo.cpp            123   0x00007ff6...    foo::bar
     ...

Build info:
    Compiler: ..., CPU: ..., Arch: ..., OS: ..., CRT: ..., C++: ..., Date: ..., Time: ...

--- END ---
```
Every line is grep-able and includes a callstack down to the failure site.

Pre-wired call sites:
| Category | Triggered by |
|---|---|
| `ASSERT` | `BX_ASSERT` failures |
| `ABORT` | `SIGABRT` / runtime invalid-parameter / `_CRT_ERROR` |
| `CRASH` | unhandled SEH / `bx::installExceptionHandler` path |
| `UNCAUGHT JS ERROR` | `Babylon::AppRuntime::UnhandledExceptionHandler` |
| `JS CONSOLE ERROR` | every `console.error(...)` from JS (Babylon.js routes recoverable errors through this) |

When you see `[Error] Error: Cannot load X` in stdout, **scroll up** --
`DumpFailure` prints the full banner + JS + native stacks **before** the
short error line. The short line is kept so legacy log scrapers still match.

### 3. JS stack on every `console.error`
`AppContext.cpp` monkey-patches `console.error` so every call automatically
appends `new Error().stack` to the message before forwarding to the polyfill.
Do not add per-callsite stack capture -- it's automatic.

### 4. Colored finish line
`Playground: Finished in <time>. (exit <code>)` is printed once at every exit
path (atexit + at_quick_exit + explicit calls). Green on success, red on
failure. Use `Diagnostics::SetExitCode(N)` before any `std::quick_exit`/`_Exit`
so the line colorizes correctly.

### 5. Headless mode + stdout routing
`--headless` runs without a visible window (still creates an HWND for input
plumbing) and **routes all `console.log`/`console.error`, bgfx trace, and
diagnostic banners to stdout** -- without it, output goes to OutputDebugString
only and is invisible from a console run. Use `--headless --once` for any
scripted/CI run.

### 6. RenderDoc capture trigger (already integrated)
`TestUtils.captureNextFrame()` JS API -> `DeviceContext::RequestCaptureNextFrame()`
-> on next bgfx frame, `BGFX_FRAME_DEBUG_CAPTURE` flag -> bgfx's
`renderDocTriggerCapture()` -> `s_renderDoc->TriggerCapture()`. bgfx loads
`renderdoc.dll` automatically if it's reachable via PATH/LoadLibrary at
init. **Don't add a parallel RenderDoc integration.** Use the `--capture=N`
CLI flag (see below) for ad-hoc capture; see
`playground/playground-renderdoc-capture.instructions.md` for the
test-capture recipe and gotchas.

## Command-line flags (Playground)

Reference, not a guess -- `--help` prints the same. **Only one short alias
per flag** (intentional design -- long-name aliases were removed).

```
-h, --help                  Show help and exit (0)
-l, --list                  List configured tests as TSV and exit (0)
    --headless              Don't show window; route all logs to stdout
    --break-on-fail         Trigger debugger break on a failing test
    --generate-references   Save rendered images as new reference PNGs
    --once                  Run only the first matching test, then exit
    --include-excluded      Force-run tests with excludeFromAutomaticTesting /
                            onlyVisual / excludedGraphicsApis set in config.json
    --save-results=BOOL     Override saving of result PNGs (default true)
    --debug-trace=BOOL      Enable/disable Babylon::DebugTrace
    --perf-trace=LEVEL      Set Babylon::PerfTrace level (None / Log)
    --capture=N             Trigger RenderDoc capture on the Nth rendered frame
                            of every executed test. Auto-extends each test's
                            render budget so the .rdc finalizes; pixel compare
                            still runs on the test's original renderCount, so
                            pass/fail is unaffected. Output: <cwd>/temp/
                            bgfx_frame<N>.rdc. Combine with --once / --test /
                            --test-index for a single capture.
    --renderdoc-dll=PATH    Pin which renderdoc.dll bgfx adopts. PATH may be
                            a file or a directory (\renderdoc.dll appended).
                            Validated at parse time: missing path / wrong
                            kind exits 2. Eagerly preloaded before bgfx::init
                            so bgfx's findModule("renderdoc.dll") adopts it.
                            Always prints a diagnostic line:
                              RenderDoc: <full path> (API X.Y.Z, FileVersion ...)
                            Use this instead of $env:PATH manipulation -- it
                            survives PATH ordering and works in headless CI.
-t, --test=PATTERN          Run tests whose title contains PATTERN (substring,
                            case-insensitive). Repeatable; multiple = OR.
    --test-index=LIST       Run only the listed indices.
                            LIST: '3' or '3,5,7' or '3-6' or '3,5-7,9'.
    --                      End of options; everything after is a script path.
```

## Exit codes

| Code | Meaning |
|---:|---|
| `0` | success / `--help` / `--list` |
| `1` | uncaught JS exception |
| `2` | command-line parse error |
| `3` | hard crash (assert / SIGABRT / unhandled exception) |
| `-1` | pixel-diff comparison failure |

PowerShell shows `-1` as `4294967295` after unsigned cast in some contexts.

## Common debugging scenarios

### Run one test by name
```powershell
.\Playground.exe --headless --once --test "MultiRenderTarget" `
    app:///Scripts/validation_native.js
```
Substring match, case-insensitive, against `test.title`.

### Run one test by index (when titles collide or you only have a survey row)
```powershell
.\Playground.exe --headless --once --test-index=286 `
    app:///Scripts/validation_native.js
```

### Run a quarantined test without editing config.json
```powershell
.\Playground.exe --headless --once --include-excluded --test "Decal Map" `
    app:///Scripts/validation_native.js
```
Bypasses `excludeFromAutomaticTesting`, `onlyVisual`, and
`excludedGraphicsApis` filters. Use this whenever you need to *debug* a
quarantined test -- never edit config.json's exclusion flags as part of
debugging.

### Discover what tests exist
```powershell
.\Playground.exe --list
```
Prints TSV with index, title, exclusion reason. Pipe to `findstr` /
`Select-String` for filtering.

### Pixel-diff investigation
1. Run the failing test with `--headless --once --include-excluded`.
2. Stdout will contain `First pixel off at <byteOffset>: Value: (R,G,B) - Expected: (R,G,B)` for the first divergent pixel.
3. Result PNG is at `<exe-parent>/Results/<referenceImage>` (e.g.
   `Apps/Playground/Results/instancecolors.png`).
4. Diff overlay (red where pixels differ) is at `<exe-parent>/Errors/<referenceImage>`.
5. Reference is `<exe-dir>/ReferenceImages/<referenceImage>`.
6. Compare visually first. If geometry/transforms look right but colors are
   off, the bug is shader/texture/uniform; if geometry is wrong, it's
   vertex/index/transform. Then capture with RenderDoc -- see the dedicated
   recipe.

### Reference image missing (pre-flight check)
If the test runner emits a line containing `MISSING_REFERENCE_IMAGE:` then the
test never rendered -- `validation_native.js` checks that
`<exe-dir>/ReferenceImages/<referenceImage>` exists *before* loading the
playground source, and short-circuits with that grep-able token + `failTest`
when the file is absent. The full message is one of:
```
MISSING_REFERENCE_IMAGE: Test 'X' has no 'referenceImage' field in config.json - cannot run pixel comparison.
MISSING_REFERENCE_IMAGE: Test 'X' reference image not found at app:///ReferenceImages/<name>.png - cannot run pixel comparison.
```
Exit code is **-1** (test failure), the run summary increments a dedicated
counter (`missingRef=N`), and the run summary still prints. Distinct from
both pixel-diff failures (which print `First pixel off`) and `[Uncaught Error]
Failed to load file '...'` from urllib (which used to swallow this case as
exit 1 with no run summary). RenderDoc capture is meaningless for these
tests -- there are zero draws because the runner never reaches `loadPlayground`.
The check is a no-op for `--generate-references` runs (refs don't exist by
design) and for `onlyVisual` tests (which don't compare pixels). On non-Win32
platforms the underlying `TestUtils.referenceImageExists` is a stub that
returns `true`, so the legacy throw-on-missing behavior is preserved.

### RenderDoc capture (one-liner)
```powershell
# Preferred: pin the DLL explicitly. Survives PATH ordering, works in CI.
.\Playground.exe --headless --once --test-index=286 --include-excluded `
    --capture=5 --renderdoc-dll="C:\path\to\renderdoc-py" `
    app:///Scripts/validation_native.js
# .rdc lives at <cwd>\temp\bgfx_frame5.rdc
```
Alternatives that also work (in resolution order -- first match wins):
1. `--renderdoc-dll=PATH` (CLI flag, parse-time validated; preferred).
2. `BN_RENDERDOC_DLL=<file-or-dir>` env var (only loaded when `--capture` set).
3. `RENDERDOC_PYTHON_PATH=<rdc-cli renderdoc-py dir>` (rdc-cli's own var, only when `--capture` set).
4. `renderdoc.dll` discoverable via `LoadLibrary` (PATH search; fragile).

Confirm the diagnostic line in stdout -- it tells you exactly which DLL was loaded:
```
RenderDoc: C:\Users\...\renderdoc-py\renderdoc.dll (API 1.6.0, FileVersion 1.41.0.0)
```
If you see `RenderDoc: WARNING: --capture requested but renderdoc.dll could not
be loaded.` -- none of the four sources resolved. Pin with `--renderdoc-dll=...`.
If you see `RenderDoc: WARNING: loaded DLL differs from RENDERDOC_PYTHON_PATH
pair.` -- the rdc-cli replay version won't match the captured version; the
warning text includes the exact `--renderdoc-dll=...` to add. Pixel pass/fail
is unchanged by `--capture`. See
`playground/playground-renderdoc-capture.instructions.md` for the full recipe
(DLL version match, multiple captures, `rdc` CLI inspection).

### "App crashed but I only see one short line"
Scroll **up** from the error line. The full `--- CRASH ---` /
`--- ASSERT ---` / `--- UNCAUGHT JS ERROR ---` banner with both JS and
native stacks is printed *before* the legacy one-liner. If that banner is
missing, the process probably terminated through a path that bypasses
`Diagnostics::Initialize()` (e.g. a static-init crash before
`InstallCrashHandler()` ran).

### bgfx assert during test (`bgfx.cpp:NNNN: ...`)
The bgfx callback in `Core/Graphics/Source/BgfxCallback.cpp` mirrors trace
output to `Diagnostics`'s sink. `BX_ASSERT` failures show up as `--- ASSERT ---`
banners. The actual assert message is in the second-to-last line of the
banner body; bx's "STR=" prefix indicates the underlying string passed to
the assert.

### Config-driven knobs per test
| Field | Effect |
|---|---|
| `excludeFromAutomaticTesting: true` | Skipped unless `--include-excluded` |
| `onlyVisual: true` | Render but skip pixel comparison (use `--include-excluded` to force pixel compare path; result is still saved) |
| `excludedGraphicsApis: ["D3D11"]` | Skipped on listed APIs unless `--include-excluded` |
| `renderCount: N` | Run scene N frames before pixel comparison (default 1) |
| `capture: true` | Call `TestUtils.captureNextFrame()` once (when `renderCount === 1`) -- triggers bgfx's RenderDoc capture. **Prefer the `--capture=N` CLI flag** for ad-hoc capture; this config knob is kept for legacy tests that hard-pin a per-test capture. |
| `threshold: N` | Per-channel tolerance for pixel comparison (default 25) |
| `errorRatio: F` | % of pixels allowed to differ (default 2.5) |
| `replace: "src,dst,..."` | String-replace pairs applied to the playground source before `eval` |
| `specificRoot: "..."` | Override `BABYLON.Tools.BaseUrl` for asset loads |

## Adding new diagnostic output -- guidelines

- Use `Diagnostics::DumpFailure` for any *failure* that would otherwise be a
  one-line log entry hard to track down. It gives a searchable banner +
  callstack + build info for free.
- Use `Babylon::Polyfills::Console::Initialize`'s `debugLog` callback for
  general informational logs (it's already wired to stdout in headless
  mode and OutputDebugString always).
- For new command-line flags, add them only to `CommandLine.cpp`, follow the
  `match("--long", "-short", FlagKind::...)` pattern, and update `PrintUsage`
  in the same file. **No long-name aliases**: pick one canonical long name.
- Update this instructions file when adding flags or new diagnostic
  categories so future agents find them.

## Building

The branch builds via the standard CMake setup. After source changes that
touch `Apps/Playground/Scripts/*.js` or `Apps/Playground/Scripts/config.json`,
the CMake `Apps/Playground/CMakeLists.txt` copies them to
`<build>/Apps/Playground/<config>/Scripts/`. For *transient* runtime tweaks
(e.g. flipping `capture: true` for one debug run), edit the build-dir copy
directly and revert when done -- that avoids accidental commits to
`config.json` and avoids a rebuild round-trip.
