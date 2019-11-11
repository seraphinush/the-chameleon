// Header
#include "particles.hpp"

#include <cmath>
#include <iostream>

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// DON'T WORRY ABOUT THIS CLASS UNTIL ASSIGNMENT 3
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

static const int MAX_PARTICLES = 25;
constexpr int NUM_SEGMENTS = 12;

bool Particles::init()
{
	std::vector<GLfloat> screen_vertex_buffer_data;
	constexpr float z = -0.1;

	for (int i = 0; i < NUM_SEGMENTS; i++)
	{
		screen_vertex_buffer_data.push_back(std::cos(float(i) / (float)NUM_SEGMENTS));
		screen_vertex_buffer_data.push_back(std::sin(M_PI * float(i) / (float)NUM_SEGMENTS));
		screen_vertex_buffer_data.push_back(z);

		screen_vertex_buffer_data.push_back(std::cos(M_PI * 2.0 * float(i + 1) / (float)NUM_SEGMENTS));
		screen_vertex_buffer_data.push_back(std::sin(M_PI * 2.0 * float(i + 1) / (float)NUM_SEGMENTS));
		screen_vertex_buffer_data.push_back(z);

		screen_vertex_buffer_data.push_back(0);
		screen_vertex_buffer_data.push_back(0);
		screen_vertex_buffer_data.push_back(z);
	}

	// Clearing errors
	gl_flush_errors();

	// Vertex Buffer creation
	glGenBuffers(1, &mesh.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBufferData(GL_ARRAY_BUFFER, screen_vertex_buffer_data.size() * sizeof(GLfloat), screen_vertex_buffer_data.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &m_instance_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_instance_vbo);

	if (gl_has_errors())
		return false;

	// Loading shaders
	if (!effect.load_from_file(shader_path("particle.vs.glsl"), shader_path("particle.fs.glsl")))
		return false;

	return true;
}

// Releases all graphics resources
void Particles::destroy()
{
	glDeleteBuffers(1, &mesh.vbo);
	glDeleteBuffers(1, &m_instance_vbo);

	glDeleteShader(effect.vertex);
	glDeleteShader(effect.fragment);
	glDeleteShader(effect.program);

	m_particles.clear();
}

void Particles::update(float ms)
{
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// HANDLE PARTICLE UPDATES HERE
	// You will need to handle both the motion of particles
	// and the removal of dead particles.
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	for (auto &particle : m_particles)
	{
		float dt = ms / 1000;
		float step_x = 1.0 * particle.velocity.x * dt;
		float step_y = 1.0 * particle.velocity.y * dt;
		particle.position.x += step_x;
		particle.position.y += step_y;
	}
}

void Particles::spawn_particle(vec2 position, int direction)
{
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// HANDLE PARTICLE SPAWNING HERE
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	int off_x_1 = 1;
	int off_x_2 = 1;
	int off_y_1 = 1;
	int off_y_2 = 1;
	switch (direction)
	{
	case 0:
		off_y_1 = 1;
		off_y_2 = -1;
		off_x_1 = -1;
		off_x_2 = -1;
		break;
	case 1:
		// fprintf(stderr, "hit left or right wall");
		off_y_1 = 1;
		off_y_2 = -1;
		off_x_1 = 1;
		off_x_2 = 1;
		break;
	case 2:
		off_x_1 = 1;
		off_x_2 = -1;
		off_y_1 = 1;
		off_y_2 = 1;
		break;
	case 3:
		// fprintf(stderr, "hit top or bottom wall");
		off_x_1 = 1;
		off_x_2 = -1;
		off_y_1 = -1;
		off_y_2 = -1;
		break;
	default:
		off_x_1 = 1;
		off_x_2 = 1;
		off_y_1 = 1;
		off_y_2 = 1;
		break;
	}
	Particle particle_1;
	particle_1.position.x = position.x;
	particle_1.position.y = position.y;
	particle_1.radius = 5;
	particle_1.velocity.x = off_x_1 * 100;
	particle_1.velocity.y = off_y_1 * 100;
	m_particles.emplace_back(particle_1);

	Particle particle_2;
	particle_2.position.x = position.x;
	particle_2.position.y = position.y;
	particle_2.radius = 5;
	particle_2.velocity.x = off_x_2 * 100;
	particle_2.velocity.y = off_y_2 * 100;
	m_particles.emplace_back(particle_2);
}

void Particles::collides_with()
{
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// HANDLE PARTICLE COLLISIONS HERE
	// You will need to write additional functions from scratch.
	// Make sure to handle both collisions between particles
	// and collisions between particles and salmon/fish/turtles.
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
}

// Draw particles using instancing
void Particles::draw(const mat3 &projection)
{
	// Setting shaders
	glUseProgram(effect.program);

	// Enabling alpha channel for textures
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);

	// Getting uniform locations
	GLint projection_uloc = glGetUniformLocation(effect.program, "projection");
	GLint color_uloc = glGetUniformLocation(effect.program, "color");
	GLint fade_particle_uloc = glGetUniformLocation(effect.program, "fade_particle");
	GLuint fade_timer_uloc = glGetUniformLocation(effect.program, "fade_timer");

	// Particle color
	float color[] = {0.4f, 0.4f, 0.4f};
	glUniform3fv(color_uloc, 1, color);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float *)&projection);
	glUniform1iv(fade_particle_uloc, 1, &fade_particle);
	glUniform1f(fade_timer_uloc, (m_fade_time > 0) ? (float)((glfwGetTime() - m_fade_time) * 20.0f) : -1);

	// Draw the screen texture on the geometry
	// Setting vertices
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);

	// Mesh vertex positions
	// Bind to attribute 0 (in_position) as in the vertex shader
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
	glVertexAttribDivisor(0, 0);

	// Load up particles into buffer
	glBindBuffer(GL_ARRAY_BUFFER, m_instance_vbo);
	glBufferData(GL_ARRAY_BUFFER, m_particles.size() * sizeof(Particle), m_particles.data(), GL_DYNAMIC_DRAW);

	// Particle translations
	// Bind to attribute 1 (in_translate) as in vertex shader
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Particle), (GLvoid *)offsetof(Particle, position));
	glVertexAttribDivisor(1, 1);

	// Particle radii
	// Bind to attribute 2 (in_scale) as in vertex shader
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (GLvoid *)offsetof(Particle, radius));
	glVertexAttribDivisor(2, 1);

	// Draw using instancing
	// https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glDrawArraysInstanced.xhtml
	glDrawArraysInstanced(GL_TRIANGLES, 0, NUM_SEGMENTS * 3, m_particles.size());

	// Reset divisor
	glVertexAttribDivisor(1, 0);
	glVertexAttribDivisor(2, 0);
}

// fade Particle

void Particles::set_fade(int value)
{
	fade_particle = value;
	m_fade_time = glfwGetTime();
}

int Particles::get_fade()
{
	return fade_particle;
}

void Particles::reset_fade_time()
{
	m_fade_time = glfwGetTime();
}

float Particles::get_fade_time() const
{
	return glfwGetTime() - m_fade_time;
}