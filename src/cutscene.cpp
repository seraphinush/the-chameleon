// header
#include "cutscene.hpp"

// stdlib
#include <algorithm>
#include <cmath>
#include <string>
#include <iostream>

Texture Cutscene::texture_dialogue_box;
Texture Cutscene::texture_dialogue_box_left;
Texture Cutscene::texture_dialogue_box_right;
Texture Cutscene::texture_dialogue_box_mid;
Texture Cutscene::dialogue_texture;
Texture Cutscene::left_dialogue_texture;
Texture Cutscene::right_dialogue_texture;
Texture Cutscene::enemy_texture;
Texture Cutscene::skip_texture;

using namespace std;

bool Cutscene::init()
{
	dialogue_counter = 1;
	current_cutscene_state = 4;

	// load shared texture
	if (!texture_dialogue_box.is_valid())
	{
		if (!texture_dialogue_box.load_from_file(textures_path("cutscenes/dialogue_box.png")))
		{
			fprintf(stderr, "Failed to load dialogue texture!");
			return false;
		}
	}

	if (!texture_dialogue_box_left.is_valid())
	{
		if (!texture_dialogue_box_left.load_from_file(textures_path("cutscenes/dialogue_box_face.png")))
		{
			fprintf(stderr, "Failed to load dialogue texture!");
			return false;
		}
	}

	if (!texture_dialogue_box_right.is_valid())
	{
		if (!texture_dialogue_box_right.load_from_file(textures_path("cutscenes/dialogue_box_face.png")))
		{
			fprintf(stderr, "Failed to load dialogue texture!");
			return false;
		}
	}

	if (!texture_dialogue_box_mid.is_valid())
	{
		if (!texture_dialogue_box_mid.load_from_file(textures_path("cutscenes/dialogue_placeholder.png")))
		{
			fprintf(stderr, "Failed to load dialogue texture!");
			return false;
		}
	}

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

	if (!enemy_texture.is_valid())
	{
		if (!enemy_texture.load_from_file(textures_path("wanderers/1.png")))
		{
			fprintf(stderr, "Failed to load enemy texture!");
			return false;
		}
	}

	if (!skip_texture.is_valid())
	{
		if (!skip_texture.load_from_file(textures_path("skip.png")))
		{
			fprintf(stderr, "Failed to load skip texture!");
			return false;
		}
	}

	// the position corresponds to the center of the texture
	float wr = std::max(SCREEN_WIDTH, SCREEN_HEIGHT) * 0.5f;
	float hr = std::max(SCREEN_WIDTH, SCREEN_HEIGHT) * 0.5f;

	TexturedVertex vertices[4];
	vertices[0].position = {-wr, +hr, 0.f};
	vertices[0].texcoord = {0.f, 1.f};
	vertices[1].position = {+wr, +hr, 0.f};
	vertices[1].texcoord = {1.f, 1.f};
	vertices[2].position = {+wr, -hr, 0.f};
	vertices[2].texcoord = {1.f, 0.f};
	vertices[3].position = {-wr, -hr, 0.f};
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

	left_dialogue_position = {350.f, 400.f};
	right_dialogue_position = {850.f, 400.f};

	left_texture_position = {200.f, 540.f};
	right_texture_position = {1000.f, 530.f};
	enemy_texture_position = {598.f, 530.f};

	physics.scale = vec2({1.f, 1.f});

	return true;
}

// release all graphics resources
void Cutscene::destroy()
{
	glDeleteBuffers(1, &mesh.vbo);
	glDeleteBuffers(1, &mesh.ibo);
	glDeleteVertexArrays(1, &mesh.vao);

	effect.release();
}

void Cutscene::update() {}

bool Cutscene::is_left_dialogue()
{
	for (int i = 0; i < 43; i++)
	{
		if (left_dialogues[i] == dialogue_counter)
			return true;
	}

	return false;
}

void Cutscene::draw(const mat3 &proj) {}

