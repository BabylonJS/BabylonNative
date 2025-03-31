$input v_position, v_texcoord0

#include "./common.sh"

uniform vec4 u_viewSize;	// vec4 (width, height, unused, unused)
uniform vec4 u_direction;	// vec4 (x, y, unused, unused)

SAMPLER2D(s_tex, 0);

#if NEED_HALF_TEXEL
uniform vec4 u_halfTexel;
#endif // NEED_HALF_TEXEL

void main()
{
#if !NEED_HALF_TEXEL
	vec4 u_halfTexel = vec4_splat(0.0);
#endif // !NEED_HALF_TEXEL

	// 13-tap weights
	float weights[7];
	weights[0] = 0.227027; // center
	weights[1] = 0.1945946;
	weights[2] = 0.1216216;
	weights[3] = 0.054054;
	weights[4] = 0.016216;
	weights[5] = 0.004629;
	weights[6] = 0.000823;

	vec4 color = texture2D(s_tex, v_texcoord0) * weights[0]; // center pixel weighted
	float alpha = color.a;

	vec2 texcoord0 = v_texcoord0 + u_halfTexel.xy; // handle half texel offset
	// vec2 normTex = vec2_splat(1.0) / u_viewSize.xy; // normalized texel size?

	for (int i = 1; i <= 6; i++)
	{
		vec2 offset = u_direction.xy * float(i); // TODO: normalize against u_viewSize.xy
		vec4 sample1 = texture2D(s_tex, texcoord0 + offset) * weights[i];
		vec4 sample2 = texture2D(s_tex, texcoord0 - offset) * weights[i];
		color += sample1 + sample2;
	}

	gl_FragColor = color;
}
