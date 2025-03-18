$input v_position, v_texcoord0, v_texcoord1

#include "./common.sh"

#define EDGE_AA 1

uniform vec4 u_viewSize;
uniform mat3 u_scissorMat;
uniform mat3 u_paintMat;
uniform vec4 u_innerCol;
uniform vec4 u_outerCol;
uniform vec4 u_scissorExtScale;
uniform vec4 u_extentRadius;
uniform vec4 u_params;
uniform vec4 u_sdf;

SAMPLER2D(s_tex, 0);
SAMPLER2D(s_tex2, 1);

#define u_scissorExt   (u_scissorExtScale.xy)
#define u_scissorScale (u_scissorExtScale.zw)
#define u_extent       (u_extentRadius.xy)
#define u_radius       (u_extentRadius.z)
#define u_feather      (u_params.x)
#define u_strokeMult   (u_params.y)
#define u_texType      (u_params.z)
#define u_type         (u_params.w)
#define u_sdfMin       (u_sdf.x)
#define u_sdfMax       (u_sdf.y)
#define u_sdfBlur      (u_sdf.z)

#define SDF_EDGE (128.0/255.0)

float sdroundrect(vec2 pt, vec2 ext, float rad)
{
	vec2 ext2 = ext - vec2(rad,rad);
	vec2 d = abs(pt) - ext2;
	return min(max(d.x, d.y), 0.0) + length(max(d, 0.0) ) - rad;
}

// Scissoring
float scissorMask(vec2 p)
{
	vec2 sc = abs(mul(u_scissorMat, vec3(p, 1.0) ).xy) - u_scissorExt;
	sc = vec2(0.5, 0.5) - sc * u_scissorScale;
	return clamp(sc.x, 0.0, 1.0) * clamp(sc.y, 0.0, 1.0);
}

// Stroke - from [0..1] to clipped pyramid, where the slope is 1px.
float strokeMask(vec2 _texcoord)
{
#if EDGE_AA
	return min(1.0, (1.0 - abs(_texcoord.x*2.0 - 1.0) )*u_strokeMult) * min(1.0, _texcoord.y);
#else
	return 1.0;
#endif // EDGE_AA
}

float sampleSDF(float edge, vec4 color)
{
	float result = (color.x - edge) / u_sdfBlur + 0.5;
	return clamp(result, 0.0, 1.0);
}

void main()
{
	vec4 result;
	float scissor = scissorMask(v_position);
	float strokeAlpha = strokeMask(v_texcoord0);

	if (u_type == 0.0) // Gradient
	{
		// Calculate gradient color using box gradient
		vec2 pt = mul(u_paintMat, vec3(v_position, 1.0) ).xy;
		float d = clamp( (sdroundrect(pt, u_extent, u_radius) + u_feather*0.5) / u_feather, 0.0, 1.0);
		vec4 color = mix(u_innerCol, u_outerCol, d);
		// Combine alpha
		color *= strokeAlpha * scissor;
		result = color;
	}
	else if (u_type == 1.0) // Image
	{
		// Calculate color from texture
		vec2 pt = mul(u_paintMat, vec3(v_position, 1.0) ).xy / u_extent;
		vec4 color = texture2D(s_tex, pt);
		if (u_texType == 1.0) color = vec4(color.xyz * color.w, color.w);
		if (u_texType == 2.0) color = color.xxxx;
		// Apply color tint and alpha
		color *= u_innerCol;
		// Combine alpha
		color *= strokeAlpha * scissor;
		result = color;
	}
	else if (u_type == 2.0) // Stencil fill
	{
		result = vec4(1.0, 1.0, 1.0, 1.0);
	}
	else if (u_type == 3.0) // Textured tris
	{
		vec4 color = texture2D(s_tex, v_texcoord0.xy);
		if (u_texType == 1.0) color = vec4(color.xyz * color.w, color.w);
		if (u_texType == 2.0) {
			float sdf = sampleSDF(u_sdfMin, color) * (1.0 - sampleSDF(u_sdfMax, color));
			color = vec4(sdf, sdf, sdf, sdf);
		}
		color *= scissor;
		result = color * u_innerCol;
	}
	else if (u_type == 4.0) // Textured tris modulated by texture
	{
		vec4 color = texture2D(s_tex, v_texcoord0.xy);
		vec2 pt = v_texcoord1.xy;
		vec4 color2 = texture2D(s_tex2, pt);
		if (u_texType == 1.0) color = vec4(color.xyz * color.w, color.w);
		if (u_texType == 2.0) {
			float sdf = sampleSDF(u_sdfMin, color) * (1.0 - sampleSDF(u_sdfMax, color));
			color = vec4(sdf, sdf, sdf, sdf);
		}
		color *= scissor;
		color *= color2;
		result = color * u_innerCol;
	}

	gl_FragColor = result;
}
