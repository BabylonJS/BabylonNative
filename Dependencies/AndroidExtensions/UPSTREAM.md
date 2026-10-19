# AndroidExtensions provenance

Upstream: https://github.com/BabylonJS/AndroidExtensions.git
Upstream `main` tip: `5fe60c8634dd9a5df76888e73e69202875e894f0`
Rewritten tip: `0d7af3acb79dd019c60ecc106aca0f7786884af4`
Imported commits: 22.

The full reachable history was imported with `git filter-repo` using
`--to-subdirectory-filter Dependencies/AndroidExtensions --prune-empty never --prune-degenerate never`,
then merged with `--allow-unrelated-histories --no-ff`. Authors, author/committer
dates, full messages, empty commits, merge topology, blob IDs and modes are
preserved; no historical commit contains gitlinks. Path rewriting changes commit
IDs and invalidates old signatures. Upstream license and attribution are retained.

To update, clone upstream `main` afresh, check its tip and history for gitlinks,
and re-run the same rewrite. Verify metadata, topology and trees, then merge the
rewritten tip without squashing and update this provenance. Keep BabylonNative
integration changes separate from upstream changes.

**Do not squash when landing: squashing discards the imported history.**
