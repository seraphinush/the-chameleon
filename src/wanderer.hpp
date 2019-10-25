#pragma once

#include "common.hpp"

// guard type 1 : wanderer
class Wanderer : public Entity
{
	// shared texture
	static Texture wanderer_texture;

private:
	float config_scale = 0.5f;
	float config_speed = 150.f;

	// TO REMOVE - placeholder for randomize path wall collision
	bool m_wall_up;
	bool m_wall_down;
	bool m_wall_left;
	bool m_wall_right;
	
	// direction
	vec2 direction = { 1, 0 };

	// animation
	int sprite_switch = 1;
	float sprite_countdown = 200.f;
	int flip_in_x = 1;

public:
	bool init();

	void destroy();

	void update(float ms);

	// projection is the 2D orthographic projection matrix
	void draw(const mat3& projection) override;

	// movemnt
	void set_position(vec2 position);
	vec2 get_position()const;

	// collision
	void set_wall_collision(char direction, bool value);
	vec2 get_bounding_box() const;
};