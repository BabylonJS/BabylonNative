/// <reference path="../../node_modules/babylonjs/babylon.module.d.ts" />
/// <reference path="../../node_modules/babylonjs-loaders/babylonjs.loaders.module.d.ts" />
/// <reference path="../../node_modules/babylonjs-materials/babylonjs.materials.module.d.ts" />
/// <reference path="../../node_modules/babylonjs-gui/babylon.gui.module.d.ts" />

var wireframe = false;
var turntable = false;
var logfps = true;
var ibl = false;
var rtt = false;
var vr = false;
var ar = false;
var xrHitTest = false;
var xrFeaturePoints = false;
var meshDetection = false;
var text = false;
var hololens = false;
var cameraTexture = false;

function CreateBoxAsync(scene) {
    BABYLON.Mesh.CreateBox("box1", 0.2, scene);
    return Promise.resolve();
}

function CreateSpheresAsync(scene) {
    var size = 12;
    for (var i = 0; i < size; i++) {
        for (var j = 0; j < size; j++) {
            for (var k = 0; k < size; k++) {
                var sphere = BABYLON.Mesh.CreateSphere("sphere" + i + j + k, 32, 0.9, scene);
                sphere.position.x = i;
                sphere.position.y = j;
                sphere.position.z = k;
            }
        }
    }

    return Promise.resolve();
}

var engine = new BABYLON.NativeEngine();
var scene = new BABYLON.Scene(engine);

