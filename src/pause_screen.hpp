#pragma once

// internal
#include "common.hpp"
#include "constants.hpp"

// start screen
class PauseScreen : public Entity
{
	// shared texture
	static Texture pointer;
	static Texture game_paused;
	static Texture resume;
	static Texture restart;
	static Texture main_menu;
	static Texture quit;

	vec2 pointer_pos;
	vec2 game_paused_pos;
	vec2 resume_pos;
	vec2 restart_pos;
	vec2 main_menu_pos;
	vec2 quit_pos;

public:
	bool init();
	void destroy();
	void update(unsigned int game_state, vec2 position);

	void draw(const mat3& projection) override;
	void draw_element(const mat3& projection, const Texture& texture, vec2 pos, vec2 scale);
};