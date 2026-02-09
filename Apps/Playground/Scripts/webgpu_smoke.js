globalThis.__babylonUseWebGPU = true;
function reportStatus(message) {
    try {
        if (typeof globalThis.__nativePlaygroundStatus === "function") {
            globalThis.__nativePlaygroundStatus(String(message));
        }
    } catch (error) {
        void error;
    }
}

reportStatus("webgpu-smoke:script-loaded");
BABYLON.Tools.Log("[WebGPUSmoke] script-loaded");

function createScene() {
    reportStatus("webgpu-smoke:createScene");
    BABYLON.Tools.Log("[WebGPUSmoke] createScene");
    var scene = new BABYLON.Scene(engine);
    scene.texturesEnabled = false;
    scene.clearColor = new BABYLON.Color4(0.08, 0.2, 0.32, 1.0);

    var camera = new BABYLON.FreeCamera("camera", new BABYLON.Vector3(0, 0, -5), scene);
    camera.setTarget(BABYLON.Vector3.Zero());
    scene.activeCamera = camera;

    var light = new BABYLON.HemisphericLight("light", new BABYLON.Vector3(0, 1, 0), scene);
    light.intensity = 1.0;

    var box = BABYLON.MeshBuilder.CreateBox("box", { size: 1.8 }, scene);
    box.rotation.x = 0.35;
    box.rotation.y = 0.7;

    var complexPbr = new BABYLON.PBRMaterial("complex-pbr", scene);
    complexPbr.albedoColor = new BABYLON.Color3(0.9, 0.55, 0.2);
    complexPbr.metallic = 0.82;
    complexPbr.roughness = 0.24;
    complexPbr.clearCoat.isEnabled = true;
    complexPbr.clearCoat.intensity = 0.95;
    complexPbr.clearCoat.roughness = 0.1;
    complexPbr.sheen.isEnabled = true;
    complexPbr.sheen.intensity = 0.45;
    complexPbr.sheen.color = new BABYLON.Color3(0.65, 0.9, 1.0);
    complexPbr.anisotropy.isEnabled = true;
    complexPbr.anisotropy.intensity = 0.7;
    complexPbr.anisotropy.direction = new BABYLON.Vector2(1.0, 0.35);
    complexPbr.iridescence.isEnabled = true;
    complexPbr.iridescence.intensity = 0.7;
    complexPbr.iridescence.minimumThickness = 130;
    complexPbr.iridescence.maximumThickness = 420;
    box.material = complexPbr;
    complexPbr.forceCompilationAsync(box).then(function () {
        reportStatus("webgpu-smoke:force-compilation-ok");
    }).catch(function (error) {
        reportStatus("webgpu-smoke:force-compilation-error:" + String(error));
    });

    scene.onBeforeRenderObservable.add(function () {
        box.rotation.y += 0.02;
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

            reportStatus("webgpu-smoke:ready-probe:" + readyProbeCount + ":scene=" + sceneReady + ":mesh=" + meshReady + ":camera=" + cameraReady + ":material=" + materialReady + ":effect=" + effectReady + ":hasEffect=" + hasSubMeshEffect + ":definesDirty=" + definesDirty + ":err=" + effectError);
            if (sceneReady || readyProbeCount >= 20) {
                clearInterval(readyProbeId);
            }
        } catch (error) {
            reportStatus("webgpu-smoke:ready-probe-error:" + String(error));
            clearInterval(readyProbeId);
        }
    }, 200);

    setTimeout(function () {
        reportStatus("webgpu-smoke:timer-fired");
        BABYLON.Tools.Log("[WebGPUSmoke] timer-fired");
        if (navigator.gpu && typeof navigator.gpu._debugStats === "function") {
            BABYLON.Tools.Log("[WebGPUSmoke] stats " + JSON.stringify(navigator.gpu._debugStats()));
            reportStatus("webgpu-smoke:stats:" + JSON.stringify(navigator.gpu._debugStats()));
        }

        if (navigator.gpu && typeof navigator.gpu._dispatchCompute === "function") {
            navigator.gpu._dispatchCompute(
                "@compute @workgroup_size(1) fn main(@builtin(global_invocation_id) _gid : vec3<u32>) {}",
                "main",
                16,
                1,
                1
            );
            reportStatus("webgpu-smoke:timer-compute-dispatched");
        }
    }, 2000);

    scene.whenReadyAsync().then(function () {
        reportStatus("webgpu-smoke:scene-ready");
        BABYLON.Tools.Log("[WebGPUSmoke] scene-ready");
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
        reportStatus("[WebGPUSmoke] whenReadyAsync failed: " + error);
        BABYLON.Tools.Error("[WebGPUSmoke] whenReadyAsync failed: " + error);
    });

    return scene;
}
