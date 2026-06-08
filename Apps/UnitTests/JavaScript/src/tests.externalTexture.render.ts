import {
    Camera,
    Constants,
    FreeCamera,
    MeshBuilder,
    NativeEngine,
    RenderTargetTexture,
    Scene,
    ShaderMaterial,
    Texture,
    Vector3,
} from "@babylonjs/core";

const vertexShader = `
    precision highp float;
    attribute vec3 position;
    attribute vec2 uv;
    uniform mat4 worldViewProjection;
    varying vec2 vUV;
    void main(void) {
        gl_Position = worldViewProjection * vec4(position, 1.0);
        vUV = uv;
    }
`;

const fragmentShader = `
    precision highp float;
    precision highp sampler2DArray;
    uniform sampler2DArray textureArraySampler;
    uniform float sliceIndex;
    varying vec2 vUV;
    void main(void) {
        gl_FragColor = texture(textureArraySampler, vec3(vUV, sliceIndex));
    }
`;

let engine: NativeEngine;
let scene: Scene;
let material: ShaderMaterial;

function startup(
    inputNativeTexture: any,
    outputNativeTexture: any,
    width: number,
    height: number
): void {
    engine = new NativeEngine();
    delete engine.getCaps().parallelShaderCompile;
    scene = new Scene(engine);

    // Wrap the output texture as a render target.
    const outputTexture = new RenderTargetTexture(
        "output",
        { width, height },
        scene,
        {
            colorAttachment: engine.wrapNativeTexture(outputNativeTexture),
            generateDepthBuffer: true,
            generateStencilBuffer: false,
        }
    );

    // Orthographic camera filling the viewport exactly.
    const camera = new FreeCamera("camera", new Vector3(0, 0, -1), scene);
    camera.setTarget(Vector3.Zero());
    camera.mode = Camera.ORTHOGRAPHIC_CAMERA;
    camera.orthoTop = 1;
    camera.orthoBottom = -1;
    camera.orthoLeft = -1;
    camera.orthoRight = 1;
    camera.outputRenderTarget = outputTexture;

    // Fullscreen quad.
    const quad = MeshBuilder.CreatePlane("quad", { size: 2 }, scene);

    // Shader material that samples from a texture array.
    material = new ShaderMaterial(
        "textureArrayShader",
        scene,
        { vertexSource: vertexShader, fragmentSource: fragmentShader },
        {
            attributes: ["position", "uv"],
            uniforms: ["worldViewProjection", "sliceIndex"],
            samplers: ["textureArraySampler"],
        }
    );

    material.onError = (_effect, errors) => {
        console.error("ShaderMaterial compilation error: " + errors);
    };

    material.backFaceCulling = false;
    material.depthFunction = Constants.ALWAYS;

    // Wrap the input texture array and bind it to the shader.
    const internalTex = engine.wrapNativeTexture(inputNativeTexture);
    const inputTexWrapper = new Texture(null, scene);
    inputTexWrapper._texture = internalTex;
    material.setTexture("textureArraySampler", inputTexWrapper);
    material.setFloat("sliceIndex", 0);

    quad.material = material;
}

async function renderSlice(sliceIndex: number): Promise<void> {
    material.setFloat("sliceIndex", sliceIndex);
    await scene.whenReadyAsync();
    scene.render();
}

(globalThis as any).startup = startup;
(globalThis as any).renderSlice = renderSlice;
