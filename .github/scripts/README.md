# .github/scripts

Helper scripts used by CI workflows and by the NativeDawn (WebGPU) Babylon-Lite parity validation.

## CI helpers

| Script | Purpose |
|---|---|
| `build-android-tests.sh` | Builds the Android test app (used by `nightly.yml`). |
| `run-android-runtime-tests.sh` | Runs the Android runtime tests on an emulator (used by `nightly.yml`). |
| `free-disk-space.ps1` | Frees runner disk space before large builds such as Hermes (see `Documentation/ContinuousIntegration.md`). |

## NativeDawn Babylon-Lite parity

These compare Babylon-Lite scenes rendered by Babylon Native + Dawn against a browser WebGPU run. The full workflow is described in
[`Plugins/NativeDawn/Dawn-Lite-Parity.md`](../../Plugins/NativeDawn/Dawn-Lite-Parity.md); setup commands are in `BUILDING.md`.

| Script | Purpose |
|---|---|
| `bundle-lite-scene.mjs` | Bundles one Babylon-Lite lab scene into a self-contained classic IIFE script loadable by the Playground (`lite_native.js`). |
| `bundle-lite-parity.mjs` | Bundles the whole scene set used for parity. |
| `prepare-lite-parity.mjs` | Prepares the parity workspace (`.native-parity`, Playwright config, scene manifest). |
| `capture-dawn-provenance.mjs` | Records Dawn/adapter/build provenance (`provenance.json`) for a result set. |
| `lite-parity-page.mjs`, `lite-parity-fixture.mjs` | Playwright page driver and fixtures that run each scene natively and in the browser. |
| `lite-parity-reporter.mjs` | Playwright reporter writing case records, scheduled-case manifests (per invocation and cumulative) and history. |
| `lite-parity-records.mjs` | Shared helpers to summarize case records and metrics. |
| `report-lite-parity.mjs` | Generates the parity report from the recorded cases and metrics. |
| `complete-lite-parity.mjs` | Completes/merges partial runs into a final report. |
| `diagnose-lite-parity.mjs` | Diagnoses failed or incomplete parity cases. |
| `lite-parity-tools.test.mjs` | Unit tests for the helpers above (`node --test .github/scripts/lite-parity-tools.test.mjs`). |
