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
            },
            createTextNode: function (text) {
                return { nodeValue: text };
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

    let clickAudioContext = null;
    let clickAudioBuffer = null;

    async function playClickBeep() {
        if (typeof AudioContext === "undefined") {
            BABYLON.Tools.Warn("AudioContext is not available");
            return;
        }

        if (!clickAudioContext) {
            clickAudioContext = new AudioContext();
        }

        if (clickAudioContext.state === "suspended") {
            await clickAudioContext.resume();
        }

        if (!clickAudioBuffer) {
            const wavBytes = createBeepWavArrayBuffer(44100, 0.25, 880, 0.4);
            clickAudioBuffer = await clickAudioContext.decodeAudioData(wavBytes.slice(0));
        }

        const source = clickAudioContext.createBufferSource();
        source.buffer = clickAudioBuffer;
        source.connect(clickAudioContext.destination);
        source.start(clickAudioContext.currentTime);
        BABYLON.Tools.Log("Click beep");
    }

    function createScene() {
        const engine = new BABYLON.NativeEngine({ adaptToDeviceRatio: true });
        const scene = new BABYLON.Scene(engine);
        scene.createDefaultCamera(true, true, true);

        const beepBuffer = createBeepWavArrayBuffer(44100, 0.25, 880, 0.4);
        let spatialSource = null;
        let spatialStarted = false;

        scene.onPointerObservable.add(function (pointerInfo) {
            if (pointerInfo.type !== BABYLON.PointerEventTypes.POINTERDOWN) {
                return;
            }

            playClickBeep().catch(function (error) {
                BABYLON.Tools.Warn("Click beep failed: " + error);
            });

            if (!spatialStarted && typeof BABYLON.Sound === "function") {
                spatialStarted = true;
                const legacySound = new BABYLON.Sound("beep", beepBuffer, scene, function () {
                    BABYLON.Tools.Log("Spatial beep started after click");
                }, {
                    autoplay: true,
                    loop: true,
                    spatialSound: true,
                    maxDistance: 20
                });
                spatialSource = legacySound;
            } else if (spatialSource && typeof spatialSource.play === "function") {
                spatialSource.play();
            }
        });

        let angle = 0;
        scene.onBeforeRenderObservable.add(function () {
            if (!spatialSource) {
                return;
            }

            angle += 0.02;
            const x = Math.cos(angle) * 3;
            const z = Math.sin(angle) * 3;

            if (typeof spatialSource.setPosition === "function") {
                spatialSource.setPosition(new BABYLON.Vector3(x, 0, z));
            } else if (spatialSource._soundSource) {
                spatialSource._soundSource.position = new BABYLON.Vector3(x, 0, z);
            }
        });

        BABYLON.Tools.Log("Click anywhere to play a beep (and start spatial audio)");

        return scene;
    }

    globalThis.createScene = createScene;
})();
