#version 330

// From vertex shader
in vec2 texcoord;

// Application data
uniform sampler2D sampler0;
uniform vec3 fcolor;
uniform int flash_map;
uniform float flash_timer;

// Output color
layout(location = 0) out  vec4 color;

void main()
{
	color = vec4(fcolor, 1.0) * texture(sampler0, vec2(texcoord.x, texcoord.y));
	if (flash_map == 1 && flash_timer > 0)
		if (gl_FragCoord.x < 1200 && gl_FragCoord.y < 800)
			color += 0.4 * flash_timer * vec4(0.1, 0.1, 0.1, 0);
}