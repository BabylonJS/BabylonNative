globalThis.__babylonUseWebGPU = true;

var previousSceneFactorySignal = globalThis.__babylonPlaygroundSceneFactoryReady;
var sceneFactorySignalVersion =
    previousSceneFactorySignal && typeof previousSceneFactorySignal.version === "number"
        ? previousSceneFactorySignal.version + 1
        : 1;
var sceneFactorySignalResolve = null;
var sceneFactorySignalPromise = new Promise(function (resolve) {
    sceneFactorySignalResolve = resolve;
});
globalThis.__babylonPlaygroundSceneFactoryReady = {
    version: sceneFactorySignalVersion,
    promise: sceneFactorySignalPromise
};

var previousSmokeReadySignal = globalThis.__babylonPlaygroundWebGpuSmokeReady;
var smokeReadySignalVersion =
    previousSmokeReadySignal && typeof previousSmokeReadySignal.version === "number"
        ? previousSmokeReadySignal.version + 1
        : 1;
var smokeReadySignalResolve = null;
var smokeReadySignalPromise = new Promise(function (resolve) {
    smokeReadySignalResolve = resolve;
});
globalThis.__babylonPlaygroundWebGpuSmokeReady = {
    version: smokeReadySignalVersion,
    promise: smokeReadySignalPromise
};

var kSmokeFontName = "wgpu_smoke_font";
var kSmokeFontUrl = "app:///Scripts/RobotoSlab.ttf";
var smokeFontLoaded = false;
var smokeFontLoadPromise = null;
var smokeCanvas = null;
var smokeCtx = null;
var smokeGradient = null;
var smokeReportedMetrics = false;
var smokeFontFamily = "sans-serif";
var smokeUploadAttemptCount = 0;
var smokeUploadSucceeded = false;
var smokeUploadStateReported = null;
var smokeMetricsInvalidReported = false;
var smokeNeedsFontRefreshUpload = false;
var smokeDisposed = false;
var smokeOnBeforeRenderLogged = false;
var smokeDrawRequestSignaled = false;
var smokeQueueCopyReadyPromise = null;
var smokeQueueCopyQueue = null;
var smokeQueueCopyTexture = null;
var smokeUploadModeReported = null;

function reportStatus(message) {
    try {
        if (typeof globalThis.__nativePlaygroundStatus === "function") {
            globalThis.__nativePlaygroundStatus(String(message));
        }
    } catch (error) {
        void error;
    }
}

function resolveSmokeReadySignal(reason) {
    if (typeof smokeReadySignalResolve !== "function") {
        return;
    }

    smokeReadySignalResolve({
        reason: String(reason || "unknown")
    });
    smokeReadySignalResolve = null;
    reportStatus("webgpu-smoke:ready:" + String(reason || "unknown"));
}

function markWebGpuDrawRequested(reason) {
    if (smokeDrawRequestSignaled) {
        return;
    }

    if (navigator.gpu && typeof navigator.gpu._markWebGpuDrawRequested === "function") {
        navigator.gpu._markWebGpuDrawRequested();
        smokeDrawRequestSignaled = true;
        reportStatus("webgpu-smoke:draw-requested:" + String(reason || "unknown"));
    }
}

function getNativeWebGpuStatsSnapshot() {
    if (!navigator.gpu) {
        return null;
    }

    var statsFn = null;
    if (typeof navigator.gpu._backendStats === "function") {
        statsFn = navigator.gpu._backendStats;
    } else if (typeof navigator.gpu._debugStats === "function") {
        // Back-compat while older bridge names are still present.
        statsFn = navigator.gpu._debugStats;
    }

    if (!statsFn) {
        return null;
    }

    try {
        return statsFn();
    } catch (error) {
        reportStatus("webgpu-smoke:stats-call-error:" + String(error));
        return null;
    }
}

