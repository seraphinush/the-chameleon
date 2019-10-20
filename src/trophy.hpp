#pragma once

#include "common.hpp"

// guard type 1 : wanderer
class Trophy : public Entity
{
	// shared texture
	static Texture trophy_texture;

private:
	float config_scale = 0.05f;

public:
	bool init();

	void destroy();

	void update(float ms);

	// projection is the 2D orthographic projection matrix
	void draw(const mat3& projection) override;

	vec2 get_position()const;

	void set_position(vec2 position);
	void set_rotation(float radians);

	// collision
	vec2 get_bounding_box() const;
};