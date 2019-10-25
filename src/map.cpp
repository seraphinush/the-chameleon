// header
#include "map.hpp"

// stlib
#include <cmath>
#include <iostream>

Texture Map::wall_texture;
Texture Map::corridor_texture;

Texture Map::corridor_texture_red;
Texture Map::corridor_texture_blue;
Texture Map::corridor_texture_green;
Texture Map::corridor_texture_yellow;

// 800 * 1200 Map of Level 1
// 31 for the \n of all chars
char level_1[40][61] = {
	"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW",
	"WCCCCCGGGGGGGGGGGGGGGGGGGGYYYYYRRRRRRRRRRRCCCCCCCCCCCCCCCCCW",
	"WCCCCCGGGGGGGGGGGGGGGGGGGGYYYYYRRRRRRRRRRRCCCCCCCCCCCCCCCCCW",
	"WCCCCCGGGGGGGGGGGGGGGGGGGGYYYYYRRRRRRRRRRRCCCCCCCCCCCCCCCCCW",
	"WCCCCCGGGGGGGGGGGGGGGGGGGGYYYYYRRRRRRRRRRRCCCCCCCCCCCCCCCCCW",
	"WCCCCCWWWWWWWWWWWWWWWWWWWWCCCCCWWWWWWWWWWWBBBBBWWWWWWWGGGGGW",
	"WCCCCCWWWWWWWWWWWWWWWWWWWWCCCCCWWWWWWWWWWWBBBBBWWWWWWWGGGGGW",
	"WCCCCCWWWWWWWWWWWWWWWWWWWWCCCCCWWWWWWWWWWWBBBBBWWWWWWWGGGGGW",
	"WCCCCCWWWWWWWWWWWWWWWWWWWWCCCCCWWWWWWWWWWWBBBBBWWWWWWWGGGGGW",
	"WCCCCCWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWBBBBBWWWWWWWGGGGGW",
	"WCCCCCWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWBBBBBWWWWWWWGGGGGW",
	"WCCCCCWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWBBBBBWWWWWWWGGGGGW",
	"WCCCCCWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWBBBBBWWWWWWWGGGGGW",
	"WCCCCCWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWBBBBBWWWWWWWGGGGGW",
	"WYYYYYBBBBBBBBBBBBBBBBWWWWWWWWWWWWWCCCCCCCCCCCCCCCCCCCCCCCCW",
	"WYYYYYBBBBBBBBBBBBBBBBWWWWWWWWWWWWWCCCCCCCCCCCCCCCCCCCCCCCCW",
	"WYYYYYBBBBBBBBBBBBBBBBWWWWWWWWWWWWWCCCCCCCCCCCCCCCCCCCCCCCCW",
	"WYYYYYWWWWWWWWWWWWWBBBWWWWWWWWWWWWWCCCCCCCCCCCCCCCCCCCCCCCCW",
	"WYYYYYWWWWWWWWWWWWWBBBWWWWWWWWWWWWWCCCCCCCCCCCCCCCCCCCCCCCCW",
	"WYYYYYWWWWWWWWWWWWWBBBWWWWWWWWWWWWWCCCCCCCCCCCCCCCCWWWRRRRRW",
	"WYYYYYWWWWWWWWWWWWWBBBWWWWWWWWWWWWWCCCCCCCCCCCCCCCCWWWRRRRRW",
	"WYYYYYWWWWWWWWWWWWWBBBWWWWWWWWWWWWWCCCCCCCCCCCCCCCCWWWRRRRRW",
	"WYYYYYWWWWWWWWWWWWWBBBBBCCCCCCCCCCCCCCCCCCCCCCCCCCCWWWRRRRRW",
	"WYYYYYWWWWWWWWWWWWWBBBBBCCCCCCCCCCCCCCCCCCCCCCCCCCCWWWRRRRRW",
	"WYYYYYWWWWWWWWWWWWWBBBBBCCCCCCCCCCCCCCCCCCCCCCCCCCCWWWRRRRRW",
	"WCCCCCWWWWWWWWWWWWWWWWWWCCCCCCCCCCCCCCCCCCCCCCCCCCCWWWRRRRRW",
	"WCCCCCWWWWWWWWWWWWWWWWWWCCCCCCCCCCCCCCCCCCCCCCCCCCCWWWRRRRRW",
	"WCCCCCWWWWWWWWWWWWWWWWWWCCCCCCCCCCCCCCCCCCCCCCCCCCCWWWRRRRRW",
	"WCCCCCWWWWWWWWWWWWWWWWWWCCCCCCCCCCCCCCCCCCCCCCCCCCCWWWRRRRRW",
	"WCCCCCWWWWWWWWWWWWWWWWWWCCCCCCCCCCCCCCCCCCCCCCCCCCCWWWRRRRRW",
	"WCCCCCWWWWWWWWWWWWWWWWWWCCCCCCCCCCCCCCCCCCCCCCCCCCCWWWRRRRRW",
	"WCCCCCWWWWWWWWWWWWWWWWWWGGGGGGGGGGGWWWWWWWWWWWWWWWWWWWRRRRRW",
	"WCCCCCWWWWWWWWWWWWWWWWWWGGGGGGGGGGGWWWWWWWWWWWWWWWWWWWRRRRRW",
	"WCCCCCWWWWWWWWWWWWWWWWWWGGGGGGGGGGGWWWWWWWWWWWWWWWWWWWRRRRRW",
	"WCCCCCRRRRRRRRRRRRRRRRRRCCCCCCCCCCCYYYYYYYYYYYYYYYYYYYCCCCCW",
	"WCCCCCRRRRRRRRRRRRRRRRRRCCCCCCCCCCCYYYYYYYYYYYYYYYYYYYCCCCCW",
	"WCCCCCRRRRRRRRRRRRRRRRRRCCCCCCCCCCCYYYYYYYYYYYYYYYYYYYCCCCCW",
	"WCCCCCRRRRRRRRRRRRRRRRRRCCCCCCCCCCCYYYYYYYYYYYYYYYYYYYCCCCCW",
	"WCCCCCRRRRRRRRRRRRRRRRRRCCCCCCCCCCCYYYYYYYYYYYYYYYYYYYCCCCCW",
	"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW"};

