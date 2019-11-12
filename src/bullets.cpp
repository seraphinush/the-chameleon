#define _USE_MATH_DEFINES

// header
#include "bullets.hpp"

#include <cmath>
#include <iostream>

static const int MAX_BULLETS = 25;
constexpr int NUM_SEGMENTS = 12;

using namespace std;

bool Bullets::init()
{
	std::vector<GLfloat> screen_vertex_buffer_data;
	constexpr float z = -0.1;

	for (int i = 0; i < NUM_SEGMENTS; i++) {
		screen_vertex_buffer_data.push_back(std::cos(M_PI * 2.0 * float(i) / (float)NUM_SEGMENTS));
		screen_vertex_buffer_data.push_back(std::sin(M_PI * 2.0 * float(i) / (float)NUM_SEGMENTS));
		screen_vertex_buffer_data.push_back(z);

		screen_vertex_buffer_data.push_back(std::cos(M_PI * 2.0 * float(i + 1) / (float)NUM_SEGMENTS));
		screen_vertex_buffer_data.push_back(std::sin(M_PI * 2.0 * float(i + 1) / (float)NUM_SEGMENTS));
		screen_vertex_buffer_data.push_back(z);

		screen_vertex_buffer_data.push_back(0);
		screen_vertex_buffer_data.push_back(0);
		screen_vertex_buffer_data.push_back(z);
	}

	// clearing errors
	gl_flush_errors();

	// vertex Buffer creation
	glGenBuffers(1, &mesh.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBufferData(GL_ARRAY_BUFFER, screen_vertex_buffer_data.size() * sizeof(GLfloat), screen_vertex_buffer_data.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &m_instance_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_instance_vbo);

	if (gl_has_errors())
		return false;

	// loading shaders
	if (!effect.load_from_file(shader_path("pebble.vs.glsl"), shader_path("pebble.fs.glsl")))
		return false;

	return true;
}

// releases all graphics resources
void Bullets::destroy()
{
	glDeleteBuffers(1, &mesh.vbo);
	glDeleteBuffers(1, &m_instance_vbo);

	glDeleteShader(effect.vertex);
	glDeleteShader(effect.fragment);
	glDeleteShader(effect.program);

	m_bullets.clear();
}

void Bullets::update(float ms)
{
	vec2 newton_F = { 0, 9.8f };
	// MOTION
	for (auto& bullet : m_bullets) {
		// s = ut + 1/2at^2
		bullet.position.x += bullet.velocity.x * (ms / 100) + 0.5f * newton_F.x * pow(ms / 100, 2);
		bullet.position.y += bullet.velocity.y * (ms / 100) + 0.5f * newton_F.y * pow(ms / 100, 2);
	}
}

void Bullets::draw(const mat3& projection)
{
	// set shaders
	glUseProgram(effect.program);

	// enable alpha channel for textures
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// depth
	glEnable(GL_DEPTH_TEST);

	// get uniform locations
	GLint projection_uloc = glGetUniformLocation(effect.program, "projection");
	GLint color_uloc = glGetUniformLocation(effect.program, "color");

	// color
	float color[] = { 1.f, 1.f, 1.f };
	glUniform3fv(color_uloc, 1, color);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)& projection);

	// draw the screen texture on the geometry
	// set vertices
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);

	// mesh vertex positions
	// bind to attribute 0 (in_position) as in the vertex shader
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glVertexAttribDivisor(0, 0);

	// load up bullet into buffer
	glBindBuffer(GL_ARRAY_BUFFER, m_instance_vbo);
	glBufferData(GL_ARRAY_BUFFER, m_bullets.size() * sizeof(Bullet), m_bullets.data(), GL_DYNAMIC_DRAW);

	// bullet translations
	// bind to attribute 1 (in_translate) as in vertex shader
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Bullet), (GLvoid*)offsetof(Bullet, position));
	glVertexAttribDivisor(1, 1);

	// bullet radii
	// bind to attribute 2 (in_scale) as in vertex shader
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(Bullet), (GLvoid*)offsetof(Bullet, radius));
	glVertexAttribDivisor(2, 1);

	// draw
	glDrawArraysInstanced(GL_TRIANGLES, 0, NUM_SEGMENTS * 3, m_bullets.size());

	// reset divisor
	glVertexAttribDivisor(1, 0);
	glVertexAttribDivisor(2, 0);
}

void Bullets::spawn_bullet(vec2 position, float radians)
{
	Bullet b;
	b.position = position;
	b.life = 5.0f;

	b.velocity = { 20*cos(radians) , 20*sin(radians)};
	b.radius = 2;
	m_bullets.emplace_back(b);
}