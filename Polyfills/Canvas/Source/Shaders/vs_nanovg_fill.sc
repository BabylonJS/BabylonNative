$input a_position, a_texcoord0
$output v_position, v_texcoord0, v_texcoord1

#include "./common.sh"

uniform vec4 u_viewSize;
uniform vec4 u_extentRadius;

void main()
{
	v_position  = a_position;
	v_texcoord0 = a_texcoord0;
	// v_texcoord1 texture coordinate for second image used for gradient
	v_texcoord1 = ((a_position.xy / u_viewSize.xy) - vec2(u_extentRadius.x / u_viewSize.x, 1.)) / vec2(u_extentRadius.y / u_viewSize.x,1.);
	gl_Position = vec4(2.0*v_position.x/u_viewSize.x - 1.0, 1.0 - 2.0*v_position.y/u_viewSize.y, 0.0, 1.0);
}
