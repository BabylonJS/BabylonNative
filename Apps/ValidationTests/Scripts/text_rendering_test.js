
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

console.log("Starting");

var engine = new BABYLON.NativeEngine();
var scene = new BABYLON.Scene(engine);

scene.createDefaultCamera(true);
CreateInputHandling(scene);
scene.createDefaultLight(true);


engine.updateDynamicTexture = function (texture, canvas, invertY, premulAlpha, format) {
    if (premulAlpha === void 0) { premulAlpha = false; }
    var webGLTexture = texture._hardwareTexture.underlyingResource;
    this._native.copyTexture(webGLTexture, canvas.getCanvasTexture());
    texture.isReady = true;
};

console.log("Reading font");
var textureGround;
var font = "bold 44px monospace";
BABYLON.Tools.LoadFile("https://raw.githubusercontent.com/CedricGuillemet/dump/master/droidsans.ttf", (data) => {
    _native.NativeCanvas.loadTTF("droidsans", data);
    /*
    var ground = BABYLON.MeshBuilder.CreateGround("ground1", { width: 1, height: 1, subdivisions: 2 }, scene);
    ground.rotation.x = Math.PI * 0.5;
    ground.position.z = 2;

    //Create dynamic texture
    var textureResolution = 512;
    textureGround = new BABYLON.DynamicTexture("dynamic texture", textureResolution, scene);
    //var textureContext = textureGround.getContext();

    var materialGround = new BABYLON.StandardMaterial("Mat", scene);
    materialGround.diffuseTexture = textureGround;
    ground.material = materialGround;
    materialGround.backFaceCulling = false;

    
    textureGround.clear();
    textureGround.drawText("BabylonNative", 0, 246, font, "White", null, true, true);
    */
    console.log("Font loaded");
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
            mesh.position.z = 3;
            mesh.rotationQuaternion = BABYLON.Quaternion.RotationAlphaBetaGamma(1, 0, 0);
            mesh.scaling.scaleInPlace(1.0);
            pushButtonCore = newMeshes[0];
            const newPushButton = pushButtonCore.clone("pushButton");
            makePushButton(newPushButton, new BABYLON.Color3(0.25, 0, 0));
            pushButtonCore.setEnabled(false);
        });

        function makePushButton(mesh, hoverColor) {
            var cylinder = mesh.getChildMeshes(false, (node) => { return node.name.indexOf("Cylinder") !== -1 })[0];
            cylinder.material.albedoColor = new BABYLON.Color3(0.5, 0.19, 0);
            var pushButton = new BABYLON.GUI.TouchMeshButton3D(mesh, {collisionMesh:cylinder}, "pushButton");
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

engine.runRenderLoop(function () {
    scene.render();
});