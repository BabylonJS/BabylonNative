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

// Samples the Y (R8) and UV (R8G8) planes of an NV12 array slice. Each plane is wrapped as a
// single-slice view (selected at CreateForJavaScript time via layerIndex), so the shader samples
// plain sampler2D. Raw passthrough: the luma and chroma bytes are written straight to the output
// (R=Y, G=U, B=V) so the readback exactly reveals which slice and plane each sampler read.
const fragmentShader = `
    precision highp float;
    uniform sampler2D yTextureSampler;
    uniform sampler2D uvTextureSampler;
    varying vec2 vUV;
    void main(void) {
        float y = texture2D(yTextureSampler, vUV).r;
        vec2 uv = texture2D(uvTextureSampler, vUV).rg;
        gl_FragColor = vec4(y, uv.x, uv.y, 1.0);
    }
`;

let engine: NativeEngine;
let scene: Scene;
let material: ShaderMaterial;

function startup(
    yNativeTexture: any,
    uvNativeTexture: any,
    outputNativeTexture: any,
    width: number,
    height: number
): void {
    engine = new NativeEngine();
    delete engine.getCaps().parallelShaderCompile;
    scene = new Scene(engine);

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

    const camera = new FreeCamera("camera", new Vector3(0, 0, -1), scene);
    camera.setTarget(Vector3.Zero());
    camera.mode = Camera.ORTHOGRAPHIC_CAMERA;
    camera.orthoTop = 1;
    camera.orthoBottom = -1;
    camera.orthoLeft = -1;
    camera.orthoRight = 1;
    camera.outputRenderTarget = outputTexture;

    const quad = MeshBuilder.CreatePlane("quad", { size: 2 }, scene);

    material = new ShaderMaterial(
        "nv12Shader",
        scene,
        { vertexSource: vertexShader, fragmentSource: fragmentShader },
        {
            attributes: ["position", "uv"],
            uniforms: ["worldViewProjection"],
            samplers: ["yTextureSampler", "uvTextureSampler"],
        }
    );

    material.onError = (_effect, errors) => {
        console.error("NV12 ShaderMaterial compilation error: " + errors);
    };

    material.backFaceCulling = false;
    material.depthFunction = Constants.ALWAYS;

    const yInternal = engine.wrapNativeTexture(yNativeTexture);
    const yWrapper = new Texture(null, scene);
    yWrapper._texture = yInternal;
    yWrapper.updateSamplingMode(Constants.TEXTURE_NEAREST_SAMPLINGMODE);
    material.setTexture("yTextureSampler", yWrapper);

    const uvInternal = engine.wrapNativeTexture(uvNativeTexture);
    const uvWrapper = new Texture(null, scene);
    uvWrapper._texture = uvInternal;
    uvWrapper.updateSamplingMode(Constants.TEXTURE_NEAREST_SAMPLINGMODE);
    material.setTexture("uvTextureSampler", uvWrapper);

    quad.material = material;
}

async function render(): Promise<void> {
    await scene.whenReadyAsync();
    scene.render();
}

(globalThis as any).startup = startup;
(globalThis as any).render = render;
