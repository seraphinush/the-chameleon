// header
#include "cutscene.hpp"
#include <cmath>
#include <string>
#include <iostream>

Texture Cutscene::dialogue_texture;
Texture Cutscene::left_dialogue_texture;
Texture Cutscene::right_dialogue_texture;

using namespace std;

bool Cutscene::init()
{
	dialogue_counter = 1;
	current_cutscene_state = 4;

	// load shared texture
	if (!dialogue_texture.is_valid())
	{
		if (!dialogue_texture.load_from_file(textures_path("cutscenes/story/1.png")))
		{
			fprintf(stderr, "Failed to load dialogue texture!");
			return false;
		}
	}

	if (!left_dialogue_texture.is_valid())
	{
		if (!left_dialogue_texture.load_from_file(textures_path("spotters/3.png")))
		{
			fprintf(stderr, "Failed to load dialogue texture!");
			return false;
		}
	}

	if (!right_dialogue_texture.is_valid())
	{
		if (!right_dialogue_texture.load_from_file(textures_path("wanderers/1.png")))
		{
			fprintf(stderr, "Failed to load dialogue texture!");
			return false;
		}
	}

	// the position corresponds to the center of the texture
	float wr = dialogue_texture.width;
	float hr = dialogue_texture.height;

	TexturedVertex vertices[4];
	vertices[0].position = {-wr, +hr, -0.0f};
	vertices[0].texcoord = {0.f, 1.f};
	vertices[1].position = {+wr, +hr, -0.02f};
	vertices[1].texcoord = {1.f, 1.f};
	vertices[2].position = {+wr, -hr, -0.02f};
	vertices[2].texcoord = {1.f, 0.f};
	vertices[3].position = {-wr, -hr, -0.02f};
	vertices[3].texcoord = {0.f, 0.f};

	uint16_t indices[] = {0, 3, 1, 1, 3, 2};

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
	motion.speed = 200.f;

	left_dialogue_position = vec2({350.f, 400.f});
	right_dialogue_position = vec2({850.f, 400.f});

	// set initial values, scale is negative to make it face the opposite way
	// 1.0 would be as big as the original texture.
	physics.scale = {0.75f, 0.95f};

	return true;
}

// release all graphics resources
void Cutscene::destroy()
{
	glDeleteBuffers(1, &mesh.vbo);
	glDeleteBuffers(1, &mesh.ibo);
	glDeleteBuffers(1, &mesh.vao);

	glDeleteShader(effect.vertex);
	glDeleteShader(effect.fragment);
	glDeleteShader(effect.program);
}

void Cutscene::update(unsigned int game_state)
{
}

bool Cutscene::is_left_dialogue()
{
	for (int i = 0; i < 12; i++)
	{
		if (left_dialogues[i] == dialogue_counter)
			return true;
	}

	return false;
}

void Cutscene::draw(const mat3 &projection)
{
	// Dialogues
	// transformation
	transform.begin();

	if (current_cutscene_state == 4)
	{
		if (is_left_dialogue())
			transform.translate(left_dialogue_position);
		else
			transform.translate(right_dialogue_position);
	}
	else if (current_cutscene_state == 6000)
	{
		transform.translate(motion.position);
	}

	transform.rotate(motion.radians);
	transform.scale(physics.scale);
	transform.end();

	// set shaders
	glUseProgram(effect.program);

	// enable alpha channel for textures
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// depth
	glEnable(GL_DEPTH_TEST);

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
	glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void *)0);
	glVertexAttribPointer(in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void *)sizeof(vec3));

	// enable and binding texture to slot 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, dialogue_texture.id);

	// set uniform values to the currently bound program
	glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float *)&transform.out);
	float color[] = {1.f, 1.f, 1.f};
	glUniform3fv(color_uloc, 1, color);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float *)&projection);

	// draw
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);

	// Left Dialogue Texture
	transform.begin();
	transform.translate(vec2({200.f, 540.f}));
	transform.rotate(motion.radians);
	transform.scale(left_scale);
	transform.end();

	// set shaders
	glUseProgram(effect.program);

	// enable alpha channel for textures
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// depth
	glEnable(GL_DEPTH_TEST);

	// get uniform locations for glUniform* calls
	transform_uloc = glGetUniformLocation(effect.program, "transform");
	color_uloc = glGetUniformLocation(effect.program, "fcolor");
	projection_uloc = glGetUniformLocation(effect.program, "projection");

	// set vertices and indices
	glBindVertexArray(mesh.vao);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);

	// input data location as in the vertex buffer
	in_position_loc = glGetAttribLocation(effect.program, "in_position");
	in_texcoord_loc = glGetAttribLocation(effect.program, "in_texcoord");
	glEnableVertexAttribArray(in_position_loc);
	glEnableVertexAttribArray(in_texcoord_loc);
	glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void *)0);
	glVertexAttribPointer(in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void *)sizeof(vec3));

	// enable and binding texture to slot 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, left_dialogue_texture.id);

	// set uniform values to the currently bound program
	glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float *)&transform.out);
	glUniform3fv(color_uloc, 1, color);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float *)&projection);

	// draw
	if (dialogue_counter != 4 && dialogue_counter != 5 && current_cutscene_state == 4)
	{
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
	}

	// Right Dialogue Texture
	transform.begin();
	transform.translate(vec2({1000.f, 540.f}));
	transform.rotate(motion.radians);
	transform.scale(right_scale);
	transform.end();

	// set shaders
	glUseProgram(effect.program);

	// enable alpha channel for textures
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// depth
	glEnable(GL_DEPTH_TEST);

	// get uniform locations for glUniform* calls
	transform_uloc = glGetUniformLocation(effect.program, "transform");
	color_uloc = glGetUniformLocation(effect.program, "fcolor");
	projection_uloc = glGetUniformLocation(effect.program, "projection");

	// set vertices and indices
	glBindVertexArray(mesh.vao);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);

	// input data location as in the vertex buffer
	in_position_loc = glGetAttribLocation(effect.program, "in_position");
	in_texcoord_loc = glGetAttribLocation(effect.program, "in_texcoord");
	glEnableVertexAttribArray(in_position_loc);
	glEnableVertexAttribArray(in_texcoord_loc);
	glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void *)0);
	glVertexAttribPointer(in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void *)sizeof(vec3));

	// enable and binding texture to slot 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, right_dialogue_texture.id);

	// set uniform values to the currently bound program
	glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float *)&transform.out);
	glUniform3fv(color_uloc, 1, color);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float *)&projection);

	// draw
	if ((dialogue_counter <= 13 || dialogue_counter >= 16) && current_cutscene_state == 4)
	{
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
	}
}

