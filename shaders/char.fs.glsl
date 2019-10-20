#version 330

// From Vertex Shader
in vec2 texcoord;

out vec3 vcolor;
out vec2 vpos;

// Application data
uniform sampler2D sampler0;
uniform vec3 fcolor;
uniform float color_change;
uniform bool is_alive;

// Output color
layout(location = 0) out vec4 color;

void main()
{
	color = vec4(fcolor, 1.0) * texture(sampler0, vec2(texcoord.x, texcoord.y));
	vec3 char_color = vec3(1.0, 1.0, 1.0);
	if (is_alive) {
		if (color_change == 1.0) {
			char_color = vec3(0.80, 0.00, 0.00);
		} else if (color_change == 2.0) {
			char_color = vec3(0.00, 0.80, 0.00);
		} else if (color_change == 3.0) {
			char_color = vec3(0.00, 0.00, 0.80);
		} else if (color_change == 4.0) {
			char_color = vec3(0.80, 0.80, 0.00);
		}
	}
	else {
		char_color = vec3(0.00, 0.00, 0.00);
	}
	color.xyz = char_color;
}
