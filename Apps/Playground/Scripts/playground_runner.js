function runPlaygroundScene() {
    if (typeof createScene === "function") {
        var engine = globalThis.engine = new BABYLON.NativeEngine({ adaptToDeviceRatio: true });
        var scene = globalThis.scene = createScene();
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
}

if (typeof _playgroundWebGPUReady !== "undefined") {
    _playgroundWebGPUReady.then(runPlaygroundScene).catch(function (error) {
        setTimeout(function () { throw error; }, 0);
    });
} else {
    runPlaygroundScene();
}