void Cutscene::draw(const mat3 &proj, vec2 wsize, const vec2 wpoint)
{
	float wscale = current_cutscene_state == LEVEL_TUTORIAL ? (float)SCREEN_WIDTH * ((float)SCREEN_WIDTH / wsize.x) : (float)SCREEN_WIDTH;
	vec2 d_face_trans_left = vec2({0.f,0.f});
	vec2 d_face_trans_right = vec2({0.f,0.f});
	vec2 d_box_trans = vec2({0.f,0.f});

	vec2 d_face_scale = vec2({(float)texture_dialogue_box_left.width / wscale, (float)texture_dialogue_box_left.height / wscale});
	vec2 d_box_scale = vec2({(float)texture_dialogue_box.width / wscale, (float)texture_dialogue_box.height / wscale});

	if (current_cutscene_state == LEVEL_TUTORIAL)
	{
		d_face_trans_left.x = wpoint.x + texture_dialogue_box_left.width * (d_face_scale.x * 3);
		d_face_trans_left.y = wpoint.y + texture_dialogue_box_left.height * (d_face_scale.y * 3); 
		d_face_trans_right.x = wpoint.x + wsize.x - texture_dialogue_box_left.width * (d_face_scale.x * 3);
		d_face_trans_right.y = wpoint.y + texture_dialogue_box_left.height * (d_face_scale.y * 3);

		d_box_trans.x = wpoint.x + texture_dialogue_box.width * (d_box_scale.x) / 2;
		d_box_trans.y = wpoint.y + texture_dialogue_box.height * (d_box_scale.y * 3);

		draw_element(proj, texture_dialogue_box_left, d_face_trans_left, d_face_scale);
		draw_element(proj, texture_dialogue_box_right, d_face_trans_right, d_face_scale);
		draw_element(proj, texture_dialogue_box, d_box_trans, d_box_scale);
	}
	else
	{
		d_face_trans_left = vec2({(float)(texture_dialogue_box_left.width / 2), (float)(texture_dialogue_box_left.height / 2)});
		d_face_trans_right = vec2({(float)(SCREEN_WIDTH - texture_dialogue_box_right.width / 2), (float)(texture_dialogue_box_right.height / 2)});

		d_box_trans = vec2({(float)(texture_dialogue_box.width / 2), (float)(texture_dialogue_box.height / 2)});

		draw_element(proj, texture_dialogue_box_left, d_face_trans_left, d_face_scale);
		draw_element(proj, texture_dialogue_box_right, d_face_trans_right, d_face_scale);
		draw_element(proj, texture_dialogue_box, d_box_trans, d_box_scale);
	}


	// OLD
	// dialogue
	/* 
	vec2 dialogue_pos = vec2({0.f, 0.f});
	vec2 dialogue_scale_temp = vec2({0.2f, 0.2f});
	if (current_cutscene_state != LEVEL_TUTORIAL)
	{
		dialogue_pos = is_left_dialogue() ? left_dialogue_position : right_dialogue_position;
	}
	else
	{
		dialogue_pos = is_left_dialogue() ? vec2({350.f, 620.f}) : vec2({850.f, 620.f});
	}
	draw_element(proj, dialogue_texture, dialogue_pos, physics.scale);

	// dialogue left
	if (dialogue_counter != 4 && dialogue_counter != 5 && current_cutscene_state == STORY_SCREEN)
	{
		draw_element(proj, dialogue_texture, left_texture_position, dialogue_scale_temp);
	}
	else if (current_cutscene_state != STORY_SCREEN)
	{
		draw_element(proj, dialogue_texture, left_texture_position, dialogue_scale_temp);
	}

	// dialogue right
	if ((dialogue_counter <= 13 || dialogue_counter >= 16) && current_cutscene_state == 4)
	{
		draw_element(proj, dialogue_texture, right_texture_position, dialogue_scale_temp);
	}
	else if (current_cutscene_state == LEVEL_TUTORIAL || current_cutscene_state == LEVEL_2_CUTSCENE)
	{
		draw_element(proj, dialogue_texture, right_texture_position, dialogue_scale_temp);
	}
	else if (current_cutscene_state == LEVEL_1_CUTSCENE && dialogue_counter <= 66)
	{
		draw_element(proj, dialogue_texture, right_texture_position, dialogue_scale_temp);
	}
	else if (current_cutscene_state == LEVEL_3_CUTSCENE && dialogue_counter >= 83)
	{
		draw_element(proj, dialogue_texture, right_texture_position, dialogue_scale_temp);
	}

	// enemy
	if (current_cutscene_state == LEVEL_1_CUTSCENE && dialogue_counter >= 58)
	{
		draw_element(proj, dialogue_texture, enemy_texture_position, dialogue_scale_temp);
	}
	else if (current_cutscene_state == LEVEL_2_CUTSCENE && dialogue_counter >= 76)
	{
		draw_element(proj, dialogue_texture, enemy_texture_position, dialogue_scale_temp);
	}
	else if (current_cutscene_state == LEVEL_3_CUTSCENE && dialogue_counter >= 81)
	{
		draw_element(proj, dialogue_texture, enemy_texture_position, dialogue_scale_temp);
	}

	// skip
	if (current_cutscene_state != LEVEL_TUTORIAL)
	{
		draw_element(proj, skip_texture, {830.f, 760.f}, dialogue_scale_temp);
	} */
}


