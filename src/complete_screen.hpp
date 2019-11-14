#pragma once

// internal
#include "common.hpp"

// complete screen
class CompleteScreen : public Entity
{
	// shared texture
	static Texture complete_screen;

public:
	bool init(vec2 screen);
	void destroy();
	void draw(const mat3 &projection) override;
};