import { ensureInitialized } from './BabylonModule';
import { NativeEngine } from '@babylonjs/core';

// This global object is owned by Babylon Native.
declare const _native: {
    whenGraphicsReady: () => Promise<void>;
};

// This JSI-based global object is owned by Babylon React Native.
// This will likely be converted to a TurboModule when they are fully supported.
declare const BabylonNative: {
    readonly initializationPromise: Promise<void>;
    setEngineInstance: (engine: NativeEngine | null) => void;
    reset: () => void;
};

export class ReactNativeEngine extends NativeEngine {
    private _isDisposed = false;

    private constructor() {
        super();
        BabylonNative.setEngineInstance(this);
    }

    public static async tryCreateAsync(abortSignal: AbortSignal): Promise<ReactNativeEngine | null> {
        if (!await ensureInitialized() || abortSignal.aborted) {
            return null;
        }

        // This waits Graphics/NativeEngine to be created (which in turn makes the whenGraphicsReady available).
        await BabylonNative.initializationPromise;

        // Check for cancellation.
        if (abortSignal.aborted) {
            return null;
        }

        // This waits for the Graphics system to be up and running.
        await _native.whenGraphicsReady();

        // Check for cancellation.
        if (abortSignal.aborted) {
            return null;
        }

        return new ReactNativeEngine();
    }

    public get isDisposed() {
        return this._isDisposed;
    }

    public dispose(): void {
        if (!this.isDisposed) {
            super.dispose();

            // Ideally we would always do a reset here as we don't want different behavior between debug and release. Unfortunately, fast refresh has some strange behavior that
            // makes it quite difficult to get this to work correctly (e.g. it re-runs previous useEffect instances, which means it can try to use Babylon Native in a de-initialized state).
            // TODO: https://github.com/BabylonJS/BabylonReactNative/issues/125
            if (!__DEV__) {
                BabylonNative.reset();
            }

            this._isDisposed = true;
        }

        BabylonNative.setEngineInstance(null);
    }
}