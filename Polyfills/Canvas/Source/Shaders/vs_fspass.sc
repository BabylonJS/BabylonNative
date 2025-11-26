$input a_position, a_texcoord0
$output v_position, v_texcoord0

#include "./common.sh"

#define NEED_HALF_TEXEL (BGFX_SHADER_LANGUAGE_HLSL < 400)

uniform vec4 u_viewSize;

#if NEED_HALF_TEXEL
uniform vec4 u_halfTexel;
#endif // NEED_HALF_TEXEL

void main()
{
#if !NEED_HALF_TEXEL
	const vec4 u_halfTexel = vec4_splat(0.0);
#endif // !NEED_HALF_TEXEL

	v_position  = a_position;
	vec2 uv = a_position.xy*3. - vec2(0.,1);
	v_texcoord0 = vec2(uv.x, 1.-uv.y) * 0.5 + vec2(0.5,0.);
	gl_Position = vec4(uv, 0.5, 1.0);
}
