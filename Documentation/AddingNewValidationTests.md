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

# Readiness and deterministic capture

The Native runner waits for scene readiness, GUI image readiness, and clean material
defines with ready effects in the active camera's render pass. Utility scenes using
the main scene's camera participate in the same check. Inspection restores the
previous render pass, including on errors.

Readiness polling does not render extra frames or consume `renderCount`. It refreshes
scene render IDs so material readiness is checked again on the next tick. A scene
that still has not converged after 240 waiting render-loop ticks fails explicitly
and follows normal once-only cleanup and suite continuation. Screenshot and
RenderDoc capture indices still count rendered frames only. Failures invalidate
pending screenshot callbacks so they cannot evaluate after cleanup or during the
next scene.

Each test restores both the seeded `Math.random` function and its seed, so a snippet
that replaces `Math.random` cannot change the sequence used by the next test.

The host-independent runner regressions execute the complete script with simulated
host services. Run them from the repository root with
`node --test Apps/Playground/Tests/validation_native.test.cjs`; CI runs them separately
from GPU validation.

# Generate Reference Images

Your test list is updated and your playground is ready to test. it's now time to generate a reference image.
open `Apps\Playground\Scripts\validation_native.js` and change `var generateReferences = false;` to true.
Run ValidationTest program, all reference images will be generated in `Apps/Playground/Results` subfolder of your build directory.
Copy the reference image for your test from that folder to `Apps/Playground/ReferenceImages` and add it with Git.

# Test new reference

Revert your change to `Apps\ValidationTests\Scripts\validation_native.js` and run validation tests.
Your new reference will compared to the rendering of your newly added Playground.