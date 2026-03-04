import { Effect } from "@babylonjs/core/Materials/effect";
import type { AbstractEngine } from "@babylonjs/core/Engines/abstractEngine";

export const vertexShader = `
attribute vec3 position;
attribute vec3 normal;

uniform mat4 world;
uniform mat4 viewProjection;

varying vec3 vNormalW;
varying vec3 vPositionW;

void main(void) {
    vec4 worldPos = world * vec4(position, 1.0);
    vPositionW = worldPos.xyz;
    vNormalW = normalize((world * vec4(normal, 0.0)).xyz);
    gl_Position = viewProjection * worldPos;
}`;

export const fragmentShader = `
precision highp float;

varying vec3 vNormalW;
varying vec3 vPositionW;

uniform vec4 baseColor;

void main(void) {
    vec3 lightDir = normalize(vec3(0.5, 1.0, -0.5));
    float ndl = max(dot(normalize(vNormalW), lightDir), 0.0);
    vec3 color = baseColor.rgb * (ndl * 0.8 + 0.2);
    gl_FragColor = vec4(color, baseColor.a);
}`;

export function createSimpleEffect(engine: AbstractEngine): Effect {
    return new Effect(
        {
            vertexSource: vertexShader.trim(),
            fragmentSource: fragmentShader.trim(),
        },
        ["position", "normal"],
        ["world", "viewProjection", "baseColor"],
        null,
        engine,
        "#define NUM_BONE_INFLUENCERS 0\n#define NUM_MORPH_INFLUENCERS 0"
    );
}
