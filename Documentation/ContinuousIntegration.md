This page describes the Continuous Integration (CI) used for BabylonNative.
The CI helps improve the quality of code for all platforms and avoid regression by testing the engine against various test scenes. Idealy, there is one test scene for every single feature of babylonjs.
Validation works by running the engine with a JS script that creates a scene. The rendering is captured and compared with a reference image.
If the rendered and reference images are too different, the test fails, an error will raise in the CI (failed build) and rendered/error images are stored as an download artefact than can help debug.
This is done for various platforms when a PR is opened, for nightly build and when a PR is merged in master branch.
To keep PR feedback fast, the slow or runner-scarce jobs run only in Azure nightly pipelines (at the repo root; both can also be queued manually): Hermes builds and the macOS-hosted Android builds run in `nightly.yml`, and the Win32 and macOS sanitizer builds run in `nightly-extended.yml`. Pushing a new commit to a PR cancels that PR's in-flight CI run.
CI will also take care of code quality: Warnings are treated as error and will break the build. A PR can't be merged if the build fails.

PR and master-push checks run through `.github/workflows/ci.yml`. The Linux and
Android builds use ccache; PR runs restore the cache saved by the latest master
push, and only master pushes save it.

Hermes builds (Win32 x64 D3D11, Linux Clang, and Android on Ubuntu) and the
macOS-hosted Android builds run only in the Azure Pipelines `nightly.yml`
pipeline, scheduled at 23:00 UTC Monday through Friday, or when that pipeline is
run manually. These configurations retain their CI build and test settings and
use the repository's npm dependency versions; the existing `Nightly` job separately tests
the Babylon.js preview download and publishes packaged binaries.

The nightly pipeline's downloadable binaries remain the existing Win32 x64
`RelWithDebInfo` Playground and UnitTests packages, with their existing artifact
names and contents.
The `Nightly` job doesn't wait for the other jobs in the pipeline. It publishes
packages after its own build and packaged validation/unit tests succeed.
Babylon.js downloads these packages only from fully succeeded runs, so a failure
in any job of `nightly.yml` still makes Babylon.js keep using the previous
successful run's packages. The packaged nightly job uploads error pictures on
failure; all Win32 jobs upload crash dumps and associated binaries on failure.
Other configurations retain their pipeline logs without artifact uploads.

Sanitizer builds (Win32 x64 V8 D3D11 and macOS) run only in the Azure Pipelines
`nightly-extended.yml` pipeline, scheduled at 01:00 UTC Tuesday through Saturday
(after `nightly.yml`), or when that pipeline is run manually. Nothing consumes its
artifacts, so sanitizer failures don't block the Babylon.js nightly packages.
The Win32 sanitizer build job runs UnitTests and publishes the Playground binaries;
the visual validation tests then run in parallel jobs (4 by default, set by the
`win32ValidationShards` pipeline parameter); with N jobs, each job takes every Nth
`config.json` entry through the Playground `--test-index` option. Each validation job uploads
rendered pictures on success or failure, and error pictures, crash dumps and
associated binaries on failure, with the job number appended to the artifact name.
The macOS sanitizer job retains its UnitTests core-dump upload on failure.

CI validation is composed of 3 main parts:
- The test datas
- The ValidationTests script
- The platform integration

