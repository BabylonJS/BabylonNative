$input v_position, v_texcoord0

#include "./common.sh"

#define EDGE_AA 1

uniform mat3 u_scissorMat;
uniform mat3 u_paintMat;
uniform vec4 u_innerCol;
uniform vec4 u_outerCol;
uniform vec4 u_scissorExtScale;
uniform vec4 u_extentRadius;
uniform vec4 u_params;

SAMPLER2D(s_tex, 0);

#define u_scissorExt   (u_scissorExtScale.xy)
#define u_scissorScale (u_scissorExtScale.zw)
#define u_extent       (u_extentRadius.xy)
#define u_radius       (u_extentRadius.z)
#define u_feather      (u_params.x)
#define u_strokeMult   (u_params.y)
#define u_texType      (u_params.z)
#define u_type         (u_params.w)

void main()
{
	vec4 color = vec4(0.,0.,0.,0.);//texture2D(s_tex, v_texcoord0);
	for (int y = -4;y < 5; y++)
	{
		for (int x = -4;x < 5; x++)
		{
			color += texture2D(s_tex, v_texcoord0 + vec2(x,y) * (1./256.));
		}
	}

	color *= 1./81.;
	gl_FragColor = color;// * vec4(1.,0.5,0.25,1.);
}
