$input v_position, v_texcoord0

#include "./common.sh"

uniform vec4 u_direction;

SAMPLER2D(s_tex, 0);

void main()
{
	// 13-tap weights
	float weights[7];
	weights[0] = 0.184944; // center
	weights[1] = 0.168506;
	weights[2] = 0.127323;
	weights[3] = 0.080657;
	weights[4] = 0.041667;
	weights[5] = 0.016028;
	weights[6] = 0.004432;

	vec4 color = vec4(0.,0.,0.,0.);

	// Sample and accumulate weighted colors
	for (int i = -6; i <= 6; i++)
	{
		vec2 offset = u_direction.xy * float(i);
		color += weights[abs(i)] * texture2D(s_tex, v_texcoord0 + offset);
	}

	gl_FragColor = color;
}
