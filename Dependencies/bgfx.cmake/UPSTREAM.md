# bgfx.cmake provenance

This wrapper was imported from https://github.com/BabylonJS/bgfx.cmake at
`8fd48e454f624f964ced930af597fa5b4526be18`. Its 53 blob files were initially
imported byte-for-byte with their original modes. Its CC0 dedication remains
unchanged in [LICENSE](LICENSE); the original attribution headers are retained.

All 706 reachable commits were rewritten under `Dependencies/bgfx.cmake` and
merged without squashing. Authors, author/committer dates, messages, historical
renames, empty commits, and merge topology are preserved. Every historical
mode-160000 gitlink was removed; no bgfx, bx, or bimg implementation or history
was imported. The rewritten tip is
`39f003f6ae0b6e92b1c3bd0152f164c0e56000b2`.
Path rewriting necessarily changes commit IDs and invalidates old signatures.
[UPSTREAM_COMMIT_MAP.txt](UPSTREAM_COMMIT_MAP.txt) maps all original commit IDs
to their imported IDs. BabylonNative's existing history was not rewritten.

The former gitlinks now correspond to pinned CMake source acquisitions:

| Source | Repository | Revision |
|---|---|---|
| bgfx | https://github.com/BabylonJS/bgfx | `64e7324430604ba61a45b51e1cbeb2b67c5d1d15` |
| bx | https://github.com/bkaradzic/bx | `a9a71f8ef449c11042607feaeae0b8f46d22869a` |
| bimg | https://github.com/bkaradzic/bimg | `671fe06dd7012a1ab85aa931019ed16112909756` |

The upstream `.github` directory is excluded from the vendored working tree in
a separate BabylonNative-only commit. All five files and their histories remain
in the imported ancestry. This exclusion is not an upstream wrapper change;
never export it as a deletion of upstream's automation.

## Updates

For a future wrapper update, fetch the original full history into a disposable
clone, use the same path/gitlink transformation without pruning commits, and
reuse the mapped import ancestry before merging new wrapper changes. Keep
adaptations separate and extend the map and verification for the new tip.
Update child pins only deliberately in `cmake/dependencies.cmake`; CMake
updates managed sources on reconfigure but never checks out explicit roots.

## Exporting wrapper changes upstream

Keep vendor-only exclusions separate from upstreamable wrapper changes when
adding commits or importing later wrapper updates. Do not send the `.github`
exclusion commit upstream, and do not export an unrestricted subtree diff from
the import tip: that would delete upstream's automation.

From the BabylonNative root, the following exports candidate wrapper CMake
changes without `.github`, BabylonNative integration/staging helpers, or provenance:

```powershell
git -C Dependencies\bgfx.cmake diff --binary --relative `
  --output=..\..\build\bgfx-wrapper.patch `
  39f003f6ae0b6e92b1c3bd0152f164c0e56000b2 HEAD -- `
  CMakeLists.txt .gitmodules cmake `
  ':(exclude)cmake/babylon-native.cmake' ':(exclude)cmake/stage-sources.cmake'
```

Create `build` first if needed. Review the candidate patch in a standalone
upstream checkout before sending it; the import deliberately removed gitlinks,
so upstream submodule removal must be handled separately there. Adapt wrapper
documentation for standalone use rather than copying this directory's
BabylonNative-specific material wholesale. In particular, verify that the
candidate contains no `.github` deletions and leaves upstream automation intact.

**Do not squash this import when landing.** Upstream currently permits only
squash merges. Landing requires a maintainer-approved history-preserving
mechanism; a normal squash merge discards the imported ancestry.