For adding new Validation Tests, [you'll find extensive documentation here.](Documentation/AddingNewValidationTests.md)

# Test datas
They are shared with babylonjs for the most part.
Validation test scenes are listed in `Apps\Playground\Scripts\config.json`. It lists playground identifier that will be opened sequentialy. Per PG is may have additional parameters for validation pixels difference threshold.
This json is a subset of babylonjs' one. The goal is to use the exact same file once the engine is in a sufficiently stable state.

```json
{
    "root": "https://cdn.babylonjs.com",
    "tests": [
        {
            "title": "setParent",
            "playgroundId": "#JD49CT#2",
            "referenceImage": "setParent.png"
        },
        ...
}
```

Once the PG is loaded and a screenshot is saved on disk, it's compared to a reference image.
Those images are accessible here : [https://github.com/BabylonJS/Babylon.js/tree/master/tests/validation/ReferenceImages](https://github.com/BabylonJS/Babylon.js/tree/master/tests/validation/ReferenceImages)

If a new scene to test is added in the json file, a new reference must be added to babylonjs repo.
If a new feature or a bug fix makes the reference image different, it must also be updated.

# ValidationTests app

To make it simple, the playground app initialize the engine and runs `Apps\Playground\Scripts\validation_native.js`.
That js script is responsible for loading the json, running each test PG, make the screen capture and compare images.
When completed, the executable returns a value (0 for success, any other value for failure) that is used by the pipeline to determine the build success.

## The app executable

It's very similar to the playground app: 
- creates a window
- initialize engine and JS standard components
- initialize validation JS components
- loads and `validation_native.js`
- returns a value depending on success

The validation JS components are function needed for the execution of the validation script.
It adds features like loading and saving .png from a byte array, asks the renderer to do a screen shot, get working directories and exiting the app.
These features are multiplatform and the code can be found here : `Apps\ValidationTests\Shared\TestUtils.h`

## The test script

The native runner reuses its engine between scenes. Cleanup disables reverse depth
and explicitly restores the native `LEQUAL` depth comparison with
`engine.setDepthFunctionToLessOrEqual()`, along with resetting stencil and scissor state.

It loads the json using XMLHttpRequest, then for each entry in the json, loads the Playground, runs it, make a capture.
The most important part is the image comparison. It works the same way as babylonjs validation test.
The function `compare` is responsible for comparing each pixel. Every pixel for a same coordinate that is too much different between reference and rendering are counted.
If the number of different pixels are over a threshold, the test fails.
The difference threshold for a pixel is parametrized by the variable `threshold`. The range is [0..255]
The number of different pixels is parametrized by the variable `errorRatio`. The range of this value is (0 : no different pixel allowed, 100: every pixel can be different)

# The platform integration

Before being able to run the validationtests App, the build must succeed at building the platform. Any syntax error, warning, linking issue,... will result in a failure.
To be successful, the build must be able to generate all executable for all platforms and run the validationTest for supported ones.
Integration for the various platform can be tricky and is very specific to the platform. 
It's possible to open a window and have an openGL or Direct3D with a headless server.
For Apple platforms and Metal, it's was not possible to do the same.

The build and test steps for PR checks are defined in the reusable workflows
under `.github/workflows/`. Nightly build and test steps are defined in
`nightly.yml` and `nightly-extended.yml`.

## Win32

By far the simplest CI tests. Runs the feshly built executable in the working directory.
On Win32, the engine-variant jobs run visual validation (`validation_native.js`) only with V8 (`Win32_x64_V8_D3D11` in PR CI and the Win32 sanitizer validation jobs in `nightly-extended.yml`), so results don't depend on engine-specific behavior. The existing packaged `Nightly` job also runs visual validation. The Chakra, JSI, Hermes and QuickJS jobs still build and run UnitTests and ModuleLoadTest; sanitizer jobs skip ModuleLoadTest.
```shell
    cd buildWin32_x64\Apps\ValidationTests
    cd Release
    ValidationTests
```
Results and Errors directory are used for the Artifacts (see below).

## Linux

Almost the same as win32 but the headles  display has to be configured:
```shell
    export DISPLAY=:99
    Xvfb :99 -screen 0 1600x900x24 &
    sleep 3
    cd build/Apps/ValidationTests
    ./ValidationTests
```
The `sleep 3` command lets Xvfb finish initializing before running the command. 3 seconds is an arbitraty value.

## Android (WIP)

The tests are performed in a virtual machine aka Android Emulator. First thing to do is to create and start it:
```shell
    echo "no" | $ANDROID_HOME/tools/bin/avdmanager create avd -n test_android_emulator -k 'system-images;android-27;google_apis;x86' --force
    nohup $ANDROID_HOME/emulator/emulator -avd test_android_emulator -no-snapshot -skin 600x400 > /dev/null 2>&1 & $ANDROID_HOME/platform-tools/adb wait-for-device shell 'while [[ -z $(getprop sys.boot_completed | tr -d '\r') ]]; do sleep 1; done; input keyevent 82'
```

Then, we can install the validationTests .apk and launch the activity:
```shell
    adb install -t -g Apps/ValidationTests/Android/app/build/outputs/apk/debug/app-debug.apk
    adb shell am start -n com.android.babylonnative.validationtests/com.android.babylonnative.validationtests.ValidationTestsActivity
```

As the Emulator runs as a separate process, it's important to wait for the activity to stop:
```shell
    while [[ $(adb shell pidof com.android.babylonnative.validationtests) -ge 1 ]]; do sleep 1; done;
```

As the validation images are stored in the Emulator, it's mandatory to pull back the resulting datas so we can build the artifact.

```shell
    adb exec-out run-as com.android.babylonnative.validationtests tar c cache/ > Captures/cache.tar
```

## Artifacts

Artifact is a zip file, accessible thru the Azure build page. That zip file contains the rendered images and error screen that can be used for debugging. On Android, it also contains the logcat.
To see the artifact, go the the build page:

![BuildPage](Images/CI_BuildPage.jpg)

Click on the published artifact item to see all the published artifact for that particular build:

![BuildPage](Images/CI_Artifacts.jpg)

Then, for the intended plaform, download the .zip:

![BuildPage](Images/CI_DownloadZip.jpg)

Finally, open the zip file. It will contain all rendered images for the build. If the build fails at build time, no artifact will be produced. If the image comparison fails, an artifact containing the image difference will be published.

![BuildPage](Images/CI_ArtifactImage.jpg)
