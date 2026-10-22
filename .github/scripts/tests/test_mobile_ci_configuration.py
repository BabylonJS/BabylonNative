from pathlib import Path
import re
import unittest


ROOT = Path(__file__).resolve().parents[3]


class MobileCiConfigurationTests(unittest.TestCase):
    def test_mobile_artifacts_are_built_inside_classic_jobs(self):
        for platform in ("android", "ios"):
            workflow = (ROOT / f".github/workflows/build-{platform}.yml").read_text()
            jobs = workflow.split("\njobs:\n", 1)[1]
            self.assertEqual(re.findall(r"^  ([\w-]+):$", jobs, re.MULTILINE), ["build"])
            self.assertIn("uses: ./.github/actions/build-mobile-tests", workflow)
        action = (ROOT / ".github/actions/build-mobile-tests/action.yml").read_text()
        self.assertEqual(action.count("for component in JsRuntimeHost UrlLib; do"), 2)
        for component in ("JsRuntimeHost", "UrlLib"):
            self.assertIn(f"UnitTests-${{{{ inputs.platform }}}}-{component}-", action)
        self.assertNotIn("UnitTests-${{ inputs.platform }}-BabylonNative-", action)

    def test_mobile_runners_only_depend_on_classic_builds(self):
        workflow = (ROOT / ".github/workflows/ci.yml").read_text()
        for job, expected in (
            ("AndroidTests", ["Android_Ubuntu_JSC", "Android_Ubuntu_V8", "Android_Ubuntu_QuickJS"]),
            ("iOSTests", ["iOS_Xcode26"]),
        ):
            needs = re.search(rf"^  {job}:\n    needs: \[([^]]+)\]", workflow, re.MULTILINE)
            self.assertIsNotNone(needs)
            self.assertEqual([item.strip() for item in needs.group(1).split(",")], expected)

    def test_mobile_gradle_cmake_version_is_installed_by_ci(self):
        gradle = (ROOT / "Apps/UnitTests/Mobile/Android/app/build.gradle").read_text()
        version = re.search(r"version '([0-9.]+)'", gradle).group(1)
        workflow = (ROOT / ".github/actions/build-mobile-tests/action.yml").read_text()
        installs = re.findall(r"^.*sdkmanager.*$", workflow, re.MULTILINE)
        self.assertTrue(
            any(f'"cmake;{version}"' in command for command in installs),
            f"Android artifact builds require SDK CMake {version} to be installed",
        )

    def test_apps_install_dependencies_before_configuring_test_bundles(self):
        cmake = (ROOT / "Apps/CMakeLists.txt").read_text()
        install = cmake.index("npm(install)")
        first_child = cmake.index("add_subdirectory(")
        self.assertLess(
            install, first_child,
            "Apps must install workspace dependencies before child configuration runs webpack",
        )

    def test_ios_harness_disables_toolchain_arc_before_project(self):
        cmake = (ROOT / "Apps/UnitTests/Mobile/CMakeLists.txt").read_text()
        setting = re.search(r"set\(ENABLE_ARC OFF CACHE [^)]+\)", cmake)
        self.assertIsNotNone(setting, "ios-cmake must not enable ARC for bgfx's manual ownership")
        self.assertLess(setting.start(), cmake.index("project("))

    def test_android_caches_a_clean_snapshot_before_installing_tests(self):
        workflow = (ROOT / ".github/workflows/test-android.yml").read_text()
        restore = workflow.index("uses: actions/cache/restore@")
        generate = workflow.index("- name: Generate clean emulator snapshot")
        save = workflow.index("uses: actions/cache/save@")
        execute = workflow.index("- name: Install and run all Android test artifacts sequentially")
        self.assertLess(restore, generate)
        self.assertLess(generate, save)
        self.assertLess(save, execute)
        self.assertEqual(workflow.count("if: steps.avd-cache.outputs.cache-hit != 'true'"), 2)
        self.assertEqual(workflow.count("force-avd-creation: false"), 2)
        self.assertEqual(workflow.count("avd-name: babylon-native-tests"), 2)
        self.assertIn("~/.android/avd/babylon-native-tests.avd", workflow)
        self.assertNotIn("~/.android/adb", workflow)
        self.assertIn("-no-snapshot-load", workflow[generate:save])
        self.assertIn("-no-snapshot-save", workflow[execute:])
        self.assertNotRegex(workflow, r"-no-snapshot(?:\s|$)")
        self.assertEqual(workflow.count("python3 .github/scripts/android_emulator_ready.py"), 2)

    def test_android_snapshot_key_tracks_toolchain_and_configuration(self):
        workflow = (ROOT / ".github/workflows/test-android.yml").read_text()
        prepare = workflow.index("- name: Identify emulator image")
        restore = workflow.index("uses: actions/cache/restore@")
        self.assertLess(prepare, restore)
        metadata = workflow[prepare:restore]
        self.assertIn("sdkmanager --install", metadata)
        self.assertIn("emulator/source.properties", metadata)
        self.assertIn("system-images/android-33/google_apis/x86_64/source.properties", metadata)
        self.assertIn("ImageOS", metadata)
        self.assertIn("steps.emulator-image.outputs.revision", workflow)
        self.assertIn("runner.arch", workflow)
        self.assertIn("hashFiles('.github/workflows/test-android.yml'", workflow)
        self.assertIn("'.github/scripts/android_emulator_ready.py')", workflow)
        self.assertNotIn("restore-keys:", workflow)


if __name__ == "__main__":
    unittest.main()
