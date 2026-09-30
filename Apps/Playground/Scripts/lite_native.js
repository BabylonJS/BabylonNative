// Babylon-Lite bundle validation harness for the NativeDawn (WebGPU) backend.
//
// Loads a single-file Babylon-Lite scene bundle (an IIFE "classic" script built
// from the Babylon-Lite repo -- see .github/scripts/bundle-lite-scene.mjs), lets
// it render to #renderCanvas via WebGPU, waits for the bundle's readiness signal
// (`canvas.dataset.ready === "true"`, set by the scene's main() after
// startEngine()), then captures the framebuffer and pixel-compares it against a
// committed reference image.
//
// The bundle is self-contained (it inlines all of Babylon-Lite), so this harness
// does NOT load babylon.max.js -- it only needs the native TestUtils API plus the
// NativeDawn-provided navigator.gpu / DOM canvas / requestAnimationFrame.
//
// Run (V8 build, WebGPU/NativeDawn):
//   Playground.exe --headless app:///Scripts/lite_native.js
// Regenerate the reference (writes Results/lite-scene1.png):
//   Playground.exe --headless --generate-references app:///Scripts/lite_native.js
(function () {
    "use strict";

    const opts = (typeof _playgroundOptions === "object" && _playgroundOptions) ? _playgroundOptions : {};
    const generateReferences = !!opts.generateReferences;

    // Fixed capture size so the reference is reproducible across machines /
    // WARP vs. discrete GPU window defaults. Matches the reference PNG.
    const W = 600;
    const H = 400;
    const BUNDLE = "app:///Scripts/scene1.playground.js";
    const REF = "lite-scene1.png";
    const THRESHOLD = 25;      // per-channel tolerance
    const ERROR_RATIO = 5.0;   // % of pixels allowed to differ
    const READY_TIMEOUT_MS = 120000;
    const SETTLE_FRAMES = 30;  // extra rendered frames after readiness to settle

    function fail(msg) {
        console.error("LITE_TEST_FAIL: " + msg);
        TestUtils.exit(-1);
    }

    function getCanvas() {
        try { return document.getElementById("renderCanvas"); } catch (e) { return null; }
    }

    console.log("Starting Babylon-Lite bundle test (scene1)");
    TestUtils.setTitle("Babylon-Lite scene1");
    TestUtils.updateSize(W, H);

    const renderCanvas = getCanvas();
    if (!renderCanvas || typeof renderCanvas.getContext !== "function") {
        fail("document.getElementById(\"renderCanvas\") did not return a canvas");
        return;
    }
    if (getCanvas() !== renderCanvas) {
        fail("document.getElementById(\"renderCanvas\") did not return a stable canvas");
        return;
    }
    if (document.querySelector("canvas") !== renderCanvas) {
        fail("document.querySelector(\"canvas\") did not return the render canvas");
        return;
    }

    // Load + evaluate the self-executing scene bundle.
    const xhr = new XMLHttpRequest();
    xhr.open("GET", BUNDLE, true);
    xhr.addEventListener("readystatechange", function () {
        if (xhr.readyState !== 4) { return; }
        if (xhr.status !== 200 && xhr.status !== 0) { fail("bundle load HTTP " + xhr.status); return; }
        try {
            // Indirect eval so the bundle runs in global scope (it grabs
            // #renderCanvas and kicks off its own WebGPU render loop).
            (0, eval)(xhr.responseText);
        } catch (e) {
            fail("bundle eval error: " + e);
            return;
        }
        waitReady(Date.now());
    });
    xhr.send();

    function waitReady(start) {
        const c = getCanvas();
        const ready = !!(c && c.dataset && c.dataset.ready === "true");
        if (ready) {
            console.log("Babylon-Lite scene1 ready (drawCalls=" +
                ((c.dataset && c.dataset.drawCalls) || "?") + ")");
            settleThenCapture(SETTLE_FRAMES);
            return;
        }
        if (Date.now() - start > READY_TIMEOUT_MS) {
            fail("scene not ready within " + (READY_TIMEOUT_MS / 1000) + "s");
            return;
        }
        setTimeout(function () { waitReady(start); }, 50);
    }

    function settleThenCapture(framesLeft) {
        if (framesLeft <= 0) { capture(); return; }
        requestAnimationFrame(function () { settleThenCapture(framesLeft - 1); });
    }

    // Load + decode the committed reference PNG (arraybuffer XHR -> native
    // image decoder). Avoids BABYLON.Tools.LoadFile so the harness needs no
    // babylon.max.js. Calls cb(referenceRgbaBytes) or cb(null) on failure.
    function loadReference(cb) {
        const rxhr = new XMLHttpRequest();
        rxhr.open("GET", "app:///ReferenceImages/" + REF, true);
        rxhr.responseType = "arraybuffer";
        rxhr.addEventListener("readystatechange", function () {
            if (rxhr.readyState !== 4) { return; }
            if ((rxhr.status !== 200 && rxhr.status !== 0) || !rxhr.response) { cb(null); return; }
            try {
                const img = TestUtils.decodeImage(rxhr.response);
                cb(TestUtils.getImageData(img));
            } catch (e) {
                console.error("reference decode error: " + e);
                cb(null);
            }
        });
        rxhr.send();
    }

    function capture() {
        TestUtils.getFrameBufferData(function (data) {
            const outDir = TestUtils.getOutputDirectory();
            if (generateReferences) {
                try {
                    TestUtils.writePNG(data, W, H, outDir + "/Results/" + REF);
                    console.log("Babylon-Lite reference written: Results/" + REF);
                    TestUtils.exit(0);
                } catch (e) { fail("writePNG error: " + e); }
                return;
            }
            loadReference(function (ref) {
              try {
                if (!ref || ref.length !== data.length) {
                    fail("reference length mismatch (ref=" + (ref ? ref.length : "null") +
                        " render=" + data.length + ")");
                    return;
                }

                let diff = 0;
                let firstOff = -1;
                for (let i = 0; i < data.length; i += 4) {
                    if (Math.abs(data[i] - ref[i]) < THRESHOLD &&
                        Math.abs(data[i + 1] - ref[i + 1]) < THRESHOLD &&
                        Math.abs(data[i + 2] - ref[i + 2]) < THRESHOLD) {
                        continue;
                    }
                    if (firstOff < 0) { firstOff = i / 4; }
                    // tint the reference red where different, for the diff overlay
                    ref[i] = 255; ref[i + 1] *= 0.5; ref[i + 2] *= 0.5;
                    diff++;
                }
                const pixels = data.length / 4;
                const ratio = (diff * 100) / pixels;
                if (diff > 0) {
                    console.log("First pixel off at pixel " + firstOff +
                        " (x=" + (firstOff % W) + ", y=" + Math.floor(firstOff / W) + ")");
                }
                console.log("Babylon-Lite scene1: diff " + diff + "/" + pixels +
                    " (" + ratio.toFixed(3) + "%, per-channel threshold " + THRESHOLD +
                    "); allowed errorRatio " + ERROR_RATIO + "%.");

                TestUtils.writePNG(data, W, H, outDir + "/Results/" + REF);
                if (ratio > ERROR_RATIO) {
                    TestUtils.writePNG(ref, W, H, outDir + "/Errors/" + REF);
                    console.error("LITE_TEST_FAIL: Babylon-Lite scene1 pixel comparison failed");
                    TestUtils.exit(-1);
                } else {
                    console.log("Babylon-Lite scene1 validated");
                    TestUtils.exit(0);
                }
              } catch (e) {
                fail("capture/compare error: " + e);
              }
            });
        });
    }
})();
