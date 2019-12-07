#pragma once

// internal
#include "common.hpp"
#include "constants.hpp"

// hud
class Hud : public Entity
{
	// shared texture
	static Texture texture_hud;
	static Texture texture_tip;

	bool show_red_tooltip;
  bool show_blue_tooltip;
  bool show_yellow_tooltip;
  bool show_green_tooltip;

public:
	bool init();
	void destroy();
	void update(unsigned int state, vec2 char_position);

	void draw(const mat3& proj) override;
	void draw(const mat3& proj, vec2 wsize, const vec2 wpoint);
	void draw_element(const mat3& proj, const Texture& texture, vec2 pos, vec2 scale);


  void set_tooltip(char color, bool value);
};