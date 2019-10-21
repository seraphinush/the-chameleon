// Header
#include "char.hpp"

// stlib
#include <string>
#include <algorithm>

Texture Char::char_texture;
using namespace std;

bool Char::init()
{
	// load shared texture
	if (!char_texture.is_valid())
	{
		if (!char_texture.load_from_file(textures_path("char.png")))
		{
			fprintf(stderr, "Failed to load char texture!");
			return false;
		}
	}

	// the position corresponds to the center of the texture
	float wr = char_texture.width * 0.5f;
	float hr = char_texture.height * 0.5f;

	TexturedVertex vertices[4];
	vertices[0].position = {-wr, +hr, -0.0f};
	vertices[0].texcoord = {0.f, 1.f};
	vertices[1].position = {+wr, +hr, -0.0f};
	vertices[1].texcoord = {1.f, 1.f};
	vertices[2].position = {+wr, -hr, -0.0f};
	vertices[2].texcoord = {1.f, 0.f};
	vertices[3].position = {-wr, -hr, -0.0f};
	vertices[3].texcoord = {0.f, 0.f};

	// counterclockwise as it's the default opengl front winding direction
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
	if (!effect.load_from_file(shader_path("char.vs.glsl"), shader_path("char.fs.glsl")))
		return false;

	motion.position = {600.f, 600.f};
	motion.radians = 0.f;
	motion.speed = 200.f;

	// set initial values, scale is negative to make it face the opposite way
	// 1.0 would be as big as the original texture.
	physics.scale = {-0.5f, 0.5f};

	m_is_alive = true;

	m_color_change = 0.0;
	m_direction_change = 0.0;

	// bound
	// TODO -- change to collision-base
	m_bound_up = false;
	m_bound_down = false;
	m_bound_left = false;
	m_bound_right = false;
	m_dash = false;

	return true;
}

// release all graphics resources
void Char::destroy()
{
	glDeleteBuffers(1, &mesh.vbo);
	glDeleteBuffers(1, &mesh.ibo);
	glDeleteBuffers(1, &mesh.vao);

	glDeleteShader(effect.vertex);
	glDeleteShader(effect.fragment);
	glDeleteShader(effect.program);
}

// update
void Char::update(float ms)
{
	float step = motion.speed * (ms / 1000);
	if (m_is_alive)
	{
		// find a bool in world and pass taht to char
		if (m_moving_right && !m_bound_right)
			move({step, 0.f});
		if (m_moving_left && !m_bound_left)
			move({-step, 0.f});
		if (m_moving_up && !m_bound_up)
			move({0.f, -step});
		if (m_moving_down && !m_bound_down)
			move({0.f, step});
	}
	else
	{
		// if dead, set upside down
		//set_rotation(-3.1415f);
	}
}

void Char::draw(const mat3 &projection)
{
	// transformation
	transform.begin();
	transform.translate(motion.position);
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

	// get uniform locations
	GLint transform_uloc = glGetUniformLocation(effect.program, "transform");
	GLint color_uloc = glGetUniformLocation(effect.program, "fcolor");
	GLint projection_uloc = glGetUniformLocation(effect.program, "projection");
	GLint color_change_uloc = glGetUniformLocation(effect.program, "color_change");
	GLint direction_change_uloc = glGetUniformLocation(effect.program, "direction_change");
	GLint is_alive_uloc = glGetUniformLocation(effect.program, "is_alive");

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
	glBindTexture(GL_TEXTURE_2D, char_texture.id);

	// set uniform values to the currently bound program
	glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float *)&transform.out);

	// color
	float color[] = {1.f, 1.f, 1.f};
	glUniform3fv(color_uloc, 1, color);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float *)&projection);

	float color_change = get_color_change();
	glUniform1f(color_change_uloc, color_change);

	float direction_change = get_direction_change();
	glUniform1f(direction_change_uloc, direction_change);

	glUniform1f(is_alive_uloc, is_alive());

	// draw
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
}

// collision
bool Char::collides_with(const Spotter &spotter)
{
	vec2 spotter_pos = spotter.get_position();
	vec2 spotter_box = spotter.get_bounding_box();

	bool collision_x_right = (motion.position.x + char_texture.width * 0.5f * physics.scale.x) >= (spotter_pos.x - spotter_box.x) &&
							 (spotter_pos.x + spotter_box.x) >= (motion.position.x + char_texture.width * 0.5f * physics.scale.x);
	bool collision_x_left = (motion.position.x - char_texture.width * 0.5f * physics.scale.x) >= (spotter_pos.x - spotter_box.x) &&
							(spotter_pos.x + spotter_box.x) >= (motion.position.x - char_texture.width * 0.5f * physics.scale.x);
	bool collision_y_right = (motion.position.y + char_texture.height * 0.5f * physics.scale.y) >= (spotter_pos.y - spotter_box.y) &&
							 (spotter_pos.y + spotter_box.y) >= (motion.position.y + char_texture.height * 0.5f * physics.scale.y);
	bool collision_y_left = (motion.position.y - char_texture.height * 0.5f * physics.scale.y) >= (spotter_pos.y - spotter_box.y) &&
							(spotter_pos.y + spotter_box.y) >= (motion.position.y - char_texture.height * 0.5f * physics.scale.y);

	return (collision_x_right || collision_x_left) && (collision_y_right || collision_y_left);
}

