# JsRuntimeHost provenance

Upstream: https://github.com/BabylonJS/JsRuntimeHost.git
Upstream `main` tip: `980cfb0197f7fda661d362df4b275cff42d1e7f2`
Rewritten tip: `c3979a068492a70f110e819d17390d42c1f18ff1`
Imported commits: 151.

The full reachable history was imported with `git filter-repo` using
`--to-subdirectory-filter Dependencies/JsRuntimeHost --prune-empty never
--prune-degenerate never --preserve-commit-hashes --preserve-commit-encoding`,
then merged with `--allow-unrelated-histories --no-ff`. Every commit was
checked against its original for author/committer identity and dates, full
message, mapped parents, and prefixed tree blob IDs and modes, including empty
commits; no historical commit contains gitlinks. Path rewriting changes
commit IDs and invalidates old signatures. Upstream license and notices remain.

To update, clone upstream `main` afresh, check its tip and all reachable
commits for gitlinks, run the same rewrite, and verify metadata, messages,
parent topology and trees through the commit map. Merge the rewritten tip
without squashing and update this provenance. Keep BabylonNative integration
changes separate from upstream changes.

**Do not squash when landing: squashing discards the imported history.**
