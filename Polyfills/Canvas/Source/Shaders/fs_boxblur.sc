$input v_position, v_texcoord0

#include "./common.sh"

uniform vec4 u_viewSize;	// vec4 (width, height, unused, unused)
uniform vec4 u_direction;	// vec4 (x, y, unused, unused)
uniform vec4 u_weights;		// vec4 (kernel, radius, offsetX, offsetY)

SAMPLER2D(s_tex, 0);

#if NEED_HALF_TEXEL
uniform vec4 u_halfTexel;
#endif // NEED_HALF_TEXEL

#define EPSILON 1.23e-6
#define kernel u_weights.x
#define radius u_weights.y
#define offsetX u_weights.z
#define offsetY u_weights.w

void main()
{
#if !NEED_HALF_TEXEL
	vec4 u_halfTexel = vec4_splat(0.0);
#endif // !NEED_HALF_TEXEL
	vec2 texcoord0 = v_texcoord0 + u_halfTexel.xy;
	vec4 color = vec4_splat(0.0);

	// sample center if no offset (ie. odd kernel)
	if (abs(offsetX) < EPSILON && abs(offsetY) < EPSILON)
	{
		color += texture2D(s_tex, texcoord0);
	}

	// shift by offset (ie. even kernel)
	vec2 direction = u_direction.xy + vec2(offsetX, offsetY);
	vec2 texelSize = vec2_splat(1.0) / u_viewSize.xy;

	// sample sides
	for (int i = 1; i <= int(radius); i++)
	{
		vec2 offset = direction * texelSize * float(i);
		color += texture2D(s_tex, texcoord0 + offset);
		color += texture2D(s_tex, texcoord0 - offset);
	}
	color = color / kernel; // distribute equally

	gl_FragColor = color;
}