bool Char::collides_with(const Wanderer &wanderer)
{
	vec2 wanderer_pos = wanderer.get_position();
	vec2 wanderer_box = wanderer.get_bounding_box();

	bool collision_x_right = (motion.position.x + char_texture.width * 0.5f * physics.scale.x) >= (wanderer_pos.x - wanderer_box.x) &&
							 (wanderer_pos.x + wanderer_box.x) >= (motion.position.x + char_texture.width * 0.5f * physics.scale.x);
	bool collision_x_left = (motion.position.x - char_texture.width * 0.5f * physics.scale.x) >= (wanderer_pos.x - wanderer_box.x) &&
							(wanderer_pos.x + wanderer_box.x) >= (motion.position.x - char_texture.width * 0.5f * physics.scale.x);
	bool collision_y_right = (motion.position.y + char_texture.height * 0.5f * physics.scale.y) >= (wanderer_pos.y - wanderer_box.y) &&
							 (wanderer_pos.y + wanderer_box.y) >= (motion.position.y + char_texture.height * 0.5f * physics.scale.y);
	bool collision_y_left = (motion.position.y - char_texture.height * 0.5f * physics.scale.y) >= (wanderer_pos.y - wanderer_box.y) &&
							(wanderer_pos.y + wanderer_box.y) >= (motion.position.y - char_texture.height * 0.5f * physics.scale.y);

	return (collision_x_right || collision_x_left) && (collision_y_right || collision_y_left);
}

bool Char::collides_with(const Trophy &trophy)
{
	float dx = motion.position.x - trophy.get_position().x;
	float dy = motion.position.y - trophy.get_position().y;
	float d_sq = dx * dx + dy * dy;
	float other_r = std::max(trophy.get_bounding_box().x, trophy.get_bounding_box().y);
	float my_r = std::max(physics.scale.x, physics.scale.y);
	float r = std::max(other_r, my_r);
	r *= 0.6f;
	if (d_sq < r * r)
		return true;
	return false;
}

bool Char::collides_with_wall()
{
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

	for (int i = 0; i < 40; i++)
	{
		// Increment the row
		for (int j = 0; j < 61; j++)
		{
			if (level_1[i][j] == 'W')
			{

				bool collision_x_right = (motion.position.x + char_texture.width * 0.5f * physics.scale.x) >= (j * 20) &&
										 (j * 20 + 20) >= (motion.position.x + char_texture.width * 0.5f * physics.scale.x);
				bool collision_x_left = (motion.position.x - char_texture.width * 0.5f * physics.scale.x) >= (j * 20) &&
										(j * 20 + 20) >= (motion.position.x - char_texture.width * 0.5f * physics.scale.x);
				bool collision_y_right = (motion.position.y + char_texture.height * 0.5f * physics.scale.y) >= (i * 20) &&
										 (i * 20 + 20) >= (motion.position.y + char_texture.height * 0.5f * physics.scale.y);
				bool collision_y_left = (motion.position.y - char_texture.height * 0.5f * physics.scale.y) >= (i * 20) &&
										(i * 20 + 20) >= (motion.position.y - char_texture.height * 0.5f * physics.scale.y);

				if ((collision_x_right || collision_x_left) && (collision_y_right || collision_y_left))
					return true;
			}
		}
	}

	return false;
}

vec2 Char::get_position() const
{
	return motion.position;
}

void Char::move(vec2 offset)
{
	motion.position.x += offset.x;
	motion.position.y += offset.y;
}

void Char::set_rotation(float radians)
{
	motion.radians = radians;
}

void Char::set_direction(char direction, bool value)
{
	if (direction == 'R')
	{
		m_moving_right = value;
	}
	else if (direction == 'L')
	{
		m_moving_left = value;
	}
	else if (direction == 'U')
	{
		m_moving_up = value;
	}
	else if (direction == 'D')
	{
		m_moving_down = value;
	}
}

char Char::get_direction()
{
	if (m_moving_up)
	{
		return 'U';
	}
	else if (m_moving_down)
	{
		return 'D';
	}
	else if (m_moving_left)
	{
		return 'L';
	}
	else if (m_moving_right)
	{
		return 'R';
	}
}

// game mode
bool Char::get_mode() const
{
	return m_game_mode;
}

void Char::set_mode(bool value)
{
	m_game_mode = value;
}

// bound
// TODO -- change to collision-base
void Char::set_bound(char direction, bool state)
{
	switch (direction)
	{
	case 'R':
		m_bound_right = state;
		break;
	case 'L':
		m_bound_left = state;
		break;
	case 'U':
		m_bound_up = state;
		break;
	case 'D':
		m_bound_down = state;
		break;
	default:
		break;
	}
}

void Char::change_color(float c)
{
	// 1.0 = r; 2.0 = g; 3.0 = b; 4.0 = y;
	m_color_change = c;
}

float Char::get_color_change() const
{
	return m_color_change;
}

void Char::change_direction(float c)
{
	m_direction_change = c;
}

float Char::get_direction_change() const
{
	return m_direction_change;
}

bool Char::is_alive() const
{
	return m_is_alive;
}

void Char::kill()
{
	m_is_alive = false;
}

bool Char::is_win() const
{
	return m_is_win;
}

void Char::win()
{
	m_is_win = true;
}

void Char::dash()
{
	// fprintf(stderr, "moving - %c", m_direction);

	vec2 offset = {7.f, 0.f};
	if (m_direction == 'U')
	{
		fprintf(stderr, "moving up");
		offset = {0.f, 7.f};
	}
	else if (m_moving_down)
	{
		offset = {0.f, -7.f};
	}
	else if (m_direction == 'L')
	{
		fprintf(stderr, "moving left");
		offset = {7.f, 0.f};
	}
	else if (m_moving_right)
	{
		offset = {-7.f, 0.f};
	}
	motion.position.x += offset.x;
	motion.position.y += offset.y;
}

void Char::set_dash(bool value)
{
	m_dash = value;
}

bool Char::get_dash()
{
	return m_dash;
}