function ensureCanvasFontLoaded() {
    if (smokeFontLoaded) {
        return Promise.resolve(true);
    }

    if (smokeFontLoadPromise) {
        return smokeFontLoadPromise;
    }

    if (typeof _native === "undefined" || !_native.Canvas || typeof _native.Canvas.loadTTFAsync !== "function") {
        reportStatus("webgpu-smoke:font-api-unavailable");
        return Promise.resolve(false);
    }

    smokeFontLoadPromise = new Promise(function (resolve) {
        BABYLON.Tools.LoadFile(
            kSmokeFontUrl,
            function (data) {
                var loadResult;
                try {
                    loadResult = _native.Canvas.loadTTFAsync(kSmokeFontName, data);
                } catch (error) {
                    reportStatus("webgpu-smoke:font-register-failed:" + String(error));
                    resolve(false);
                    return;
                }

                Promise.resolve(loadResult).then(function () {
                    smokeFontLoaded = true;
                    smokeFontFamily = "\"" + kSmokeFontName + "\", sans-serif";
                    smokeNeedsFontRefreshUpload = true;
                    reportStatus("webgpu-smoke:font-loaded");
                    resolve(true);
                }).catch(function (error) {
                    reportStatus("webgpu-smoke:font-register-failed:" + String(error));
                    resolve(false);
                });
            },
            undefined,
            undefined,
            true,
            function (request, error) {
                var reason = "";
                if (error) {
                    reason = String(error);
                } else if (request && typeof request.status !== "undefined") {
                    reason = "status=" + String(request.status);
                }
                reportStatus("webgpu-smoke:font-download-failed:" + reason);
                resolve(false);
            }
        );
    }).then(function (result) {
        if (!result) {
            smokeFontLoadPromise = null;
        }
        return result;
    });

    return smokeFontLoadPromise;
}

function ensureSmokeCanvasContext() {
    if (smokeDisposed) {
        smokeDisposed = false;
    }

    if (smokeCanvas && smokeCtx) {
        return smokeCtx;
    }

    if (typeof _native === "undefined" || !_native.Canvas) {
        return null;
    }

    try {
        smokeCanvas = new _native.Canvas();
    } catch (error) {
        reportStatus("webgpu-smoke:canvas-ctor-failed:" + String(error));
        return null;
    }
    smokeCanvas.width = 512;
    smokeCanvas.height = 512;
    smokeCtx = smokeCanvas.getContext("2d");
    return smokeCtx;
}

function ensureStandardQueueCopyPath() {
    if (smokeQueueCopyReadyPromise) {
        return smokeQueueCopyReadyPromise;
    }

    smokeQueueCopyReadyPromise = (async function () {
        if (!navigator.gpu || typeof navigator.gpu.requestAdapter !== "function") {
            reportStatus("webgpu-smoke:queue-copy-ready:0:navigator.gpu-missing");
            return false;
        }

        var adapter = await navigator.gpu.requestAdapter();
        if (!adapter || typeof adapter.requestDevice !== "function") {
            reportStatus("webgpu-smoke:queue-copy-ready:0:adapter-missing");
            return false;
        }

        var device = await adapter.requestDevice();
        if (!device || !device.queue || typeof device.queue.copyExternalImageToTexture !== "function") {
            reportStatus("webgpu-smoke:queue-copy-ready:0:queue-copy-missing");
            return false;
        }

        var width = smokeCanvas && smokeCanvas.width ? smokeCanvas.width : 512;
        var height = smokeCanvas && smokeCanvas.height ? smokeCanvas.height : 512;
        var format = "bgra8unorm";
        if (typeof navigator.gpu.getPreferredCanvasFormat === "function") {
            format = navigator.gpu.getPreferredCanvasFormat();
        }

        var textureUsageCopyDst = (typeof GPUTextureUsage !== "undefined" && GPUTextureUsage.COPY_DST) ? GPUTextureUsage.COPY_DST : 0x08;
        var textureUsageTextureBinding = (typeof GPUTextureUsage !== "undefined" && GPUTextureUsage.TEXTURE_BINDING) ? GPUTextureUsage.TEXTURE_BINDING : 0x04;
        smokeQueueCopyTexture = device.createTexture({
            label: "webgpu-smoke.copyExternalImageToTexture.dst",
            size: [width, height, 1],
            format: format,
            usage: textureUsageCopyDst | textureUsageTextureBinding
        });
        smokeQueueCopyQueue = device.queue;
        reportStatus("webgpu-smoke:queue-copy-ready:1");
        return true;
    })().catch(function (error) {
        reportStatus("webgpu-smoke:queue-copy-ready:0:error:" + String(error));
        return false;
    });

    return smokeQueueCopyReadyPromise;
}

