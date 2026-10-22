import contextlib
import importlib.util
import io
import json
from pathlib import Path
import subprocess
import tarfile
import tempfile
from types import SimpleNamespace
import unittest
from unittest.mock import patch


spec = importlib.util.spec_from_file_location(
    "mobile_test_artifacts", Path(__file__).parents[1] / "mobile_test_artifacts.py"
)
mobile = importlib.util.module_from_spec(spec)
spec.loader.exec_module(mobile)


class MobileArtifactsTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)
        self.logs = self.root / "logs"
        self.logs.mkdir()

    def create_artifacts(self, platform):
        root = self.root / platform
        root.mkdir()
        app = self.root / "Test.app"
        app.mkdir(exist_ok=True)
        (app / "Test").write_bytes(b"executable")
        apk = self.root / "test.apk"
        apk.write_bytes(b"apk")
        for component in mobile.COMPONENTS:
            for engine in mobile.ENGINES if platform == "Android" else ("JavaScriptCore",):
                output = root / mobile.artifact_name(platform, component, engine)
                mobile.package(SimpleNamespace(
                    output=output, component=component, engine=engine, platform=platform,
                    app=apk if platform == "Android" else app, test_apk=apk,
                ))
        return root

    def test_exact_artifact_matrices(self):
        for platform, count in (("Android", 9), ("iOS", 3)):
            root = self.create_artifacts(platform)
            self.assertEqual(len(mobile.discover(root, platform)), count)
            (root / "unexpected").mkdir()
            with self.assertRaisesRegex(RuntimeError, "unexpected"):
                mobile.discover(root, platform)

    def test_missing_artifact_and_payload_fail(self):
        root = self.create_artifacts("Android")
        directory, _ = mobile.discover(root, "Android")[0]
        (directory / "app.apk").unlink()
        with self.assertRaisesRegex(RuntimeError, "Missing app.apk"):
            mobile.discover(root, "Android")
        directory.rename(root / "wrong-name")
        with self.assertRaisesRegex(RuntimeError, "Artifact mismatch"):
            mobile.discover(root, "Android")

    def test_manifest_identity_is_validated(self):
        root = self.create_artifacts("iOS")
        directory, manifest = mobile.discover(root, "iOS")[0]
        manifest["application_id"] = "wrong.application"
        (directory / "manifest.json").write_text(json.dumps(manifest))
        with self.assertRaisesRegex(RuntimeError, "Invalid artifact manifest"):
            mobile.discover(root, "iOS")

    def test_android_instrumentation_must_really_finish(self):
        root = self.create_artifacts("Android")
        directory, manifest = mobile.discover(root, "Android")[0]
        for text, code, passes in (
            ("OK (1 test)\nINSTRUMENTATION_CODE: -1\n", 0, True),
            ("FAILURES!!!\nINSTRUMENTATION_CODE: -1\n", 0, False),
            ("OK (1 test)\nINSTRUMENTATION_CODE: 0\n", 0, False),
            ("INSTRUMENTATION_FAILED: process crashed", 0, False),
            ("OK (0 tests)\nINSTRUMENTATION_CODE: -1\n", 0, False),
            ("OK (1 test)\nINSTRUMENTATION_CODE: -1\n", 1, False),
        ):
            with self.subTest(text=text, code=code), patch.object(
                mobile.subprocess, "run", return_value=SimpleNamespace(
                    returncode=code, stdout=text, stderr=""
                )
            ), contextlib.redirect_stdout(io.StringIO()):
                if passes:
                    mobile.run_android(directory, manifest, self.logs)
                else:
                    with self.assertRaisesRegex(RuntimeError, "did not complete"):
                        mobile.run_android(directory, manifest, self.logs)

    def test_ios_requires_explicit_zero_test_exit(self):
        root = self.create_artifacts("iOS")
        directory, manifest = mobile.discover(root, "iOS")[0]
        for stderr, code, passes in (
            ("0\n", 0, True), ("1\n", 0, False), ("", 0, False),
            ("application crashed\n", 0, False), ("0\n", 1, False),
        ):
            with self.subTest(stderr=stderr, code=code), patch.object(
                mobile.subprocess, "run", return_value=SimpleNamespace(
                    returncode=code, stdout="test output", stderr=stderr
                )
            ), contextlib.redirect_stdout(io.StringIO()), contextlib.redirect_stderr(io.StringIO()):
                if passes:
                    mobile.run_ios(directory, manifest, self.logs, "test-simulator")
                else:
                    with self.assertRaisesRegex(RuntimeError, "report exit zero"):
                        mobile.run_ios(directory, manifest, self.logs, "test-simulator")

    def test_android_uninstalls_both_packages_after_crash_or_timeout(self):
        root = self.create_artifacts("Android")
        directory, manifest = mobile.discover(root, "Android")[0]
        for timeout in (False, True):
            calls = []

            def execute(command, **kwargs):
                calls.append(command)
                if "instrument" in command:
                    if timeout:
                        raise subprocess.TimeoutExpired(command, 1200)
                    return SimpleNamespace(
                        returncode=0, stdout="INSTRUMENTATION_RESULT: shortMsg=Process crashed.\n",
                        stderr="",
                    )
                return SimpleNamespace(returncode=0, stdout="", stderr="")

            with self.subTest(timeout=timeout), patch.object(
                mobile.subprocess, "run", side_effect=execute
            ), contextlib.redirect_stdout(io.StringIO()):
                with self.assertRaises((RuntimeError, subprocess.TimeoutExpired)):
                    mobile.run_android(directory, manifest, self.logs)
                self.assertEqual(calls[-2:], [
                    ["adb", "uninstall", manifest["application_id"] + ".test"],
                    ["adb", "uninstall", manifest["application_id"]],
                ])

    def test_unsafe_archive_fails_before_install(self):
        root = self.create_artifacts("iOS")
        directory, manifest = mobile.discover(root, "iOS")[0]
        with tarfile.open(directory / "app.tar.gz", "w:gz") as archive:
            entry = tarfile.TarInfo("../escape")
            archive.addfile(entry, io.BytesIO())
        with patch.object(mobile.subprocess, "run") as run:
            with self.assertRaisesRegex(RuntimeError, "Unsafe application archive"):
                mobile.run_ios(directory, manifest, self.logs, "test-simulator")
            run.assert_not_called()

    def test_all_artifacts_run_sequentially_even_after_failure(self):
        root = self.create_artifacts("Android")
        visited = []

        def run(directory, manifest, logs):
            visited.append(directory.name)
            if len(visited) == 1:
                raise subprocess.TimeoutExpired("instrument", 1200)

        with patch.object(mobile, "run_android", side_effect=run), contextlib.redirect_stdout(io.StringIO()):
            with self.assertRaisesRegex(RuntimeError, "Mobile unit-test artifacts failed"):
                mobile.run(SimpleNamespace(
                    platform="Android", artifacts=root, logs=self.logs, simulator="booted"
                ))
        self.assertEqual(visited, sorted(path.name for path in root.iterdir()))
        self.assertEqual(len(visited), 9)
        self.assertTrue((self.logs / f"{visited[0]}.error.log").is_file())


if __name__ == "__main__":
    unittest.main()
