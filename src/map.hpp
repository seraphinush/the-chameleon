#pragma once

// internal
#include "common.hpp"
#include "constants.hpp"

#include "char.hpp"
class Char;

class Map : public Entity
{
	// shared texture
	static Texture wall_texture;
	static Texture wall_light_texture;

	static Texture corridor_texture;
	static Texture corridor_texture_red;
	static Texture corridor_texture_blue;
	static Texture corridor_texture_green;
	static Texture corridor_texture_yellow;

	static Texture trophy_texture;

private:
	vec2 translation_tile;
	float m_dead_time;
	float m_flash_time;
	int flash_map;
	char current_level[40][61];

public:
	bool init();
	void destroy();

	// draw tiles
	void draw(const mat3 &projection) override;
	void draw_element(const mat3 &projection, const Texture &texture);

	void set_current_map(int level);
	vec2 get_spawn_pos() const;

	// color detection
	int get_tile_type(vec2 pos);

	// wall collision
	void check_wall(Char &ch, const float ms);

	// char dead time getters and setters .. ported over from water
	void set_char_dead();
	void reset_char_dead_time();
	float get_char_dead_time() const;

	// flash
	void set_flash(int value);
	void reset_flash_time();
	int get_flash();
	float get_flash_time() const;

	// Pathing helper functions
	vec2 get_tile_center_coords(vec2 tile_indices);
	vec2 get_grid_coords(vec2 position);
	bool is_wall(vec2 grid_coords);
};