function disposeSmokeCanvas() {
    smokeDisposed = true;
    smokeUploadSucceeded = false;
    smokeNeedsFontRefreshUpload = false;
    smokeUploadStateReported = null;
    smokeGradient = null;
    smokeReportedMetrics = false;
    smokeMetricsInvalidReported = false;
    smokeUploadAttemptCount = 0;
    smokeQueueCopyReadyPromise = null;
    smokeQueueCopyQueue = null;
    smokeQueueCopyTexture = null;
    smokeUploadModeReported = null;

    try {
        if (smokeCtx && typeof smokeCtx.destroy === "function") {
            smokeCtx.destroy();
        }
    } catch (error) {
        void error;
    }

    try {
        if (smokeCanvas && typeof smokeCanvas.destroy === "function") {
            smokeCanvas.destroy();
        }
    } catch (error) {
        void error;
    }

    smokeCtx = null;
    smokeCanvas = null;
}

globalThis.__webgpuSmokeDispose = disposeSmokeCanvas;

function drawCanvasTextureSource(frameSeed) {
    var ctx = ensureSmokeCanvasContext();
    if (!ctx) {
        reportStatus("webgpu-smoke:canvas-unavailable");
        return null;
    }

    if (typeof ctx.flush === "function") {
        // CanvasWgpu currently applies clear-on-resize during flush. Prime once
        // before issuing draw commands so the presentation flush keeps content.
        ctx.flush();
    }

    if (!smokeGradient) {
        smokeGradient = ctx.createLinearGradient(0, 0, 512, 512);
        smokeGradient.addColorStop(0.0, "#162c4e");
        smokeGradient.addColorStop(0.4, "#3a95ff");
        smokeGradient.addColorStop(1.0, "#b25dff");
    }
    ctx.fillStyle = smokeGradient;
    ctx.fillRect(0, 0, 512, 512);

    ctx.filter = "blur(10px)";
    ctx.fillStyle = "#ffb438";
    ctx.fillRect(36, 44, 440, 96);
    ctx.filter = "none";

    ctx.fillStyle = "#0b1230";
    ctx.fillRect(40, 128, 432, 176);
    ctx.fillStyle = "#f6fbff";
    ctx.strokeStyle = "#02050c";
    ctx.lineWidth = 8;
    ctx.font = "bold 132px " + smokeFontFamily;
    var titleMetrics = ctx.measureText("WGPU");
    var titleWidth = titleMetrics && titleMetrics.width ? titleMetrics.width : 0;
    if (!isFinite(titleWidth) || titleWidth <= 0 || titleWidth > 2048) {
        // Some simulator runs can start before custom font registration resolves.
        // Fallback keeps text rendering deterministic and avoids invalid metrics.
        smokeFontFamily = "sans-serif";
        ctx.font = "bold 132px " + smokeFontFamily;
        titleMetrics = ctx.measureText("WGPU");
        titleWidth = titleMetrics && titleMetrics.width ? titleMetrics.width : 0;
    }
    if (!isFinite(titleWidth) || titleWidth <= 0 || titleWidth > 2048) {
        if (!smokeMetricsInvalidReported) {
            reportStatus("webgpu-smoke:metrics:title-invalid");
            smokeMetricsInvalidReported = true;
        }
        // Keep uploads alive even when simulator text metrics are temporarily invalid.
        // This avoids long startup gaps where no canvas texture reaches native.
        titleWidth = 360;
    } else {
        smokeMetricsInvalidReported = false;
    }
    if (!smokeReportedMetrics) {
        reportStatus("webgpu-smoke:metrics:title=" + String(titleWidth));
        smokeReportedMetrics = true;
    }
    ctx.strokeText("WGPU", 54, 246);
    ctx.fillText("WGPU", 54, 246);
    ctx.font = "bold 46px " + smokeFontFamily;
    ctx.fillStyle = "#8fe9ff";
    ctx.strokeText("Canvas", 58, 300);
    ctx.fillText("Canvas", 58, 300);

    ctx.fillStyle = "#0a162c";
    ctx.fillRect(36, 360, 440, 116);
    ctx.fillStyle = "#bcdaff";
    ctx.font = "bold 34px " + smokeFontFamily;
    ctx.strokeText("WebGPU + CanvasWgpu", 52, 426);
    ctx.fillText("WebGPU + CanvasWgpu", 52, 426);
    ctx.font = "30px " + smokeFontFamily;
    ctx.fillStyle = "#ffeeba";
    ctx.strokeText("frame " + String(frameSeed), 52, 470);
    ctx.fillText("frame " + String(frameSeed), 52, 470);

    if (typeof ctx.flush === "function") {
        ctx.flush();
    }

    return smokeCanvas;
}

