$input a_position, a_texcoord0
$output v_position, v_texcoord0

#include "./common.sh"

void main()
{
	v_position  = a_position;
	vec2 uv = a_position.xy*3. - vec2(0.,1);
	v_texcoord0 = vec2(uv.x, 1.-uv.y) * 0.5 + vec2(0.5,0.);
	gl_Position = vec4(uv, 0.5, 1.0);
}
