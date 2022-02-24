/// <reference path="../../node_modules/babylonjs/babylon.module.d.ts" />
/// <reference path="../../node_modules/babylonjs-loaders/babylonjs.loaders.module.d.ts" />
/// <reference path="../../node_modules/babylonjs-materials/babylonjs.materials.module.d.ts" />
/// <reference path="../../node_modules/babylonjs-gui/babylon.gui.module.d.ts" />

const engine = new BABYLON.NativeEngine();


// This is necessary because Babylon Native doesn't clear the back buffer by default
engine.clear(BABYLON.Color3.Black(), true, true, true);

const shader = `
    uniform float progress;

    //uniform sampler2D outSampler;
    uniform sampler2D inSampler;

    varying vec2 vUV;

    void main(void) 
    {
        //vec4 outColor = texture2D(outSampler, vUV);
        vec4 inColor = texture2D(inSampler, vUV);
        vec4 outColor = 0.5 * inColor;
        gl_FragColor = mix(inColor, outColor, progress);
    }
`;

let inTexture = null;

const renderer = new BABYLON.EffectRenderer(engine);

const effectWrapper = new BABYLON.EffectWrapper({
  engine: engine,
  name: "Transition",
  fragmentShader: shader,
  uniformNames: ["progress", "scale"],
  samplerNames: ["outSampler", "inSampler"],
});

let animationProgressPercent = 0.99;
let sentTexturesReadyMessage = false;

BabylonJs_OnReady("Babylon ready");

// animationProgressPercent - 0.01 to 0.99
function setAnimationProgressPercent(progressPercent) 
{
  animationProgressPercent = progressPercent;
}

function startRender() 
{
  engine.runRenderLoop(() => 
  {
    // Should be done upfront for perf
    const isReady =
      effectWrapper.effect.isReady() &&
      inTexture &&
      inTexture.isReady();

    if (!isReady) {
      return;
    }

    renderer.setViewport();
    renderer.applyEffectWrapper(effectWrapper);

    //effectWrapper.effect.setTexture("outSampler", outTexture);
    effectWrapper.effect.setTexture("inSampler", inTexture);
    effectWrapper.effect.setFloat("progress", animationProgressPercent);
    renderer.draw();
  });
}

function loadNativeTexture(nativePromise, width, height)
{
    nativePromise.then((nativeTexture) =>
    {
        const internalTexture = new BABYLON.InternalTexture(
            engine,
            BABYLON.InternalTextureSource.Unknown,
            true
        );
        internalTexture._hardwareTexture = new BABYLON.WebGLHardwareTexture(
            nativeTexture
        );

        internalTexture.generateMipMaps = false;
        internalTexture.baseWidth = internalTexture.width = width;
        internalTexture.baseHeight = internalTexture.height = height;
        internalTexture.isReady = true;
        inTexture = new BABYLON.ThinTexture(internalTexture);
    });
}
