/// <reference types="node" />
import { ThinNativeEngine } from "@babylonjs/core/Engines/thinNativeEngine";
import fs from "fs";
import { AllEffects } from "../src/effects/effectList";

let effectName: string = "unknown";

declare const global: {
    window: Object;
    _native: any;
};

/**
 * Mock the global objects that the NativeEngine expects to find when
 * running in a Native environment.
 *
 * When an Effect is created, Babylon calls `createProgram` on the engine.
 * Our mock intercepts this call and writes the final preprocessed vertex and
 * fragment shader source to disk so that ShaderTool can compile them offline.
 */
function createMockNativeLayer(): void {
    global.window = {};

    global._native = {
        Engine: class MockEngine {
            setCommandDataStream() {}
            setHardwareScalingLevel() {}
            submitCommands() {}
            createProgram(vertex: string, fragment: string) {
                // When CreateProgram is called, we have the final preprocessed shader code
                saveShaderFiles(effectName, vertex, fragment);
            }
            getUniforms() {
                return [];
            }
            getAttributes() {
                return [];
            }
        },
        NativeDataStream: class {
            VALIDATION_ENABLED = false;
        },
    };
    global._native.Engine.PROTOCOL_VERSION = (ThinNativeEngine as any).PROTOCOL_VERSION;
    global._native.Engine.COMMAND_SETDEPTHTEST = new Uint32Array();
}

/**
 * Write the vertex and fragment shader source to files under dist/shaders/<name>/.
 */
function saveShaderFiles(name: string, vertex: string, fragment: string): void {
    const dir = `./dist/shaders/${name}`;
    fs.mkdirSync(dir, { recursive: true });

    const vertexFileName = `${dir}/vertex.fx`;
    console.log(`Writing ${vertexFileName}`);
    fs.writeFileSync(vertexFileName, vertex);

    const fragmentFileName = `${dir}/fragment.fx`;
    console.log(`Writing ${fragmentFileName}`);
    fs.writeFileSync(fragmentFileName, fragment);
}

/**
 * Prepare all registered Effects for offline compilation.
 *
 * Creates a mocked NativeEngine and runs each effect through it, which
 * causes the final preprocessed shader source to be captured and written to
 * dist/shaders.
 */
async function main(): Promise<void> {
    try {
        createMockNativeLayer();
        const engine = new ThinNativeEngine();
        engine._caps.parallelShaderCompile = undefined;

        for (const effect of AllEffects) {
            effectName = effect.name;
            console.log(`Preparing ${effectName} effect...`);
            effect.create(engine);
        }
    } catch (error) {
        console.warn(error);
    }
}

main().catch(console.error);
