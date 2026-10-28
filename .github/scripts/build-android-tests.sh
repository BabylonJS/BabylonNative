#!/usr/bin/env bash
set -euo pipefail

project="${1:?Expected Gradle project directory}"
engine="${2:?Expected JavaScript engine}"
destination="${3:?Expected artifact directory}"

echo "y" | "$ANDROID_HOME/cmdline-tools/latest/bin/sdkmanager" "ndk;27.2.12479018" "cmake;3.30.3"
bash "$project/gradlew" -p "$project" -PabiFilters=x86_64 -PndkVersion=27.2.12479018 \
    "-PjsEngine=$engine" assembleDebug assembleDebugAndroidTest

mkdir -p "$destination"
cp "$project/app/build/outputs/apk/debug/app-debug.apk" "$destination/app.apk"
cp "$project/app/build/outputs/apk/androidTest/debug/app-debug-androidTest.apk" "$destination/tests.apk"