function pushCanvasTexturePayload(sourceCanvas) {
    if (!sourceCanvas) {
        return false;
    }

    if (!smokeQueueCopyQueue || !smokeQueueCopyTexture) {
        if (smokeUploadAttemptCount % 20 === 0) {
            reportStatus("webgpu-smoke:queue-copy-not-ready");
        }
        return false;
    }

    var ok = false;
    try {
        smokeQueueCopyQueue.copyExternalImageToTexture(
            { source: sourceCanvas },
            { texture: smokeQueueCopyTexture },
            [sourceCanvas.width, sourceCanvas.height, 1]
        );
        ok = true;
    } catch (error) {
        reportStatus("webgpu-smoke:queue-copy-failed:" + String(error));
        return false;
    }

    if (smokeUploadModeReported !== "queue-copyExternalImageToTexture") {
        smokeUploadModeReported = "queue-copyExternalImageToTexture";
        reportStatus("webgpu-smoke:canvas-upload-mode:queue-copyExternalImageToTexture");
    }
    if (smokeUploadStateReported !== ok) {
        reportStatus("webgpu-smoke:canvas-texture-uploaded:" + (ok ? "1" : "0"));
        smokeUploadStateReported = ok;
    }
    return ok;
}

function uploadCanvasTexture(frameSeed) {
    smokeUploadAttemptCount += 1;
    var sourceCanvas = drawCanvasTextureSource(frameSeed);
    if (!sourceCanvas) {
        smokeUploadSucceeded = false;
        return false;
    }
    var hadSuccessfulUpload = smokeUploadSucceeded;
    var ok = pushCanvasTexturePayload(sourceCanvas);
    if (ok) {
        smokeUploadSucceeded = true;
    } else if (!hadSuccessfulUpload) {
        smokeUploadSucceeded = false;
    }
    if (ok && smokeNeedsFontRefreshUpload) {
        smokeNeedsFontRefreshUpload = false;
    }
    if (ok) {
        resolveSmokeReadySignal("canvas-texture-uploaded");
    }
    if (!ok && smokeUploadAttemptCount % 10 === 0) {
        reportStatus("webgpu-smoke:canvas-texture-retry:" + String(smokeUploadAttemptCount));
    }
    return ok;
}

