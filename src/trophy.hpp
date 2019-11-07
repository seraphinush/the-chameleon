#pragma once

// internal
#include "common.hpp"

// goal
class Trophy : public Entity
{
	// shared texture
	static Texture trophy_texture;

private:
	// config
	const float config_scale = 0.2f;

public:
	bool init();
	void destroy();
	void draw(const mat3& projection) override;

	// movement
	// use to set position per level
	void set_position(vec2 position);
	vec2 get_position() const;

	// collision
	vec2 get_bounding_box() const;
};