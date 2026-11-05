import { ensureInitialized, reset } from './BabylonModule';
import { NativeEngine } from '@babylonjs/core';

// This JSI-based global object is owned by Babylon React Native.
// This will likely be converted to a TurboModule when they are fully supported.
declare const BabylonNative: {
    readonly initializationPromise: Promise<void>;
    reset: () => void;
    resetInitializationPromise: () => void;
};

export class ReactNativeEngine extends NativeEngine {
    public _isDisposed = false;

    public static async tryCreateAsync(abortSignal: AbortSignal): Promise<ReactNativeEngine | null> {
        if (!await ensureInitialized() || abortSignal.aborted) {
            return null;
        }

        // This waits Graphics/NativeEngine to be created.
        await BabylonNative.initializationPromise;

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

            BabylonNative.resetInitializationPromise();
            reset();

            this._isDisposed = true;
        }
    }
}