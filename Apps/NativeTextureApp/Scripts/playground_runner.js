if (typeof createScene === "function") {
    var engine = new BABYLON.NativeEngine({adaptToDeviceRatio: true});
    var scene = createScene();
    engine.runRenderLoop(function () {
        scene.render();
    });
}