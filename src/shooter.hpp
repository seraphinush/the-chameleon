#pragma once

// internal
#include "common.hpp"
#include "char.hpp"
#include "bullets.hpp"

// guard type 2 : spotter
class Shooter : public Entity
{
	// shared texture
	static Texture shooter_texture;

private:
	// config
	const float config_scale = 0.15;

	// animation
	int shooter_sprite_switch = 1;
	float shooter_sprite_countdown = 1500.f;
	

public:
	bool init();
	void destroy();
	void update(float ms);
	void draw(const mat3& projection) override;

	// movement
	void set_position(vec2 position);
	vec2 get_position() const;
	
	void set_rotation(float radians);

	// detects if char in radius

	// collision
	vec2 get_bounding_box() const;

	bool collision_with(Char m_char);

	bool is_shooting;

	Bullets bullets;
};