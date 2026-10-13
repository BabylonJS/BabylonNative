"""Verify the complete, path-rewritten bgfx.cmake import against its original repository."""
import argparse
import json
import subprocess
from pathlib import Path


def git(repo, *args):
    location = ["--git-dir", str(repo.resolve())] if (repo / "HEAD").is_file() else ["-C", str(repo)]
    return subprocess.check_output(["git", *location, *args])


def tree(repo, revision):
    entries = {}
    for entry in git(repo, "ls-tree", "-rz", revision).split(b"\0"):
        if entry:
            metadata, name = entry.split(b"\t", 1)
            entries[name.decode()] = metadata.decode()
    return entries


def commit(repo, revision):
    header, message = git(repo, "cat-file", "commit", revision).split(b"\n\n", 1)
    lines = header.splitlines()
    parents = [line[7:].decode() for line in lines if line.startswith(b"parent ")]
    identity = [line for line in lines if line.startswith((b"author ", b"committer ", b"encoding "))]
    return parents, identity, message


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("original", type=Path)
    parser.add_argument("imported", type=Path)
    parser.add_argument("mapping", type=Path)
    parser.add_argument("--original-tip", default="8fd48e454f624f964ced930af597fa5b4526be18")
    parser.add_argument("--final-tip", help="Also verify all original file histories remain reachable here")
    args = parser.parse_args()
    mapping = dict(line.split() for line in args.mapping.read_text().splitlines()[1:])
    imported_tip = mapping[args.original_tip]
    originals = git(args.original, "rev-list", args.original_tip).decode().splitlines()
    imports = git(args.imported, "rev-list", imported_tip).decode().splitlines()
    assert len(originals) == len(imports) == len(mapping) == 706
    assert set(imports) == {mapping[sha] for sha in originals}
    prefix = "Dependencies/bgfx.cmake/"
    for sha in originals:
        rewritten = mapping[sha]
        actual = tree(args.imported, rewritten)
        expected = {prefix + path: value for path, value in tree(args.original, sha).items()
                    if not value.startswith("160000 ")}
        assert actual == expected, f"Tree mismatch: {sha}"
        parents, identity, message = commit(args.original, sha)
        new_parents, new_identity, new_message = commit(args.imported, rewritten)
        assert new_parents == [mapping[parent] for parent in parents], sha
        assert identity == new_identity and message == new_message, sha
    current = tree(args.original, args.original_tip)
    files = {}
    for path, metadata in current.items():
        if metadata.startswith("160000 "):
            continue
        old_log = git(args.original, "log", "--follow", "--format=%H", args.original_tip, "--", path).decode().splitlines()
        new_log = git(args.imported, "log", "--follow", "--format=%H", imported_tip, "--", prefix + path).decode().splitlines()
        assert set(new_log) == {mapping[sha] for sha in old_log}, f"File history mismatch: {path}"
        if args.final_tip:
            final_log = git(args.imported, "log", "--follow", "--format=%H", args.final_tip, "--", prefix + path).decode().splitlines()
            assert {mapping[sha] for sha in old_log}.issubset(final_log), f"Final file history mismatch: {path}"
        files[path] = {"commits": len(old_log), "earliest_original": old_log[-1],
                       "earliest_imported": mapping[old_log[-1]]}
    assert len(files) == 53
    print(json.dumps({"original_tip": args.original_tip, "imported_tip": imported_tip,
                      "commits": len(imports), "all_trees_blob_mode_fidelity": True,
                      "authors_dates_messages_topology_preserved": True,
                      "final_tip_file_histories_verified": args.final_tip,
                      "historical_gitlinks": 0, "file_histories": files}, indent=2))


if __name__ == "__main__":
    main()