bool Map::init()
{
	m_dead_time = -1;

	// load shared texture
	if (!wall_texture.is_valid())
	{
		if (!wall_texture.load_from_file(textures_path("wall_tile.png")))
		{
			fprintf(stderr, "Failed to load wall texture!");
			return false;
		}
	}

	if (!corridor_texture.is_valid())
	{
		if (!corridor_texture.load_from_file(textures_path("corridor_tile.png")))
		{
			fprintf(stderr, "Failed to load corridor texture!");
			return false;
		}
	}

	if (!corridor_texture_red.is_valid())
	{
		if (!corridor_texture_red.load_from_file(textures_path("corridor_tile_red.png")))
		{
			fprintf(stderr, "Failed to load corridor texture!");
			return false;
		}
	}

	if (!corridor_texture_blue.is_valid())
	{
		if (!corridor_texture_blue.load_from_file(textures_path("corridor_tile_blue.png")))
		{
			fprintf(stderr, "Failed to load corridor texture!");
			return false;
		}
	}

	if (!corridor_texture_green.is_valid())
	{
		if (!corridor_texture_green.load_from_file(textures_path("corridor_tile_green.png")))
		{
			fprintf(stderr, "Failed to load corridor texture!");
			return false;
		}
	}

	if (!corridor_texture_yellow.is_valid())
	{
		if (!corridor_texture_yellow.load_from_file(textures_path("corridor_tile_yellow.png")))
		{
			fprintf(stderr, "Failed to load corridor texture!");
			return false;
		}
	}

	// vertex buffer in local coordinates
	TexturedVertex vertices[4];
	vertices[0].position = {0.f, 20.f, 0.f}; // top left
	vertices[0].texcoord = {0.f, 1.f};
	vertices[1].position = {20.f, 20.f, 0.f}; // top right
	vertices[1].texcoord = {1.f, 1.f};
	vertices[2].position = {20.f, 0.f, 0.f}; // bottom right
	vertices[2].texcoord = {1.f, 0.f};
	vertices[3].position = {0.f, 0.f, 0.f}; // bottom left
	vertices[3].texcoord = {0.f, 0.f};

	// counterclockwise as it's the default opengl front winding direction
	uint16_t indices[] = {0, 3, 1, 1, 3, 2};

	// clea errors
	gl_flush_errors();

	// vertex buffer creation
	glGenBuffers(1, &mesh.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TexturedVertex) * 4, vertices, GL_STATIC_DRAW);

	// index buffer creation
	glGenBuffers(1, &mesh.ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * 6, indices, GL_STATIC_DRAW);

	// vertex array (Container for Vertex + Index buffer)
	glGenVertexArrays(1, &mesh.vao);

	// clear errors
	gl_flush_errors();

	if (gl_has_errors())
		return false;

	// load shaders
	if (!effect.load_from_file(shader_path("map.vs.glsl"), shader_path("map.fs.glsl")))
		return false;

	physics.scale = {1.0f, 1.0f};

	return true;
}

