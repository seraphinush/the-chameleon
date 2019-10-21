#pragma once

#include "common.hpp"

// guard type 1 : wanderer
class Wanderer : public Entity
{
	// shared texture
	static Texture wanderer_texture;

private:
	float config_scale = 0.05f;

public:
	bool init();

	void destroy();

	void update(float ms);

	// projection is the 2D orthographic projection matrix
	void draw(const mat3& projection) override;

	void set_position(vec2 position);

	void set_rotation(float radians);

	vec2 get_position()const;

	// collision
	vec2 get_bounding_box() const;

	// direction
	vec2 m_direction_wanderer = { -1, 0 };

	int sprite_switch = 1;
	float wanderer_sprite_countdown = 200.f;
	int flip_in_x = 1;
};