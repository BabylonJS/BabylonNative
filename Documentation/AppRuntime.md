# AppRuntime

[JsRuntime](JsRuntime.md) is the most fundamental of all Babylon Native 
components, but it is not in any way self-contained. While the `JsRuntime`
abstraction provides a mechanism for getting onto the JavaScript thread and 
interacting with the JavaScript engine instance, it does not actually 
provide either of the things it gives access to. It is merely an integration
point, not an implementation.

The AppRuntime component is the "canonical" implementation that underlies
a JsRuntime. `AppRuntime` is designed to make it easy to integrate, 
initialize, and use everything needed for a Babylon Native app including
a dedicated thread for the JavaScript engine instance to run on, the 
JavaScript engine instance itself, and a `JsRuntime` to provide access to 
both of the prior resources. For most dedicated Babylon Native scenarios, 
creating an `AppRuntime` is the fastest and safest way to create and 
control JavaScript in a way that can be easily consumed by Babylon Native
components.

## AppRuntime Configuration

For the most part, AppRuntime usage should be quite straightforward, and
examples of how to use it can be found in every implementation of the 
provided Playground example app. However, AppRuntime allows for an unusual
amount of customization at CMake configuration time, which is worth 
a slightly more specific analysis.

AppRuntime is designed to be able to support multiple host platforms 
-- Win32, iOS, etc. -- and multiple JavaScript engines -- JavaScriptCore, 
Chakra, V8 (_not_ JSI, which is exclusive to React Native) -- by 
swapping out platform-specific parts of its implementation using the build 
system at configuration time. This implementation-swapping is controlled
by two CMake variables.

- `NAPI_JAVASCRIPT_ENGINE`: Thought this is technically a N-API 
    configuration variable, AppRuntime also reads and reacts to this 
    variable in order to select the implementation that will instantiate
    the desired JavaScript engine. At present, this configuration variable,
    which is defined in the 
    [N-API CMakeLists.txt](../Dependencies/napi/CMakeLists.txt), can take 
    any of the following values:
    - `Chakra`, the default value for Windows targets.
    - `v8`, the default value for Android targets.
    - `JavaScriptCore`, the default value for Apple targets.
    - Note that, while `JSI` is also an allowed value for
        `NAPI_JAVASCRIPT_ENGINE`, AppRuntime does not have an 
        implementation that can instantiate and own a JSI JavaScript engine
        instance.
- `BABYLON_NATIVE_PLATFORM`: This configuration variable is set internally
    by the Babylon Native build system depending on what target platform is
    specified and/or detected. For example, when configuring on Windows 
    without specifying a target platform, the default Win32 target will be 
    used and `BABYLON_NATIVE_PLATFORM` will be set to `Win32`. However, if
    `CMAKE_SYSTEM_NAME` is set to `WindowsStore` and `CMAKE_SYSTEM_VERSION` 
    is set to `10.0`, a Universal Windows target will be specified and
    `BABYLON_NATIVE_PLATFORM` will be set to `UWP`. At present, this 
    configuration variable will be set by the 
    [root-level CMakeLists.txt](../CMakeLists.txt) and can take any of the
    following values:
    - `Android`
    - `Apple` (for all Apple target platforms including iOS and macOS)
    - `UWP`
    - `Win32`
    - `Unix` (for Linux and other non-Apple Unix-like targets)

As mentioned above, these configuration-time variables will cause CMake to
select the platform- and engine-specific behaviors required to allow 
the AppRuntime component to function correctly in the desired scenario. 
None of these configuration variables change the contract of `AppRuntime`
at all; the exact same calling code should work with Chakra on Win32 as 
works with V8 on Android. It is not, however, possible to use every 
possible combination of platforms and engines as some JavaScript engines 
are only available on certain platforms.

As a final note, AppRuntime is not designed to be able to target more than
one platform/engine configuration within the same build. It is not, for 
example, possible to build a Win32 AppRuntime with the ability to target 
both Chakra _and_ V8, nor is it possible to build a version of AppRuntime
that can serve for both Win32 and UWP. Differently-configured `AppRuntime`s
are fundamentally divergent and mutually exclusive types, and to change 
which platform or engine is being used, AppRuntime must be reconfigured and
built again.
