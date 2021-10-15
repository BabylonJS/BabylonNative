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

function CreateBoxAsync() {
    BABYLON.Mesh.CreateBox("box1", 0.2);
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

var engine = new BABYLON.NativeEngine();
var scene = new BABYLON.Scene(engine);
/*
var HTMLVideoElement = {};
document = {
    createElement(type) {
        if (type == "video") {
            return null;
        }
        return undefined;
    }
}
*/

BABYLON.VideoTexture.prototype._getVideo = function (src) {
    var video = new NativeVideoStream();
    video.src = src;
    console.log("get video", src);
    return video;
}


engine.updateVideoTexture = function (texture, video, invertY) {
    if (texture && texture._hardwareTexture) {
        var webGLTexture = texture._hardwareTexture.underlyingResource;
        video.updateVideoTexture(webGLTexture, invertY);
    }
}

var toTranslate = [];
var idealx = [];
var matInfo;
var matLogo;
var animate = function (frameIndex, camera) {
    if (frameIndex == 0) {
        for (let i = 0; i < toTranslate.length; i++) {
            idealx.push(toTranslate[i].position.x);
            toTranslate[i].position.x += (i & 6) * 50 + 100;
        }
    } else {
        if (frameIndex < 400) {
            for (let i = 0; i < toTranslate.length; i++) {
                toTranslate[i].position.x -= 1;
                toTranslate[i].position.x = Math.max(idealx[i], toTranslate[i].position.x);
            }
        } else {
            if (frameIndex > 500 && frameIndex < 700) {
                camera.alpha = camera.alpha + (4.7163802410155204 - camera.alpha) * 0.03;
                camera.beta = camera.beta + (1.5717776373304784 - camera.beta) * 0.03;

                camera.orthoLeft += (-12. - camera.orthoLeft) * 0.03;
                camera.orthoRight += (0. - camera.orthoRight) * 0.03;
                camera.orthoTop += (3.0 - camera.orthoTop) * 0.03;
                camera.orthoBottom += (-4 - camera.orthoBottom) * 0.03;
            } else if (frameIndex > 1000 && frameIndex < 1200) {
                matInfo.alpha = Math.min(matInfo.alpha + 0.01, 1.);
            } else if (frameIndex > 1200 && frameIndex < 1400) {
                matLogo.alpha = Math.min(matLogo.alpha + 0.01, 1.);
            }
        }
    }
}
var createTextPlane = function (text, scene) {
    var planeOpts = {
        height: 7.2,
        width: 12.8,
        sideOrientation: BABYLON.Mesh.DOUBLESIDE
    };
    console.log("O0");
    var plane = BABYLON.MeshBuilder.CreatePlane("plane", planeOpts, scene);
    console.log("O1");
    //Create dynamic texture
    var textureResolution = 512;
    var textureGround = new BABYLON.DynamicTexture("dynamic texture", { width: 512, height: 256 }, scene);
    var textureContext = textureGround.getContext();
    textureGround.hasAlpha = true;
    console.log("O2");
    var materialGround = new BABYLON.StandardMaterial("Mat", scene);
    materialGround.diffuseTexture = textureGround;
    plane.material = materialGround;
    console.log("O3");
    //Add text to dynamic texture
    var font = "bold 44px monospace";
    textureGround.drawText(text, 20, 135, font, "brown", "transparent", true, true);
    //plane.visibility = 0.99;
    materialGround.alpha = 1;
    matInfo = materialGround;
    console.log("O4");
    toTranslate.push(plane);
    return plane;
}

var createTextPlane2 = function (scene) {
    var planeOpts = {
        height: 7.2,
        width: 12.8,
        sideOrientation: BABYLON.Mesh.DOUBLESIDE
    };
    var plane = BABYLON.MeshBuilder.CreatePlane("plane", planeOpts, scene);

    //Create dynamic texture
    var textureResolution = 512;
    var textureGround = new BABYLON.DynamicTexture("dynamic texture", { width: 512, height: 256 }, scene);
    var textureContext = textureGround.getContext();
    textureGround.hasAlpha = true;
    var materialGround = new BABYLON.StandardMaterial("Mat", scene);
    materialGround.diffuseTexture = textureGround;
    plane.material = materialGround;

    //Add text to dynamic texture
    var font = "bold 44px monospace";
    textureGround.drawText("Rendered", 20, 50, font, "white", "black", true, true);
    textureGround.drawText("Composited", 20, 100, font, "white", "transparent", true, true);
    textureGround.drawText("Encoded", 20, 150, font, "white", "transparent", true, true);
    textureGround.drawText("With BabylonNative", 20, 200, font, "white", "transparent", true, true);
    materialGround.alpha = 0.;
    matInfo = materialGround;
    return plane;
}

var createTextPlane3 = function (scene) {
    var planeOpts = {
        height: 7.2,
        width: 12.8,
        sideOrientation: BABYLON.Mesh.DOUBLESIDE
    };
    var plane = BABYLON.MeshBuilder.CreatePlane("plane", planeOpts, scene);

    //Create dynamic texture
    var textureGround = new BABYLON.Texture("https://doc.babylonjs.com/img/defaultImage.png", scene);
    textureGround.hasAlpha = false;
    var materialGround = new BABYLON.StandardMaterial("Mat", scene);
    materialGround.diffuseTexture = textureGround;
    materialGround.alpha = 0.;
    plane.material = materialGround;
    matLogo = materialGround;
    return plane;
}



    // This creates and positions a free camera (non-mesh)
    //var camera = new BABYLON.ArcRotateCamera("camera1", 0.9, -0.9, 50, new BABYLON.Vector3(0, 5, -10), scene);
    //var camera = new BABYLON.ArcRotateCamera("camera1", -8, 0.0, 50, new BABYLON.Vector3(0, 5, -10), scene);
    var camera = new BABYLON.ArcRotateCamera("camera1", 4.077175598143289, 0.8614449090346448, 50, new BABYLON.Vector3(6, 5, -5), scene);
    camera.mode = BABYLON.Camera.ORTHOGRAPHIC_CAMERA;
    camera.orthoLeft = -20;
    camera.orthoRight = 20;
    camera.orthoTop = 20;
    camera.orthoBottom = -20;

var boom;

var font = "bold 44px monospace";
BABYLON.Tools.LoadFile("https://raw.githubusercontent.com/CedricGuillemet/dump/master/droidsans.ttf", (data) => {
    _native.NativeCanvas.loadTTFAsync("droidsans", data);

//BABYLON.SceneLoader.LoadAssetContainer("https://playground.babylonjs.com/scenes/BoomBox/", "BoomBox.gltf", scene, function (container) {
BABYLON.SceneLoader.LoadAssetContainer("https://raw.githubusercontent.com/KhronosGroup/glTF-Sample-Models/master/2.0/FlightHelmet/glTF/", "FlightHelmet.gltf", scene, function (container) {
        //BABYLON.VideoTexture.CreateFromWebCam(scene, function (camTexture) {
    console.log("AAA");
            var meshes = container.meshes;
            boom = meshes[0];
            boom.position.z = -10;
            boom.position.x = 4;
            boom.position.y = 1.1;
            //boom.scaling = new BABYLON.Vector3(150, 150, 150);
    boom.scaling = new BABYLON.Vector3(5,5,5);
            

    console.log("AA");
            // This creates a light, aiming 0,1,0 - to the sky (non-mesh)
            var light = new BABYLON.HemisphericLight("light", new BABYLON.Vector3(0, 1, 0), scene);

            // Default intensity is 1. Let's dim the light a small amount
    light.intensity = 1.7;
    console.log("A");

            // Our built-in 'ground' shape.
            var ground = BABYLON.MeshBuilder.CreateGround("ground", { width: 1000, height: 1000 }, scene);
            var gridMat = new BABYLON.GridMaterial("groundMaterial", scene);
            gridMat.mainColor = new BABYLON.Color3(0.8, 0.8, 0.8);
            gridMat.lineColor = new BABYLON.Color3(0.3, 0.3, 0.3);
            ground.material = gridMat;
            
            var webcamPlane = BABYLON.MeshBuilder.CreatePlane("webcamPlane", { width: 12.8, height: 7.2 }, scene);
            webcamPlane.position.z = 10;
            webcamPlane.position.y = 4.6;
            var webcamMat = new BABYLON.StandardMaterial("mat", scene);
            var videoTexture;

    console.log("B");
            //videoTexture = camTexture;
    videoTexture = new BABYLON.Texture("https://playground.babylonjs.com/textures/earth.jpg", scene);
            webcamMat.diffuseTexture = videoTexture;
            webcamMat.diffuseTexture.vScale = -1;
            webcamPlane.material = webcamMat;


            toTranslate.push(webcamPlane);
            createTextPlane("Camera", scene).position = new BABYLON.Vector3(13, 4.6, 10);

    console.log("C");

            BABYLON.Effect.ShadersStore["customVertexShader"] = "\r\n" +
                "precision highp float;\r\n" +

                "// Attributes\r\n" +
                "attribute vec3 position;\r\n" +
                "attribute vec2 uv;\r\n" +

                "// Uniforms\r\n" +
                "uniform mat4 worldViewProjection;\r\n" +

                "// Varying\r\n" +
                "varying vec2 vUV;\r\n" +

                "void main(void) {\r\n" +
                "    gl_Position = worldViewProjection * vec4(position, 1.0);\r\n" +

                "    vUV = uv;\r\n" +
                "}\r\n";

            BABYLON.Effect.ShadersStore["customFragmentShader"] = `
	   precision highp float;

    	varying vec2 vUV;

    	uniform sampler2D textureSampler;

    	void main(void) {
            vec2 vUV2 = vec2(vUV.x, 1.-vUV.y);
            vec4 col = texture2D(textureSampler, vUV2);
            vec2 pixels = vec2(80.,45.);
            vec2 puv = vUV2;
            puv.x -= mod(vUV2.x, 1.0 / pixels.x);
            puv.y -= mod(vUV2.y, 1.0 / pixels.y);
            vec4 colPixel = texture2D(textureSampler, puv);
            col = mix( vec4(1.,1.,1.,1.) - col, col, step(vUV.x-vUV.y*0.1,0.7));
            col = mix(col, colPixel, step(vUV.x-vUV.y*0.1+0.0,0.2));
    	    gl_FragColor = col;
            gl_FragColor.a = 1.;
    	}`;

            var shaderMaterial = new BABYLON.ShaderMaterial("shader", scene, {
                vertex: "custom",
                fragment: "custom",
            },
                {
                    attributes: ["position", "normal", "uv"],
                    uniforms: ["world", "worldView", "worldViewProjection", "view", "projection"]
                });
            //var tmptex = new BABYLON.Texture("textures/earth.jpg", scene);
    console.log("D");
            // shaderMaterial.setTexture("textureSampler", tmptex);
            shaderMaterial.setTexture("textureSampler", videoTexture);

            var webcamPlane2 = BABYLON.MeshBuilder.CreatePlane("webcamPlane2", { width: 12.8, height: 7.2 }, scene);
            webcamPlane2.position.z = 0;
            webcamPlane2.position.y = 4.6;
            webcamPlane2.material = shaderMaterial;
            //var webcamMat = shaderMaterial;
            toTranslate.push(webcamPlane2);
            createTextPlane("Effects", scene).position = new BABYLON.Vector3(13, 4.6, 0);
    console.log("E");
            // video

            var planeOpts = {
                height: 3,
                width: 4,
                sideOrientation: BABYLON.Mesh.DOUBLESIDE
            };
            var ANote0Video = BABYLON.MeshBuilder.CreatePlane("videoplane", planeOpts, scene);
            ANote0Video.position = new BABYLON.Vector3(-4.5, 6.2, -20);
            var ANote0VideoMat = new BABYLON.StandardMaterial("m", scene);
            var ANote0VideoVidTex = new BABYLON.VideoTexture("https://playground.babylonjs.com/", "textures/babylonjs.mp4", scene);
            ANote0VideoMat.diffuseTexture = ANote0VideoVidTex;
            ANote0VideoMat.roughness = 1;
            ANote0VideoMat.emissiveColor = new BABYLON.Color3.White();
            ANote0Video.material = ANote0VideoMat;
    console.log("F");
            toTranslate.push(boom);
            createTextPlane("3D Objects", scene).position = new BABYLON.Vector3(13, 4.6, -10);

            toTranslate.push(ANote0Video);
            createTextPlane("Video", scene).position = new BABYLON.Vector3(13, 4.6, -20);


            createTextPlane2(scene).position = new BABYLON.Vector3(0, 4.6, -30);
    createTextPlane3(scene).position = new BABYLON.Vector3(0, 4.6, -40);

    container.addAllToScene(scene);
    console.log("All done!");
            
            var frameIndex = 0;
            scene.onBeforeRenderObservable.add(() => {
                if (boom) {
                    boom.rotationQuaternion.multiplyInPlace(BABYLON.Quaternion.FromEulerAngles(0, 0.01, 0));

                    animate(frameIndex, camera);
                    frameIndex++;
                }

            });

        //}, { maxWidth: 640, maxHeight: 480 }); // webcam ready
    }); // load asset


}, undefined, undefined, true); // font

    engine.runRenderLoop(function () {
        scene.render();
    });