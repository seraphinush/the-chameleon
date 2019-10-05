#version 330

// From Vertex Shader
in vec3 vcolor;
in vec2 vpos; // Distance from local origin

// Application data
uniform sampler2D sampler0;
uniform vec3 fcolor;
uniform int light_up;
uniform float color_change;
uniform bool is_alive;

// Output color
layout(location = 0) out  vec4 color;

void main()
{
	color = vec4(fcolor * vcolor, 1.0);
	vec3 salmon_color = vec3(1.0, 1.0, 1.0);
	if (is_alive) {
		if (color_change == 1.0) {
			salmon_color = vec3(1.0, 0.0, 0.0);
		} else if (color_change == 2.0) {
			salmon_color = vec3(1.0, 1.0, 0.0);
		} else if (color_change == 3.0) {
			salmon_color = vec3(0.0, 0.0, 1.0);
		} else if (color_change == 4.0) {
			salmon_color = vec3(0.0, 1.0, 0.0);
		}
	}
	else {
		salmon_color = vec3(0.0, 0.0, 0.0);
	}
	color.xyz = salmon_color;
}