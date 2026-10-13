"""Isolated configure, packaging and installation contracts; no network is needed."""
import argparse
import os
import shutil
import subprocess
from pathlib import Path

PINS = {
    "bgfx": ("https://github.com/BabylonJS/bgfx.git", "f15c1ff18254f3fb85ffc1a669d1870c9714afc9"),
    "bx": ("https://github.com/bkaradzic/bx.git", "a9a71f8ef449c11042607feaeae0b8f46d22869a"),
    "bimg": ("https://github.com/bkaradzic/bimg.git", "671fe06dd7012a1ab85aa931019ed16112909756"),
}
HERE = Path(__file__).resolve().parent
WRAPPER = HERE.parent


def git(root, *args):
    return subprocess.check_output(["git", "-C", str(root), *args], text=True).strip()


def copy_sources(source, destination):
    shutil.copytree(source, destination, ignore=shutil.ignore_patterns(".git"), dirs_exist_ok=True)


class Contracts:
    def __init__(self, args):
        self.args = args
        self.work = args.work.resolve()
        self.work.mkdir(parents=True, exist_ok=True)
        self.sources = args.sources.resolve()
        self.fixture = self.work / "parent"
        copy_sources(HERE / "fixture", self.fixture)
        self.environment = os.environ.copy()
        # Local full-history clones exercise the real pinned FetchContent path
        # without downloading the same repositories again for each fixture.
        count = int(self.environment.get("GIT_CONFIG_COUNT", "0"))
        for index, (name, (url, pin)) in enumerate(PINS.items(), count):
            source = self.sources / name
            assert git(source, "rev-parse", "HEAD") == pin, source
            self.environment[f"GIT_CONFIG_KEY_{index}"] = f"url.{source.as_uri()}/.insteadOf"
            self.environment[f"GIT_CONFIG_VALUE_{index}"] = url
        self.environment["GIT_CONFIG_COUNT"] = str(count + len(PINS))
        self.sequence = 0

    def run(self, *command, failure=False):
        self.sequence += 1
        result = subprocess.run([str(arg) for arg in command], env=self.environment,
                                stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
        log = self.work / f"{self.sequence:02d}.log"
        log.write_text(f"{' '.join(map(str, command))}\nexit={result.returncode}\n{result.stdout}",
                       encoding="utf-8")
        assert (result.returncode != 0) if failure else (result.returncode == 0), \
            f"{log}\n{result.stdout[-12000:]}"
        return result.stdout

    def configure(self, name, settings=None, failure=False, source=None):
        binary = self.work / name
        command = [self.args.cmake, "-S", source or self.fixture, "-B", binary,
                   "-G", self.args.generator, f"-DTEST_WRAPPER={WRAPPER}",
                   "-DBGFX_CONFIG_MAX_DRAW_CALLS=1"]
        if self.args.platform:
            command += ["-A", self.args.platform]
        command += [f"-D{key}={value}" for key, value in (settings or {}).items()]
        output = self.run(*command, failure=failure)
        return binary, output

    def explicit(self):
        return {f"{name.upper()}_DIR": self.sources / name for name in PINS}

    def resolved(self, binary):
        return (binary / "resolved.txt").read_text().splitlines()

    def assert_roots(self, binary, roots=None):
        expected = roots or [self.sources / name for name in PINS]
        assert [Path(line).resolve() for line in self.resolved(binary)[:3]] == \
            [Path(root).resolve() for root in expected]

    def managed(self):
        binary, _ = self.configure("default")
        roots = [binary / "_deps" / "bgfx.cmake-src" / name for name in PINS]
        self.assert_roots(binary, roots)
        for root, (_, pin) in zip(roots, PINS.values()):
            assert git(root, "rev-parse", "HEAD") == pin
        assert Path(self.resolved(binary)[3]) == WRAPPER
        assert not (binary / "_deps" / "bgfx.cmake-src" / ".git").exists()
        assert not (binary / "_deps" / "bgfx.cmake-subbuild").exists()
        self.configure("default")
        self.assert_roots(binary, roots)
        cache = (binary / "CMakeCache.txt").read_text()
        assert not any(f"{name.upper()}_DIR:" in cache for name in PINS)
        previous = git(self.sources / "bgfx", "rev-parse", f"{PINS['bgfx'][1]}^")
        self.configure("default", {"TEST_BGFX_REPOSITORY": (self.sources / "bgfx").as_uri(),
                                   "TEST_BGFX_REV": previous})
        assert git(roots[0], "rev-parse", "HEAD") == previous
        self.configure("default", {"TEST_BGFX_REV": PINS["bgfx"][1]})
        assert git(roots[0], "rev-parse", "HEAD") == PINS["bgfx"][1]
        if os.name == "nt":
            before = (roots[0] / "src/version.h").read_bytes()
            self.configure("default", {"TEST_STAGE": str(roots[0].parent).lower()})
            assert (roots[0] / "src/version.h").read_bytes() == before
            for root, (_, pin) in zip(roots, PINS.values()):
                assert git(root, "rev-parse", "HEAD") == pin

    def roots(self):
        binary, _ = self.configure("absolute", self.explicit())
        self.assert_roots(binary)
        assert Path(self.resolved(binary)[4]) == binary / "_deps/bgfx.cmake-build"
        version = self.resolved(binary)[5]
        if os.name == "nt":
            lower = {key: str(value).lower() for key, value in self.explicit().items()}
            binary, _ = self.configure("windows-path-case", lower)
            assert self.resolved(binary)[5] == version
        relative = {f"{name.upper()}_DIR": Path(os.path.relpath(self.sources / name, WRAPPER)).as_posix()
                    for name in PINS}
        binary, _ = self.configure("relative", relative)
        self.assert_roots(binary)
        binary, _ = self.configure("custom-content-base",
                                   {**self.explicit(), "FETCHCONTENT_BASE_DIR": self.work / "content with spaces"})
        assert Path(self.resolved(binary)[4]) == self.work / "content with spaces/bgfx.cmake-build"
        spaced = self.work / "sources with spaces" / "bx"
        copy_sources(self.sources / "bx", spaced)
        settings = {f"{name.upper()}_DIR": "invalid-cache-value" for name in PINS}
        settings.update({f"TEST_{name.upper()}_DIR": self.sources / name for name in PINS})
        settings.update(TEST_NORMAL_ROOTS="ON", TEST_BX_DIR=spaced)
        binary, _ = self.configure("parent-normal", settings)
        self.assert_roots(binary, [self.sources / "bgfx", spaced, self.sources / "bimg"])
        overrides = {f"FETCHCONTENT_SOURCE_DIR_{name.upper()}": self.sources / name for name in PINS}
        binary, _ = self.configure("standard-overrides", overrides)
        self.assert_roots(binary)
        binary, _ = self.configure("mixed", {"BGFX_DIR": self.sources / "bgfx"})
        self.assert_roots(binary, [self.sources / "bgfx", binary / "_deps/bgfx.cmake-src/bx",
                                   binary / "_deps/bgfx.cmake-src/bimg"])
        settings = self.explicit()
        settings["BX_DIR"] = self.work / "missing-explicit-bx"
        binary, output = self.configure("invalid", settings, failure=True)
        assert "BX_DIR:" in output and "missing include/bx/bx.h" in output
        assert not (binary / "_deps/bgfx.cmake-src/bx").exists()

    def integration(self):
        settings = self.explicit()
        settings.update(TEST_INTEGRATION="ON",
                        TEST_INTEGRATION_MODULE=WRAPPER / "cmake/babylon-native.cmake")
        binary, _ = self.configure("nested-parent", settings)
        self.assert_roots(binary)
        assert Path(self.resolved(binary)[3]) == WRAPPER
        assert Path(self.resolved(binary)[4]) == binary / "_deps/bgfx.cmake-build"
        local = self.work / "local wrapper"
        copy_sources(WRAPPER, local)
        settings["FETCHCONTENT_SOURCE_DIR_BGFX.CMAKE"] = local
        binary, _ = self.configure("legacy-wrapper-override", settings)
        assert Path(self.resolved(binary)[3]) == local
        self.assert_roots(binary)

    def package(self):
        package = self.work / "package" / "bgfx.cmake-src"
        settings = self.explicit()
        settings["TEST_STAGE"] = package
        before = (self.sources / "bgfx/src/version.h").read_bytes()
        binary, _ = self.configure("stage", settings)
        self.configure("stage", settings)
        if os.name == "nt":
            lease = package / "bgfx/.staging-lease"
            lease.write_text("Keep this staged directory open during refresh.", encoding="ascii")
            with lease.open("rb"):
                _, output = self.configure("stage", settings, failure=True)
                assert "Cannot refresh the staged bgfx source directory" in output
            lease.unlink()
            self.configure("stage", settings)
        version = self.resolved(binary)[5]
        assert not list(package.rglob(".git"))
        assert (self.sources / "bgfx/src/version.h").read_bytes() == before
        settings = {"TEST_INTEGRATION": "ON",
                    "TEST_INTEGRATION_MODULE": WRAPPER / "cmake/babylon-native.cmake",
                    "FETCHCONTENT_SOURCE_DIR_BGFX.CMAKE": package,
                    "FETCHCONTENT_FULLY_DISCONNECTED": "ON"}
        binary, _ = self.configure("fresh-disconnected", settings)
        self.assert_roots(binary, [package / name for name in PINS])
        assert self.resolved(binary)[5] == version
        settings["CMAKE_DISABLE_FIND_PACKAGE_Git"] = "ON"
        binary, _ = self.configure("fresh-no-git-tool", settings)
        assert self.resolved(binary)[5] == version
        (package / "bgfx/.bgfx-cmake-version").unlink()
        settings.pop("CMAKE_DISABLE_FIND_PACKAGE_Git")
        binary, _ = self.configure("archive-header-fallback", settings)
        assert self.resolved(binary)[5] == "1.166.9149"

    def options(self):
        settings = self.explicit()
        settings["TEST_OPTIONS"] = "ON"
        self.configure("all-options", settings)

    def install(self):
        settings = self.explicit()
        settings["TEST_BIN2C"] = "ON"
        binary, _ = self.configure("standalone", settings)
        prefix = self.work / "installed"
        self.run(self.args.cmake, "--build", binary, "--config", "RelWithDebInfo", "--parallel", "4")
        self.run(self.args.cmake, "--install", binary, "--config", "RelWithDebInfo", "--prefix", prefix)
        consumer, _ = self.configure("consumer", {"CMAKE_PREFIX_PATH": prefix}, source=HERE / "consumer")
        self.run(self.args.cmake, "--build", consumer, "--config", "RelWithDebInfo", "--parallel", "4")
        executables = list(binary.rglob("bin2c.exe")) if os.name == "nt" else list(binary.rglob("bin2c"))
        assert executables, "bin2c was not built"

    def native_package(self):
        assert self.args.native_sources, "--native-sources must name a source-tree build's _deps"
        native = WRAPPER.parents[1]
        package = self.work / "native"
        for relative in git(native, "ls-files").splitlines():
            if relative.startswith(("Apps/", ".github/")):
                continue
            source = native / relative
            if source.is_file():
                target = package / relative
                target.parent.mkdir(parents=True, exist_ok=True)
                shutil.copy2(source, target)
        settings = {"BABYLON_NATIVE_BUILD_APPS": "OFF", "BABYLON_NATIVE_BUILD_SOURCETREE": "OFF",
                    "FETCHCONTENT_FULLY_DISCONNECTED": "ON", "GRAPHICS_API": "D3D11",
                    "NAPI_JAVASCRIPT_ENGINE": "Chakra"}
        for source in self.args.native_sources.resolve().glob("*-src"):
            target = self.work / "deps" / source.name
            copy_sources(source, target)
            settings[f"FETCHCONTENT_SOURCE_DIR_{source.name[:-4].upper()}"] = target
        assert not list((self.work / "deps").rglob(".git"))
        binary, _ = self.configure("fresh-native-disconnected", settings, source=package)
        assert (binary / "_deps/bgfx.cmake-build/cmake/bgfx/bgfx.vcxproj").is_file() \
            if os.name == "nt" else (binary / "_deps/bgfx.cmake-build/cmake/bgfx").is_dir()
        cache = (binary / "CMakeCache.txt").read_text()
        assert "FETCHCONTENT_FULLY_DISCONNECTED:BOOL=ON" in cache
        assert not (package / ".git").exists()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("contract", choices=["managed", "roots", "integration", "package", "options", "install", "native-package"])
    parser.add_argument("--sources", type=Path, required=True)
    parser.add_argument("--work", type=Path, required=True)
    parser.add_argument("--cmake", required=True)
    parser.add_argument("--generator", required=True)
    parser.add_argument("--platform", default="")
    parser.add_argument("--native-sources", type=Path)
    args = parser.parse_args()
    contracts = Contracts(args)
    getattr(contracts, args.contract.replace("-", "_"))()
    print(f"PASS {args.contract}: command logs in {contracts.work}")


if __name__ == "__main__":
    main()
