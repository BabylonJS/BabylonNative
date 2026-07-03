/// <reference path="../../node_modules/babylonjs/babylon.module.d.ts" />

(function () {
    if (typeof document === "undefined") {
        globalThis.document = {
            addEventListener: function () { },
            removeEventListener: function () { },
            createElement: function (tag) {
                if (tag === "audio") {
                    return new Audio();
                }
                return {};
            }
        };
    }

    function createBeepWavArrayBuffer(sampleRate, durationSeconds, frequency, amplitude) {
        const frameCount = Math.max(1, Math.floor(sampleRate * durationSeconds));
        const bytesPerSample = 2;
        const channelCount = 1;
        const dataSize = frameCount * channelCount * bytesPerSample;
        const buffer = new ArrayBuffer(44 + dataSize);
        const view = new DataView(buffer);

        const writeString = function (offset, value) {
            for (let i = 0; i < value.length; i++) {
                view.setUint8(offset + i, value.charCodeAt(i));
            }
        };

        writeString(0, "RIFF");
        view.setUint32(4, 36 + dataSize, true);
        writeString(8, "WAVE");
        writeString(12, "fmt ");
        view.setUint32(16, 16, true);
        view.setUint16(20, 1, true);
        view.setUint16(22, channelCount, true);
        view.setUint32(24, sampleRate, true);
        view.setUint32(28, sampleRate * channelCount * bytesPerSample, true);
        view.setUint16(32, channelCount * bytesPerSample, true);
        view.setUint16(34, 8 * bytesPerSample, true);
        writeString(36, "data");
        view.setUint32(40, dataSize, true);

        for (let frame = 0; frame < frameCount; frame++) {
            const sample = Math.sin((2 * Math.PI * frequency * frame) / sampleRate) * amplitude;
            const intSample = Math.max(-32768, Math.min(32767, Math.round(sample * 32767)));
            view.setInt16(44 + frame * bytesPerSample, intSample, true);
        }

        return buffer;
    }

    function createScene() {
        const engine = new BABYLON.NativeEngine({ adaptToDeviceRatio: true });
        const scene = new BABYLON.Scene(engine);
        scene.createDefaultCamera(true, true, true);

        const resumeAudioIfNeeded = function () {
            if (typeof AudioContext !== "undefined") {
                const probe = new AudioContext();
                if (probe.state === "suspended") {
                    probe.resume();
                }
            }
        };
        scene.onPointerObservable.add(resumeAudioIfNeeded);

        const beepBuffer = createBeepWavArrayBuffer(44100, 0.25, 880, 0.4);
        let spatialSource = null;

        if (typeof BABYLON.Sound === "function") {
            const legacySound = new BABYLON.Sound("beep", beepBuffer, scene, null, {
                autoplay: true,
                loop: true,
                spatialSound: true,
                maxDistance: 20
            });
            spatialSource = legacySound;
            BABYLON.Tools.Log("Playing legacy BABYLON.Sound beep");
        }

        if (typeof BABYLON.CreateAudioEngineAsync === "function" && typeof BABYLON.CreateSoundAsync === "function") {
            BABYLON.CreateAudioEngineAsync().then(function (audioEngine) {
                return BABYLON.CreateSoundAsync("beepV2", beepBuffer, { audioEngine: audioEngine });
            }).then(function (sound) {
                sound.play();
                spatialSource = sound;
                BABYLON.Tools.Log("Playing CreateSoundAsync beep");
            }).catch(function (error) {
                BABYLON.Tools.Warn("CreateSoundAsync path failed: " + error);
            });
        }

        let angle = 0;
        scene.onBeforeRenderObservable.add(function () {
            angle += 0.02;
            const x = Math.cos(angle) * 3;
            const z = Math.sin(angle) * 3;

            if (spatialSource) {
                if (typeof spatialSource.setPosition === "function") {
                    spatialSource.setPosition(new BABYLON.Vector3(x, 0, z));
                } else if (spatialSource._soundSource) {
                    spatialSource._soundSource.position = new BABYLON.Vector3(x, 0, z);
                }
            }
        });

        return scene;
    }

    globalThis.createScene = createScene;
})();
