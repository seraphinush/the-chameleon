#pragma once

#include <vector>

#include "common.hpp"

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// DON'T WORRY ABOUT THIS CLASS UNTIL ASSIGNMENT 3
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

// Salmon pew-pews
class Particles : public Entity
{
public:
	// Data structure for particle contains information needed
	// to render and simulate a basic particle (apart from mesh.vbo),
	// we will use this layout to pass information from m_particles to the pipeline.
	struct Particle
	{
		float life = 0.0f; // remove particle when its life reaches 0
		vec2 position;
		vec2 velocity;
		float radius;
	};

	// Creates all the associated render resources
	bool init();

	// Releases all associated resources
	void destroy();

	// Updates all particles
	// ms represents the number of milliseconds elapsed from the previous update() call
	void update(float ms);

	// Renders the particles
	// projection is the 2D orthographic projection matrix
	void draw(const mat3 &projection) override;

	// Spawn new particle
	void spawn_particle(vec2 position, int direction);

	// Trigger collision checks
	void collides_with();

	// fade particles
	void set_fade(int value);
	int get_fade();
	void reset_fade_time();
	float get_fade_time() const;

private:
	GLuint m_instance_vbo;			   // vbo for instancing particles
	std::vector<Particle> m_particles; // vector of particles
	float m_fade_time;
	int fade_particle;
};