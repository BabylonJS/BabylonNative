import * as Mocha from "mocha";
import { expect } from "chai";
import {
  Color4, FreeCamera, MeshBuilder, NativeEngine, RenderTargetTexture,
  Scene, ShaderMaterial, Vector3
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
            for (let index = 0; index < centers.length; ++index) {
              const offset = (32 * 64 + centers[index]) * 4;
              expect(Array.from(pixels.subarray(offset, offset + 4)),
                `unIndexed=${unIndexed}, count=${count}, instance=${index}`)
                .to.deep.equal(expected[index]);
            }
          }
          mesh.dispose();
        }
      } finally {
        scene.dispose();
        engine.dispose();
      }
    });
  });
}
