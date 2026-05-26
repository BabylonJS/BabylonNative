// JS-side scaffolding for Tests.Perf.GLBAnimation.cpp.
//
// Loads Horse.glb from raw.githubusercontent.com, then steps every animation in the GLB frame
// by frame (group.from -> group.to in increments of 1) and renders each step via scene.render().
//
// Phase timings are reported back to C++ through four globals that the C++ side installs:
//   _perfNow(): number                             -- high-resolution wall time in ms (sub-ms)
//   _perfReport(label: string, durationMs: number) -- record a labeled timing
//   _perfDone()                                    -- signal end of benchmark
//   _perfFail(reason: string)                      -- signal a JS-side failure and end benchmark
//
// All timings on this side use _perfNow() (steady_clock-backed, sub-millisecond resolution).

import {
    AnimationGroup,
    AssetContainer,
    NativeEngine,
    Scene,
    SceneLoader,
} from "@babylonjs/core";

declare const _perfNow: () => number;
declare const _perfReport: (label: string, durationMs: number) => void;
declare const _perfDone: () => void;
declare const _perfFail: (reason: string) => void;

const HORSE_GLB_URL =
    "https://raw.githubusercontent.com/CedricGuillemet/dump/master/Horse.glb";

interface StepResult {
    frameCount: number;
    totalMs: number;
    minMs: number;
    maxMs: number;
    avgMs: number;
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

        const tick = () => {
            if (currentFrame > to) {
                engine.stopRenderLoop(tick);
                const totalMs = _perfNow() - start;
                resolve({
                    frameCount: count,
                    totalMs,
                    minMs: count > 0 ? min : 0,
                    maxMs: max,
                    avgMs: count > 0 ? sum / count : 0,
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
        const tick = () => {
            if (i >= count) {
                engine.stopRenderLoop(tick);
                const totalMs = _perfNow() - start;
                resolve({
                    frameCount: i,
                    totalMs,
                    minMs: i > 0 ? min : 0,
                    maxMs: max,
                    avgMs: i > 0 ? sum / i : 0,
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
    // Report the totals and individual frame stats as separate entries. The frame count is
    // embedded in the label (not passed as a ms value) so it is not formatted/aggregated as a
    // duration in the C++ summary.
    _perfReport(label + " (frames=" + result.frameCount + ") total (sum)", result.totalMs);
    _perfReport(label + "   min frame", result.minMs);
    _perfReport(label + "   max frame", result.maxMs);
    _perfReport(label + "   avg frame", result.avgMs);
}

async function runBenchmark(): Promise<void> {
    const jsStart = _perfNow();

    // 1. Engine + Scene + Camera setup.
    const setupStart = _perfNow();
    const engine = new NativeEngine();
    const scene = new Scene(engine);
    scene.createDefaultCamera(true, true, true);
    _perfReport("[JS]     NativeEngine + Scene + DefaultCamera", _perfNow() - setupStart);

    // 2. GLB fetch + parse.
    const loadStart = _perfNow();
    const container: AssetContainer = await SceneLoader.LoadAssetContainerAsync(
        "",
        HORSE_GLB_URL,
        scene,
    );
    _perfReport("[JS]     Horse.glb load (fetch + parse)", _perfNow() - loadStart);

    // 3. addAllToScene.
    const addStart = _perfNow();
    container.addAllToScene();
    _perfReport("[JS]     addAllToScene", _perfNow() - addStart);

    // 4. Wait for scene to be fully ready (textures, materials, etc.).
    const readyStart = _perfNow();
    await scene.whenReadyAsync();
    _perfReport("[JS]     scene.whenReadyAsync", _perfNow() - readyStart);

    const groups = container.animationGroups;
    _perfReport(`[JS]     animation groups discovered: count=${groups.length}`, 0);

    // 5. Step through every animation frame by frame.
    const renderAllStart = _perfNow();
    if (groups.length === 0) {
        const result = await renderStaticFrames(engine, scene, 1);
        reportFrameStats("[JS]     (no animations) static frame", result);
    } else {
        for (const group of groups) {
            // Disable looping so goToFrame is purely a pose set, not a play.
            group.loopAnimation = false;
            const result = await stepAnimationGroup(engine, scene, group);
            const label =
                "[JS]     '" + group.name + "' " +
                "range=[" + group.from + ", " + group.to + "]";
            reportFrameStats(label, result);
        }
    }
    _perfReport("[JS]     render all animations (sum)", _perfNow() - renderAllStart);

    _perfReport("[JS]     benchmark total (JS-side wall time)", _perfNow() - jsStart);
    _perfDone();
}

runBenchmark().catch((err) => {
    const message = err && err.stack ? String(err.stack) : String(err);
    console.error("[perf.glbAnimation] FAILED: " + message);
    _perfFail(message);
});
