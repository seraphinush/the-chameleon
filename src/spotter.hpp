#pragma once

#include "common.hpp"

// guard type 2 : spotter
class Spotter : public Entity
{
	// shared texture
	static Texture spotter_texture;

private:
	float config_scale = 0.5f;

	// animation
	int spotter_sprite_switch = 1;
	float spotter_sprite_countdown = 1500.f;

public:
	bool init();
	void destroy();
	void update(float ms);
	void draw(const mat3& projection) override;

	// movement
	vec2 get_position()const;

	void set_position(vec2 position);

	// collision
	vec2 get_bounding_box() const;
};