import { useEffect, useState } from 'react';
import { Platform } from 'react-native';
import { PERMISSIONS, check, request } from 'react-native-permissions';
import { Engine, WebXRSessionManager, WebXRExperienceHelper, Color4, Tools, VideoTexture } from '@babylonjs/core';
import { ReactNativeEngine } from './ReactNativeEngine';
import { ensureInitialized } from './BabylonModule';

import * as base64 from 'base-64';

// These are errors that are normally thrown by WebXR's requestSession, so we should throw the same errors under similar circumstances so app code can be written the same for browser or native.
// https://developer.mozilla.org/en-US/docs/Web/API/XRSystem/requestSession
// https://developer.mozilla.org/en-US/docs/Web/API/DOMException#Error_names
enum DOMError {
    NotSupportedError = 9,
    InvalidStateError = 11,
    SecurityError = 18,
}

class DOMException {
    public constructor(private readonly error: DOMError) { }
    get code(): number { return this.error; }
    get name(): string { return DOMError[this.error]; }
}

// Requests the camera permission and throws if the permission could not be granted
async function requestCameraPermissionAsync() : Promise<void> {
    const cameraPermission = Platform.select({
        android: PERMISSIONS.ANDROID.CAMERA,
        ios: PERMISSIONS.IOS.CAMERA,
    });

    // Only Android, iOS and Windows are supported.
    if (cameraPermission === undefined) {
        throw new DOMException(DOMError.NotSupportedError);
    }

    // If the permission has not been granted yet, but also not been blocked, then request permission.
    let permissionStatus = await check(cameraPermission);
    if (permissionStatus == "denied")
    {
        permissionStatus = await request(cameraPermission);
    }

    // If the permission has still not been granted, then throw an appropriate exception, otherwise continue with the actual XR session initialization.
    switch(permissionStatus) {
        case "unavailable":
            throw new DOMException(DOMError.NotSupportedError);
        case "denied":
        case "blocked":
            throw new DOMException(DOMError.SecurityError);
        case "granted":
            return;
    }
}

// Override the WebXRSessionManager.initializeSessionAsync to insert a camera permissions request. It would be cleaner to do this directly in the native XR implementation, but there are a couple problems with that:
// 1. React Native does not provide a way to hook into the permissions request result (at least on Android).
// 2. If it is done on the native side, then we need one implementation per platform.
{
    const originalInitializeSessionAsync = WebXRSessionManager.prototype.initializeSessionAsync;
    WebXRSessionManager.prototype.initializeSessionAsync = async function (...args: Parameters<typeof originalInitializeSessionAsync>): ReturnType<typeof originalInitializeSessionAsync> {
        if (Platform.OS === "windows")
        {
            // Launching into immersive mode on Windows HMDs doesn't require a runtime permission check.
            // The Spatial Perception capability should be enabled in the project's Package.appxmanifest.
            return originalInitializeSessionAsync.apply(this, args);
        }

        await requestCameraPermissionAsync();

        return originalInitializeSessionAsync.apply(this, args);
    }
}

ensureInitialized().then(() => {
    // Override the navigator.mediaDevices.getUserMedia to insert a camera permissions request. It would be cleaner to do this directly in the NativeCamera implementation, but there are a couple problems with that:
    // 1. React Native does not provide a way to hook into the permissions request result (at least on Android).
    // 2. If it is done on the native side, then we need one implementation per platform.
    {
        const originalGetUserMedia = navigator.mediaDevices.getUserMedia;
        navigator.mediaDevices.getUserMedia = async function (...args: Parameters<typeof originalGetUserMedia>): ReturnType<typeof originalGetUserMedia> {
            await requestCameraPermissionAsync();

            return originalGetUserMedia.apply(this, args);
        }
    }
});

if (Platform.OS === "android" || Platform.OS === "ios") {
    const originalEnterXRAsync: (...args: any[]) => Promise<WebXRSessionManager> = WebXRExperienceHelper.prototype.enterXRAsync;
    WebXRExperienceHelper.prototype.enterXRAsync = async function (...args: any[]): Promise<WebXRSessionManager> {
        // TODO: https://github.com/BabylonJS/BabylonNative/issues/649
        // Android/iOS require manually clearing the default frame buffer to prevent garbage from being rendered for a few frames during the XR transition
        const sessionManager = await originalEnterXRAsync.apply(this, args);
        const scene = sessionManager.scene;
        const beforeRenderObserver = scene.onBeforeRenderObservable.add(() => {
            scene.getEngine().unBindFramebuffer(undefined!);
            scene.getEngine().clear(scene.clearColor, true, false);
        });
        sessionManager.onXRSessionEnded.add(() => {
            scene.onBeforeRenderObservable.remove(beforeRenderObserver);
        });
        return sessionManager;
    };
} else if (Platform.OS === "windows") {
    const originalEnterXRAsync: (...args: any[]) => Promise<WebXRSessionManager> = WebXRExperienceHelper.prototype.enterXRAsync;
    WebXRExperienceHelper.prototype.enterXRAsync = async function (...args: any[]): Promise<WebXRSessionManager> {
        // TODO: https://github.com/BabylonJS/BabylonNative/issues/577
        // Windows HMDs require different rendering behaviors than default xr rendering for mobile devices
        const sessionManager = await originalEnterXRAsync.apply(this, args);
        sessionManager.scene.clearColor = new Color4(0, 0, 0, 0);
        sessionManager.scene.autoClear = true;
        return sessionManager;
    };
}

