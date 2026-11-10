# .github/scripts

Helper scripts used by CI workflows.

| Script | Purpose |
|---|---|
| `build-android-tests.sh` | Builds the Android test app (used by `nightly.yml`). |
| `run-android-runtime-tests.sh` | Runs the Android runtime tests on an emulator (used by `nightly.yml`). |
| `free-disk-space.ps1` | Frees runner disk space before large builds such as Hermes (see `Documentation/ContinuousIntegration.md`). |
