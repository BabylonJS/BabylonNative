if (typeof createScene === "function") {
    var engine = new BABYLON.NativeEngine({adaptToDeviceRatio: true});
    var scene = createScene();

    if (scene.then) {
        scene.then(scene => {
            engine.runRenderLoop(function () {
                scene.render();
            });
        });
    }
    else {
        engine.runRenderLoop(function () {
            scene.render();
        });
    }
}