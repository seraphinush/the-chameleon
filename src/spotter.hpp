#pragma once

// internal
#include "common.hpp"

// guard type 2 : spotter
class Spotter : public Entity
{
	// shared texture
	static Texture spotter_texture;

private:
	// config
	const float config_scale = 0.25;

	// animation
	int spotter_sprite_switch = 1;
	float spotter_sprite_countdown = 1500.f;

	// detection
	float radius = 70.f;

	// alert
	bool m_alert_mode;

public:
	bool init();
	void destroy();
	void update(float ms);
	void draw(const mat3& projection) override;

	// movement
	void set_position(vec2 pos);
	vec2 get_position() const;

	// collision
	vec2 get_bounding_box() const;

	// detection
	bool is_in_sight(vec2 pos);

	// alert
	void set_alert_mode(bool val);

	vec2 direction;
	
	void reset_direction();

private:
	float check_sgn(float value);
};