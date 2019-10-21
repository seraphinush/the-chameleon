#pragma once

#include "common.hpp"
#include "char.hpp"

class Char;

// Salmon enemy
class Map : public Entity
{
	// Shared between all turtles, no need to load one for each instance
	static Texture wall_texture;
	static Texture corridor_texture;

	static Texture corridor_texture_red;
	static Texture corridor_texture_blue;
	static Texture corridor_texture_green;
	static Texture corridor_texture_yellow;

public:
	// Creates all the associated render resources and default transform
	bool init();

	// Releases all the associated resources
	void destroy();

	// Renders the salmon
	// projection is the 2D orthographic projection matrix
	void draw(const mat3 &projection) override;

	void draw_wall(const mat3 &projection);

	void draw_corridor(const mat3 &projection);

	void draw_corridor_red(const mat3 &projection);
	void draw_corridor_blue(const mat3 &projection);
	void draw_corridor_green(const mat3 &projection);
	void draw_corridor_yellow(const mat3 &projection);

	// Returns the current turtle position
	vec2 get_position() const;

	// Sets the new turtle position
	void set_position(vec2 position);

	// Returns the turtle' bounding box for collision detection, called by collides_with()
	vec2 get_bounding_box() const;

	float collision_with(Char m_char);

	// char dead time getters and setters
	void set_char_dead();

	void reset_char_dead_time();

	float get_char_dead_time() const;

	void set_flash(int value);

	void reset_flash_time();

	float get_flash_time() const;


private:
	vec2 translation_tile;
	float m_dead_time;
	float m_flash_time;
	int flash_map;
};