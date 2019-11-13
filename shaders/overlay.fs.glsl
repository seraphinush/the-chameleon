#version 330

uniform sampler2D screen_texture;
uniform float time;
uniform float dead_timer;
uniform float alert_mode;
uniform float oscillation_value;
uniform int cooldown;
uniform int MAX_COOLDOWN;

uniform int window_width;
uniform int window_height;

in vec2 uv;

layout(location = 0) out vec4 color;


vec2 distort(vec2 uv) 
{
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// HANDLE THE WATER WAVE DISTORTION HERE (you may want to try sin/cos)
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	vec2 coord = uv.xy;
    return coord;
}
vec4 color_shift(vec4 in_color) 
{

	float width = window_width - 300 -100 ;
	float decrement = width/MAX_COOLDOWN; // Max cooldown set to 50
	float off = cooldown * decrement;
	vec4 color = in_color;
		if (gl_FragCoord.x < window_width - 300 - off && gl_FragCoord.x > 100 && gl_FragCoord.y > 50 && gl_FragCoord.y < 60)
			color = vec4(1.0, 0.0, 0.0, 0.5);
	return color;
}

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

	// red line

    // vec4 in_color = texture(screen_texture, coord);
    color = color_shift(color);
}