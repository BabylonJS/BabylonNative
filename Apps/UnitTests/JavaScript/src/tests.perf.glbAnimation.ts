// JS-side scaffolding for Tests.Perf.GLBAnimation.cpp.
//
// Enumerates `<exe-dir>/glb/*.glb` via TestUtils.listFiles (exposed by the TestUtils
// plugin), then for each GLB: fetches the bytes, parses them through SceneLoader, and steps
// every animation in the asset frame by frame (group.from -> group.to in increments of 1)
// rendering each step via scene.render().
//
// Phase timings (and memory deltas) are reported back to C++ through five globals that the
// C++ side installs:
//   _perfNow(): number                                              -- wall time ms (sub-ms)
//   _memBytes(): number                                             -- process private bytes
//   _perfReport(label, durationMs[, deltaBytes])                    -- record a labeled timing
//   _perfDone()                                                     -- signal end of benchmark
//   _perfFail(reason)                                               -- signal a JS-side failure
//
// Memory deltas are computed against the process-level private byte counter (Win32
// PrivateUsage / macOS phys_footprint / Linux VmData) -- engine-agnostic since it does not
// depend on which allocator the JS engine uses internally. This lets the benchmark serve as
// a cross-engine memory comparison: same phases, same labels, different engine -> different
// deltas.
//
// All timings on this side use _perfNow() (steady_clock-backed, sub-millisecond resolution).

import {
    AnimationGroup,
    AssetContainer,
    NativeEngine,
    Scene,
    SceneLoader,
    Tools,
} from "@babylonjs/core";

declare const _perfNow: () => number;
declare const _memBytes: () => number;
declare const _perfReport: (
    label: string,
    durationMs: number,
    deltaBytes?: number,
) => void;
declare const _perfDone: () => void;
declare const _perfFail: (reason: string) => void;

// Installed by the TestUtils plugin on the C++ side; listFiles enumerates regular files
// in <exe-dir>/<relativePath> (basenames only, sorted, no recursion).
declare const TestUtils: {
    listFiles: (relativePath: string) => string[];
};

// GLB binaries are copied next to the executable under `glb/` by Apps/UnitTests/CMakeLists.txt.
// The same string is used both to list them and to build the per-file `app:///` URL.
const GLB_FOLDER = "glb";

interface StepResult {
    frameCount: number;
    totalMs: number;
    minMs: number;
    maxMs: number;
    avgMs: number;
    deltaBytes: number;
}

function stepAnimationGroup(
    engine: NativeEngine,
    scene: Scene,
    group: AnimationGroup,
): Promise<StepResult> {
    return new Promise((resolve) => {
        const from = Math.floor(group.from);
        const to = Math.ceil(group.to);

        let currentFrame = from;
        let count = 0;
        let sum = 0;
        let min = Number.POSITIVE_INFINITY;
        let max = 0;
        const start = _perfNow();
        // Memory delta is captured per group (not per frame): per-frame deltas would be ~0
        // for most engines once the steady-state allocator is warmed up, and the noise from
        // GC events would swamp the signal. Per-group totals show the net retained growth
        // across rendering an entire animation.
        const memStart = _memBytes();

        const tick = () => {
            if (currentFrame > to) {
                engine.stopRenderLoop(tick);
                const totalMs = _perfNow() - start;
                const deltaBytes = _memBytes() - memStart;
                resolve({
                    frameCount: count,
                    totalMs,
                    minMs: count > 0 ? min : 0,
                    maxMs: max,
                    avgMs: count > 0 ? sum / count : 0,
                    deltaBytes,
                });
                return;
            }
            const t0 = _perfNow();
            group.goToFrame(currentFrame);
            scene.render();
            const dt = _perfNow() - t0;
            if (dt < min) {
                min = dt;
            }
            if (dt > max) {
                max = dt;
            }
            sum += dt;
            count++;
            currentFrame++;
        };

        engine.runRenderLoop(tick);
    });
}

function renderStaticFrames(
    engine: NativeEngine,
    scene: Scene,
    count: number,
): Promise<StepResult> {
    return new Promise((resolve) => {
        let i = 0;
        let sum = 0;
        let min = Number.POSITIVE_INFINITY;
        let max = 0;
        const start = _perfNow();
        const memStart = _memBytes();
        const tick = () => {
            if (i >= count) {
                engine.stopRenderLoop(tick);
                const totalMs = _perfNow() - start;
                const deltaBytes = _memBytes() - memStart;
                resolve({
                    frameCount: i,
                    totalMs,
                    minMs: i > 0 ? min : 0,
                    maxMs: max,
                    avgMs: i > 0 ? sum / i : 0,
                    deltaBytes,
                });
                return;
            }
            const t0 = _perfNow();
            scene.render();
            const dt = _perfNow() - t0;
            if (dt < min) {
                min = dt;
            }
            if (dt > max) {
                max = dt;
            }
            sum += dt;
            i++;
        };
        engine.runRenderLoop(tick);
    });
}

function reportFrameStats(label: string, result: StepResult): void {
    // The total carries the memory delta for the whole group; min/max/avg are pure per-frame
    // timing stats and have no associated mem delta (would be misleading at per-frame scale).
    _perfReport(
        label + " (frames=" + result.frameCount + ") total (sum)",
        result.totalMs,
        result.deltaBytes,
    );
    _perfReport(label + "   min frame", result.minMs);
    _perfReport(label + "   max frame", result.maxMs);
    _perfReport(label + "   avg frame", result.avgMs);
}

