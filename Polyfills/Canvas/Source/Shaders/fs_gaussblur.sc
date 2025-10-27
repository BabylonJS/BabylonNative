$input v_position, v_texcoord0

#include "./common.sh"

// 13-taps
#define W_MID 6
#define W_USIZE 5

uniform vec4 u_viewSize;			// vec4 (width, height, unused, unused)
uniform vec4 u_direction;			// vec4 (x, y, unused, unused)
uniform vec4 u_sigma;				// vec4 (sigma, unused, unused, unused)
uniform vec4 u_weights[W_USIZE];	// vec4 (weight0, weight1, ..., weight12, unused, unused, unused)

SAMPLER2D(s_tex, 0);

#if NEED_HALF_TEXEL
uniform vec4 u_halfTexel;
#endif // NEED_HALF_TEXEL

float getWeight(int i)
{
	int vecIndex = int(i / 4);
	int compIndex = int(mod(i, 4));
	return u_weights[vecIndex][compIndex];
}

void main()
{
#if !NEED_HALF_TEXEL
	vec4 u_halfTexel = vec4_splat(0.0);
#endif // !NEED_HALF_TEXEL
	vec2 texcoord0 = v_texcoord0 + u_halfTexel.xy;
	vec4 color = texture2D(s_tex, texcoord0) * getWeight(W_MID); // center pixel
	vec2 texelSize = vec2_splat(1.0) / u_viewSize.xy;

	for (int i = 1; i <= W_MID; i++)
	{
		vec2 offset = u_direction.xy * texelSize * float(i);
		color += texture2D(s_tex, texcoord0 + offset) * getWeight(W_MID + i);
		color += texture2D(s_tex, texcoord0 - offset) * getWeight(W_MID - i);
	}

	gl_FragColor = color;
}
