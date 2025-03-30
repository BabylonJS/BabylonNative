$input v_position, v_texcoord0

#include "./common.sh"

uniform vec4 u_direction; // vec4 (x, y, unused, unused)

SAMPLER2D(s_tex, 0);

void main()
{
	// 13-tap weights
	float weights[7];
	weights[0] = 0.227027; // center
	weights[1] = 0.1945946;
	weights[2] = 0.1216216;
	weights[3] = 0.054054;
	weights[4] = 0.016216;
	weights[5] = 0.004629;
	weights[6] = 0.000823;

	vec4 color = texture2D(s_tex, v_texcoord0) * weights[0]; // center pixel weighted
	float alpha = color.a;

	// PR cedric: texture coordinate is normalized. Range for sampling is [0..1]. A pixel size in normalized coordinate system is vec2(1./texture width, 1./texture height). Also, take care of half texel : https://asawicki.info/news_1516_half-pixel_offset_in_directx_11
	for (int i = 1; i <= 6; i++)
	{
		vec2 offset = u_direction.xy * float(i);

		vec4 sample1 = texture2D(s_tex, v_texcoord0 + offset) * weights[i];
		vec4 sample2 = texture2D(s_tex, v_texcoord0 - offset) * weights[i];

		color.rgb += sample1.rgb + sample2.rgb; // Sum RGB values
		alpha += sample1.a * weights[i] + sample2.a * weights[i]; // Sum alpha values
	}

	gl_FragColor = vec4(color.rgb, alpha); // Combine blurred color and alpha
}
