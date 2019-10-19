// Header
#include "char.hpp"

// internal
#include "spotter.hpp"
#include "wanderer.hpp"

// stlib
#include <string>
#include <algorithm>

bool Char::init()
{
	m_vertices.clear();
	m_indices.clear();

	// read the char mesh from a file, which contains a list of vertices and indices
	FILE *mesh_file = fopen(mesh_path("char.mesh"), "r");
	if (mesh_file == nullptr)
		return false;

	// read vertices and colors
	size_t num_vertices;
	fscanf(mesh_file, "%zu\n", &num_vertices);
	for (size_t i = 0; i < num_vertices; ++i)
	{
		float x, y, z;
		float _u[3]; // unused
		int r, g, b;
		fscanf(mesh_file, "%f %f %f %f %f %f %d %d %d\n", &x, &y, &z, _u, _u + 1, _u + 2, &r, &g, &b);
		Vertex vertex;
		vertex.position = {x, y, -z};
		vertex.color = {(float)r / 255, (float)g / 255, (float)b / 255};
		m_vertices.push_back(vertex);
	}

	// read associated indices
	size_t num_indices;
	fscanf(mesh_file, "%zu\n", &num_indices);
	for (size_t i = 0; i < num_indices; ++i)
	{
		int idx[3];
		fscanf(mesh_file, "%d %d %d\n", idx, idx + 1, idx + 2);
		m_indices.push_back((uint16_t)idx[0]);
		m_indices.push_back((uint16_t)idx[1]);
		m_indices.push_back((uint16_t)idx[2]);
	}

	// close
	fclose(mesh_file);

	// clear errors
	gl_flush_errors();

	// vertex buffer creation
	glGenBuffers(1, &mesh.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * m_vertices.size(), m_vertices.data(), GL_STATIC_DRAW);

	// index buffer creation
	glGenBuffers(1, &mesh.ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * m_indices.size(), m_indices.data(), GL_STATIC_DRAW);

	// vertex array (container for vertex + index buffer)
	glGenVertexArrays(1, &mesh.vao);
	if (gl_has_errors())
		return false;

	// load shaders
	if (!effect.load_from_file(shader_path("char.vs.glsl"), shader_path("char.fs.glsl")))
		return false;

	// set initial values
	motion.position = {600.f, 400.f};
	motion.radians = 0.f;
	motion.speed = 200.f;

	physics.scale = {-35.f, 35.f};

	m_is_alive = true;

	m_color_change = 0.0;
	m_direction_change = 0.0;

	// bound
	// TODO -- change to collision-base
	m_bound_up = false;
	m_bound_down = false;
	m_bound_left = false;
	m_bound_right = false;

	m_wall_collision = false;

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

		if (m_moving_right && m_wall_collision) {
			move({ -step * 5.f, 0.f });
		}
		if (m_moving_left && m_wall_collision) {
			move({ step * 5.f, 0.f });
		}
		if (m_moving_up && m_wall_collision) {
			move({ 0.f, step * 5.f });
		}
		if (m_moving_down && m_wall_collision) {
			move({ 0.f, -step * 5.f });
		}
		m_wall_collision = false;


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
		set_rotation(-3.1415f);
	}
}

void Char::draw(const mat3 &projection)
{
	// transformation
	transform.begin();

	transform.translate(get_position());
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
	GLint in_color_loc = glGetAttribLocation(effect.program, "in_color");
	glEnableVertexAttribArray(in_position_loc);
	glEnableVertexAttribArray(in_color_loc);
	glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
	glVertexAttribPointer(in_color_loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)sizeof(vec3));

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

	// get number of indices from buffer,
	// we know our vbo contains both colour and position information, so...
	GLint size = 0;
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	GLsizei num_indices = size / sizeof(uint16_t);

	// draw
	glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_SHORT, nullptr);
}

// collision
// Simple bounding box collision check
// This is a SUPER APPROXIMATE check that puts a circle around the bounding boxes and sees
// if the center point of either object is inside the other's bounding-box-circle. You don't
// need to try to use this technique.
bool Char::collides_with(const Spotter &spotter)
{
	float dx = motion.position.x - spotter.get_position().x;
	float dy = motion.position.y - spotter.get_position().y;
	float d_sq = dx * dx + dy * dy;
	float other_r = std::max(spotter.get_bounding_box().x, spotter.get_bounding_box().y);
	float my_r = std::max(physics.scale.x, physics.scale.y);
	float r = std::max(other_r, my_r);
	r *= 0.6f;
	if (d_sq < r * r)
		return true;
	return false;
}

bool Char::collides_with(const Wanderer &wanderer)
{
	float dx = motion.position.x - wanderer.get_position().x;
	float dy = motion.position.y - wanderer.get_position().y;
	float d_sq = dx * dx + dy * dy;
	float other_r = std::max(wanderer.get_bounding_box().x, wanderer.get_bounding_box().y);
	float my_r = std::max(physics.scale.x, physics.scale.y);
	float r = std::max(other_r, my_r);
	r *= 0.6f;
	if (d_sq < r * r)
		return true;
	return false;
}

vec2 Char::get_position() const
{
	return motion.position;
}

void Char::move(vec2 off)
{
	motion.position.x += off.x;
	motion.position.y += off.y;
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

void Char::set_wall_collision(bool c) {
	m_wall_collision = c;
}

bool Char::get_wall_collision() {
	return m_wall_collision;
}