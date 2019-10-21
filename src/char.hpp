#pragma once

// interval
#include "common.hpp"
#include "map.hpp"
#include "spotter.hpp"
#include "trophy.hpp"
#include "wanderer.hpp"

// stlib
#include <vector>

class Spotter;
class Wanderer;
class Trophy;

class Char : public Entity
{
	// shared texture
	static Texture char_texture;

public:
	bool init();

	void destroy();

	void update(float ms);

	void draw(const mat3 &projection) override;

	// collision
	bool collides_with(const Spotter &spotter);

	bool collides_with(const Wanderer &wanderer);

	bool collides_with(const Trophy& trophy);

	vec2 get_position() const;

	void move(vec2 off);

	void set_rotation(float radians);

	void set_direction(char direction, bool value);

	// game mode
	bool get_mode() const;

	void set_mode(bool value);

	// bound
	// TODO -- change to collision-base
	void set_bound(char direction, bool state);

	void change_color(float c);

	float get_color_change() const;

	void change_direction(float c);

	float get_direction_change() const;
	
	bool is_alive() const;

	bool is_win() const;

	void win();

	void kill();

	bool collides_with_wall();

	char get_direction();

private:
	bool m_is_alive;
	bool m_is_win;

	// key press
	bool m_moving_right;
	bool m_moving_left;
	bool m_moving_up;
	bool m_moving_down;

	// game mode
	bool m_game_mode;

	// color
	float m_color_change;
	float m_direction_change;

  // bound
	// TODO -- change to collision-base
	bool m_bound_right;
	bool m_bound_left;
	bool m_bound_up;
	bool m_bound_down;

  std::vector<Vertex> m_vertices;
	std::vector<uint16_t> m_indices;
};