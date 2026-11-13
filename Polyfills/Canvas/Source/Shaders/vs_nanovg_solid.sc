$input a_position, a_texcoord0, a_color0
$output v_texcoord0, v_color0

#include "./common.sh"

void main()
{
	v_texcoord0 = a_texcoord0;
	v_color0 = a_color0;
	gl_Position = vec4(2.0*a_position.x/u_viewRect.z - 1.0, 1.0 - 2.0*a_position.y/u_viewRect.w, 0.0, 1.0);
}