// release all graphics resources
void Map::destroy()
{
	glDeleteBuffers(1, &mesh.vbo);
	glDeleteBuffers(1, &mesh.ibo);
	glDeleteBuffers(1, &mesh.vao);

	glDeleteShader(effect.vertex);
	glDeleteShader(effect.fragment);
	glDeleteShader(effect.program);
}

void Map::draw(const mat3 &projection)
{

	translation_tile = vec2({0.0, 0.0});

	for (int i = 0; i < 40; i++)
	{
		// Increment the row
		for (int j = 0; j < 61; j++)
		{
			if (level_1[i][j] == 'W')
			{
				// Draw a Wall
				draw_element(projection, wall_texture);
			}
			else if (level_1[i][j] == 'C')
			{
				// Draw a Corridor
				draw_element(projection, corridor_texture);
			}
			else if (level_1[i][j] == 'R')
			{
				// Draw a Corridor
				draw_element(projection, corridor_texture_red);
			}
			else if (level_1[i][j] == 'B')
			{
				// Draw a Corridor
				draw_element(projection, corridor_texture_blue);
			}
			else if (level_1[i][j] == 'G')
			{
				// Draw a Corridor
				draw_element(projection, corridor_texture_green);
			}
			else if (level_1[i][j] == 'Y')
			{
				// Draw a Corridor
				draw_element(projection, corridor_texture_yellow);
			}

			translation_tile.x += 20.0;
		}
		translation_tile.x = 0.0;
		translation_tile.y += 20.0;
	}
}

void Map::draw_element(const mat3 &projection, const Texture &texture)
{
	// transformation
	transform.begin();
	transform.translate(translation_tile);
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
	GLint flash_map_uloc = glGetUniformLocation(effect.program, "flash_map");
	GLuint flash_timer_uloc = glGetUniformLocation(effect.program, "flash_timer");

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
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float *)&projection);
	glUniform1iv(flash_map_uloc, 1, &flash_map);
	glUniform1f(flash_timer_uloc, (m_flash_time > 0) ? (float)((glfwGetTime() - m_flash_time) * 10.0f) : -1);

	// draw
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
}

vec2 Map::get_position() const
{
	return motion.position;
}

void Map::set_position(vec2 position)
{
	motion.position = position;
}

void Map::is_wall_collision(Char &character)
{
	vec2 pos = character.get_position();
	vec2 box = character.get_bounding_box();

	// get 4 corners of char: top left, top right, bottom left, bottom right
	vec2 pos_top_left = {pos.x - box.x, pos.y - box.y};
	vec2 pos_top_right = {pos.x + box.x, pos.y - box.y};
	vec2 pos_bottom_left = {pos.x - box.x, pos.y + box.y};
	vec2 pos_bottom_right = {pos.x + box.x, pos.y + box.y};

	// top left
	int tile_x_top_left = (int)pos_top_left.x / 20;
	int tile_y_top_left = (int)pos_top_left.y / 20;
	int tile_x_top_right = (int)pos_top_right.x / 20;
	int tile_y_top_right = (int)pos_top_right.y / 20;
	int tile_x_bottom_left = (int)pos_bottom_left.x / 20;
	int tile_y_bottom_left = (int)pos_bottom_left.y / 20;
	int tile_x_bottom_right = (int)pos_bottom_right.x / 20;
	int tile_y_bottom_right = (int)pos_bottom_right.y / 20;

	if (level_1[tile_y_top_left][tile_x_top_left] == 'W' && level_1[tile_y_top_right][tile_x_top_right] == 'W')
	{
		character.set_wall_collision('U', true);
	}
	else
	{
		character.set_wall_collision('U', false);
	}

	if (level_1[tile_y_top_right][tile_x_top_right] == 'W' && level_1[tile_y_bottom_right][tile_x_bottom_right] == 'W')
	{
		character.set_wall_collision('R', true);
	}
	else
	{
		character.set_wall_collision('R', false);
	}

	if (level_1[tile_y_bottom_right][tile_x_bottom_right] == 'W' && level_1[tile_y_bottom_left][tile_x_bottom_left] == 'W')
	{
		character.set_wall_collision('D', true);
	}
	else
	{
		character.set_wall_collision('D', false);
	}

	if (level_1[tile_y_bottom_left][tile_x_bottom_left] == 'W' && level_1[tile_y_top_left][tile_x_top_left] == 'W')
	{
		character.set_wall_collision('L', true);
	}
	else
	{
		character.set_wall_collision('L', false);
	}
}

