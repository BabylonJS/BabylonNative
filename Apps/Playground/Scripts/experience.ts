/// <reference path="./testutils.d.ts" />
/// <reference path="../../node_modules/babylonjs/babylon.module.d.ts" />

import { 
    NativeEngine, ArcRotateCamera, Scene, SceneLoader, Tools, MeshBuilder, Vector3,
    Mesh, VertexData, StandardMaterial, Color3, Color4, Texture, RenderTargetTexture,
    WebXRFeatureName, PointsCloudSystem, VideoTexture, IWebXRHitResult, IWebXRVertexData,
    WebXRMeshDetector, WebXRImageTracking, IWebXRTrackedImage, WebXRHitTest
} from "babylonjs";

const wireframe = false;
const turntable = false;
const logfps = true;
const ibl = false;
const rtt = false;
const vr = false;
const ar = false;
const xrHitTest = false;
const xrFeaturePoints = false;
const meshDetection = false;
const text = false;
const hololens = false;
const cameraTexture = false;
const imageCapture = false;
const imageTracking = false;
const readPixels = false;

function CreateBoxAsync(scene: Scene) {
    MeshBuilder.CreateBox("box1", {size: 0.2}, scene);
    return Promise.resolve();
}

function CreateSpheresAsync(scene: Scene) {
    const size = 12;
    for (let i = 0; i < size; i++) {
        for (let j = 0; j < size; j++) {
            for (let k = 0; k < size; k++) {
                const sphere = MeshBuilder.CreateSphere("sphere" + i + j + k, {segments:32, diameter: 0.9}, scene);
                sphere.position.x = i;
                sphere.position.y = j;
                sphere.position.z = k;
            }
        }
    }

    return Promise.resolve();
}

const engine = new NativeEngine();
const scene = new Scene(engine);

