if (typeof createScene === "function") {
    const engine = new BABYLON.NativeEngine({adaptToDeviceRatio: true});
    const scene = createScene();
    if (scene.then) {
        scene.then(function (scene) {
            engine.runRenderLoop(function () {
                scene.render();
            });
        })
    } else {
        engine.runRenderLoop(function () {
            scene.render();
        });
    }
}
