// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
//
// WGSL port of the bgfx NanoVG shaders (vs_nanovg_fill.sc / fs_nanovg_fill.sc)
// used by the Canvas polyfill. WGSL is compiled by Dawn at runtime, so unlike
// the bgfx path there is no offline shader-compilation step and no per-platform
// blob to check in.
//
// Uniform layout note: bgfx uploaded these as individual uniforms whose byte
// ranges overlapped inside GLNVGfragUniforms (u_extentRadius read 4 floats from
// `extent`, u_params read 4 floats from `feather`, so both saw `feather`). Here
// the buffer is a single explicit std140-compatible struct, so `feather` is
// simply stored twice; NanoVGDawnFragUniforms in nanovg_dawn.cpp must match this
// layout exactly.

#pragma once

namespace Babylon::Polyfills::Internal
{
    // 3x3 affine transforms arrive as 3 padded vec4 columns:
    //   col0 = (t0, t1, 0, 0), col1 = (t2, t3, 0, 0), col2 = (t4, t5, 1, 0)
    // which is NanoVG's 2x3 affine matrix, matching glnvg__xformToMat3x4.
    static constexpr const char* NANOVG_DAWN_WGSL = R"WGSL(
struct FragUniforms {
    scissorMat0 : vec4<f32>,
    scissorMat1 : vec4<f32>,
    scissorMat2 : vec4<f32>,
    paintMat0 : vec4<f32>,
    paintMat1 : vec4<f32>,
    paintMat2 : vec4<f32>,
    innerCol : vec4<f32>,
    outerCol : vec4<f32>,
    // xy = scissorExt, zw = scissorScale
    scissorExtScale : vec4<f32>,
    // xy = extent, z = radius, w = feather
    extentRadius : vec4<f32>,
    // x = feather, y = strokeMult, z = texType, w = type
    params : vec4<f32>,
    // x = sdfMin, y = sdfMax, z = sdfBlur
    sdf : vec4<f32>,
    // xy = viewport size in pixels, zw = unused
    viewRect : vec4<f32>,
};

@group(0) @binding(0) var<uniform> u : FragUniforms;
@group(0) @binding(1) var s_samp : sampler;
@group(0) @binding(2) var s_tex : texture_2d<f32>;
@group(0) @binding(3) var s_tex2 : texture_2d<f32>;

struct VertexOutput {
    @builtin(position) clipPosition : vec4<f32>,
    @location(0) v_position : vec2<f32>,
    @location(1) v_texcoord0 : vec2<f32>,
    @location(2) v_texcoord1 : vec2<f32>,
};

fn applyXform(c0 : vec4<f32>, c1 : vec4<f32>, c2 : vec4<f32>, p : vec2<f32>) -> vec2<f32> {
    return vec2<f32>(
        c0.x * p.x + c1.x * p.y + c2.x,
        c0.y * p.x + c1.y * p.y + c2.y);
}

@vertex
fn vertexMain(
    @location(0) a_position : vec2<f32>,
    @location(1) a_texcoord0 : vec2<f32>
) -> VertexOutput {
    var out : VertexOutput;
    out.v_position = a_position;
    out.v_texcoord0 = a_texcoord0;

    // Second texture coordinate, used when a gradient modulates an image.
    let viewW = u.viewRect.x;
    let viewH = u.viewRect.y;
    out.v_texcoord1 =
        ((a_position / vec2<f32>(viewW, viewH)) - vec2<f32>(u.extentRadius.x / viewW, 1.0))
        / vec2<f32>(u.extentRadius.y / viewW, 1.0);

    // NanoVG works in pixel space with Y down; WebGPU clip space is Y up, and
    // its Z range is [0,1] (not GL's [-1,1]), so Z is 0 rather than the .sc
    // shader's 0.0-in-[-1,1].
    out.clipPosition = vec4<f32>(
        2.0 * a_position.x / viewW - 1.0,
        1.0 - 2.0 * a_position.y / viewH,
        0.0,
        1.0);
    return out;
}

fn sdroundrect(pt : vec2<f32>, ext : vec2<f32>, rad : f32) -> f32 {
    let ext2 = ext - vec2<f32>(rad, rad);
    let d = abs(pt) - ext2;
    return min(max(d.x, d.y), 0.0) + length(max(d, vec2<f32>(0.0, 0.0))) - rad;
}

fn scissorMask(p : vec2<f32>) -> f32 {
    let scissorExt = u.scissorExtScale.xy;
    let scissorScale = u.scissorExtScale.zw;
    var sc = abs(applyXform(u.scissorMat0, u.scissorMat1, u.scissorMat2, p)) - scissorExt;
    sc = vec2<f32>(0.5, 0.5) - sc * scissorScale;
    return clamp(sc.x, 0.0, 1.0) * clamp(sc.y, 0.0, 1.0);
}

// Stroke - from [0..1] to clipped pyramid, where the slope is 1px.
fn strokeMask(texcoord : vec2<f32>) -> f32 {
    let strokeMult = u.params.y;
    return min(1.0, (1.0 - abs(texcoord.x * 2.0 - 1.0)) * strokeMult) * min(1.0, texcoord.y);
}

fn sampleSDF(edge : f32, color : vec4<f32>) -> f32 {
    let sdfBlur = u.sdf.z;
    let result = (color.x - edge) / sdfBlur + 0.5;
    return clamp(result, 0.0, 1.0);
}

@fragment
fn fragmentMain(in : VertexOutput) -> @location(0) vec4<f32> {
    let extent = u.extentRadius.xy;
    let radius = u.extentRadius.z;
    let feather = u.params.x;
    let texType = u.params.z;
    let shaderType = u.params.w;
    let sdfMin = u.sdf.x;
    let sdfMax = u.sdf.y;

    let scissor = scissorMask(in.v_position);
    let strokeAlpha = strokeMask(in.v_texcoord0);

    var result = vec4<f32>(0.0, 0.0, 0.0, 0.0);

    if (shaderType == 0.0) {
        // Gradient: calculate the color using a box gradient.
        let pt = applyXform(u.paintMat0, u.paintMat1, u.paintMat2, in.v_position);
        let d = clamp((sdroundrect(pt, extent, radius) + feather * 0.5) / feather, 0.0, 1.0);
        var color = mix(u.innerCol, u.outerCol, d);
        color = color * strokeAlpha * scissor;
        result = color;
    } else if (shaderType == 1.0) {
        // Image
        let pt = applyXform(u.paintMat0, u.paintMat1, u.paintMat2, in.v_position) / extent;
        var color = textureSample(s_tex, s_samp, pt);
        if (texType == 1.0) {
            color = vec4<f32>(color.xyz * color.w, color.w);
        }
        if (texType == 2.0) {
            color = color.xxxx;
        }
        color = color * u.innerCol;
        color = color * strokeAlpha * scissor;
        result = color;
    } else if (shaderType == 2.0) {
        // Stencil fill
        result = vec4<f32>(1.0, 1.0, 1.0, 1.0);
    } else if (shaderType == 3.0) {
        // Textured triangles
        var color = textureSample(s_tex, s_samp, in.v_texcoord0);
        if (texType == 1.0) {
            color = vec4<f32>(color.xyz * color.w, color.w);
        }
        if (texType == 2.0) {
            let sdf = sampleSDF(sdfMin, color) * (1.0 - sampleSDF(sdfMax, color));
            color = vec4<f32>(sdf, sdf, sdf, sdf);
        }
        color = color * scissor;
        result = color * u.innerCol;
    } else if (shaderType == 4.0) {
        // Textured triangles modulated by a second texture
        var color = textureSample(s_tex, s_samp, in.v_texcoord0);
        let color2 = textureSample(s_tex2, s_samp, in.v_texcoord1);
        if (texType == 1.0) {
            color = vec4<f32>(color.xyz * color.w, color.w);
        }
        if (texType == 2.0) {
            let sdf = sampleSDF(sdfMin, color) * (1.0 - sampleSDF(sdfMax, color));
            color = vec4<f32>(sdf, sdf, sdf, sdf);
        }
        color = color * scissor;
        color = color * color2;
        result = color * u.innerCol;
    }

    return result;
}
)WGSL";
}