// TO REMOVE - placeholder for randomize path wall collision
void Map::is_wall_collision(Wanderer &wanderer)
{
	vec2 pos = wanderer.get_position();
	vec2 box = wanderer.get_bounding_box();

	// get 4 corners of wanderer: top left, top right, bottom left, bottom right
	vec2 pos_top_left = {pos.x - box.x, pos.y - box.y};
	vec2 pos_top_right = {pos.x + box.x, pos.y - box.y};
	vec2 pos_bottom_left = {pos.x - box.x, pos.y + box.y};
	vec2 pos_bottom_right = {pos.x + box.x, pos.y + box.y};

	int tile_x;
	int tile_y;

	// top left
	int tile_x_top_left = (int)pos_top_left.x / 20;
	int tile_y_top_left = (int)pos_top_left.y / 20;
	int tile_x_top_right = (int)pos_top_right.x / 20;
	int tile_y_top_right = (int)pos_top_right.y / 20;
	int tile_x_bottom_left = (int)pos_bottom_left.x / 20;
	int tile_y_bottom_left = (int)pos_bottom_left.y / 20;
	int tile_x_bottom_right = (int)pos_bottom_right.x / 20;
	int tile_y_bottom_right = (int)pos_bottom_right.y / 20;

	if (level_1[tile_y_top_left][tile_x_top_left] == 'W' && level_1[tile_y_top_right][tile_x_top_right] == 'W')
	{
		wanderer.set_wall_collision('U', true);
	}
	else
	{
		wanderer.set_wall_collision('U', false);
	}

	if (level_1[tile_y_top_right][tile_x_top_right] == 'W' && level_1[tile_y_bottom_right][tile_x_bottom_right] == 'W')
	{
		wanderer.set_wall_collision('R', true);
	}
	else
	{
		wanderer.set_wall_collision('R', false);
	}

	if (level_1[tile_y_bottom_right][tile_x_bottom_right] == 'W' && level_1[tile_y_bottom_left][tile_x_bottom_left] == 'W')
	{
		wanderer.set_wall_collision('D', true);
	}
	else
	{
		wanderer.set_wall_collision('D', false);
	}

	if (level_1[tile_y_bottom_left][tile_x_bottom_left] == 'W' && level_1[tile_y_top_left][tile_x_top_left] == 'W')
	{
		wanderer.set_wall_collision('L', true);
	}
	else
	{
		wanderer.set_wall_collision('L', false);
	}
}

int Map::get_tile(Char character)
{
	vec2 pos = character.get_position();
	int x = (int)pos.x / 20;
	int y = (int)pos.y / 20;

	if (level_1[y][x] == 'W') return 1;
	else if (level_1[y][x] == 'C') return 6;
	else if (level_1[y][x] == 'R') return 2;
	else if (level_1[y][x] == 'G') return 3;
	else if (level_1[y][x] == 'B') return 4;
	else if (level_1[y][x] == 'Y') return 5;
	else return 0;
}

void Map::set_char_dead()
{
	m_dead_time = glfwGetTime();
}

void Map::reset_char_dead_time()
{
	m_dead_time = -1;
}

float Map::get_char_dead_time() const
{
	return glfwGetTime() - m_dead_time;
}

void Map::set_flash(int value)
{
	flash_map = value;
	m_flash_time = glfwGetTime();
}

void Map::reset_flash_time()
{
	m_flash_time = glfwGetTime();
}

float Map::get_flash_time() const
{
	return glfwGetTime() - m_flash_time;
}