// header
#include "wanderer.hpp"

#include <cmath>
#include <string> 
#include <iostream>

// texture
Texture Wanderer::wanderer_texture;
using namespace std;

bool Wanderer::init()
{
	// load shared texture
	if (!wanderer_texture.is_valid())
	{

		if (!wanderer_texture.load_from_file(textures_path("wanderers/1.png")))
		{
			fprintf(stderr, "Failed to load wanderer texture!");
			return false;
		}
	}

	// the position corresponds to the center of the texture
	float wr = wanderer_texture.width * 0.5f;
	float hr = wanderer_texture.height * 0.5f;

	TexturedVertex vertices[4];
	vertices[0].position = { -wr, +hr, -0.00f };
	vertices[0].texcoord = { 0.f, 1.f };
	vertices[1].position = { +wr, +hr, -0.00f };
	vertices[1].texcoord = { 1.f, 1.f };
	vertices[2].position = { +wr, -hr, -0.00f };
	vertices[2].texcoord = { 1.f, 0.f };
	vertices[3].position = { -wr, -hr, -0.00f };
	vertices[3].texcoord = { 0.f, 0.f };

	// counterclockwise as it's the default opengl front winding direction
	uint16_t indices[] = { 0, 3, 1, 1, 3, 2 };

	// clear errors
	gl_flush_errors();

	// vertex buffer creation
	glGenBuffers(1, &mesh.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TexturedVertex) * 4, vertices, GL_STATIC_DRAW);

	// index buffer creation
	glGenBuffers(1, &mesh.ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * 6, indices, GL_STATIC_DRAW);

	// vertex array (container for vertex + index buffer)
	glGenVertexArrays(1, &mesh.vao);
	if (gl_has_errors())
		return false;

	// load shaders
	if (!effect.load_from_file(shader_path("textured.vs.glsl"), shader_path("textured.fs.glsl")))
		return false;

	motion.radians = 0.f;
	motion.speed = 150.f;

	// set initial values, scale is negative to make it face the opposite way
	// 1.0 would be as big as the original texture.
	physics.scale = { config_scale, config_scale };

	return true;
}

// release all graphics resources
void Wanderer::destroy()
{
	glDeleteBuffers(1, &mesh.vbo);
	glDeleteBuffers(1, &mesh.ibo);
	glDeleteBuffers(1, &mesh.vao);

	glDeleteShader(effect.vertex);
	glDeleteShader(effect.fragment);
	glDeleteShader(effect.program);
}

void Wanderer::update(float ms)
{
	
	// movement
	float step_in_x = m_direction_wanderer.x * motion.speed * (ms / 1000);
	float step_in_y = m_direction_wanderer.y * motion.speed * (ms / 1000);
	motion.position.x += step_in_x;
	motion.position.y += step_in_y;

	if (wanderer_sprite_countdown > 0.f)
		wanderer_sprite_countdown -= ms;

	if (sprite_switch < 6) {
		sprite_switch++;
	}
	else {
		sprite_switch = 1;

	}
}

void Wanderer::draw(const mat3& projection)
{
	// transformation
	transform.begin();
	transform.translate(motion.position);
	transform.rotate(motion.radians);
	vec2 scale = { 0, 0 };
	scale.x = flip_in_x*4*physics.scale.x;
	scale.y = 4* physics.scale.y;
	transform.scale(scale);
	transform.end();

	// set shaders
	glUseProgram(effect.program);

	// enable alpha channel for textures
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // depth
	glDisable(GL_DEPTH_TEST);

	// get uniform locations for glUniform* calls
	GLint transform_uloc = glGetUniformLocation(effect.program, "transform");
	GLint color_uloc = glGetUniformLocation(effect.program, "fcolor");
	GLint projection_uloc = glGetUniformLocation(effect.program, "projection");

	// set vertices and indices
	glBindVertexArray(mesh.vao);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);

	// input data location as in the vertex buffer
	GLint in_position_loc = glGetAttribLocation(effect.program, "in_position");
	GLint in_texcoord_loc = glGetAttribLocation(effect.program, "in_texcoord");
	glEnableVertexAttribArray(in_position_loc);
	glEnableVertexAttribArray(in_texcoord_loc);
	glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)0);
	glVertexAttribPointer(in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)sizeof(vec3));

	if (wanderer_sprite_countdown < 0) {
		string temp_str = "data/textures/wanderers/" + to_string(sprite_switch) + ".png";
		string s(PROJECT_SOURCE_DIR);
		s += temp_str;
		const char* path = s.c_str();

		wanderer_texture.load_from_file(path);
		wanderer_sprite_countdown = 200.f;
	}

	// enable and binding texture to slot 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, wanderer_texture.id);

	// set uniform values to the currently bound program
	glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)& transform.out);
	float color[] = { 1.f, 1.f, 1.f };
	glUniform3fv(color_uloc, 1, color);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)& projection);

	// draw
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
}

vec2 Wanderer::get_position() const
{
	return motion.position;
}

void Wanderer::set_position(vec2 position)
{
	motion.position = position;
}
void Wanderer::set_rotation(float radians)
{
	motion.radians = radians;
}

// returns the local bounding coordinates scaled by the current size of the wanderer
// fabs is to avoid negative scale due to the facing direction.
vec2 Wanderer::get_bounding_box() const
{
	return { std::fabs(physics.scale.x) * wanderer_texture.width * 0.5f, std::fabs(physics.scale.y) * wanderer_texture.height * 0.5f };
}