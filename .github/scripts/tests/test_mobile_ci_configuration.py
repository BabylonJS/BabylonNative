from pathlib import Path
import re
import unittest


ROOT = Path(__file__).resolve().parents[3]


class MobileCiConfigurationTests(unittest.TestCase):
    def test_mobile_gradle_cmake_version_is_installed_by_ci(self):
        gradle = (ROOT / "Apps/UnitTests/Mobile/Android/app/build.gradle").read_text()
        version = re.search(r"version '([0-9.]+)'", gradle).group(1)
        workflow = (ROOT / ".github/workflows/build-mobile-tests.yml").read_text()
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
