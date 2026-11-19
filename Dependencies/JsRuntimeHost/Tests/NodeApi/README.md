# Node-API conformance tests

`NodeApiTests` runs JavaScript tests against dynamically loaded native addons through `node_lite`, a minimal test host rather than a Node.js implementation. Each desktop test runs in a separate process. Addons resolve the host's Node-API implementation; they must not link a second static copy of the runtime.

The suite is built with `JSRUNTIMEHOST_TESTS=ON` for desktop C Node-API engines. JSI is excluded because its C++ shim does not implement the C addon interface. Build the `NodeApiTests` target and run the executable from its output directory. The build stages JavaScript assets, addons, and engine runtime libraries there.

The enabled native directories are listed in `JSR_NODE_API_NATIVE_TEST_DIRS` in `CMakeLists.txt`. Other vendored directories are not enabled automatically. `test_reference_double_free/test_wrap.js` retains its inherited visible skip: V8 aborts when wrapping the plain JavaScript object used by this test.

On Android, the existing runtime test app embeds the harness for V8 and JavaScriptCore. Scripts run in-process with independent runtimes; addons remain separate shared libraries packaged with the app. Other Android engines and iOS are not enabled.

These are selected conformance cases, not proof of complete support for a particular Node-API version. The suite originates from [JsRuntimeHost#116](https://github.com/BabylonJS/JsRuntimeHost/pull/116). Microsoft and V8 source notices are retained; `LICENSE` contains the Node.js copyright and permission notices.
