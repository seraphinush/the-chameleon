#pragma once

// internal
#include "char.hpp"
#include "common.hpp"
class Char;

class Map : public Entity
{
	// shared texture
	static Texture wall_texture;
	static Texture corridor_texture;

	static Texture corridor_texture_red;
	static Texture corridor_texture_blue;
	static Texture corridor_texture_green;
	static Texture corridor_texture_yellow;

private:
	vec2 translation_tile;
	float m_dead_time;
	float m_flash_time;
	int flash_map;

public:
	bool init();
	void destroy();

	// draw tiles
	void draw(const mat3 &projection) override;
	void draw_element(const mat3 &projection, const Texture &texture);

	void set_position(vec2 position);
	vec2 get_position() const;
	vec2 get_bounding_box() const;

	// color detection
	float collides_with(Char m_char);

	// wall collision
	void is_wall(Char &m_char);

	// char dead time getters and setters .. ported over from water
	void set_char_dead();

	void reset_char_dead_time();

	float get_char_dead_time() const;

	void set_flash(int value);
	int get_flash();

	void reset_flash_time();

	float get_flash_time() const;
};