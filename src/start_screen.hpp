#pragma once

#include "common.hpp"

// start screen
class StartScreen : public Entity
{
	// shared texture
	static Texture pointer;
	static Texture start_game;
	static Texture controls;
	static Texture quit;
	static Texture game_title;

public:
	bool init();

	void destroy();

	void update(float ms);

	// projection is the 2D orthographic projection matrix
	void draw(const mat3& projection) override;
};