// Runs the fetch -> parse -> addToScene -> render-all-animations cycle for a single GLB.
// Each GLB gets its own NativeEngine + Scene so the per-asset timings are not polluted by
// state held over from previous assets (animation groups, materials, GC pressure).
async function runOneGlb(name: string): Promise<void> {
    const tag = "'" + name + "'";
    const url = "app:///" + GLB_FOLDER + "/" + name;

    // Engine + Scene + Camera (per-GLB so each measurement starts clean).
    const setupT0 = _perfNow();
    const setupM0 = _memBytes();
    const engine = new NativeEngine();
    const scene = new Scene(engine);
    scene.createDefaultCamera(true, true, true);
    _perfReport("[JS]     " + tag + " setup (NativeEngine+Scene+Camera)",
        _perfNow() - setupT0, _memBytes() - setupM0);

    // Fetch the GLB bytes from the local app:/// URL (XHR -> file read, no network).
    const fetchT0 = _perfNow();
    const fetchM0 = _memBytes();
    const buffer = (await Tools.LoadFileAsync(url, true)) as ArrayBuffer;
    _perfReport(
        "[JS]     " + tag + " fetch (XHR arraybuffer, " + buffer.byteLength + " bytes)",
        _perfNow() - fetchT0, _memBytes() - fetchM0,
    );

    // Parse the GLB through the SceneLoader using the already-fetched bytes.
    // Passing a Uint8Array bypasses the URL fetch path; pluginExtension tells the loader
    // which plugin (glTF 2.0) to use since there is no filename to infer it from.
    const parseT0 = _perfNow();
    const parseM0 = _memBytes();
    const container: AssetContainer = await SceneLoader.LoadAssetContainerAsync(
        "",
        new Uint8Array(buffer),
        scene,
        undefined,
        ".glb",
    );
    _perfReport("[JS]     " + tag + " parse (SceneLoader)",
        _perfNow() - parseT0, _memBytes() - parseM0);

    // addAllToScene.
    const addT0 = _perfNow();
    const addM0 = _memBytes();
    container.addAllToScene();
    _perfReport("[JS]     " + tag + " addAllToScene",
        _perfNow() - addT0, _memBytes() - addM0);

    // Wait for scene to be fully ready (textures, materials, etc.).
    const readyT0 = _perfNow();
    const readyM0 = _memBytes();
    await scene.whenReadyAsync();
    _perfReport("[JS]     " + tag + " scene.whenReadyAsync",
        _perfNow() - readyT0, _memBytes() - readyM0);

    const groups = container.animationGroups;
    _perfReport("[JS]     " + tag + " animation groups discovered: count=" + groups.length, 0);

    // Step through every animation frame by frame.
    const renderAllStart = _perfNow();
    const renderAllMemStart = _memBytes();
    if (groups.length === 0) {
        const result = await renderStaticFrames(engine, scene, 1);
        reportFrameStats("[JS]     " + tag + " (no animations) static frame", result);
    } else {
        for (const group of groups) {
            // Disable looping so goToFrame is purely a pose set, not a play.
            group.loopAnimation = false;
            const result = await stepAnimationGroup(engine, scene, group);
            const label =
                "[JS]     " + tag + " '" + group.name + "' " +
                "range=[" + group.from + ", " + group.to + "]";
            reportFrameStats(label, result);
        }
    }
    _perfReport("[JS]     " + tag + " render all animations (sum)",
        _perfNow() - renderAllStart, _memBytes() - renderAllMemStart);

    // Dispose so the next GLB starts with a fresh engine/scene state.
    const disposeT0 = _perfNow();
    const disposeM0 = _memBytes();
    scene.dispose();
    engine.dispose();
    _perfReport("[JS]     " + tag + " dispose (scene+engine)",
        _perfNow() - disposeT0, _memBytes() - disposeM0);
}

async function runBenchmark(): Promise<void> {
    const jsStart = _perfNow();
    const jsMemStart = _memBytes();

    if (typeof TestUtils === "undefined" || typeof TestUtils.listFiles !== "function") {
        throw new Error("TestUtils.listFiles not available -- TestUtils plugin not initialized?");
    }

    const names = TestUtils.listFiles(GLB_FOLDER).filter((n) => n.toLowerCase().endsWith(".glb"));
    _perfReport("[JS]     glb files discovered: count=" + names.length, 0);

    if (names.length === 0) {
        _perfFail(
            "No .glb files found in '" + GLB_FOLDER + "/' next to the executable. " +
            "Drop at least one .glb into Apps/UnitTests/glb/ and rebuild.",
        );
        return;
    }

    for (const name of names) {
        await runOneGlb(name);
    }

    _perfReport(
        "[JS]     benchmark total (JS-side wall time)",
        _perfNow() - jsStart,
        _memBytes() - jsMemStart,
    );
    _perfDone();
}

runBenchmark().catch((err) => {
    const message = err && err.stack ? String(err.stack) : String(err);
    console.error("[perf.glbAnimation] FAILED: " + message);
    _perfFail(message);
});
