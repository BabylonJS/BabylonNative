import { NativeModules } from 'react-native';
import { NativeEngine } from '@babylonjs/core';

// This global object is part of Babylon Native.
declare const _native: {
    whenGraphicsReady: () => Promise<void>;
    engineInstance: NativeEngine;
}

const NativeBabylonModule: {
    initialize(): Promise<boolean>;
    whenInitialized(): Promise<boolean>;
    reset(): Promise<boolean>;
} = NativeModules.BabylonModule;

export const BabylonModule = {
    initialize: async () => {
        const initialized = await NativeBabylonModule.initialize();
        if (initialized) {
            await _native.whenGraphicsReady();
        }
        return initialized;
    },

    whenInitialized: NativeBabylonModule.whenInitialized,
    reset: NativeBabylonModule.reset,

    createEngine: () => {
        const engine = new NativeEngine();
        _native.engineInstance = engine;
        return engine;
    }
};