CreateBoxAsync(scene).then(function () {
//CreateSpheresAsync(scene).then(function () {
//BABYLON.SceneLoader.AppendAsync("https://raw.githubusercontent.com/KhronosGroup/glTF-Sample-Models/master/2.0/Box/glTF/Box.gltf").then(function () {
//BABYLON.SceneLoader.AppendAsync("https://raw.githubusercontent.com/KhronosGroup/glTF-Sample-Models/master/2.0/BoxTextured/glTF/BoxTextured.gltf").then(function () {
//BABYLON.SceneLoader.AppendAsync("https://raw.githubusercontent.com/KhronosGroup/glTF-Sample-Models/master/2.0/Suzanne/glTF/Suzanne.gltf").then(function () {
//BABYLON.SceneLoader.AppendAsync("https://raw.githubusercontent.com/KhronosGroup/glTF-Sample-Models/master/2.0/Avocado/glTF/Avocado.gltf").then(function () {
//BABYLON.SceneLoader.AppendAsync("https://raw.githubusercontent.com/KhronosGroup/glTF-Sample-Models/master/2.0/BoomBox/glTF/BoomBox.gltf").then(function () {
//BABYLON.SceneLoader.AppendAsync("https://raw.githubusercontent.com/KhronosGroup/glTF-Sample-Models/master/2.0/Sponza/glTF/Sponza.gltf").then(function () {
//BABYLON.SceneLoader.AppendAsync("https://raw.githubusercontent.com/KhronosGroup/glTF-Sample-Models/master/2.0/BrainStem/glTF/BrainStem.gltf").then(function () {
//BABYLON.SceneLoader.AppendAsync("https://raw.githubusercontent.com/KhronosGroup/glTF-Sample-Models/master/2.0/FlightHelmet/glTF/FlightHelmet.gltf").then(function () {
//BABYLON.SceneLoader.AppendAsync("https://raw.githubusercontent.com/KhronosGroup/glTF-Sample-Models/master/2.0/EnvironmentTest/glTF/EnvironmentTest.gltf").then(function () {
//BABYLON.SceneLoader.AppendAsync("https://raw.githubusercontent.com/KhronosGroup/glTF-Sample-Models/master/2.0/BoxAnimated/glTF/BoxAnimated.gltf").then(function () {
//BABYLON.SceneLoader.AppendAsync("https://raw.githubusercontent.com/KhronosGroup/glTF-Sample-Models/master/2.0/AnimatedMorphCube/glTF/AnimatedMorphCube.gltf").then(function () {
//BABYLON.SceneLoader.AppendAsync("https://raw.githubusercontent.com/KhronosGroup/glTF-Sample-Models/master/2.0/RiggedSimple/glTF/RiggedSimple.gltf").then(function () {
//BABYLON.SceneLoader.AppendAsync("https://raw.githubusercontent.com/stevk/glTF-Asset-Generator/skins/Output/Animation_Skin/Animation_Skin_01.gltf").then(function () {
//BABYLON.SceneLoader.AppendAsync("https://raw.githubusercontent.com/KhronosGroup/glTF-Sample-Models/master/2.0/RiggedFigure/glTF/RiggedFigure.gltf").then(function () {
//BABYLON.SceneLoader.AppendAsync("https://raw.githubusercontent.com/KhronosGroup/glTF-Sample-Models/master/2.0/CesiumMan/glTF/CesiumMan.gltf").then(function () {
//BABYLON.SceneLoader.AppendAsync("https://raw.githubusercontent.com/KhronosGroup/glTF-Sample-Models/master/2.0/ClearCoatTest/glTF/ClearCoatTest.gltf").then(function () {
    BABYLON.Tools.Log("Loaded");

    scene.createDefaultCamera(true, true, true);
    scene.activeCamera.alpha += Math.PI;

    if (ibl) {
        scene.createDefaultEnvironment({ createGround: false, createSkybox: false });
    }
    else {
        scene.createDefaultLight(true);
    }

    if (cameraTexture) {
        var cameraBox = BABYLON.Mesh.CreateBox("box1", 0.25);
        var mat = new BABYLON.StandardMaterial("mat", scene);
        mat.diffuseColor = BABYLON.Color3.Black();

        BABYLON.VideoTexture.CreateFromWebCam(scene, function (videoTexture) {
            mat.emissiveTexture = videoTexture;
            cameraBox.material = mat;
        }, { maxWidth: 256, maxHeight: 256, facingMode: "environment" });
    }

    if (wireframe) {
        var material = new BABYLON.StandardMaterial("wireframe", scene);
        material.wireframe = true;
        material.pointsCloud = true;

        for (var index = 0; index < scene.meshes.length; index++) {
            scene.meshes[0].material = material;
        }
    }

    if (rtt) {
        var rttTexture = new BABYLON.RenderTargetTexture("rtt", 1024, scene);
        scene.meshes.forEach(mesh => {
            rttTexture.renderList.push(mesh);
        });
        rttTexture.activeCamera = scene.activeCamera;
        rttTexture.vScale = -1;

        scene.customRenderTargets.push(rttTexture);

        var rttMaterial = new BABYLON.StandardMaterial("rttMaterial", scene);
        rttMaterial.diffuseTexture = rttTexture;

        var plane = BABYLON.MeshBuilder.CreatePlane("rttPlane", { width: 4, height: 4 }, scene);
        plane.position.y = 1;
        plane.position.z = -5;
        plane.rotation.y = Math.PI;
        plane.material = rttMaterial;
    }

    if (turntable) {
        scene.beforeRender = function () {
            scene.meshes[0].rotate(BABYLON.Vector3.Up(), 0.005 * scene.getAnimationRatio());
        };
    }

    if (logfps) {
        var logFpsLoop = function () {
            BABYLON.Tools.Log("FPS: " + Math.round(engine.getFps()));
            window.setTimeout(logFpsLoop, 1000);
        };

        window.setTimeout(logFpsLoop, 3000);
    }

    engine.runRenderLoop(function () {
        scene.render();
    });

    if (vr || ar || hololens) {
        setTimeout(function () {
            scene.createDefaultXRExperienceAsync({ disableDefaultUI: true, disableTeleportation: true }).then((xr) => {
                if (xrHitTest) {
                    // Create the hit test module. OffsetRay specifies the target direction, and entityTypes can be any combination of "mesh", "plane", and "point".
                    const xrHitTestModule = xr.baseExperience.featuresManager.enableFeature(
                        BABYLON.WebXRFeatureName.HIT_TEST,
                        "latest",
                        { offsetRay: { origin: { x: 0, y: 0, z: 0 }, direction: { x: 0, y: 0, z: -1 } }, entityTypes: ["mesh"] });

                    // When we receive hit test results, if there were any valid hits move the position of the mesh to the hit test point.
                    xrHitTestModule.onHitTestResultObservable.add((results) => {
                        if (results.length) {
                            scene.meshes[0].position.x = results[0].position.x;
                            scene.meshes[0].position.y = results[0].position.y;
                            scene.meshes[0].position.z = results[0].position.z;
                        }
                    });
                }
                else {
                    setTimeout(function () {
                        scene.meshes[0].position.z = 2;
                        scene.meshes[0].rotate(BABYLON.Vector3.Up(), 3.14159);
                    }, 5000);
                }

                // Showing visualization for ARKit LiDAR mesh data
                if (meshDetection) {
                    var mat = new BABYLON.StandardMaterial("mat", scene);
                    mat.wireframe = true;
                    mat.diffuseColor = BABYLON.Color3.Blue();
                    const xrMeshes = xr.baseExperience.featuresManager.enableFeature(
                        BABYLON.WebXRFeatureName.MESH_DETECTION,
                        "latest",
                        {convertCoordinateSystems: true});
                    console.log("Enabled mesh detection.");
                    const meshMap = new Map();

                    // adding meshes
                    xrMeshes.onMeshAddedObservable.add(mesh=> {
                        try {
                            console.log("Mesh added.");
                            // create new mesh object
                            var customMesh = new BABYLON.Mesh("custom", scene);
                            var vertexData = new BABYLON.VertexData();
                            vertexData.positions = mesh.positions;
                            vertexData.indices = mesh.indices;
                            vertexData.normals = mesh.normals;
                            vertexData.applyToMesh(customMesh, true);
                            customMesh.material = mat;
                            // add mesh and mesh id to map
                            meshMap.set(mesh.id, customMesh);
                        } catch (ex) {
                            console.error(ex);
                        }
                    });

                    // updating meshes
                    xrMeshes.onMeshUpdatedObservable.add(mesh=> {
                        try {
                            console.log("Mesh updated.");
                            if (meshMap.has(mesh.id)) {
                                var vertexData = new BABYLON.VertexData();
                                vertexData.positions = mesh.positions;
                                vertexData.indices = mesh.indices;
                                vertexData.normals = mesh.normals;
                                vertexData.applyToMesh(meshMap.get(mesh.id), true);
                            }
                        } catch (ex) {
                            console.error(ex);
                        }
                    });

                    // removing meshes
                    xrMeshes.onMeshRemovedObservable.add(mesh => {
                        try {
                            console.log("Mesh removed.");
                            if (meshMap.has(mesh.id)) {
                                meshMap.get(mesh.id).dispose();
                                meshMap.delete(mesh.id);
                            }
                        } catch (ex) {
                            console.error(ex);
                        }
                    });
                }

                // Below is an example of how to process feature points.
                if (xrFeaturePoints) {
                    // First we attach the feature point system feature the XR experience.
                    const xrFeaturePointsModule = xr.baseExperience.featuresManager.enableFeature(
                        BABYLON.WebXRFeatureName.FEATURE_POINTS,
                        "latest",
                        {});

                    // Next We create the point cloud system which we will use to display feature points.
                    var pcs = new BABYLON.PointsCloudSystem("pcs", 5, scene);
                    var featurePointInitFunc = function (particle, i, s) {
                        particle.position = new BABYLON.Vector3(0, -5, 0);
                    }

                    // Add some starting points and build the mesh.
                    pcs.addPoints(3000, featurePointInitFunc);
                    pcs.buildMeshAsync().then((mesh) => {
                        mesh.alwaysSelectAsActiveMesh = true;
                    });

                    // Define the logic for how to display a particular particle in the particle system
                    // which represents a feature point.
                    pcs.updateParticle = function (particle) {
                        // Grab the feature point cloud from the xrFeaturePointsModule.
                        var featurePointCloud = xrFeaturePointsModule.featurePointCloud;

                        // Find the index of this particle in the particle system. If there exists a
                        // mapping to a feature point then display its position, otherwise hide it somewhere far away.
                        var index = particle.idx;
                        if (index >= featurePointCloud.length) {
                            // Hide the particle not currently in use.
                            particle.position = new BABYLON.Vector3(-100, -100, -100);
                        }
                        else {
                            // To display a feature point set its position to the position of the feature point
                            // and set its color on the scale from white->red where white = least confident and
                            // red = most confident.
                            particle.position = featurePointCloud[index].position;
                            particle.color = new BABYLON.Color4(1, 1 - featurePointCloud[index].confidenceValue, 1 - featurePointCloud[index].confidenceValue, 1);
                        }

                        return particle;
                    }

                    // Listen for changes in feature points both being added and updated, and only update
                    // our display every 60 changes to the feature point cloud to avoid slowdowns.
                    var featurePointChangeCounter = 0;
                    xrFeaturePointsModule.onFeaturePointsAddedObservable.add((addedPointIds) => {
                        if (++featurePointChangeCounter % 60 == 0) {
                            pcs.setParticles();
                        }
                    });

                    xrFeaturePointsModule.onFeaturePointsUpdatedObservable.add((updatedPointIds) => {
                        if (++featurePointChangeCounter % 60 == 0) {
                            pcs.setParticles();
                        }
                    });
                }

                let sessionMode = vr ? "immersive-vr" : "immersive-ar"
                if (hololens) {
                    // Because HoloLens 2 is a head mounted display, its Babylon.js immersive experience more closely aligns to vr
                    sessionMode = "immersive-vr";

                    // Below is an example for enabling hand tracking. The code is not unique to HoloLens 2, and may be reused for other WebXR hand tracking enabled devices.
                    xr.baseExperience.featuresManager.enableFeature(
                        BABYLON.WebXRFeatureName.HAND_TRACKING,
                        "latest",
                        { xrInput: xr.input });
                }

                xr.baseExperience.enterXRAsync(sessionMode, "unbounded", xr.renderTarget).then((xrSessionManager) => {
                    if (hololens) {
                        // Pass through, head mounted displays (HoloLens 2) require autoClear and a black clear color
                        xrSessionManager.scene.autoClear = true;
                        xrSessionManager.scene.clearColor = new BABYLON.Color4(0, 0, 0, 0);
                    }
                });
            });
        }, 5000);
    }

    if (text) {
        var Writer = BABYLON.MeshWriter(scene, { scale: 1.0, defaultFont: "Arial" });
        new Writer(
            "Lorem ipsum dolor sit amet...",
            {
                "anchor": "center",
                "letter-height": 5,
                "color": "#FF0000"
            }
        );
    }

}, function (ex) {
    console.log(ex.message, ex.stack);
});
