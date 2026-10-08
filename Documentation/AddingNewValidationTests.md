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
| `wait-for-readiness` | Renders before utility-layer scenes, GUI images, or material effects are ready | Waits for those as described below |
| `undeclared-name` | Reads the browser-only global `name` (`window.name`) | Declares `var name = ""` for the example code |
| `prime-effect-layers` | Captures effect-layer (glow/highlight) output on the first frame | Renders one extra frame before counting frames |
| `leak-cleanup` | Leaves scenes, textures, or Draco state behind after disposal | Disposes stray scenes, releases leaked textures, and resets Draco after the test |
| `wait-for-import` | Starts `ImportMeshAsync`/`AppendSceneAsync`/`LoadAssetContainerAsync` in `createScene` without awaiting it | Waits for loader promises started before the scene is returned |
| `opaque-clear-alpha` | Clears opaque but leaves alpha < 1 in the final image | Skips canvas-background compositing unless the test sets `canvasBackgroundColor`, the scene uses a frame graph, or `clearColor.a < 1` |

Without `opaque-clear-alpha`, the runner matches the browser screenshot and always composites
translucent pixels over the canvas background (`canvasBackgroundColor`, or green-yellow by default).

Without `wait-for-readiness`, the runner matches the browser harness: it waits for the
main scene's `executeWhenReady` (10-minute timeout), then renders `renderCount` frames.

## wait-for-readiness

The runner waits for scene readiness, GUI image readiness, and clean material
defines with ready effects in the active camera's render pass. Utility scenes using
the main scene's camera participate in the same check. Inspection restores the
previous render pass, including on errors.

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