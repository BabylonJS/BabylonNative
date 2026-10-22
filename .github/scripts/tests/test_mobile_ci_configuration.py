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
        self.assertEqual(action.count("for component in BabylonNative JsRuntimeHost UrlLib; do"), 2)
        for component in ("BabylonNative", "JsRuntimeHost", "UrlLib"):
            self.assertIn(f"UnitTests-${{{{ inputs.platform }}}}-{component}-", action)

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


if __name__ == "__main__":
    unittest.main()
