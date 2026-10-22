import argparse
import json
from pathlib import Path
import re
import shutil
import subprocess
import sys
import tarfile


COMPONENTS = ("BabylonNative", "JsRuntimeHost", "UrlLib")
ENGINES = ("JavaScriptCore", "V8", "QuickJS")
APPLICATION_IDS = {
    "BabylonNative": "com.babylonnative.unittests",
    "JsRuntimeHost": "com.jsruntimehost.unittests",
    "UrlLib": "com.urllib.unittests",
}


def artifact_name(platform, component, engine):
    return f"UnitTests-{platform}-{component}-{engine}"


def package(args):
    output = args.output
    output.mkdir(parents=True, exist_ok=False)
    manifest = {
        "platform": args.platform,
        "component": args.component,
        "engine": args.engine,
        "application_id": APPLICATION_IDS[args.component],
    }
    if args.platform == "Android":
        if not args.app.is_file() or not args.test_apk or not args.test_apk.is_file():
            raise RuntimeError("Both Android application and instrumentation APKs are required")
        shutil.copyfile(args.app, output / "app.apk")
        shutil.copyfile(args.test_apk, output / "tests.apk")
    else:
        if not args.app.is_dir() or args.app.suffix != ".app":
            raise RuntimeError(f"Missing iOS simulator application: {args.app}")
        with tarfile.open(output / "app.tar.gz", "w:gz") as archive:
            archive.add(args.app, arcname="UnitTests.app")
    (output / "manifest.json").write_text(json.dumps(manifest, indent=2) + "\n")


def discover(root, platform):
    expected = {
        artifact_name(platform, component, engine)
        for component in COMPONENTS
        for engine in (ENGINES if platform == "Android" else ("JavaScriptCore",))
    }
    actual = {path.name for path in root.iterdir() if path.is_dir()}
    if actual != expected:
        raise RuntimeError(
            f"Artifact mismatch: missing={sorted(expected - actual)}, unexpected={sorted(actual - expected)}"
        )
    manifests = []
    for name in sorted(expected):
        directory = root / name
        manifest = json.loads((directory / "manifest.json").read_text())
        component, engine = manifest["component"], manifest["engine"]
        if (
            manifest["platform"] != platform
            or name != artifact_name(platform, component, engine)
            or manifest["application_id"] != APPLICATION_IDS[component]
        ):
            raise RuntimeError(f"Invalid artifact manifest: {directory}")
        files = ("app.apk", "tests.apk") if platform == "Android" else ("app.tar.gz",)
        for file in files:
            if not (directory / file).is_file():
                raise RuntimeError(f"Missing {file} in {directory}")
        manifests.append((directory, manifest))
    return manifests


def run_android(directory, manifest, logs):
    application = manifest["application_id"]
    subprocess.run(["adb", "install", "-r", str(directory / "app.apk")], check=True, timeout=120)
    subprocess.run(["adb", "install", "-r", str(directory / "tests.apk")], check=True, timeout=120)
    subprocess.run(["adb", "shell", "pm", "clear", application], check=True, timeout=120)
    result = subprocess.run(
        ["adb", "shell", "am", "instrument", "-w", "-r",
         f"{application}.test/androidx.test.runner.AndroidJUnitRunner"],
        text=True, capture_output=True, timeout=1200,
    )
    text = result.stdout + result.stderr
    (logs / f"{directory.name}.log").write_text(text)
    print(text, flush=True)
    # adb can exit zero even when instrumentation crashes or reports test failures.
    if (
        result.returncode
        or not re.search(r"^INSTRUMENTATION_CODE: -1\s*$", text, re.MULTILINE)
        or not re.search(r"^OK \([1-9][0-9]* tests?\)\s*$", text, re.MULTILINE)
    ):
        raise RuntimeError(f"Android tests failed or did not complete: {directory.name}")
    for package_id in (application + ".test", application):
        subprocess.run(["adb", "uninstall", package_id], check=True, timeout=120)


def run_ios(directory, manifest, logs, simulator):
    with tarfile.open(directory / "app.tar.gz") as archive:
        for entry in archive.getmembers():
            path = Path(entry.name)
            if path.is_absolute() or ".." in path.parts or path.parts[0] != "UnitTests.app":
                raise RuntimeError(f"Unsafe application archive entry: {entry.name}")
            if entry.issym() or entry.islnk():
                raise RuntimeError(f"Unexpected link in simulator application: {entry.name}")
        archive.extractall(directory, filter="data")
    application = manifest["application_id"]
    subprocess.run(
        ["xcrun", "simctl", "install", simulator, str(directory / "UnitTests.app")],
        check=True, timeout=120,
    )
    result = subprocess.run(
        ["xcrun", "simctl", "launch", "--console", simulator, application],
        text=True, capture_output=True, timeout=1200,
    )
    (logs / f"{directory.name}.stdout.log").write_text(result.stdout)
    (logs / f"{directory.name}.stderr.log").write_text(result.stderr)
    print(result.stdout, flush=True)
    print(result.stderr, file=sys.stderr, flush=True)
    lines = result.stderr.strip().splitlines()
    if result.returncode or not lines or lines[-1].strip() != "0":
        raise RuntimeError(f"iOS tests failed or did not report exit zero: {directory.name}")
    subprocess.run(["xcrun", "simctl", "uninstall", simulator, application], check=True, timeout=120)


def run(args):
    artifacts = discover(args.artifacts, args.platform)
    args.logs.mkdir(parents=True, exist_ok=True)
    failed = []
    for directory, manifest in artifacts:
        print(f"Running {directory.name}", flush=True)
        try:
            if args.platform == "Android":
                run_android(directory, manifest, args.logs)
            else:
                run_ios(directory, manifest, args.logs, args.simulator)
        except (RuntimeError, subprocess.SubprocessError) as error:
            failed.append(directory.name)
            (args.logs / f"{directory.name}.error.log").write_text(str(error) + "\n")
            print(f"::error::{directory.name}: {error}", flush=True)
    if failed:
        raise RuntimeError("Mobile unit-test artifacts failed: " + ", ".join(failed))


def main():
    parser = argparse.ArgumentParser()
    commands = parser.add_subparsers(dest="command", required=True)
    pack = commands.add_parser("package")
    pack.add_argument("--component", choices=COMPONENTS, required=True)
    pack.add_argument("--engine", choices=ENGINES, required=True)
    pack.add_argument("--app", type=Path, required=True)
    pack.add_argument("--test-apk", type=Path)
    pack.add_argument("--output", type=Path, required=True)
    execute = commands.add_parser("run")
    execute.add_argument("--artifacts", type=Path, required=True)
    execute.add_argument("--logs", type=Path, required=True)
    execute.add_argument("--simulator", default="booted")
    for command in (pack, execute):
        command.add_argument("--platform", choices=("Android", "iOS"), required=True)
    args = parser.parse_args()
    if args.command == "package":
        package(args)
    else:
        run(args)


if __name__ == "__main__":
    main()
