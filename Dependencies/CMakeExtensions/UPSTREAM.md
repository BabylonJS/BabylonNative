# CMakeExtensions provenance

Upstream: https://github.com/BabylonJS/CMakeExtensions.git
Upstream `main` tip: `631780e42886e5f12bfd1a5568c7395f1d657f43`
Rewritten tip: `aef02bd0704122d6000d39ff096d4a1c472509d3`
Imported commits: 19.

The full reachable history was imported with `git filter-repo` using
`--to-subdirectory-filter Dependencies/CMakeExtensions --prune-empty never --prune-degenerate never`,
then merged with `--allow-unrelated-histories --no-ff`. Authors, author/committer
dates, full messages, empty commits, merge topology, blob IDs and modes are
preserved; no historical commit contains gitlinks. Path rewriting changes commit
IDs and invalidates old signatures. Upstream license and attribution are retained.

To update, clone upstream `main` afresh, check its tip and history for gitlinks,
and re-run the same rewrite. Verify metadata, topology and trees, then merge the
rewritten tip without squashing and update this provenance. Keep BabylonNative
integration changes separate from upstream changes.

**Do not squash when landing: squashing discards the imported history.**
