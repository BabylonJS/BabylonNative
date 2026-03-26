---
applyTo: "**/*.{rdc,frag,vert,comp,glsl,hlsl,spv}"
---

# rdc-cli Command Quick Reference

> Adapted from [rudybear/renderdoc-skill](https://github.com/rudybear/renderdoc-skill) for GitHub Copilot.

Complete reference for all 66 `rdc-cli` commands.

## `rdc assert-clean`

Assert capture log has no messages at or above given severity.

| Flag | Help | Type | Default |
|------|------|------|---------|
| `--min-severity` | Minimum severity threshold. | choice | HIGH |
| `--json` | JSON output. | flag | |

## `rdc assert-count`

Assert a capture metric satisfies a numeric comparison.

**Arguments:** `what` (choice, required)

| Flag | Help | Type | Default |
|------|------|------|---------|
| `--expect` | Expected count value. | integer | |
| `--op` | Comparison operator. | choice | eq |
| `--pass` | Filter by render pass name. | text | |
| `--json` | JSON output. | flag | |

## `rdc assert-image`

Compare two images pixel-by-pixel.

**Arguments:** `expected` (file, required), `actual` (file, required)

| Flag | Help | Type | Default |
|------|------|------|---------|
| `--threshold` | Diff ratio threshold (%). | float | 0.0 |
| `--diff-output` | Write diff visualization PNG. | file | |
| `--json` | JSON output. | flag | |

## `rdc assert-pixel`

Assert pixel RGBA at (x, y) matches expected value within tolerance.

**Arguments:** `eid` (integer, required), `x` (integer, required), `y` (integer, required)

| Flag | Help | Type | Default |
|------|------|------|---------|
| `--expect` | Expected RGBA as 4 space-separated floats. | text | |
| `--tolerance` | Per-channel tolerance. | float | 0.01 |
| `--target` | Render target index. | integer | 0 |
| `--json` | JSON output. | flag | |

## `rdc assert-state`

Assert pipeline state value at EID matches expected.

**Arguments:** `eid` (integer, required), `key_path` (text, required)

| Flag | Help | Type | Default |
|------|------|------|---------|
| `--expect` | Expected value. | text | |
| `--json` | JSON output. | flag | |

## `rdc attach`

Attach to a running RenderDoc target by ident.

**Arguments:** `ident` (integer, required)

| Flag | Help | Type | Default |
|------|------|------|---------|
| `--host` | Target host. | text | localhost |

## `rdc bindings`

Show bound resources per shader stage.

**Arguments:** `eid` (integer, optional)

| Flag | Help | Type | Default |
|------|------|------|---------|
| `--binding` | Filter by binding index. | integer | |
| `--set` | Filter by descriptor set index. | integer | |
| `--json` | Output JSON. | flag | |
| `--no-header` | Omit TSV header | flag | |
| `--jsonl` | JSONL output | flag | |
| `-q, --quiet` | Print primary key column only | flag | |

## `rdc buffer`

Export buffer raw data.

**Arguments:** `id` (integer, required)

| Flag | Help | Type | Default |
|------|------|------|---------|
| `-o, --output` | Write to file | path | |
| `--raw` | Force raw output even on TTY | flag | |

## `rdc capture`

Execute application and capture a frame.

| Flag | Help | Type | Default |
|------|------|------|---------|
| `-o, --output` | Output .rdc file path. | path | |
| `--api` | Capture API name. | text | |
| `--list-apis` | List capture APIs and exit. | flag | |
| `--frame` | Queue capture at frame N. | integer | |
| `--trigger` | Inject only; do not auto-capture. | flag | |
| `--timeout` | Capture timeout in seconds. | float | 60.0 |
| `--wait-for-exit` | Wait for process to exit. | flag | |
| `--keep-alive` | Keep target process running after capture. | flag | |
| `--auto-open` | Open capture after success. | flag | |
| `--api-validation` | Enable API validation. | flag | |
| `--callstacks` | Capture callstacks. | flag | |
| `--hook-children` | Hook child processes. | flag | |
| `--ref-all-resources` | Reference all resources. | flag | |
| `--soft-memory-limit` | Soft memory limit (MB). | integer | |
| `--delay-for-debugger` | Debugger attach delay (s). | integer | |
| `--json` | Output as JSON. | flag | |

## `rdc capture-copy`

Copy a capture from the target to a local path.

**Arguments:** `capture_id` (integer, required), `dest` (text, required)

| Flag | Help | Type | Default |
|------|------|------|---------|
| `--ident` | Target ident (default: most recent). | integer | |
| `--host` | Target host. | text | localhost |
| `--timeout` | Timeout in seconds. | float | 30.0 |

## `rdc capture-list`

List captures from the attached target.

| Flag | Help | Type | Default |
|------|------|------|---------|
| `--ident` | Target ident (default: most recent). | integer | |
| `--host` | Target host. | text | localhost |
| `--timeout` | Timeout in seconds. | float | 5.0 |
| `--json` | Output as JSON. | flag | |

## `rdc capture-trigger`

Trigger a capture on the attached target.

| Flag | Help | Type | Default |
|------|------|------|---------|
| `--ident` | Target ident (default: most recent). | integer | |
| `--host` | Target host. | text | localhost |
| `--num-frames` | Number of frames to capture. | integer | 1 |

## `rdc cat`

Output VFS leaf node content.

**Arguments:** `path` (text, required)

| Flag | Help | Type | Default |
|------|------|------|---------|
| `--json` | JSON output | flag | |
| `--raw` | Force raw output even on TTY | flag | |
| `-o, --output` | Write binary output to file | path | |

## `rdc close`

Close daemon-backed session.

| Flag | Help | Type | Default |
|------|------|------|---------|
| `--shutdown` | Send shutdown RPC to daemon. | flag | |

## `rdc count`

Output a single integer count to stdout.

**Arguments:** `what` (choice, required)

| Flag | Help | Type | Default |
|------|------|------|---------|
| `--pass` | Filter by render pass name. | text | |

## `rdc counters`

Query GPU performance counters.

| Flag | Help | Type | Default |
|------|------|------|---------|
| `--list` | List available counters. | flag | |
| `--eid` | Filter to specific event ID. | integer | |
| `--name` | Filter counters by name substring. | text | |
| `--json` | JSON output. | flag | |
| `--no-header` | Omit TSV header | flag | |
| `--jsonl` | JSONL output | flag | |
| `-q, --quiet` | Print primary key column only | flag | |

## `rdc debug pixel`

Debug pixel shader at (X, Y) for event EID.

**Arguments:** `eid` (integer, required), `x` (integer, required), `y` (integer, required)

| Flag | Help | Type | Default |
|------|------|------|---------|
| `--trace` | Full execution trace (TSV) | flag | |
| `--dump-at` | Var snapshot at LINE | integer | |
| `--sample` | MSAA sample index | integer | |
| `--primitive` | Primitive ID override | integer | |
| `--json` | JSON output | flag | |
| `--no-header` | Suppress TSV header row | flag | |

## `rdc debug thread`

Debug compute shader thread at workgroup (GX,GY,GZ) thread (TX,TY,TZ).

**Arguments:** `eid` (int, required), `gx` `gy` `gz` (int, required), `tx` `ty` `tz` (int, required)

| Flag | Help | Type | Default |
|------|------|------|---------|
| `--trace` | Full execution trace (TSV) | flag | |
| `--dump-at` | Var snapshot at LINE | integer | |
| `--json` | JSON output | flag | |
| `--no-header` | Suppress TSV header row | flag | |

## `rdc debug vertex`

Debug vertex shader for vertex VTX_ID at event EID.

**Arguments:** `eid` (integer, required), `vtx_id` (integer, required)

| Flag | Help | Type | Default |
|------|------|------|---------|
| `--trace` | Full execution trace (TSV) | flag | |
| `--dump-at` | Var snapshot at LINE | integer | |
| `--instance` | Instance index (default 0) | integer | 0 |
| `--json` | JSON output | flag | |
| `--no-header` | Suppress TSV header row | flag | |

## `rdc diff`

Compare two RenderDoc captures side-by-side.

**Arguments:** `capture_a` (file, required), `capture_b` (file, required)

| Flag | Help | Type | Default |
|------|------|------|---------|
| `--draws` | Compare draw calls | flag | |
| `--resources` | Compare resources | flag | |
| `--passes` | Compare passes | flag | |
| `--stats` | Compare stats | flag | |
| `--framebuffer` | Compare framebuffer | flag | |
| `--pipeline` | Compare pipeline at EID | text | |
| `--json` | JSON output | flag | |
| `--format` | Output format | choice | tsv |
| `--shortstat` | One-line summary | flag | |
| `--no-header` | Omit TSV header | flag | |
| `--verbose` | Verbose output | flag | |
| `--timeout` | Timeout in seconds | float | 60.0 |
| `--target` | Color target index (default 0) | integer | 0 |
| `--threshold` | Max diff ratio % to count as identical | float | 0.0 |
| `--eid` | Compare at specific EID (default: last draw) | integer | |
| `--diff-output` | Write diff PNG here | path | |

## `rdc doctor`

Run environment checks for rdc-cli. No arguments or options.

## `rdc draw`

Show draw call detail.

**Arguments:** `eid` (integer, optional)

| Flag | Help | Type | Default |
|------|------|------|---------|
| `--json` | JSON output | flag | |

## `rdc draws`

List draw calls.

| Flag | Help | Type | Default |
|------|------|------|---------|
| `--pass` | Filter by pass name | text | |
| `--sort` | Sort field | text | |
| `--limit` | Max rows | integer | |
| `--no-header` | Omit TSV header | flag | |
| `--json` | JSON output | flag | |
| `--jsonl` | JSONL output | flag | |
| `-q, --quiet` | Only EID column | flag | |

## `rdc event`

Show single API call detail.

**Arguments:** `eid` (integer, required)

| Flag | Help | Type | Default |
|------|------|------|---------|
| `--json` | JSON output | flag | |

## `rdc events`

List all events.

| Flag | Help | Type | Default |
|------|------|------|---------|
| `--type` | Filter by type | text | |
| `--filter` | Filter by name glob | text | |
| `--limit` | Max rows | integer | |
| `--range` | EID range N:M | text | |
| `--no-header` | Omit TSV header | flag | |
| `--json` | JSON output | flag | |
| `--jsonl` | JSONL output | flag | |
| `-q, --quiet` | Only EID column | flag | |

## `rdc goto`

Update current event ID via daemon.

**Arguments:** `eid` (integer, required)

## `rdc gpus`

List GPUs available at capture time.

| Flag | Help | Type | Default |
|------|------|------|---------|
| `--json` | Output as JSON. | flag | |

## `rdc info`

Show capture metadata.

| Flag | Help | Type | Default |
|------|------|------|---------|
| `--json` | JSON output | flag | |

## `rdc log`

Show debug/validation messages from the capture.

| Flag | Help | Type | Default |
|------|------|------|---------|
| `--level` | Filter by severity. | choice | |
| `--eid` | Filter by event ID. | integer | |
| `--json` | JSON output | flag | |
| `--no-header` | Omit TSV header | flag | |
| `--jsonl` | JSONL output | flag | |
| `-q, --quiet` | Print primary key column only | flag | |

## `rdc ls`

List VFS directory contents.

**Arguments:** `path` (text, optional)

| Flag | Help | Type | Default |
|------|------|------|---------|
| `-F, --classify` | Append type indicator (/ * @) | flag | |
| `-l, --long` | Long format (TSV with metadata) | flag | |
| `--json` | JSON output | flag | |
| `--no-header` | Omit TSV header (with -l) | flag | |
| `--jsonl` | JSONL output (with -l) | flag | |
| `-q, --quiet` | Print name only (with -l) | flag | |

## `rdc mesh`

Export post-transform mesh as OBJ.

**Arguments:** `eid` (integer, optional)

| Flag | Help | Type | Default |
|------|------|------|---------|
| `--stage` | Mesh data stage (default: vs-out) | choice | vs-out |
| `-o, --output` | Write to file | path | |
| `--json` | JSON output | flag | |
| `--no-header` | Suppress OBJ header comment | flag | |

## `rdc open`

Create local default session and start daemon skeleton.

**Arguments:** `capture` (text, optional)

| Flag | Help | Type | Default |
|------|------|------|---------|
| `--preload` | Preload shader cache after open. | flag | |
| `--proxy` | Proxy host[:port] for remote replay. | text | |
| `--remote` | Remote host. | text | |
| `--listen` | Listen on [ADDR]:PORT. | text | |
| `--connect` | Connect to an already-running external daemon. | text | |
| `--token` | Authentication token (required with --connect). | text | |

## `rdc pass`

Show detail for a single render pass by 0-based index or name.

**Arguments:** `identifier` (text, required)

| Flag | Help | Type | Default |
|------|------|------|---------|
| `--json` | Output JSON. | flag | |

## `rdc passes`

List render passes.

| Flag | Help | Type | Default |
|------|------|------|---------|
| `--json` | Output JSON. | flag | |
| `--no-header` | Omit TSV header | flag | |
| `--jsonl` | JSONL output | flag | |
| `-q, --quiet` | Print primary key column only | flag | |

## `rdc pick-pixel`

Read pixel color at (X, Y) from the current render target.

**Arguments:** `x` (integer, required), `y` (integer, required), `eid` (integer, optional)

| Flag | Help | Type | Default |
|------|------|------|---------|
| `--target` | Color target index (default 0) | integer | 0 |
| `--json` | JSON output | flag | |

## `rdc pipeline`

Show pipeline summary for current or specified EID.

**Arguments:** `eid` (integer, optional), `section` (text, optional)

| Flag | Help | Type | Default |
|------|------|------|---------|
| `--json` | Output JSON. | flag | |

## `rdc pixel`

Query pixel history at (X, Y) for the current or specified event.

**Arguments:** `x` (integer, required), `y` (integer, required), `eid` (integer, optional)

| Flag | Help | Type | Default |
|------|------|------|---------|
| `--target` | Color target index (default 0) | integer | 0 |
| `--sample` | MSAA sample index (default 0) | integer | 0 |
| `--json` | JSON output | flag | |
| `--no-header` | Omit TSV header | flag | |
| `--jsonl` | JSONL output | flag | |
| `-q, --quiet` | Print primary key column only | flag | |

## `rdc resource`

Show details of a specific resource.

**Arguments:** `resid` (integer, required)

| Flag | Help | Type | Default |
|------|------|------|---------|
| `--json` | Output JSON. | flag | |

## `rdc resources`

List all resources.

| Flag | Help | Type | Default |
|------|------|------|---------|
| `--json` | Output JSON. | flag | |
| `--type` | Filter by resource type (exact, case-insensitive). | text | |
| `--name` | Filter by name substring (case-insensitive). | text | |
| `--sort` | Sort order. | choice | id |
| `--no-header` | Omit TSV header | flag | |
| `--jsonl` | JSONL output | flag | |
| `-q, --quiet` | Print primary key column only | flag | |

## `rdc rt`

Export render target as PNG.

**Arguments:** `eid` (integer, optional)

| Flag | Help | Type | Default |
|------|------|------|---------|
| `-o, --output` | Write to file | path | |
| `--target` | Color target index (default 0) | integer | 0 |
| `--raw` | Force raw output even on TTY | flag | |
| `--overlay` | Render with debug overlay | choice | |
| `--width` | Overlay render width | integer | 256 |
| `--height` | Overlay render height | integer | 256 |

## `rdc script`

Execute a Python script inside the daemon process.

**Arguments:** `script_file` (file, required)

| Flag | Help | Type | Default |
|------|------|------|---------|
| `--arg` | Script argument. | text | |
| `--json` | Raw JSON output. | flag | |

## `rdc search`

Search shader disassembly text for PATTERN (regex).

**Arguments:** `pattern` (text, required)

| Flag | Help | Type | Default |
|------|------|------|---------|
| `--stage` | Filter by stage (vs/ps/cs/...). | text | |
| `--limit` | Max results. | integer | 200 |
| `-C, --context` | Context lines. | integer | 0 |
| `--case-sensitive` | Case-sensitive search. | flag | |
| `--json` | JSON output. | flag | |

## `rdc sections`

List all embedded sections.

| Flag | Help | Type | Default |
|------|------|------|---------|
| `--json` | Output as JSON. | flag | |

## `rdc shader`

Show shader metadata for a stage at EID.

**Arguments:** `first` (text, optional), `second` (text, optional)

| Flag | Help | Type | Default |
|------|------|------|---------|
| `--reflect` | Include reflection data (inputs/outputs/cbuffers). | flag | |
| `--constants` | Include constant buffer values. | flag | |
| `--source` | Include debug source code. | flag | |
| `--target` | Disassembly target format (e.g., 'dxil', 'spirv', 'glsl'). | text | |
| `--targets` | List available disassembly targets. | flag | |
| `-o, --output` | Output file path. | path | |
| `--all` | Get all shader data for all stages. | flag | |
| `--json` | Output JSON. | flag | |

## `rdc shader-build`

Build a shader from source file.

**Arguments:** `source_file` (file, required)

| Flag | Help | Type | Default |
|------|------|------|---------|
| `--stage` | Shader stage | choice | |
| `--entry` | Entry point name | text | main |
| `--encoding` | Encoding name (default: GLSL) | text | GLSL |
| `--json` | JSON output | flag | |
| `-q, --quiet` | Print only shader_id | flag | |

## `rdc shader-encodings`

List available shader encodings for this capture.

| Flag | Help | Type | Default |
|------|------|------|---------|
| `--json` | JSON output | flag | |

## `rdc shader-map`

Output EID-to-shader mapping as TSV.

| Flag | Help | Type | Default |
|------|------|------|---------|
| `--no-header` | Omit TSV header row. | flag | |
| `--json` | JSON output. | flag | |
| `--jsonl` | JSONL output. | flag | |
| `-q, --quiet` | Print EID column only. | flag | |

## `rdc shader-replace`

Replace shader at EID/STAGE with a built shader.

**Arguments:** `eid` (integer, required), `stage` (choice, required)

| Flag | Help | Type | Default |
|------|------|------|---------|
| `--with` | Built shader ID from shader-build | integer | |
| `--json` | JSON output | flag | |

## `rdc shader-restore`

Restore original shader at EID/STAGE.

**Arguments:** `eid` (integer, required), `stage` (choice, required)

| Flag | Help | Type | Default |
|------|------|------|---------|
| `--json` | JSON output | flag | |

## `rdc shader-restore-all`

Restore all replaced shaders and free built resources.

| Flag | Help | Type | Default |
|------|------|------|---------|
| `--json` | JSON output | flag | |

## `rdc shaders`

List unique shaders in capture.

| Flag | Help | Type | Default |
|------|------|------|---------|
| `--stage` | Filter by shader stage. | choice | |
| `--sort` | Sort order. | choice | name |
| `--json` | Output JSON. | flag | |
| `--no-header` | Omit TSV header | flag | |
| `--jsonl` | JSONL output | flag | |
| `-q, --quiet` | Print primary key column only | flag | |

## `rdc snapshot`

Export a complete rendering state snapshot for a draw event.

**Arguments:** `eid` (integer, required)

| Flag | Help | Type | Default |
|------|------|------|---------|
| `-o, --output` | Output directory | path | |
| `--json` | JSON output | flag | |

## `rdc stats`

Show per-pass breakdown, top draws, largest resources.

| Flag | Help | Type | Default |
|------|------|------|---------|
| `--json` | JSON output | flag | |
| `--no-header` | Omit TSV header | flag | |
| `--jsonl` | JSONL output | flag | |
| `-q, --quiet` | Print primary key column only | flag | |

## `rdc status`

Show current daemon-backed session status. No options.

## `rdc tex-stats`

Show texture min/max statistics and optional histogram.

**Arguments:** `resource_id` (integer, required), `eid` (integer, optional)

| Flag | Help | Type | Default |
|------|------|------|---------|
| `--mip` | Mip level (default 0) | integer | 0 |
| `--slice` | Array slice (default 0) | integer | 0 |
| `--histogram` | Show 256-bucket histogram | flag | |
| `--json` | JSON output | flag | |

## `rdc texture`

Export texture as PNG.

**Arguments:** `id` (integer, required)

| Flag | Help | Type | Default |
|------|------|------|---------|
| `-o, --output` | Write to file | path | |
| `--mip` | Mip level (default 0) | integer | 0 |
| `--raw` | Force raw output even on TTY | flag | |

## `rdc thumbnail`

Export capture thumbnail.

| Flag | Help | Type | Default |
|------|------|------|---------|
| `--maxsize` | Max thumbnail dimension. | integer | 0 |
| `-o, --output` | Write image to file. | path | |
| `--json` | Output as JSON. | flag | |

## `rdc tree`

Display VFS subtree structure.

**Arguments:** `path` (text, optional)

| Flag | Help | Type | Default |
|------|------|------|---------|
| `--depth` | Tree depth | integer | 2 |
| `--json` | JSON output | flag | |

## `rdc usage`

Show resource usage (which events read/write a resource).

**Arguments:** `resource_id` (integer, optional)

| Flag | Help | Type | Default |
|------|------|------|---------|
| `--all` | Show all resources usage matrix. | flag | |
| `--type` | Filter by resource type. | text | |
| `--usage` | Filter by usage type. | text | |
| `--json` | JSON output. | flag | |
| `--no-header` | Omit TSV header | flag | |
| `--jsonl` | JSONL output | flag | |
| `-q, --quiet` | Print primary key column only | flag | |