// Babylon Native includes a native atob polyfill, but it relies JSI to deal with the strings, and JSI has a bug where it assumes strings are null terminated, and a base 64 string can contain one of these.
// So for now, provide a JavaScript based atob polyfill.
declare const global: any;
global.atob = base64.decode;

// Polyfill console.time and console.timeEnd if needed (as of React Native 0.64 these are not implemented).
if (!console.time) {
    const consoleTimes = new Map<string, number>();

    console.time = (label = "default"): void => {
        consoleTimes.set(label, performance.now());
    };

    console.timeEnd = (label = "default"): void => {
        const end = performance.now();
        const start = consoleTimes.get(label);
        if (!!start) {
            consoleTimes.delete(label);
            console.log(`${label}: ${end - start} ms`);
        }
    }
}

// Hook Tools performance counter functions to forward to NativeTracing.
// Ideally this should be hooked more directly in Babylon.js so it works with Babylon Native as well, but we need to determine a pattern for augmenting Babylon.js with Babylon Native specific JS logic.
declare var _native: {
    enablePerformanceLogging(): void,
    disablePerformanceLogging(): void,
    startPerformanceCounter(counter: string): unknown,
    endPerformanceCounter(counter: unknown): void,
};

{
    const setPerformanceLogLevel: ((level: number) => void) | undefined = Object.getOwnPropertyDescriptor(Tools, "PerformanceLogLevel")?.set;
    if (!setPerformanceLogLevel) {
        console.warn(`NativeTracing was not hooked into Babylon.js performance logging because the Tools.PerformanceLogLevel property does not exist.`);
    } else {
        // Keep a map of trace region opaque pointers since Tools.EndPerformanceCounter just takes a counter name as an argument.
        const traceRegions = new Map<string, unknown>();
        let currentLevel = Tools.PerformanceNoneLogLevel;
        Object.defineProperty(Tools, "PerformanceLogLevel", {
            set: (level: number) => {
                // No-op if the log level isn't changing, otherwise we can end up with multiple wrapper layers repeating the same work.
                if (level !== currentLevel) {
                    currentLevel = level;

                    // Invoke the original PerformanceLevel setter.
                    setPerformanceLogLevel(currentLevel);

                    if (currentLevel === Tools.PerformanceNoneLogLevel) {
                        _native.disablePerformanceLogging();
                    } else {
                        _native.enablePerformanceLogging();

                        // When Tools.PerformanceLogLevel is set, it assigns the Tools.StartPerformanceCounter and Tools.EndPerformanceCounter functions, so we need to assign
                        // these functions again in order to wrap them.

                        const originalStartPerformanceCounter = Tools.StartPerformanceCounter;
                        Tools.StartPerformanceCounter = (counterName: string, condition = true) => {
                            // Call into native before so the time it takes is not captured in the JS perf counter interval.
                            if (condition) {
                                if (traceRegions.has(counterName)) {
                                    console.warn(`Performance counter '${counterName}' already exists.`);
                                } else {
                                    traceRegions.set(counterName, _native.startPerformanceCounter(counterName));
                                }
                            }

                            originalStartPerformanceCounter(counterName, condition);
                        };

                        const originalEndPerformanceCounter = Tools.EndPerformanceCounter;
                        Tools.EndPerformanceCounter = (counterName: string, condition = true) => {
                            originalEndPerformanceCounter(counterName, condition);

                            // Call into native after so the time it takes is not captured in the JS perf counter interval.
                            if (condition) {
                                const traceRegion = traceRegions.get(counterName);
                                if (traceRegion) {
                                    _native.endPerformanceCounter(traceRegion);
                                    traceRegions.delete(counterName);
                                } else {
                                    console.warn(`Performance counter '${counterName}' does not exist.`);
                                }
                            }
                        }
                    }
                }
            },
        });
    }
}

export function useEngine(): Engine | undefined {
    const [engine, setEngine] = useState<Engine>();

    useEffect(() => {
        const abortController = new AbortController();
        let engine: ReactNativeEngine | undefined = undefined;

        (async () => {
            setEngine(engine = await ReactNativeEngine.tryCreateAsync(abortController.signal) ?? undefined);
        })();

        return () => {
            abortController.abort();
            // NOTE: Do not use setEngine with a callback to dispose the engine instance as that callback does not get called during component unmount when compiled in release.
            engine?.dispose();
            setEngine(undefined);
        };
    }, []);

    return engine;
}
