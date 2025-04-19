$input v_position, v_texcoord0

#include "./common.sh"

uniform vec4 u_viewSize;	// vec4 (width, height, unused, unused)
uniform vec4 u_direction;	// vec4 (x, y, unused, unused)
uniform vec4 u_weights;		// vec4 (kernel size, kernel mid, unused, unused) // TODO: use 3rd & 4th arguments for handling even cases?

SAMPLER2D(s_tex, 0);

#if NEED_HALF_TEXEL
uniform vec4 u_halfTexel;
#endif // NEED_HALF_TEXEL

void main()
{
#if !NEED_HALF_TEXEL
	vec4 u_halfTexel = vec4_splat(0.0);
#endif // !NEED_HALF_TEXEL
	vec2 texcoord0 = v_texcoord0 + u_halfTexel.xy;
	vec4 color = texture2D(s_tex, texcoord0); // sample center
	vec2 texelSize = vec2_splat(1.0) / u_viewSize.xy;

	// sample sides
	for (int i = 1; i <= int(u_weights.y); i++)
	{
		vec2 offset = u_direction.xy * texelSize * float(i);
		color += texture2D(s_tex, texcoord0 + offset);
		color += texture2D(s_tex, texcoord0 - offset);
	}
	color = color / u_weights.x; // distribute equally

	gl_FragColor = color;
}
