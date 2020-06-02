import { Engine } from '@babylonjs/core';

const disposedPropertyName = "EngineHelper_IsDisposed";

export function IsEngineDisposed(engine: Engine): boolean {
    return (engine as any)[disposedPropertyName];
}

export function DisposeEngine(engine: Engine) {
    if (engine && !IsEngineDisposed(engine)) {
        engine.dispose();
        (engine as any)[disposedPropertyName] = true;
    }
}