void Cutscene::draw_element(const mat3& proj, const Texture& texture, vec2 pos, vec2 scale)
{
	// transformation
	transform.begin();
	transform.translate(pos);
	transform.scale(scale);
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
	glBindTexture(GL_TEXTURE_2D, texture.id);

	// set uniform values to the currently bound program
	glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float *)&transform.out);
	float color[] = {1.f, 1.f, 1.f};
	glUniform3fv(color_uloc, 1, color);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float *)&proj);

	// draw
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
}

bool Cutscene::dialogue_done(unsigned int cutscene_state)
{
	if (cutscene_state == 4 && dialogue_counter == 27)
		return true;
	else if (cutscene_state == LEVEL_TUTORIAL && dialogue_counter == 49)
		return true;
	else if (cutscene_state == LEVEL_1_CUTSCENE && dialogue_counter == 69)
		return true;
	else if (cutscene_state == LEVEL_2_CUTSCENE && dialogue_counter == 80)
		return true;
	else if (cutscene_state == LEVEL_3_CUTSCENE && dialogue_counter == 92)
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

		dialogue_texture.~Texture();
		dialogue_texture.load_from_file(path);

		if (dialogue_counter == 4)
		{
			left_dialogue_texture.~Texture();
			left_dialogue_texture.load_from_file(textures_path("piere_background_less.png"));
			left_scale = {0.1f, 1.f};
		}

		if (dialogue_counter == 4)
		{
			right_dialogue_texture.~Texture();
			right_dialogue_texture.load_from_file(textures_path("agent_roger.png"));
			right_scale = {-0.2f, 1.25f};
		}
		else if (dialogue_counter == 14)
		{
			right_dialogue_texture.~Texture();
			right_dialogue_texture.load_from_file(textures_path("intel.png"));
			right_scale = {0.1f, 0.8f};
		}

		left_texture_position = {200.f, 540.f};
		right_texture_position = {1000.f, 530.f};
	}
	else if (cutscene_state == LEVEL_TUTORIAL)
	{
		string temp_str = "data/textures/cutscenes/tutorial/" + to_string(dialogue_counter) + ".png";
		string s(PROJECT_SOURCE_DIR);
		s += temp_str;
		const char *path = s.c_str();

		dialogue_texture.~Texture();
		dialogue_texture.load_from_file(path);

		if (dialogue_counter == 28)
		{
			right_dialogue_texture.~Texture();
			right_dialogue_texture.load_from_file(textures_path("intel.png"));
			right_scale = {0.1f, 0.8f};
			left_dialogue_texture.~Texture();
			left_dialogue_texture.load_from_file(textures_path("piere_background_less.png"));
			left_scale = {0.1f, 1.f};
		}

		left_texture_position = {200.f, 740.f};
		right_texture_position = {1000.f, 730.f};
	}
	else if (cutscene_state == LEVEL_1_CUTSCENE)
	{
		string temp_str = "data/textures/cutscenes/level1/" + to_string(dialogue_counter) + ".png";
		string s(PROJECT_SOURCE_DIR);
		s += temp_str;
		const char *path = s.c_str();

		dialogue_texture.~Texture();
		dialogue_texture.load_from_file(path);

		if (dialogue_counter == 50)
		{
			right_dialogue_texture.~Texture();
			right_dialogue_texture.load_from_file(textures_path("intel.png"));
			right_scale = {0.1f, 0.8f};
			left_dialogue_texture.~Texture();
			left_dialogue_texture.load_from_file(textures_path("piere_background_less.png"));
			left_scale = {0.1f, 1.f};
			enemy_texture.~Texture();
			enemy_texture.load_from_file(textures_path("wanderers/1.png"));
			enemy_scale = {0.08f, 0.8f};
			skip_texture.~Texture();
			skip_texture.load_from_file(textures_path("skip.png"));
		}

		left_texture_position = {200.f, 540.f};
		right_texture_position = {1000.f, 530.f};
	}
	else if (cutscene_state == LEVEL_2_CUTSCENE)
	{
		string temp_str = "data/textures/cutscenes/level2/" + to_string(dialogue_counter) + ".png";
		string s(PROJECT_SOURCE_DIR);
		s += temp_str;
		const char *path = s.c_str();
		
		dialogue_texture.~Texture();
		dialogue_texture.load_from_file(path);

		if (dialogue_counter == 70)
		{
			right_dialogue_texture.~Texture();
			right_dialogue_texture.load_from_file(textures_path("intel.png"));
			right_scale = {0.1f, 0.8f};
			left_dialogue_texture.~Texture();
			left_dialogue_texture.load_from_file(textures_path("piere_background_less.png"));
			left_scale = {0.1f, 1.f};
			enemy_texture.~Texture();
			enemy_texture.load_from_file(textures_path("spotters/1.png"));
			enemy_scale = {0.08f, 0.7f};
			skip_texture.~Texture();
			skip_texture.load_from_file(textures_path("skip.png"));
		}

		left_texture_position = {200.f, 540.f};
		right_texture_position = {1000.f, 530.f};
	}
	else if (cutscene_state == LEVEL_3_CUTSCENE)
	{
		string temp_str = "data/textures/cutscenes/level3/" + to_string(dialogue_counter) + ".png";
		string s(PROJECT_SOURCE_DIR);
		s += temp_str;
		const char *path = s.c_str();

		dialogue_texture.~Texture();
		dialogue_texture.load_from_file(path);

		if (dialogue_counter == 81)
		{
			right_dialogue_texture.~Texture();
			right_dialogue_texture.load_from_file(textures_path("intel.png"));
			right_scale = {0.1f, 0.8f};
			left_dialogue_texture.~Texture();
			left_dialogue_texture.load_from_file(textures_path("piere_background_less.png"));
			left_scale = {0.1f, 1.f};
			enemy_texture.~Texture();
			enemy_texture.load_from_file(textures_path("survivor-idle_shotgun_0.png"));
			enemy_scale = {-0.09f, 0.9f};
			skip_texture.~Texture();
			skip_texture.load_from_file(textures_path("skip.png"));
		}

		left_texture_position = {200.f, 540.f};
		right_texture_position = {1000.f, 530.f};
	}
}