CreateBoxAsync(scene).then(function () {
//CreateSpheresAsync(scene).then(function () {
//SceneLoader.AppendAsync("https://raw.githubusercontent.com/KhronosGroup/glTF-Sample-Models/master/2.0/Box/glTF/Box.gltf").then(function () {
//SceneLoader.AppendAsync("https://raw.githubusercontent.com/KhronosGroup/glTF-Sample-Models/master/2.0/BoxTextured/glTF/BoxTextured.gltf").then(function () {
//SceneLoader.AppendAsync("https://raw.githubusercontent.com/KhronosGroup/glTF-Sample-Models/master/2.0/Suzanne/glTF/Suzanne.gltf").then(function () {
//SceneLoader.AppendAsync("https://raw.githubusercontent.com/KhronosGroup/glTF-Sample-Models/master/2.0/Avocado/glTF/Avocado.gltf").then(function () {
//SceneLoader.AppendAsync("https://raw.githubusercontent.com/KhronosGroup/glTF-Sample-Models/master/2.0/BoomBox/glTF/BoomBox.gltf").then(function () {
//SceneLoader.AppendAsync("https://raw.githubusercontent.com/KhronosGroup/glTF-Sample-Models/master/2.0/Sponza/glTF/Sponza.gltf").then(function () {
//SceneLoader.AppendAsync("https://raw.githubusercontent.com/KhronosGroup/glTF-Sample-Models/master/2.0/BrainStem/glTF/BrainStem.gltf").then(function () {
//SceneLoader.AppendAsync("https://raw.githubusercontent.com/KhronosGroup/glTF-Sample-Models/master/2.0/FlightHelmet/glTF/FlightHelmet.gltf").then(function () {
//SceneLoader.AppendAsync("https://raw.githubusercontent.com/KhronosGroup/glTF-Sample-Models/master/2.0/EnvironmentTest/glTF/EnvironmentTest.gltf").then(function () {
//SceneLoader.AppendAsync("https://raw.githubusercontent.com/KhronosGroup/glTF-Sample-Models/master/2.0/BoxAnimated/glTF/BoxAnimated.gltf").then(function () {
//SceneLoader.AppendAsync("https://raw.githubusercontent.com/KhronosGroup/glTF-Sample-Models/master/2.0/AnimatedMorphCube/glTF/AnimatedMorphCube.gltf").then(function () {
//SceneLoader.AppendAsync("https://raw.githubusercontent.com/KhronosGroup/glTF-Sample-Models/master/2.0/RiggedSimple/glTF/RiggedSimple.gltf").then(function () {
//SceneLoader.AppendAsync("https://raw.githubusercontent.com/stevk/glTF-Asset-Generator/skins/Output/Animation_Skin/Animation_Skin_01.gltf").then(function () {
//SceneLoader.AppendAsync("https://raw.githubusercontent.com/KhronosGroup/glTF-Sample-Models/master/2.0/RiggedFigure/glTF/RiggedFigure.gltf").then(function () {
//SceneLoader.AppendAsync("https://raw.githubusercontent.com/KhronosGroup/glTF-Sample-Models/master/2.0/CesiumMan/glTF/CesiumMan.gltf").then(function () {
//SceneLoader.AppendAsync("https://raw.githubusercontent.com/KhronosGroup/glTF-Sample-Models/master/2.0/ClearCoatTest/glTF/ClearCoatTest.gltf").then(function () {
    Tools.Log("Loaded");

    // This creates and positions a free camera (non-mesh)
    scene.createDefaultCamera(true, true, true);
    const camera = scene.activeCamera as ArcRotateCamera;
    camera.alpha += Math.PI;

    if (ibl) {
        scene.createDefaultEnvironment({ createGround: false, createSkybox: false });
    }
    else {
        scene.createDefaultLight(true);
    }

    if (cameraTexture) {
        camera.position.set(0, 1, -10);
        camera.setTarget(new Vector3(0, 1, 0));

        scene.meshes[0].setEnabled(false);
        const plane = MeshBuilder.CreatePlane("plane", {size: 1, sideOrientation: Mesh.DOUBLESIDE});
        // Mirror the plane vertically since invertY is not supported for textures in Babylon Native.
        plane.rotation.y = Math.PI;
        plane.rotation.z = Math.PI;

        plane.position.y = 1;

        const mat = new StandardMaterial("mat", scene);
        mat.diffuseColor = Color3.Black();

        const constraints = { maxWidth: 1280, maxHeight: 720, facingMode: 'environment'};
         navigator.mediaDevices.getUserMedia({ video: constraints }).then((stream) => {
            VideoTexture.CreateFromStreamAsync(scene, stream, constraints).then((videoTexture) => {
                const videoSize = videoTexture.getSize();
                mat.emissiveTexture = videoTexture;
                plane.material = mat;
                plane.scaling.x = 5;
                plane.scaling.y = 5 * (videoSize.height / videoSize.width);
                console.log("Video texture size (NEW): " + videoSize);
            });

            if (imageCapture) {
                new Promise(resolve => setTimeout(resolve, 1000)).then(() => {
                    const imageCapture = new ImageCapture(stream.getVideoTracks()[0]);
                    console.log(`Capabilities: ${JSON.stringify(imageCapture.getPhotoCapabilities(), null, 2)}`);
                    console.log(`Settings: ${JSON.stringify(imageCapture.getPhotoSettings(), null, 2)}`);
                    imageCapture.takePhoto({fillLightMode: "flash"}).then((blob) => {
                        console.log(`takePhoto finished with a blob of size ${blob.size} and type '${blob.type}'`);
                        blob.arrayBuffer().then((buffer) => {
                            const imageData = new Uint8Array(buffer);
                            console.log(`Retrieved photo ArrayBuffer of size ${imageData.byteLength}`);
                            console.log(`JPEG header bytes should be 0xff, 0xd8, 0xff.`);
                            console.log(`Header bytes are 0x${imageData[0].toString(16)}, 0x${imageData[1].toString(16)}, 0x${imageData[2].toString(16)}`);

                            // Un-mirror the plane since the photo is not mirrored (this matches WebAPI behavior).
                            plane.rotation.y = 0;
                            plane.rotation.z = 0;

                            const imageTexture = new Texture("data:fromblob", scene, true, undefined, undefined, undefined, undefined, imageData);
                            mat.emissiveTexture = imageTexture;
                        });
                    });
                });
            }
        });
    }

    if (readPixels) {
        const texture = new Texture("https://assets.babylonjs.com/textures/earth.jpg", scene);
        texture.onLoadObservable.addOnce(() => {
            if (!texture) {
                return;
            }
            const mip = 1;
            const textureWidth = texture.getSize().width >> mip;
            const textureHeight = texture.getSize().height >> mip;
            const x = textureWidth / 4;
            const y = textureHeight / 4;
            const width = textureWidth / 2;
            const height = textureHeight / 2;
            // This read will create a new buffer.
            texture.readPixels(undefined, mip, undefined, undefined, undefined, x, y, width, height)?.then((buffer) => {
                console.log(`Read ${buffer.byteLength} pixel bytes.`);
                return buffer;
            })
            .then(buffer => {
                // This read reuses the existing buffer.
                texture.readPixels(undefined, mip, buffer, undefined, undefined, x, y, width, height)?.then((buffer) => {
                    console.log(`Read ${buffer.byteLength} pixel bytes.`);
                });
            });
        });
    }

    if (wireframe) {
        const material = new StandardMaterial("wireframe", scene);
        material.wireframe = true;

        for (let index = 0; index < scene.meshes.length; index++) {
            scene.meshes[index].material = material;
        }
    }

    if (rtt) {
        const rttTexture = new RenderTargetTexture("rtt", 1024, scene);
        if (rttTexture.renderList) {
            scene.meshes.forEach(mesh => {
                rttTexture.renderList!.push(mesh);
            });
        }
        rttTexture.activeCamera = camera;
        rttTexture.vScale = -1;

        scene.customRenderTargets.push(rttTexture);

        const rttMaterial = new StandardMaterial("rttMaterial", scene);
        rttMaterial.diffuseTexture = rttTexture;

        const plane = MeshBuilder.CreatePlane("rttPlane", { width: 4, height: 4 }, scene);
        plane.position.y = 1;
        plane.position.z = -5;
        plane.rotation.y = Math.PI;
        plane.material = rttMaterial;
    }

    if (turntable) {
        scene.beforeRender = function () {
            scene.meshes[0].rotate(Vector3.Up(), 0.005 * scene.getAnimationRatio());
        };
    }

    if (logfps) {
        engine.captureGPUFrameTime(true);

        const logFpsLoop = function () {
            Tools.Log(`FPS: ${Math.round(engine.getFps())} | GPU Frame Time: ${(engine.getGPUFrameTimeCounter().lastSecAverage / 1000000).toFixed(2)}ms`);
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
                        WebXRFeatureName.HIT_TEST,
                        "latest",
                        { offsetRay: { origin: { x: 0, y: 0, z: 0 }, direction: { x: 0, y: 0, z: -1 } }, entityTypes: ["mesh"] }) as WebXRHitTest;

                    // When we receive hit test results, if there were any valid hits move the position of the mesh to the hit test point.
                    xrHitTestModule.onHitTestResultObservable.add((results: IWebXRHitResult[]) => {
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
                        scene.meshes[0].rotate(Vector3.Up(), 3.14159);
                    }, 5000);
                }

                // Showing visualization for ARKit LiDAR mesh data
                if (meshDetection) {
                    const mat = new StandardMaterial("mat", scene);
                    mat.wireframe = true;
                    mat.diffuseColor = Color3.Blue();
                    const xrMeshes = xr.baseExperience.featuresManager.enableFeature(
                        WebXRFeatureName.MESH_DETECTION,
                        "latest",
                        {convertCoordinateSystems: true}) as WebXRMeshDetector;
                    console.log("Enabled mesh detection.");
                    const meshMap = new Map();

                    // adding meshes
                    xrMeshes.onMeshAddedObservable.add((mesh: IWebXRVertexData) => {
                        try {
                            console.log("Mesh added.");
                            // create new mesh object
                            const customMesh = new Mesh("custom", scene);
                            const vertexData = new VertexData();
                            vertexData.positions = mesh.positions !== undefined ? mesh.positions : null;
                            vertexData.indices = mesh.indices !== undefined ? mesh.indices : null;
                            vertexData.normals = mesh.normals !== undefined ? mesh.normals : null;
                            vertexData.applyToMesh(customMesh, true);
                            customMesh.material = mat;
                            // add mesh and mesh id to map
                            meshMap.set(mesh.id, customMesh);
                        } catch (ex) {
                            console.error(ex);
                        }
                    });

                    // updating meshes
                    xrMeshes.onMeshUpdatedObservable.add((mesh: IWebXRVertexData) => {
                        try {
                            console.log("Mesh updated.");
                            if (meshMap.has(mesh.id)) {
                                const vertexData = new VertexData();
                                vertexData.positions = mesh.positions !== undefined ? mesh.positions : null;
                                vertexData.indices = mesh.indices !== undefined ? mesh.indices : null;
                                vertexData.normals = mesh.normals !== undefined ? mesh.normals : null;
                                vertexData.applyToMesh(meshMap.get(mesh.id), true);
                            }
                        } catch (ex) {
                            console.error(ex);
                        }
                    });

                    // removing meshes
                    xrMeshes.onMeshRemovedObservable.add((mesh: IWebXRVertexData)  => {
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
                        WebXRFeatureName.FEATURE_POINTS,
                        "latest",
                        {}) as any;

                    // Next We create the point cloud system which we will use to display feature points.
                    const pcs = new PointsCloudSystem("pcs", 5, scene);
                    interface FeaturePointParticle {
                        position: Vector3;
                        color?: Color4;
                        idx?: number;
                    }

                    type FeaturePointInitFunc = (particle: FeaturePointParticle, i: number, s: number) => void;

                    const featurePointInitFunc: FeaturePointInitFunc = function (particle, i, s) {
                        particle.position = new Vector3(0, -5, 0);
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
                        const featurePointCloud = xrFeaturePointsModule.featurePointCloud;

                        // Find the index of this particle in the particle system. If there exists a
                        // mapping to a feature point then display its position, otherwise hide it somewhere far away.
                        const index = particle.idx;
                        if (index >= featurePointCloud.length) {
                            // Hide the particle not currently in use.
                            particle.position = new Vector3(-100, -100, -100);
                        }
                        else {
                            // To display a feature point set its position to the position of the feature point
                            // and set its color on the scale from white->red where white = least confident and
                            // red = most confident.
                            particle.position = featurePointCloud[index].position;
                            particle.color = new Color4(1, 1 - featurePointCloud[index].confidenceValue, 1 - featurePointCloud[index].confidenceValue, 1);
                        }

                        return particle;
                    }

                    // Listen for changes in feature points both being added and updated, and only update
                    // our display every 60 changes to the feature point cloud to avoid slowdowns.
                    let featurePointChangeCounter = 0;
                    xrFeaturePointsModule.onFeaturePointsAddedObservable.add((addedPointIds: number[]): void => {
                        if (++featurePointChangeCounter % 60 == 0) {
                            pcs.setParticles();
                        }
                    });

                    xrFeaturePointsModule.onFeaturePointsUpdatedObservable.add((addedPointIds: number[]) => {
                        if (++featurePointChangeCounter % 60 == 0) {
                            pcs.setParticles();
                        }
                    });
                }

                let sessionMode: XRSessionMode = vr ? "immersive-vr" : "immersive-ar";
                if (hololens) {
                    // Because HoloLens 2 is a head mounted display, its Babylon.js immersive experience more closely aligns to vr
                    sessionMode = "immersive-vr";

                    // Below is an example for enabling hand tracking. The code is not unique to HoloLens 2, and may be reused for other WebXR hand tracking enabled devices.
                    xr.baseExperience.featuresManager.enableFeature(
                        WebXRFeatureName.HAND_TRACKING,
                        "latest",
                        { xrInput: xr.input });
                }

                // Test image tracking and detection.
                // To test image tracking locally either bring up the images below on your machine by loading the URL or by printing them out.
                // Then gain tracking on them during the AR Session by orienting your camera towards the image, tracking will be represented by a colored cube at the center of the image.
                if (imageTracking) {
                    const webXRTrackingMeshes: Mesh[] = [];
                    const webXRImageTracking = xr.baseExperience.featuresManager.enableFeature(
                        WebXRFeatureName.IMAGE_TRACKING,
                        "latest",
                        {
                            images: [
                                { src: "https://raw.githubusercontent.com/KhronosGroup/glTF-Sample-Models/master/2.0/IridescentDishWithOlives/screenshot/screenshot_Large.jpg", estimatedRealWorldWidth: .2 },
                                { src: "https://raw.githubusercontent.com/KhronosGroup/glTF-Sample-Models/master/2.0/DragonAttenuation/screenshot/screenshot_large.png", estimatedRealWorldWidth: .2 },   
                        ]}) as WebXRImageTracking;


                    webXRImageTracking.onTrackedImageUpdatedObservable.add((imageObject: IWebXRTrackedImage) => {
                        if (webXRTrackingMeshes[imageObject.id] === undefined) {
                            webXRTrackingMeshes[imageObject.id] = Mesh.CreateBox("box1", 0.05, scene);
                            const mat: StandardMaterial = new StandardMaterial("mat", scene);
                            mat.diffuseColor = Color3.Random();
                            webXRTrackingMeshes[imageObject.id].material = mat;
                        }
                        webXRTrackingMeshes[imageObject.id].setEnabled(!imageObject.emulated);
                        imageObject.transformationMatrix.decomposeToTransformNode(webXRTrackingMeshes[imageObject.id]);
                    });
                }

                xr.baseExperience.enterXRAsync(sessionMode, "unbounded", xr.renderTarget).then((xrSessionManager) => {
                    if (hololens) {
                        // Pass through, head mounted displays (HoloLens 2) require autoClear and a black clear color
                        xrSessionManager.scene.autoClear = true;
                        xrSessionManager.scene.clearColor = new Color4(0, 0, 0, 0);
                    }
                });
            });
        }, 5000);
    }

    if (text) {
        // @ts-ignore
        const Writer = MeshWriter(scene, { scale: 1.0, defaultFont: "Arial" });
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
