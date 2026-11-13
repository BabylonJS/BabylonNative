$input v_texcoord0, v_color0

#include "./common.sh"

void main()
{
	float strokeAlpha = min(1.0, (1.0 - abs(v_texcoord0.x*2.0 - 1.0) ) ) * min(1.0, v_texcoord0.y);
	gl_FragColor = v_color0 * strokeAlpha;
}
