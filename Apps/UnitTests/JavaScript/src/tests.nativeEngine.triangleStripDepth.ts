import * as Mocha from "mocha";
import { expect } from "chai";
import {
    Camera,
    Color4,
    FreeCamera,
    Material,
    Matrix,
    Mesh,
    NativeEngine,
    RenderTargetTexture,
    Scene,
    ShaderMaterial,
    Vector3,
    VertexBuffer,
} from "@babylonjs/core";

async function renderTriangleStripDepthOverlap(instanced: boolean, depthWrite: boolean): Promise<number[]> {
    const engine = new NativeEngine();
    const scene = new Scene(engine);
    try {
        const camera = new FreeCamera("camera", new Vector3(0, 0, -3), scene);
        camera.setTarget(Vector3.Zero());
        camera.mode = Camera.ORTHOGRAPHIC_CAMERA;
        camera.orthoTop = 1;
        camera.orthoBottom = -1;
        camera.orthoLeft = -1;
        camera.orthoRight = 1;
        scene.activeCamera = camera;

        const material = new ShaderMaterial(
            "triangleStripDepth",
            scene,
            {
                vertexSource: `
                    precision highp float;
                    attribute vec3 position;
                    uniform mat4 viewProjection;
                    #include<instancesDeclaration>
                    varying vec3 vColor;
                    void main(void) {
                        #include<instancesVertex>
                        gl_Position = viewProjection * finalWorld * vec4(position, 1.0);
                        vColor = finalWorld[3].z < 0.25 ? vec3(1.0, 0.0, 0.0) : vec3(0.0, 0.0, 1.0);
                    }`,
                fragmentSource: `
                    precision highp float;
                    varying vec3 vColor;
                    void main(void) {
                        gl_FragColor = vec4(vColor, 1.0);
                    }`,
            },
            {
                attributes: ["position"],
                uniforms: ["world", "viewProjection"],
            }
        );
        material.fillMode = Material.TriangleStripDrawMode;
        material.disableDepthWrite = !depthWrite;

        const createStrip = (name: string): Mesh => {
            const mesh = new Mesh(name, scene);
            mesh.setVerticesData(VertexBuffer.PositionKind, [-0.8, -0.8, 0, 0.8, -0.8, 0, -0.8, 0.8, 0, 0.8, 0.8, 0]);
            mesh.isUnIndexed = true;
            mesh.material = material;
            return mesh;
        };

        const renderTarget = new RenderTargetTexture("triangleStripDepthTarget", { width: 16, height: 16 }, scene, {
            generateDepthBuffer: true,
            generateStencilBuffer: false,
            samples: 1,
        });
        renderTarget.clearColor = new Color4(0, 0, 0, 1);
        renderTarget.activeCamera = camera;

        if (instanced) {
            const strip = createStrip("instancedStrip");
            const matrices = new Float32Array(32);
            Matrix.Identity().copyToArray(matrices, 0);
            Matrix.Translation(0, 0, 0.5).copyToArray(matrices, 16);
            strip.thinInstanceSetBuffer("matrix", matrices, 16, true);
            renderTarget.renderList!.push(strip);
        } else {
            const nearStrip = createStrip("nearStrip");
            const farStrip = createStrip("farStrip");
            farStrip.position.z = 0.5;
            nearStrip.alphaIndex = 0;
            farStrip.alphaIndex = 1;
            renderTarget.setRenderingOrder(0, (a, b) => a.getMesh().alphaIndex - b.getMesh().alphaIndex);
            renderTarget.renderList!.push(nearStrip, farStrip);
        }

        await scene.whenReadyAsync();
        renderTarget.render();
        const pixels = await renderTarget.readPixels();
        if (!(pixels instanceof Uint8Array)) {
            throw new Error("Expected RGBA8 triangle-strip render target readback");
        }
        const offset = (8 * 16 + 8) * 4;
        return Array.from(pixels.subarray(offset, offset + 4));
    } finally {
        scene.dispose();
        engine.dispose();
    }
}

export function registerTriangleStripDepthTests(
    describe: typeof Mocha.describe,
    it: typeof Mocha.it,
    skipCanvasGpuTests: boolean
): void {
    describe("NativeEngine triangle-strip depth writes", function () {
        this.timeout(10000);
        for (const instanced of [false, true]) {
            for (const depthWrite of [false, true]) {
                (skipCanvasGpuTests ? it.skip : it)(
                    `${instanced ? "instanced" : "non-instanced"} strips preserve depthWrite=${depthWrite}`,
                    async function () {
                        const pixel = await renderTriangleStripDepthOverlap(instanced, depthWrite);
                        expect(pixel).to.deep.equal(depthWrite ? [255, 0, 0, 255] : [0, 0, 255, 255]);
                    }
                );
            }
        }
    });
}
