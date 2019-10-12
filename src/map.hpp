#pragma once

#include "common.hpp"
#include "char.hpp"

// Salmon enemy 
class Map : public Entity
{
	// Shared between all turtles, no need to load one for each instance
	static Texture wall_texture;
	static Texture corridor_texture;

public:
	// Creates all the associated render resources and default transform
	bool init();

	// Releases all the associated resources
	void destroy();

	// Renders the salmon
	// projection is the 2D orthographic projection matrix
	void draw(const mat3& projection) override;

	void draw_wall(const mat3& projection);

	void draw_corridor(const mat3& projection);

	// Returns the current turtle position
	vec2 get_position()const;

	// Sets the new turtle position
	void set_position(vec2 position);

	// Returns the turtle' bounding box for collision detection, called by collides_with()
	vec2 get_bounding_box() const;

	float collision_with(Char m_char);

private:
	Char m_char;
	vec2 translation_tile;
};