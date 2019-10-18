#pragma once

#include "common.hpp"

// story screen
class StoryScreen : public Entity
{
	// shared texture
	static Texture pointer;
	static Texture story_game;
	static Texture controls;
	static Texture quit;
	static Texture game_title;

	vec2 pointer_position;

public:
	bool init();

	void destroy();

	void update(unsigned int game_state);

	// projection is the 2D orthographic projection matrix
	void draw(const mat3 &projection) override;
};