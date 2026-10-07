$input v_position, v_texcoord0

#include "./common.sh"

SAMPLER2D(s_tex, 0);
uniform vec4 u_canvasCopy;

void main()
{
    vec2 uv = v_texcoord0;
    if (u_canvasCopy.x > 0.5)
    {
        uv.y = 1.0 - uv.y;
    }
    vec4 color = texture2D(s_tex, uv);
    if (u_canvasCopy.y < 0.5)
    {
        color.rgb = color.a > 0.0 ? color.rgb / color.a : vec3_splat(0.0);
    }
    gl_FragColor = color;
}
