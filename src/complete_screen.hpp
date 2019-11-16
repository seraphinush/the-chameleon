#pragma once

// internal
#include "common.hpp"
#include "constants.hpp"

// complete screen
class CompleteScreen : public Entity
{
	// shared texture
	static Texture complete_screen;

public:
	bool init();
	void destroy();
	void draw(const mat3 &projection) override;
};