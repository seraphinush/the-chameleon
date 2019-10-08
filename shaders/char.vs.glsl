#version 330 

// Input attributes
in vec3 in_position;
in vec3 in_color;

out vec3 vcolor;
out vec2 vpos;

// Application data
uniform mat3 transform;
uniform mat3 projection;
uniform float direction_change;
uniform bool is_alive;

void main()
{
	vpos = in_position.xy;
	vcolor = in_color;
	float cs = cos(1.5708);
	float sn = sin(1.5708);
	if (is_alive) {
		if(direction_change == 0.0){
			vpos = in_position.xy;
		} else if(direction_change == 1.0){
			vpos = vec2(-in_position.x, in_position.y);
		} else if(direction_change == 2.0){
			vpos = vec2(in_position.x*cs - in_position.y*sn, in_position.x*sn + in_position.y*cs);
		} else if(direction_change == 3.0){
			vpos = -vec2(in_position.x*cs - in_position.y*sn, in_position.x*sn + in_position.y*cs);
		}
	}
	vec3 pos = projection * transform * vec3(vpos, 1.0);
	gl_Position = vec4(pos.xy, 0.01, 1.0);
}