async function createScene(engineArg) {
    reportStatus("webgpu-smoke:createScene");
    BABYLON.Tools.Log("[WebGPUSmoke] createScene");

    if (!engineArg || typeof engineArg.getClassName !== "function") {
        throw new Error("WebGPU smoke requires a valid Babylon engine instance.");
    }

    var engineClassName = String(engineArg.getClassName());
    reportStatus("webgpu-smoke:engine-class:" + engineClassName);
    var constructorName = "";
    if (engineArg.constructor && typeof engineArg.constructor.name === "string") {
        constructorName = engineArg.constructor.name;
    }
    reportStatus("webgpu-smoke:engine-ctor:" + constructorName);

    // Babylon builds can report "AbstractEngine" via getClassName even when
    // running WebGPUEngine internals. Require at least one WebGPU signal.
    var hasWebGpuSignal =
        engineClassName === "WebGPUEngine" ||
        constructorName.indexOf("WebGPU") !== -1 ||
        engineArg.isWebGPU === true ||
        (typeof navigator !== "undefined" &&
            navigator.gpu &&
            typeof navigator.gpu.requestAdapter === "function");
    if (!hasWebGpuSignal) {
        throw new Error("WebGPU smoke requires WebGPU engine signals; class=" + engineClassName + ", ctor=" + constructorName);
    }

    // Signal draw intent immediately when scene creation begins so native
    // presentation does not stay in a gray wait state while async scene
    // readiness work continues.
    markWebGpuDrawRequested("createScene");

    var fontLoadPromise = ensureCanvasFontLoaded().then(function (fontReady) {
        reportStatus("webgpu-smoke:font-ready:" + (fontReady ? "1" : "0"));
        return fontReady;
    }).catch(function (error) {
        reportStatus("webgpu-smoke:font-ready-error:" + String(error));
        return false;
    });

    var fontReadyAtStartup = await Promise.race([
        fontLoadPromise,
        new Promise(function (resolve) { setTimeout(function () { resolve(false); }, 120); })
    ]);
    reportStatus("webgpu-smoke:font-ready-startup:" + (fontReadyAtStartup ? "1" : "0"));
    fontLoadPromise.then(function (fontReady) {
        if (fontReady && !fontReadyAtStartup) {
            smokeNeedsFontRefreshUpload = true;
            reportStatus("webgpu-smoke:font-ready-late:1");
        }
    });

    var scene = new BABYLON.Scene(engineArg);
    scene.clearColor = new BABYLON.Color4(0.08, 0.2, 0.32, 1.0);
    ensureStandardQueueCopyPath();

    var camera = new BABYLON.FreeCamera("camera", new BABYLON.Vector3(0, 0, -5), scene);
    camera.setTarget(BABYLON.Vector3.Zero());
    scene.activeCamera = camera;

    var light = new BABYLON.HemisphericLight("light", new BABYLON.Vector3(0, 1, 0), scene);
    light.intensity = 1.0;

    var box = BABYLON.MeshBuilder.CreateBox("box", { size: 1.8 }, scene);
    // Keep the imported-texture face visible for screenshots while still spinning.
    box.rotation.x = 0.0;
    box.rotation.y = 0.0;

    var standard = new BABYLON.StandardMaterial("wgpu-smoke-standard", scene);
    standard.diffuseColor = new BABYLON.Color3(0.88, 0.58, 0.26);
    standard.specularColor = new BABYLON.Color3(0.4, 0.4, 0.4);
    standard.emissiveColor = new BABYLON.Color3(0.04, 0.04, 0.06);
    box.material = standard;

    var smokeFrameCounter = 0;
    scene.onBeforeRenderObservable.add(function () {
        smokeFrameCounter += 1;
        if (!smokeOnBeforeRenderLogged) {
            smokeOnBeforeRenderLogged = true;
            reportStatus("webgpu-smoke:onBeforeRender:first");
            markWebGpuDrawRequested("onBeforeRender");
        }
        box.rotation.y += 0.02;
        box.rotation.x = Math.sin(Date.now() * 0.0006) * 0.12;

        // Keep refresh traffic minimal to avoid steady-state upload churn.
        // We only refresh when startup upload has not succeeded yet or when
        // late font registration requires a one-time texture rebuild.
        var shouldRefreshUpload = false;
        if (smokeNeedsFontRefreshUpload) {
            shouldRefreshUpload = true;
        } else if (!smokeUploadSucceeded) {
            shouldRefreshUpload = smokeFrameCounter % 15 === 0;
        }

        if (shouldRefreshUpload) {
            uploadCanvasTexture(smokeFrameCounter % 10000);
        }

        if (globalThis.__nativePlaygroundVerboseStats === true && smokeFrameCounter % 300 === 0) {
            var stats = getNativeWebGpuStatsSnapshot();
            if (stats) {
                reportStatus(
                    "webgpu-smoke:frame-stats:" +
                    String(smokeFrameCounter) +
                    ":pipeline=" + String(stats.renderPipelineCreateCount || 0) +
                    ":submit=" + String(stats.queueSubmitCount || 0) +
                    ":draw=" + String(stats.drawCallCount || 0) +
                    ":texture=" + String(stats.textureCreateCount || 0) +
                    ":view=" + String(stats.textureViewCreateCount || 0) +
                    ":bindGroup=" + String(stats.bindGroupCreateCount || 0) +
                    ":buffer=" + String(stats.bufferCreateCount || 0) +
                    ":canvasSkip=" + String(stats.canvasTextureImportSkipCount || stats.debugTextureImportSkipCount || 0)
                );
            }
        }
    });

    scene.whenReadyAsync().then(function () {
        reportStatus("webgpu-smoke:scene-ready");
        markWebGpuDrawRequested("scene-ready");
        resolveSmokeReadySignal("scene-ready");

        if (!uploadCanvasTexture(Date.now() % 10000)) {
            reportStatus("webgpu-smoke:canvas-texture-upload-failed");
        }

        if (navigator.gpu && typeof navigator.gpu._dispatchCompute === "function") {
            navigator.gpu._dispatchCompute(
                "@compute @workgroup_size(1) fn main(@builtin(global_invocation_id) _gid : vec3<u32>) {}",
                "main",
                16,
                1,
                1
            );
            reportStatus("webgpu-smoke:compute-dispatched");
        }

        // Assert BabylonJS WebGPU path usage based on JS-visible WebGPU API counters.
        setTimeout(function () {
            try {
                var stats = getNativeWebGpuStatsSnapshot();
                if (!stats) {
                    reportStatus("webgpu-smoke:babylon-webgpu-path:0:missing-backend-stats");
                    return;
                }
                var hasPipeline = Number(stats.renderPipelineCreateCount || 0) > 0;
                var hasSubmit = Number(stats.queueSubmitCount || 0) > 0;
                var hasDraw = Number(stats.drawCallCount || 0) > 0 || stats.drawPathActive === true;
                var ok = hasPipeline && hasSubmit && hasDraw;
                reportStatus(
                    "webgpu-smoke:babylon-webgpu-path:" + (ok ? "1" : "0") +
                    ":pipeline=" + String(stats.renderPipelineCreateCount || 0) +
                    ":submit=" + String(stats.queueSubmitCount || 0) +
                    ":draw=" + String(stats.drawCallCount || 0) +
                    ":drawPath=" + String(!!stats.drawPathActive) +
                    ":backendMode=" + String(stats.backendMode || "unknown")
                );
            } catch (error) {
                reportStatus("webgpu-smoke:babylon-webgpu-path:0:error:" + String(error));
            }
        }, 1000);

        setTimeout(function () {
            try {
                var stats = getNativeWebGpuStatsSnapshot();
                if (!stats) {
                    return;
                }
                reportStatus(
                    "webgpu-smoke:runtime-counters:" +
                    "frames=" + String(stats.nativeRenderFrameCount || 0) +
                    ":submit=" + String(stats.queueSubmitCount || 0) +
                    ":draw=" + String(stats.drawCallCount || 0) +
                    ":canvasSkip=" + String(stats.canvasTextureImportSkipCount || stats.debugTextureImportSkipCount || 0) +
                    ":canvasHash=" + String(stats.canvasTextureHash || stats.debugTextureHash || 0) +
                    ":canvasW=" + String(stats.canvasTextureWidth || stats.debugTextureWidth || 0) +
                    ":canvasH=" + String(stats.canvasTextureHeight || stats.debugTextureHeight || 0) +
                    ":gpuBytes=" + String(stats.estimatedGpuMemoryBytes || 0)
                );
            } catch (error) {
                reportStatus("webgpu-smoke:runtime-counters-error:" + String(error));
            }
        }, 10000);
    }).catch(function (error) {
        reportStatus("webgpu-smoke:whenReadyAsync-failed:" + String(error));
        resolveSmokeReadySignal("scene-ready-failed");
        BABYLON.Tools.Error("[WebGPUSmoke] whenReadyAsync failed: " + error);
    });

    // Keep one face reliably visible with the imported canvas texture so visual
    // screenshots can verify end-to-end texture pointer interop across platforms.
    var overlayPlane = BABYLON.MeshBuilder.CreatePlane("canvasOverlay", { size: 1.35 }, scene);
    overlayPlane.position = new BABYLON.Vector3(0, 0, 1.01);
    overlayPlane.parent = box;
    overlayPlane.rotation = new BABYLON.Vector3(0, 0, 0);
    var overlayMaterial = new BABYLON.StandardMaterial("canvasOverlayMaterial", scene);
    overlayMaterial.diffuseColor = new BABYLON.Color3(1, 1, 1);
    overlayMaterial.emissiveColor = new BABYLON.Color3(0.14, 0.14, 0.14);
    overlayMaterial.specularColor = new BABYLON.Color3(0, 0, 0);
    overlayPlane.material = overlayMaterial;

    return scene;
}

if (typeof sceneFactorySignalResolve === "function") {
    sceneFactorySignalResolve(createScene);
}
reportStatus("webgpu-smoke:createScene-ready:" + String(sceneFactorySignalVersion));
