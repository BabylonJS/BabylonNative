globalThis.__babylonUseWebGPU = true;

var kSmokeFontName = "wgpu_smoke_font";
var kSmokeFontUrl = "https://raw.githubusercontent.com/CedricGuillemet/dump/master/droidsans.ttf";
var smokeFontLoaded = false;
var smokeFontLoadPromise = null;
var smokeCanvas = null;
var smokeCtx = null;

function reportStatus(message) {
    try {
        if (typeof globalThis.__nativePlaygroundStatus === "function") {
            globalThis.__nativePlaygroundStatus(String(message));
        }
    } catch (error) {
        void error;
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
    if (smokeCanvas && smokeCtx) {
        return smokeCtx;
    }

    if (typeof _native === "undefined" || !_native.Canvas) {
        return null;
    }

    smokeCanvas = new _native.Canvas();
    smokeCanvas.width = 512;
    smokeCanvas.height = 512;
    smokeCtx = smokeCanvas.getContext("2d");
    return smokeCtx;
}

function buildCanvasTexturePayload(frameSeed) {
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

    var gradient = ctx.createLinearGradient(0, 0, 512, 512);
    gradient.addColorStop(0.0, "#162c4e");
    gradient.addColorStop(0.4, "#3a95ff");
    gradient.addColorStop(1.0, "#b25dff");
    ctx.fillStyle = gradient;
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
    ctx.font = "bold 132px \"" + kSmokeFontName + "\"";
    var titleMetrics = ctx.measureText("WGPU");
    reportStatus("webgpu-smoke:metrics:title=" + String(titleMetrics && titleMetrics.width ? titleMetrics.width : 0));
    ctx.strokeText("WGPU", 54, 246);
    ctx.fillText("WGPU", 54, 246);
    ctx.font = "bold 46px \"" + kSmokeFontName + "\"";
    ctx.fillStyle = "#8fe9ff";
    ctx.strokeText("Canvas", 58, 300);
    ctx.fillText("Canvas", 58, 300);

    ctx.fillStyle = "#0a162c";
    ctx.fillRect(36, 360, 440, 116);
    ctx.fillStyle = "#bcdaff";
    ctx.font = "bold 34px \"" + kSmokeFontName + "\"";
    ctx.strokeText("WebGPU + CanvasWgpu", 52, 426);
    ctx.fillText("WebGPU + CanvasWgpu", 52, 426);
    ctx.font = "30px \"" + kSmokeFontName + "\"";
    ctx.fillStyle = "#ffeeba";
    ctx.strokeText("frame " + String(frameSeed), 52, 470);
    ctx.fillText("frame " + String(frameSeed), 52, 470);

    if (typeof ctx.flush === "function") {
        ctx.flush();
    }

    return smokeCanvas.getCanvasTexture();
}

function pushCanvasTexturePayload(payload) {
    if (!payload) {
        return false;
    }

    if (!navigator.gpu || typeof navigator.gpu._setDebugTextureFromNative !== "function") {
        reportStatus("webgpu-smoke:debug-texture-bridge-missing");
        return false;
    }

    var ok = !!navigator.gpu._setDebugTextureFromNative(payload);
    reportStatus("webgpu-smoke:canvas-texture-uploaded:" + (ok ? "1" : "0"));
    return ok;
}

async function createScene() {
    reportStatus("webgpu-smoke:createScene");
    BABYLON.Tools.Log("[WebGPUSmoke] createScene");

    var fontReady = await ensureCanvasFontLoaded();
    reportStatus("webgpu-smoke:font-ready:" + (fontReady ? "1" : "0"));

    var scene = new BABYLON.Scene(engine);
    scene.clearColor = new BABYLON.Color4(0.08, 0.2, 0.32, 1.0);

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

    standard.forceCompilationAsync(box).then(function () {
        reportStatus("webgpu-smoke:force-compilation-ok");
    }).catch(function (error) {
        reportStatus("webgpu-smoke:force-compilation-error:" + String(error));
    });

    var readyProbeCount = 0;
    var readyProbeId = setInterval(function () {
        try {
            readyProbeCount += 1;
            var meshReady = scene.meshes.length > 0 ? scene.meshes[0].isReady(true) : true;
            var cameraReady = scene.activeCamera ? scene.activeCamera.isReady(true) : true;
            var sceneReady = scene.isReady(false);

            var materialReady = true;
            var effectReady = "none";
            var effectError = "";
            var definesDirty = "na";
            var hasSubMeshEffect = false;
            if (scene.meshes.length > 0 && scene.meshes[0].material && scene.meshes[0].subMeshes && scene.meshes[0].subMeshes.length > 0) {
                var mesh = scene.meshes[0];
                var subMesh = mesh.subMeshes[0];
                materialReady = mesh.material.isReadyForSubMesh(mesh, subMesh, false);
                if (subMesh.materialDefines) {
                    definesDirty = subMesh.materialDefines.isDirty ? "true" : "false";
                }
                hasSubMeshEffect = !!subMesh.effect;

                var activeEffect = null;
                if (subMesh._drawWrappers && subMesh._drawWrappers.length > 0) {
                    for (var i = 0; i < subMesh._drawWrappers.length; ++i) {
                        if (subMesh._drawWrappers[i] && subMesh._drawWrappers[i].effect) {
                            activeEffect = subMesh._drawWrappers[i].effect;
                            break;
                        }
                    }
                }

                if (!activeEffect && mesh.material && typeof mesh.material.getEffect === "function") {
                    activeEffect = mesh.material.getEffect();
                }

                if (activeEffect) {
                    effectReady = activeEffect.isReady() ? "ready" : "not-ready";
                    if (activeEffect._compilationError) {
                        effectError = String(activeEffect._compilationError).replace(/\s+/g, " ").slice(0, 180);
                    }
                }
            }

            reportStatus(
                "webgpu-smoke:ready-probe:" +
                readyProbeCount +
                ":scene=" + sceneReady +
                ":mesh=" + meshReady +
                ":camera=" + cameraReady +
                ":material=" + materialReady +
                ":effect=" + effectReady +
                ":hasEffect=" + hasSubMeshEffect +
                ":definesDirty=" + definesDirty +
                ":err=" + effectError
            );
            if (sceneReady || readyProbeCount >= 20) {
                clearInterval(readyProbeId);
            }
        } catch (error) {
            reportStatus("webgpu-smoke:ready-probe-error:" + String(error));
            clearInterval(readyProbeId);
        }
    }, 200);

    scene.onBeforeRenderObservable.add(function () {
        box.rotation.y += 0.02;
        box.rotation.x = Math.sin(Date.now() * 0.0006) * 0.12;
    });

    scene.whenReadyAsync().then(function () {
        reportStatus("webgpu-smoke:scene-ready");

        var payload = buildCanvasTexturePayload(Date.now() % 10000);
        if (!pushCanvasTexturePayload(payload)) {
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
    }).catch(function (error) {
        reportStatus("webgpu-smoke:whenReadyAsync-failed:" + String(error));
        BABYLON.Tools.Error("[WebGPUSmoke] whenReadyAsync failed: " + error);
    });

    setTimeout(function () {
        var payload = buildCanvasTexturePayload(Date.now() % 10000);
        pushCanvasTexturePayload(payload);
    }, 2000);

    var statsProbeCount = 0;
    var statsProbe = setInterval(function () {
        statsProbeCount += 1;
        if (navigator.gpu && typeof navigator.gpu._debugStats === "function") {
            reportStatus("webgpu-smoke:stats:" + JSON.stringify(navigator.gpu._debugStats()));
        }
        if (navigator.gpu && typeof navigator.gpu._isDrawPathActive === "function") {
            reportStatus("webgpu-smoke:draw-path-active:" + String(!!navigator.gpu._isDrawPathActive()));
        }
        if (statsProbeCount >= 20) {
            clearInterval(statsProbe);
        }
    }, 500);

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
