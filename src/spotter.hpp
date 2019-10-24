#pragma once

#include "common.hpp"

// guard type 2 : spotter
class Spotter : public Entity
{
	// shared texture
	static Texture spotter_texture;

private:
	float config_scale = 0.5f;

public:
	bool init();

	void destroy();

	void update(float ms);

	// projection is the 2D orthographic projection matrix
	void draw(const mat3& projection) override;

	vec2 get_position()const;

	void set_position(vec2 position);

	// collision
	vec2 get_bounding_box() const;

	// animation
	int spotter_sprite_switch = 1;
	float spotter_sprite_countdown = 1500.f;
};