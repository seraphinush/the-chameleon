#pragma once

#include "common.hpp"
#include <vector>

class Turtle;
class Fish;

class Salmon : public Entity
{
public:
	// Creates all the associated render resources and default transform
	bool init();

	// Releases all associated resources
	void destroy();
	
	// Update salmon position based on direction
	// ms represents the number of milliseconds elapsed from the previous update() call
	void update(float ms);
	
	// Renders the salmon
	void draw(const mat3& projection)override;

	// Collision routines for turtles and fish
	bool collides_with(const Turtle& turtle);
	bool collides_with(const Fish& fish);

	// Returns the current salmon position
	vec2 get_position() const;

	// Moves the salmon's position by the specified offset
	void move(vec2 off);

	// Set salmon rotation in radians
	void set_rotation(float radians);

	// Set a particular salmon direction to true/false
	void set_direction(char direction, bool value);

	// Change the game mode
	void set_mode(bool value);

	// Get the game mode
	bool get_mode();

	// True if the salmon is alive
	bool is_alive()const;

	// Kills the salmon, changing its alive state and triggering on death events
	void kill();

	// Called when the salmon collides with a fish, starts lighting up the salmon
	void light_up();

	void change_color(float c);
	float get_color_change();

private:
	float m_light_up_countdown_ms; // Used to keep track for how long the salmon should be lit up
	bool m_is_alive; // True if the salmon is alive
	bool m_moving_right; // True if the salmon is moving right (when right arrow is pressed)
	bool m_moving_left; // True if the salmon is moving left (when left arrow is pressed)
	bool m_moving_up; // True if the salmon is moving up (when up arrow is pressed)
	bool m_moving_down; // True if the salmon is moving down (when down arrow is pressed)
	bool m_default_mode;
	float m_color_change;

  std::vector<Vertex> m_vertices;
	std::vector<uint16_t> m_indices;
};