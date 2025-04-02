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

	// 9-tap weights
	float weights[5];
	weights[0] = 0.227027; // center
	weights[1] = 0.1945946;
	weights[2] = 0.1216216;
	weights[3] = 0.054054;
	weights[4] = 0.016216;

	vec4 color = texture2D(s_tex, v_texcoord0) * weights[0]; // center pixel weighted

	// convert NDC (-1 to 1) to 0 UV (0 to 1)
	vec2 texcoord0 = v_texcoord0 * 0.5 + 0.5;
	// TODO: + u_halfTexel.xy; // handle half texel offset

	for (int i = 1; i <= 4; i++)
	{
		vec2 offset = u_direction.xy * float(i); // TODO: normalize against u_viewSize.xy
		color += texture2D(s_tex, texcoord0 + offset) * weights[i];
		color += texture2D(s_tex, texcoord0 - offset) * weights[i];
	}

	gl_FragColor = color;
}
