# Node-API conformance tests

`NodeApiTests` runs JavaScript tests against dynamically loaded native addons through `node_lite`, a minimal test host rather than a Node.js implementation. Each desktop test runs in a separate process. Addons resolve the host's Node-API implementation; they must not link a second static copy of the runtime.

The suite is built with `JSRUNTIMEHOST_TESTS=ON` for desktop C Node-API engines. JSI is excluded because its C++ shim does not implement the C addon interface. Build the `NodeApiTests` target and run the executable from its output directory. The build stages JavaScript assets, addons, and engine runtime libraries there.

The enabled native directories are listed in `JSR_NODE_API_NATIVE_TEST_DIRS` in `CMakeLists.txt`: every vendored directory whose tests need only Node-API version 5, the version JsRuntimeHost implements. The remaining directories need a newer version or `NAPI_EXPERIMENTAL`, and are enabled only once the shared headers support it:

| Directory | Needs |
|---|---|
| `6_object_wrap`, `test_finalizer` | `NAPI_EXPERIMENTAL` |
| `test_bigint`, `test_instance_data` | 6 |
| `test_typedarray` | 7 |
| `test_object` | 6 and 8 |
| `test_cannot_run_js`, `test_error`, `test_properties`, `test_reference` | 9 |
| `test_string` | 10 |

The vendored sources are not modified. `include/js_native_api.h` adapts them to the shared header: it defines the `node_api_basic_env` family of aliases and maps the standard three-argument `napi_run_script` to the shared four-argument variant.

An addon that imports an API an engine does not implement cannot load, so `CMakeLists.txt` drops that directory for that engine. Scripts that fail because of a known engine defect or a runner limitation are listed in `kExclusions` in `test_main.cpp` and reported as skipped with the reason. Remove an entry when its defect is fixed, so the test guards the fix.

On Android, the existing runtime test app embeds the harness for V8 and JavaScriptCore. Scripts run in-process with independent runtimes; addons remain separate shared libraries packaged with the app. The runner cannot spawn child processes, so the scripts that do are skipped there. Other Android engines and iOS are not enabled.

These are Node-API version 5 conformance cases, not proof of complete support for any version. The suite originates from [JsRuntimeHost#116](https://github.com/BabylonJS/JsRuntimeHost/pull/116). Microsoft and V8 source notices are retained; `LICENSE` contains the Node.js copyright and permission notices.
