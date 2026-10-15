# UrlLib provenance

Upstream: https://github.com/BabylonJS/UrlLib.git
Upstream `main` tip: `0c991337a1160ba7a2d062bf8e342d0a66f48dc9`
Rewritten tip: `6bf23f200b4e430ab21377a5591cac1a7af96fb4`
Imported commits: 53.

The full reachable history was imported with `git filter-repo` using
`--to-subdirectory-filter Dependencies/UrlLib --prune-empty never
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