void Cutscene::set_dialogue_counter(unsigned int cutscene_state, unsigned int counter_value)
{
	dialogue_counter = counter_value;
	current_cutscene_state = cutscene_state;

	if (cutscene_state == 4 && counter_value == 1)
	{
		dialogue_texture.~Texture();
		dialogue_texture.load_from_file(textures_path("cutscenes/story/1.png"));
		left_dialogue_texture.~Texture();
		left_dialogue_texture.load_from_file(textures_path("spotters/3.png"));
		right_dialogue_texture.~Texture();
		right_dialogue_texture.load_from_file(textures_path("wanderers/1.png"));
		skip_texture.~Texture();
		skip_texture.load_from_file(textures_path("skip.png"));
		left_texture_position = {200.f, 540.f};
		right_texture_position = {1000.f, 530.f};
	}
	else if (cutscene_state == LEVEL_TUTORIAL && counter_value == 28)
	{
		dialogue_texture.~Texture();
		dialogue_texture.load_from_file(textures_path("cutscenes/tutorial/28.png"));
		right_dialogue_texture.~Texture();
		right_dialogue_texture.load_from_file(textures_path("intel.png"));
		right_scale = {0.1f, 0.8f};
		left_dialogue_texture.~Texture();
		left_dialogue_texture.load_from_file(textures_path("piere_background_less.png"));
		left_scale = {0.1f, 1.f};
		left_texture_position = {200.f, 740.f};
		right_texture_position = {1000.f, 730.f};
	}
	else if (cutscene_state == LEVEL_1_CUTSCENE && counter_value == 50)
	{
		dialogue_texture.~Texture();
		dialogue_texture.load_from_file(textures_path("cutscenes/level1/50.png"));
		right_dialogue_texture.~Texture();
		right_dialogue_texture.load_from_file(textures_path("intel.png"));
		right_scale = {0.1f, 0.8f};
		left_dialogue_texture.~Texture();
		left_dialogue_texture.load_from_file(textures_path("piere_background_less.png"));
		left_scale = {0.1f, 1.f};
		skip_texture.~Texture();
		skip_texture.load_from_file(textures_path("skip.png"));
		left_texture_position = {200.f, 540.f};
		right_texture_position = {1000.f, 530.f};
		enemy_texture.~Texture();
		enemy_texture.load_from_file(textures_path("wanderers/1.png"));
		enemy_scale = {0.08f, 0.8f};
	}
	else if (cutscene_state == LEVEL_2_CUTSCENE && counter_value == 70)
	{
		dialogue_texture.~Texture();
		dialogue_texture.load_from_file(textures_path("cutscenes/level2/70.png"));
		right_dialogue_texture.~Texture();
		right_dialogue_texture.load_from_file(textures_path("intel.png"));
		right_scale = {0.1f, 0.8f};
		left_dialogue_texture.~Texture();
		left_dialogue_texture.load_from_file(textures_path("piere_background_less.png"));
		left_scale = {0.1f, 1.f};
		skip_texture.~Texture();
		skip_texture.load_from_file(textures_path("skip.png"));
		left_texture_position = {200.f, 540.f};
		right_texture_position = {1000.f, 530.f};
		enemy_texture.~Texture();
		enemy_texture.load_from_file(textures_path("spotters/1.png"));
		enemy_scale = {0.08f, 0.7f};
	}
	else if (cutscene_state == LEVEL_3_CUTSCENE && counter_value == 81)
	{
		dialogue_texture.~Texture();
		dialogue_texture.load_from_file(textures_path("cutscenes/level3/81.png"));
		right_dialogue_texture.~Texture();
		right_dialogue_texture.load_from_file(textures_path("intel.png"));
		right_scale = {0.1f, 0.8f};
		left_dialogue_texture.~Texture();
		left_dialogue_texture.load_from_file(textures_path("piere_background_less.png"));
		left_scale = {0.1f, 1.f};
		skip_texture.~Texture();
		skip_texture.load_from_file(textures_path("skip.png"));
		left_texture_position = {200.f, 540.f};
		right_texture_position = {1000.f, 530.f};
		enemy_texture.~Texture();
		enemy_texture.load_from_file(textures_path("survivor-idle_shotgun_0.png"));
		enemy_scale = {-0.09f, 0.9f};
	}
}