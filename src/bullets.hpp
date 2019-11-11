#pragma once

#include <vector>

#include "common.hpp"
#include "char.hpp"

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// DON'T WORRY ABOUT THIS CLASS UNTIL ASSIGNMENT 3
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

using namespace std;
// Salmon pew-pews

class Char;

class Bullets : public Entity
{
public:
	// Data structure for pebble contains information needed
	// to render and simulate a basic pebble (apart from mesh.vbo), 
	// we will use this layout to pass information from m_pebbles to the pipeline.
	struct Bullet {
		float life = 0.0f; // remove pebble when its life reaches 0
		vec2 position;
		vec2 velocity;
		float radius;
	};

	// Creates all the associated render resources
	bool init();

	// Releases all associated resources
	void destroy();

	// Updates all pebbles
	// ms represents the number of milliseconds elapsed from the previous update() call
	void update(float ms);

	// Renders the pebbles
	// projection is the 2D orthographic projection matrix
	void draw(const mat3& projection) override;

	// Spawn new pebble
	void spawn_bullet(vec2 position, float radians);

	// Trigger collision checks
	void collides_with();
	
	bool collision_check(Char m_char);

	float radians;

	std::vector<Bullet> m_bullets; // vector of pebbles

	float cooldown;

	// CHECK FOR COLLISION
	//bool collision_with(Pebble pebble1, Pebble pebble2);

	//void bounce_off(Pebble pebble1, Pebble pebble2);

private:
	GLuint m_instance_vbo; // vbo for instancing pebbles
	//std::vector<Bullet> m_bullets; // vector of pebbles
};