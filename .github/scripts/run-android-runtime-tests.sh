#!/usr/bin/env bash
set -euo pipefail

artifact_root="${1:?Expected artifact directory}"
shift
if [[ $# -eq 0 ]]; then
    echo "Expected at least one JavaScript engine" >&2
    exit 1
fi

mkdir -p test-results
trap 'adb logcat -d > test-results/logcat.txt' EXIT
ready=false
for ((attempt = 0; attempt < 60; attempt++)); do
    connectivity=$(timeout 10s adb shell dumpsys connectivity)
    active=$(sed -n 's/^Active default network: \([0-9][0-9]*\).*$/\1/p' <<< "$connectivity")
    if [[ -n "$active" ]] && grep -F 'NetworkAgentInfo{' <<< "$connectivity" |
        grep -F "network{$active}" | grep -w VALIDATED > /dev/null; then
        ready=true
        break
    fi
    sleep 2
done
if [[ "$ready" != true ]]; then
    echo "::error::Emulator network did not become ready"
    printf '%s\n' "$connectivity"
    exit 1
fi

shopt -s nullglob
artifacts=("$artifact_root"/*)
if [[ ${#artifacts[@]} -ne $# ]]; then
    echo "::error::Expected exactly $# Android artifacts"
    exit 1
fi
for engine in "$@"; do
    directory="$artifact_root/UnitTests-Android-JsRuntimeHost-$engine"
    test -f "$directory/app.apk"
    test -f "$directory/tests.apk"
done

failed=()
application=com.jsruntimehost.unittests
for engine in "$@"; do
    name="UnitTests-Android-JsRuntimeHost-$engine"
    directory="$artifact_root/$name"
    echo "::group::Running $name"
    set +e
    (
        set -euo pipefail
        installed=()
        trap 'code=$?; for package in "${installed[@]}"; do timeout 120s adb uninstall "$package" || code=1; done; exit "$code"' EXIT
        timeout 120s adb install -r "$directory/app.apk"
        installed+=("$application")
        timeout 120s adb install -r "$directory/tests.apk"
        installed=("$application.test" "$application")
        timeout 120s adb shell pm clear "$application"
        timeout 20m adb shell am instrument -w -r \
            "$application.test/androidx.test.runner.AndroidJUnitRunner" 2>&1 | tee "test-results/$name.log"
        grep -Eq '^OK \([1-9][0-9]* tests?\)[[:space:]]*$' "test-results/$name.log"
        grep -Eq '^INSTRUMENTATION_CODE: -1[[:space:]]*$' "test-results/$name.log"
    )
    result=$?
    set -e
    if [[ $result -ne 0 ]]; then
        echo "::error::$name failed (exit $result)"
        failed+=("$name")
    fi
    echo "::endgroup::"
done
if [[ ${#failed[@]} -ne 0 ]]; then
    printf 'Failed: %s\n' "${failed[@]}"
    exit 1
fi