bool Cutscene::dialogue_done(unsigned int cutscene_state)
{
	if (cutscene_state == 4 && dialogue_counter == 27)
		return true;
	else if (cutscene_state == 6000 && dialogue_counter == 29)
		return true;
	else if (cutscene_state == 1500 && dialogue_counter == 31)
		return true;
	else if (cutscene_state == 2500 && dialogue_counter == 33)
		return true;
	else if (cutscene_state == 3500 && dialogue_counter == 35)
		return true;

	return false;
}

void Cutscene::increment_dialogue_counter(unsigned int cutscene_state)
{
	dialogue_counter++;
	current_cutscene_state = cutscene_state;

	if (cutscene_state == 4)
	{
		string temp_str = "data/textures/cutscenes/story/" + to_string(dialogue_counter) + ".png";
		string s(PROJECT_SOURCE_DIR);
		s += temp_str;
		const char *path = s.c_str();

		dialogue_texture.load_from_file(path);

		if (dialogue_counter > 3)
		{
			left_dialogue_texture.load_from_file(textures_path("char.png"));
			left_scale = {0.08f, 0.6f};
		}

		if (dialogue_counter > 3 && dialogue_counter < 14)
		{
			right_dialogue_texture.load_from_file(textures_path("char.png"));
			right_scale = {0.08f, 0.6f};
		}
		else if (dialogue_counter > 13)
		{
			right_dialogue_texture.load_from_file(textures_path("intel.png"));
			right_scale = {0.1f, 0.8f};
		}
	}
	else if (cutscene_state == 6000)
	{
		string temp_str = "data/textures/cutscenes/tutorial/" + to_string(dialogue_counter) + ".png";
		string s(PROJECT_SOURCE_DIR);
		s += temp_str;
		const char *path = s.c_str();

		dialogue_texture.load_from_file(path);
		physics.scale = {0.9f, 0.8f};
		motion.position = {400.f, 700.f};
	}
	else if (cutscene_state == 1500)
	{
		string temp_str = "data/textures/cutscenes/level1/" + to_string(dialogue_counter) + ".png";
		string s(PROJECT_SOURCE_DIR);
		s += temp_str;
		const char *path = s.c_str();

		dialogue_texture.load_from_file(path);
		physics.scale = {0.9f, 0.8f};
		motion.position = {350.f, 400.f};
	}
	else if (cutscene_state == 2500)
	{
		string temp_str = "data/textures/cutscenes/level2/" + to_string(dialogue_counter) + ".png";
		string s(PROJECT_SOURCE_DIR);
		s += temp_str;
		const char *path = s.c_str();

		dialogue_texture.load_from_file(path);
		physics.scale = {0.9f, 0.8f};
		motion.position = {350.f, 400.f};
	}
	else if (cutscene_state == 3500)
	{
		string temp_str = "data/textures/cutscenes/level3/" + to_string(dialogue_counter) + ".png";
		string s(PROJECT_SOURCE_DIR);
		s += temp_str;
		const char *path = s.c_str();

		dialogue_texture.load_from_file(path);
		physics.scale = {0.9f, 0.8f};
		motion.position = {350.f, 400.f};
	}
}

void Cutscene::set_dialogue_counter(unsigned int cutscene_state, unsigned int counter_value)
{
	dialogue_counter = counter_value;
	current_cutscene_state = cutscene_state;

	if (cutscene_state == 6000)
	{
		dialogue_texture.load_from_file(textures_path("cutscenes/tutorial/28.png"));
		physics.scale = {0.9f, 0.8f};
		motion.position = {400.f, 700.f};
	}
	else if (cutscene_state == 1500)
	{
		dialogue_texture.load_from_file(textures_path("cutscenes/level1/30.png"));
		physics.scale = {0.9f, 0.8f};
		motion.position = {350.f, 400.f};
	}
	else if (cutscene_state == 2500)
	{
		dialogue_texture.load_from_file(textures_path("cutscenes/tutorial/32.png"));
		physics.scale = {0.9f, 0.8f};
		motion.position = {350.f, 400.f};
	}
	else if (cutscene_state == 3500)
	{
		dialogue_texture.load_from_file(textures_path("cutscenes/tutorial/34.png"));
		physics.scale = {0.9f, 0.8f};
		motion.position = {350.f, 400.f};
	}
}