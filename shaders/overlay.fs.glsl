#version 330

uniform sampler2D screen_texture;
uniform float time;
uniform float dead_timer;
uniform float alert_mode;
uniform float oscillation_value;

in vec2 uv;

layout(location = 0) out vec4 color;

void main()
{
	vec2 coord = uv;
	if (alert_mode == 1.0) {
		if (uv.y > 0.5) {
			color = vec4(0.8, 0.0, 0.0, uv.y - oscillation_value);
		}
		if (uv.y < 0.5) {
			color = vec4(0.8, 0.0, 0.0, (1.0 - uv.y) - oscillation_value);
		}
	} else {
		if (uv.y > 0.5) {
			color = vec4(0.0, 0.0, 0.0, uv.y - 0.1);
		}
		if (uv.y < 0.5) {
			color = vec4(0.0, 0.0, 0.0, (1.0 - uv.y) - 0.1);
		}
	}
}