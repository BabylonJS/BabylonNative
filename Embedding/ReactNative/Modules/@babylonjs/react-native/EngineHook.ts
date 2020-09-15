import { useEffect, useState } from 'react';
import { Platform } from 'react-native';
import { PERMISSIONS, check, request } from 'react-native-permissions';
import { Engine, NativeEngine, WebXRSessionManager } from '@babylonjs/core';
import { BabylonModule } from './BabylonModule';
import { DisposeEngine } from './EngineHelpers';
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

// Override the WebXRSessionManager.initializeSessionAsync to insert a camera permissions request. It would be cleaner to do this directly in the native XR implementation, but there are a couple problems with that:
// 1. React Native does not provide a way to hook into the permissions request result (at least on Android).
// 2. If it is done on the native side, then we need one implementation per platform.
{
    const originalInitializeSessionAsync: (...args: any[]) => Promise<any> = WebXRSessionManager.prototype.initializeSessionAsync;
    WebXRSessionManager.prototype.initializeSessionAsync = async function (...args: any[]): Promise<any> {
        const cameraPermission = Platform.select({
            android: PERMISSIONS.ANDROID.CAMERA,
            ios: PERMISSIONS.IOS.CAMERA,
        });

        // Only Android and iOS are supported.
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
                return originalInitializeSessionAsync.apply(this, args);
        }
    }
}

// Babylon Native includes a native atob polyfill, but it relies JSI to deal with the strings, and JSI has a bug where it assumes strings are null terminated, and a base 64 string can contain one of these.
// So for now, provide a JavaScript based atob polyfill.
declare const global: any;
global.atob = base64.decode;

export function useEngine(): Engine | undefined {
    const [engine, setEngine] = useState<Engine>();

    useEffect(() => {
        let disposed = false;

        (async () => {
            if (await BabylonModule.initialize() && !disposed)
            {
                setEngine(new NativeEngine());
            }
        })();

        // NOTE: This is a workaround for https://github.com/BabylonJS/BabylonReactNative/issues/60
        function heartbeat() {
            if (!disposed) {
                setTimeout(heartbeat, 10);
            }
        }
        heartbeat();

        return () => {
            disposed = true;
            setEngine(engine => {
                if (engine) {
                    DisposeEngine(engine);
                }
                return undefined;
            });
        };
    }, []);

    return engine;
}