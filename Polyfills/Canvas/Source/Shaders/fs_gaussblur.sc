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
	weights[0] = 0.340; // center
	weights[1] = 0.250;
	weights[2] = 0.130;
	weights[3] = 0.060;
	weights[4] = 0.020;

	vec2 texcoord0 = v_texcoord0 + u_halfTexel.xy;
	vec4 color = texture2D(s_tex, texcoord0) * weights[0]; // center pixel
	vec2 texelSize = vec2_splat(1.0) / u_viewSize.xy;

	for (int i = 1; i <= 4; i++)
	{
		vec2 offset = u_direction.xy * texelSize * float(i);
		color += texture2D(s_tex, texcoord0 + offset) * weights[i];
		color += texture2D(s_tex, texcoord0 - offset) * weights[i];
	}

	gl_FragColor = color;
}
