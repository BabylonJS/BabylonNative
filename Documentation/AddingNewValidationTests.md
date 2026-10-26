# Validation Tests
Validation tests load playground, perform rendering and compare the results with a reference Image.
The reference is part of the repo so adding new tests is updating the tests list and commit a new reference image.

# Playground

Most of the tests are [playground made on the web](https://playground.babylonjs.com/)
Once it's done, you can save it, get a snippet Id and add it to the test lists.

# Tests lists

In order to add a new test scene, first thing to do is to add a few lines in `Apps\ValidationTests\Scripts\config.json`. 

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

`title` : a string used for Window title and logging results in the console
`playgroundId` : the snippet id of the playground you want to test
`referenceImage` : the reference image name you want to compare to. You don't have a reference yet, so choose a self-explanatory name with .png extension.

# Re-enabling excluded tests

Verify excluded tests against the existing reference images, then run them together
without `--include-excluded` to check automatic selection and inter-test state.
When enabling a passing test globally, monitor the platform CI runs and add only
the backend or JavaScript-engine exclusions supported by actual failures.
Record the failure in `reason`; avoid a global exclusion for a platform-specific
problem. Do not change pixel tolerances or regenerate references just to make an
excluded test pass.

# Example workarounds

Some examples have bugs that only show up in the Native runner, for example rendering
before their resources are ready. Fix these in the example. Until an example is fixed,
its test can enable runner workarounds by ID in `config.json`:

```json
{
    "title": "GUI slider",
    "playgroundId": "#XXXXXX#1",
    "referenceImage": "gui-slider.png",
    "workarounds": ["wait-for-readiness"]
}
```

Only tests that list a workaround get it. Unknown IDs fail config loading. The
`workarounds` lists are the list of examples to fix. `--list` prints them in its last
TSV column. When a workaround changes what the runner does for a test, the runner logs
`Workaround '<id>' applied for '<title>'`.

| ID | Example bug | Runner behavior |
|---|---|---|
| `wait-for-readiness` | Renders before utility-layer scenes, GUI images, sprites, or material effects are ready | Waits for those as described below |
| `undeclared-name` | Reads the browser-only global `name` (`window.name`) | Declares `var name = ""` for the example code |
| `prime-effect-layers` | Captures effect-layer (glow/highlight) output on the first frame | Renders one extra frame before counting frames |
| `leak-cleanup` | Leaves textures or Draco state behind after disposal | Releases leaked textures and resets Draco after the test (stray scenes are disposed after every test) |
| `wait-for-import` | Starts `ImportMeshAsync`/`AppendSceneAsync`/`LoadAssetContainerAsync` (or `SceneLoader` equivalents), or a `NodeMaterial` load (`loadAsync`/`ParseFromFileAsync`/`ParseFromSnippetAsync`), in `createScene` without awaiting it | Waits for loader promises started before the scene is returned |
| `refresh-export-matrices` | Changes node transforms right before a glTF export; the exporter's no-op root-node check reads stale cached world matrices and drops the node | Recomputes every node's world matrix before `GLTF2Export.GLTFAsync`/`GLBAsync` |
| `wait-for-clustered-lights` | Uses a `ClusteredLightContainer`; Babylon.js 9.29 scene readiness skips its proxy shader, so lit materials can all report not-ready on the first frame and draw nothing | Calls `_isReady()` on each container (which starts compiling the proxy shader) and waits, without rendering, until all are ready |
| `wait-for-area-light-ltc` | Uses area lights; Babylon.js material readiness only checks the asynchronously downloaded LTC textures once `AREALIGHTUSED` is already in the defines, so an effect reused from the engine cache (e.g. compiled by the previous area-light test) reports ready before the LTC data arrives and area-lit surfaces render black | Waits, without rendering, until the scene's LTC1/LTC2 textures are ready |
| `opaque-clear-alpha` | Clears opaque but leaves alpha < 1 in the final image | Skips canvas-background compositing unless the test sets `canvasBackgroundColor`, the scene uses a frame graph, or `clearColor.a < 1` |
| `xhr-events` | Relies on `XMLHttpRequest` behavior the native polyfill lacks: the `load` event, or an asynchronous failure for a URL that cannot be opened | Patches new `XMLHttpRequest` instances to raise `load` before `loadend` for every completed response (`error` only for status 0), and to report `open()` URL-open failures as a DONE request with status 0 (`readystatechange`, `error`, `loadend`); other `open()` errors still throw |
| `es-module-import` | Loads an ES module through `Tools.LoadScript`'s module path (for example the SPZ decoder); the native script host has no ES module loader | Wraps `Tools.LoadScript` for module loads only: fetches each default-imported module, requires its sole export to be a trailing `export default name` (Emscripten `EXPORT_ES6` output), binds `import.meta.url`, and runs the inline source as an async function. Other module syntax is rejected |
| `global-self` | Reaches code that uses the browser/worker global `self` (for example GUI `InputText` focus registering clipboard listeners); the native host does not define it | Defines `self` as the polyfilled `window` while the test runs, then removes it |

Without `opaque-clear-alpha`, the runner matches the browser screenshot and always composites
translucent pixels over the canvas background (`canvasBackgroundColor`, or green-yellow by default).

Without `wait-for-readiness`, the runner matches the browser harness: it waits for the
main scene's `executeWhenReady` (10-minute timeout), then renders `renderCount` frames.

## wait-for-readiness

The runner waits for scene readiness, GUI image readiness, sprite-manager readiness, and clean material
defines with ready effects in the active camera's render pass. Utility scenes using
the main scene's camera participate in the same check. Inspection restores the
previous render pass, including on errors.

Sprite-manager readiness includes packed-atlas metadata, which `Scene.executeWhenReady`
does not wait for. When `scene.spritesEnabled` is false, sprite-manager readiness is
skipped; otherwise, all sprite managers must be ready.

The initial readiness wait and its 10-minute timeout cover both the main scene
and associated utility scenes. Their pending model/texture loads do not consume
the subsequent convergence checks.

Readiness polling does not render extra frames or consume `renderCount`. It refreshes
scene render IDs so material readiness is checked again on the next tick. A scene
that still has not converged after 240 waiting render-loop ticks fails explicitly
and follows normal once-only cleanup and suite continuation. Screenshot and
RenderDoc capture indices still count rendered frames only.

# Deterministic capture

Failures invalidate pending screenshot callbacks so they cannot evaluate after
cleanup or during the next scene.

Each test restores both the seeded `Math.random` function and its seed, so a snippet
that replaces `Math.random` cannot change the sequence used by the next test.

Terminal notifications from the engine's `onEffectErrorObservable` fail the active
test without waiting for readiness or scene-creation timeouts. Errors with remaining
fallbacks, a retained ready pipeline, or a disposed effect do not fail the test.
Cleanup runs outside the compiler notification callback and cancels pending readiness,
screenshot, and creation-timeout work. A late-loaded scene cannot replace the next test.
This uses published engine APIs; failures that do not emit an effect-error notification
still depend on the normal timeouts or a separate engine fix.

# Generate Reference Images

Your test list is updated and your playground is ready to test. it's now time to generate a reference image.
open `Apps\Playground\Scripts\validation_native.js` and change `var generateReferences = false;` to true.
Run ValidationTest program, all reference images will be generated in `Apps/Playground/Results` subfolder of your build directory.
Copy the reference image for your test from that folder to `Apps/Playground/ReferenceImages` and add it with Git.

# Test new reference

Revert your change to `Apps\ValidationTests\Scripts\validation_native.js` and run validation tests.
Your new reference will compared to the rendering of your newly added Playground.