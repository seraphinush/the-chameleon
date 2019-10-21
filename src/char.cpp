// Header
#include "char.hpp"

// stlib
#include <cmath>
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
	vertices[0].position = { -wr, +hr, -0.0f };
	vertices[0].texcoord = { 0.f, 1.f };
	vertices[1].position = { +wr, +hr, -0.0f };
	vertices[1].texcoord = { 1.f, 1.f };
	vertices[2].position = { +wr, -hr, -0.0f };
	vertices[2].texcoord = { 1.f, 0.f };
	vertices[3].position = { -wr, -hr, -0.0f };
	vertices[3].texcoord =  {0.f, 0.f };

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
	if (!effect.load_from_file(shader_path("char.vs.glsl"), shader_path("char.fs.glsl")))
		return false;

	motion.position = { 600.f, 600.f };
	motion.radians = 0.f;
	motion.speed = 200.f;

	physics.scale = { -config_scale, config_scale };

	// initial values
	m_is_alive = true;
	m_color_change = 0.0;
	m_direction_change = 0.0;

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
		fprintf(stderr, "%d %d %d %d\n", m_wall_right, m_wall_left, m_wall_up, m_wall_down);

		if (m_moving_right && !m_wall_right)
			move({step, 0.f});
		if (m_moving_left && !m_wall_left)
			move({-step, 0.f});
		if (m_moving_up && !m_wall_up)
			move({0.f, -step});
		if (m_moving_down && !m_wall_down)
			move({0.f, step});
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
	glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)0);
	glVertexAttribPointer(in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)sizeof(vec3));
 
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

// aabb-aabb collision
bool Char::collision(vec2 pos, vec2 box)
{
	float half_width = char_texture.width * 0.5f * std::fabs(physics.scale.x);
	float half_height = char_texture.height * 0.5f * std::fabs(physics.scale.y);

	bool collision_x_right = (motion.position.x + half_width) >= (pos.x - box.x) &&	(motion.position.x + half_width) <= (pos.x + box.x);
	bool collision_x_left = (motion.position.x - half_width) >= (pos.x - box.x) && (motion.position.x - half_width)	<= (pos.x + box.x);
	bool collision_y_top = (motion.position.y + half_height) >= (pos.y - box.y) && (motion.position.y + half_height) <= (pos.y + box.y);
	bool collision_y_down = (motion.position.y - half_height) >= (pos.y - box.y) && (motion.position.y - half_height) <= (pos.y + box.y);

	if ((motion.position.x + half_width) >= (pos.x + box.x) &&	(motion.position.x - half_width) <= (pos.x - box.x))
		return collision_y_top || collision_y_down;

	if ((motion.position.y + half_height) >= (pos.y + box.y) && (motion.position.y - half_height) <= (pos.y - box.y))
		return collision_x_right || collision_x_left;

	return (collision_x_right || collision_x_left) && (collision_y_top || collision_y_down);
}

bool Char::collides_with(const Spotter &spotter)
{
	vec2 pos = spotter.get_position();
	vec2 box = spotter.get_bounding_box();
	return collision(pos, box);
}

	bool Char::collides_with(const Wanderer &wanderer)
{
	vec2 pos = wanderer.get_position();
	vec2 box = wanderer.get_bounding_box();
	return collision(pos, box);
}

bool Char::collides_with(const Trophy &trophy)
{
	vec2 pos = trophy.get_position();
	vec2 box = trophy.get_bounding_box();
	return collision(pos, box);
}

vec2 Char::get_position() const
{
	return motion.position;
}

vec2 Char::get_bounding_box() const
{
	return { std::fabs(physics.scale.x) * char_texture.width * 0.5f, std::fabs(physics.scale.y) * char_texture.height * 0.5f };
}

void Char::set_wall_collision(char direction, bool value)
{
	if (direction == 'R')
	{
		if (value) fprintf(stderr, "setting wall collision !\n");
		m_wall_right = value;
	}
	else if (direction == 'L')
	{
		if (value) fprintf(stderr, "setting wall collision !\n");
		m_wall_left = value;
	}
	else if (direction == 'U')
	{
		if (value) fprintf(stderr, "setting wall collision !\n");
		m_wall_up = value;
	}
	else if (direction == 'D')
	{
		if (value) fprintf(stderr, "setting wall collision !\n");
		m_wall_down = value;
	}

	//if (value) fprintf(stderr, "%d %d %d %d\n", m_wall_right,m_wall_left,m_wall_up,m_wall_down);
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
		m_moving_right = value;
	else if (direction == 'L')
		m_moving_left = value;
	else if (direction == 'U')
		m_moving_up = value;
	else if (direction == 'D')
		m_moving_down = value;
}

void Char::change_color(float color)
{
	// 1.0 = r; 2.0 = g; 3.0 = b; 4.0 = y;
	m_color_change = color;
}

float Char::get_color_change() const
{
	return m_color_change;
}

void Char::change_direction(float direction)
{
	m_direction_change = direction;
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