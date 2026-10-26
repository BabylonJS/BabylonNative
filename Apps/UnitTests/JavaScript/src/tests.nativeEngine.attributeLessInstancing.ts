import * as Mocha from "mocha";
import { expect } from "chai";
import {
    Color4, FreeCamera, Material, Mesh, MeshBuilder, NativeEngine, RenderTargetTexture,
    Scene, ShaderMaterial, Vector3, VertexData
} from "@babylonjs/core";

export function registerAttributeLessInstancingTests(
    describe: typeof Mocha.describe,
    it: typeof Mocha.it,
    enabled: boolean
): void {
    describe("NativeEngine attribute-less instancing", function () {
        (enabled ? it : it.skip)("draws every instance without an instance buffer", async function () {
            this.timeout(15000);
            const engine = new NativeEngine();
            const scene = new Scene(engine);
            try {
                scene.clearColor = new Color4(0, 0, 0, 1);
                const camera = new FreeCamera("camera", new Vector3(0, 0, -5), scene);
                camera.setTarget(Vector3.Zero());
                const material = new ShaderMaterial("instances", scene, {
                    vertexSource: `
            precision highp float;
            attribute vec3 position;
            varying vec3 instanceColor;
            void main() {
              float offset = (float(gl_InstanceID) - 1.0) * 0.6;
              gl_Position = vec4(position.x * 0.2 + offset, position.y * 0.2, 0.5, 1.0);
              instanceColor = vec3(float(gl_InstanceID == 0), float(gl_InstanceID == 1), float(gl_InstanceID == 2));
            }`,
                    fragmentSource: `
            precision highp float;
            varying vec3 instanceColor;
            void main() { gl_FragColor = vec4(instanceColor, 1.0); }`
                }, { attributes: ["position"], uniforms: [] });
                material.backFaceCulling = false;
                const target = new RenderTargetTexture("instances", 64, scene, false);
                for (const unIndexed of [false, true]) {
                    const mesh = MeshBuilder.CreatePlane("plane", { size: 2 }, scene);
                    if (unIndexed) {
                        mesh.convertToUnIndexedMesh();
                    }
                    mesh.material = material;
                    target.renderList = [mesh];
                    for (const count of [0, 1, 3]) {
                        mesh.forcedInstanceCount = count;
                        await material.forceCompilationAsync(mesh, { useInstances: count > 0 });
                        await scene.whenReadyAsync();
                        const readinessDeadline = Date.now() + 10000;
                        while (!target.isReadyForRendering()) {
                            if (Date.now() >= readinessDeadline) {
                                throw new Error(`Render target did not become ready: unIndexed=${unIndexed}, count=${count}`);
                            }
                            await new Promise<void>(resolve => setTimeout(resolve, 10));
                        }
                        target.render();
                        const pixels = await target.readPixels();
                        if (!(pixels instanceof Uint8Array)) {
                            throw new Error("Expected RGBA8 instance rendering readback");
                        }
                        const expected = [
                            [255, 0, 0, 255],
                            count === 3 ? [0, 255, 0, 255] : [0, 0, 0, 255],
                            count === 3 ? [0, 0, 255, 255] : [0, 0, 0, 255]
                        ];
                        const centers = [13, 32, 51];
                        const actual = centers.map(center => {
                            const offset = (32 * 64 + center) * 4;
                            return Array.from(pixels.subarray(offset, offset + 4));
                        });
                        expect(actual, `unIndexed=${unIndexed}, count=${count}`).to.deep.equal(expected);
                    }
                    mesh.dispose();
                }
            } finally {
                scene.dispose();
                engine.dispose();
            }
        });

        // Node materials declare world0-3 unconditionally but read them only under INSTANCES.
        (enabled ? it : it.skip)("draws without instances when the shader declares unused instance inputs", async function () {
            this.timeout(15000);
            const engine = new NativeEngine();
            const scene = new Scene(engine);
            try {
                scene.clearColor = new Color4(0, 0, 0, 1);
                const camera = new FreeCamera("camera", new Vector3(0, 0, -5), scene);
                camera.setTarget(Vector3.Zero());
                const material = new ShaderMaterial("unusedInstanceInputs", scene, {
                    vertexSource: `
            precision highp float;
            attribute vec3 position;
            attribute vec4 color;
            attribute vec4 matricesIndices;
            attribute vec4 matricesWeights;
            attribute vec4 world0;
            attribute vec4 world1;
            attribute vec4 world2;
            attribute vec4 world3;
            void main() {
            #ifdef INSTANCES
              gl_Position = mat4(world0, world1, world2, world3) * vec4(position, 1.0) + color + matricesIndices + matricesWeights;
            #else
              gl_Position = vec4(position.xy * 0.5, 0.5, 1.0);
            #endif
            }`,
                    fragmentSource: `
            precision highp float;
            void main() { gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0); }`
                }, { attributes: ["position", "color", "matricesIndices", "matricesWeights", "world0", "world1", "world2", "world3"], uniforms: [] });
                material.backFaceCulling = false;
                const target = new RenderTargetTexture("unusedInstanceInputs", 64, scene, false);
                const mesh = MeshBuilder.CreatePlane("plane", { size: 2 }, scene);
                mesh.material = material;
                target.renderList = [mesh];
                await material.forceCompilationAsync(mesh);
                await scene.whenReadyAsync();
                const readinessDeadline = Date.now() + 10000;
                while (!target.isReadyForRendering()) {
                    if (Date.now() >= readinessDeadline) {
                        throw new Error("Render target did not become ready");
                    }
                    await new Promise<void>(resolve => setTimeout(resolve, 10));
                }
                target.render();
                const pixels = await target.readPixels();
                if (!(pixels instanceof Uint8Array)) {
                    throw new Error("Expected RGBA8 readback");
                }
                const offset = (32 * 64 + 32) * 4;
                expect(Array.from(pixels.subarray(offset, offset + 4))).to.deep.equal([255, 0, 0, 255]);
            } finally {
                scene.dispose();
                engine.dispose();
            }
        });

        // A degenerate triangle fan is skipped after its zeroed instance data is bound; that binding
        // must not reach the next draw, where attribute-less instancing calls setInstanceCount.
        (enabled ? it : it.skip)("does not leak fallback instance data from a skipped draw", async function () {
            this.timeout(15000);
            const engine = new NativeEngine();
            const scene = new Scene(engine);
            try {
                scene.clearColor = new Color4(0, 0, 0, 1);
                const camera = new FreeCamera("camera", new Vector3(0, 0, -5), scene);
                camera.setTarget(Vector3.Zero());
                const fanMaterial = new ShaderMaterial("skippedFan", scene, {
                    vertexSource: `
            precision highp float;
            attribute vec3 position;
            attribute vec4 world0;
            attribute vec4 world1;
            attribute vec4 world2;
            attribute vec4 world3;
            void main() {
            #ifdef INSTANCES
              gl_Position = mat4(world0, world1, world2, world3) * vec4(position, 1.0);
            #else
              gl_Position = vec4(position, 1.0);
            #endif
            }`,
                    fragmentSource: `
            precision highp float;
            void main() { gl_FragColor = vec4(1.0); }`
                }, { attributes: ["position", "world0", "world1", "world2", "world3"], uniforms: [] });
                fanMaterial.fillMode = Material.TriangleFanDrawMode;
                const instancedMaterial = new ShaderMaterial("attributeLess", scene, {
                    vertexSource: `
            precision highp float;
            attribute vec3 position;
            void main() {
              float offset = (float(gl_InstanceID) - 1.0) * 0.6;
              gl_Position = vec4(position.x * 0.2 + offset, position.y * 0.2, 0.5, 1.0);
            }`,
                    fragmentSource: `
            precision highp float;
            void main() { gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0); }`
                }, { attributes: ["position"], uniforms: [] });
                instancedMaterial.backFaceCulling = false;
                const target = new RenderTargetTexture("skippedDraw", 64, scene, false);
                for (const unIndexed of [false, true]) {
                    const fan = new Mesh("fan", scene);
                    const fanData = new VertexData();
                    fanData.positions = [-0.5, 0, 0.5, 0.5, 0, 0.5];
                    fanData.indices = [0, 1];
                    fanData.applyToMesh(fan);
                    const plane = MeshBuilder.CreatePlane("plane", { size: 2 }, scene);
                    if (unIndexed) {
                        fan.convertToUnIndexedMesh();
                        plane.convertToUnIndexedMesh();
                    }
                    fan.material = fanMaterial;
                    plane.material = instancedMaterial;
                    plane.forcedInstanceCount = 3;
                    target.renderList = [fan, plane];
                    await fanMaterial.forceCompilationAsync(fan);
                    await instancedMaterial.forceCompilationAsync(plane, { useInstances: true });
                    await scene.whenReadyAsync();
                    const readinessDeadline = Date.now() + 10000;
                    while (!target.isReadyForRendering()) {
                        if (Date.now() >= readinessDeadline) {
                            throw new Error(`Render target did not become ready: unIndexed=${unIndexed}`);
                        }
                        await new Promise<void>(resolve => setTimeout(resolve, 10));
                    }
                    target.render();
                    const pixels = await target.readPixels();
                    if (!(pixels instanceof Uint8Array)) {
                        throw new Error("Expected RGBA8 readback");
                    }
                    const actual = [13, 32, 51].map(center => {
                        const offset = (32 * 64 + center) * 4;
                        return Array.from(pixels.subarray(offset, offset + 4));
                    });
                    expect(actual, `unIndexed=${unIndexed}`).to.deep.equal([[0, 255, 0, 255], [0, 255, 0, 255], [0, 255, 0, 255]]);
                    fan.dispose();
                    plane.dispose();
                }
            } finally {
                scene.dispose();
                engine.dispose();
            }
        });
    });
}
