var wireframe = false;
var turntable = false;
var logfps = true;
var ibl = false;
var rtt = false;
var vr = false;
var ar = false;
var xrHitTest = false;
var xrFeaturePoints = false;
var text = false;
var hololens = false;

function CreateBoxAsync() {
    BABYLON.Mesh.CreateBox("box1", 0.2).setEnabled(false);
    return Promise.resolve();
}

function CreateSpheresAsync() {
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

function CreateInputHandling(scene) {
    var inputManager = new InputManager();
    var priorX = inputManager.pointerX;
    var priorY = inputManager.pointerY;
    var x = 0;
    var y = 0;
    scene.onBeforeRenderObservable.add(function () {
        x = inputManager.pointerX;
        y = inputManager.pointerY;

        if (inputManager.isPointerDown) {
            scene.activeCamera.alpha += 0.01 * (priorX - x);
            scene.activeCamera.beta += 0.01 * (priorY - y);
        }

        priorX = x;
        priorY = y;
    });
}

document = {
    createElement: function (type) {
        if (type === "canvas") {
            return new OffscreenCanvas();
        }
        else if (type === "span") {
            return { style: {}, appendChild: function () { }, getBoundingClientRect: function () { return { top: 0 }; } };
        }
        else if (type === "div") {
            return {
                style: {}, appendChild: function () { }, getBoundingClientRect: function () { return { top: 0 }; }
            };
        }
        return {};
    },
    removeEventListener: function () { },
    body: {
        appendChild: function () { }, removeChild: function () { }
    }
}

_native.whenGraphicsReady().then(function () {

    var engine = new BABYLON.NativeEngine();
    var scene = new BABYLON.Scene(engine);

    engine.updateDynamicTexture = function (texture, canvas, invertY, premulAlpha, format) {
        if (premulAlpha === void 0) { premulAlpha = false; }
        var webGLTexture = texture._hardwareTexture.underlyingResource;
        this._native.copyTexture(webGLTexture, canvas.getCanvasTexture());
        texture.isReady = true;
    };

    CreateBoxAsync().then(function () {
        //CreateSpheresAsync().then(function () {
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
        /*
        scene.createDefaultCamera(true);
        scene.activeCamera.alpha += Math.PI;
        CreateInputHandling(scene);
        */
        var camera = new BABYLON.FreeCamera("camera1", new BABYLON.Vector3(0, 2, -4), scene);
        camera.setTarget(BABYLON.Vector3.Zero());
        //camera.attachControl(canvas, true);
        CreateInputHandling(scene);
        
        BABYLON.Tools.LoadFile("https://raw.githubusercontent.com/CedricGuillemet/dump/master/droidsans.ttf", (data) => {
            OffscreenCanvas.loadTTF("droidsans", data);
            /*
            var ground = BABYLON.MeshBuilder.CreateGround("ground1", { width: 1, height: 1, subdivisions: 2 }, scene);
            ground.rotation.x = Math.PI * 0.5;
            ground.position.z = 2;

            //Create dynamic texture
            var textureResolution = 512;
            var textureGround = new BABYLON.DynamicTexture("dynamic texture", textureResolution, scene);
            var textureContext = textureGround.getContext();

            var materialGround = new BABYLON.StandardMaterial("Mat", scene);
            materialGround.diffuseTexture = textureGround;
            ground.material = materialGround;
            materialGround.backFaceCulling = false;

            var font = "bold 44px monospace";
            textureGround.clear();
            textureGround.drawText("BabylonNative", 0, 50, font, "White", null, true, true);
            */

            var manager = new BABYLON.GUI.GUI3DManager(scene);

            const addButtonPanel = function (manager) {

                var anchor = new BABYLON.TransformNode("");


                var panel = new BABYLON.GUI.SpherePanel();
                panel.margin = 0.2;

                manager.addControl(panel);
                panel.linkToTransformNode(anchor);
                panel.position.z = -1.5;

                // Let's add some buttons!
                var addButton = function () {
                    var button = new BABYLON.GUI.HolographicButton("orientation");
                    panel.addControl(button);

                    button.text = "Button #" + panel.children.length;
                }

                panel.blockLayout = true;
                for (var index = 0; index < 60; index++) {
                    addButton();
                }
                panel.blockLayout = false;
            }

            addButtonPanel(manager);
            
            const addPushButton = (scene, manager) => {
                var pushButtonCore;

                // The first parameter can be used to specify which mesh to import. Here we import all meshes
                BABYLON.SceneLoader.ImportMesh("", "https://david.blob.core.windows.net/babylonjs/MRTK/", "pushButton.glb", scene, function (newMeshes) {
                    var mesh = newMeshes[0];
                    mesh.position.z = 1;
                    mesh.rotationQuaternion = BABYLON.Quaternion.RotationAlphaBetaGamma(1, 0, 0);
                    mesh.scaling.scaleInPlace(1.0);
                    pushButtonCore = newMeshes[0];
                    const newPushButton = pushButtonCore.clone("pushButton");
                    makePushButton(newPushButton, new Color3(0.25, 0, 0));
                    pushButtonCore.setEnabled(false);
                });

                function makePushButton(mesh, hoverColor) {
                    var cylinder = mesh.getChildMeshes(false, (node) => { return node.name.indexOf("Cylinder") !== -1 })[0];
                    cylinder.material.albedoColor = new Color3(0.5, 0.19, 0);
                    var pushButton = new BABYLON.GUI.TouchMeshButton3D(mesh, "pushButton");
                    pushButton.pointerEnterAnimation = () => {
                        cylinder.material.albedoColor = hoverColor;
                    };
                    pushButton.pointerOutAnimation = () => {
                        cylinder.material.albedoColor = new BABYLON.Color3(0.5, 0.19, 0);
                    };
                    pushButton.pointerDownAnimation = () => {
                        cylinder.position.y = 0;
                    }
                    pushButton.pointerUpAnimation = () => {
                        cylinder.position.y = 0.21;
                    }
                    pushButton.onPointerDownObservable.add(() => {
                        console.log(pushButton.name + " pushed.");
                    });
                    manager.addControl(pushButton);
                }
            }

            addPushButton(scene, manager);
            
        }, undefined, undefined, true);


        if (ibl) {
            scene.createDefaultEnvironment({ createGround: false, createSkybox: false });
        }
        else {
            scene.createDefaultLight(true);
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
                            xrSessionManager.scene.clearColor = BABYLON.Color4.FromColor3(Color3.Black());
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
});
