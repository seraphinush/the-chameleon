#version 330

// From Vertex Shader
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
	color = vec4(fcolor * vcolor, 1.0);
	vec3 char_color = vec3(1.0, 1.0, 1.0);
	if (is_alive) {
		if (color_change == 1.0) {
			char_color = vec3(1.00, 0.10, 0.10);
		} else if (color_change == 2.0) {
			char_color = vec3(1.00, 0.80, 0.00);
		} else if (color_change == 3.0) {
			char_color = vec3(0.10, 0.10, 1.00);
		} else if (color_change == 4.0) {
			char_color = vec3(0.00, 0.80, 0.00);
		}
	}
	else {
		char_color = vec3(0.00, 0.00, 0.00);
	}
	color.xyz = char_color;
}
