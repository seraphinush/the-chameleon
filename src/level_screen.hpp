#pragma once

// internal
#include "common.hpp"

// start screen
class LevelScreen : public Entity
{
	// shared texture
	static Texture pointer;
	static Texture tutorial;
	static Texture level1;
	static Texture level2;
	static Texture level3;

	vec2 pointer_position;

public:
	bool init();
	void destroy();
	void update(unsigned int game_state);
	void draw(const mat3& projection) override;
};