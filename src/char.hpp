#pragma once

// internal
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

private:
	// config
	float config_scale = 0.2f;

	bool m_is_alive;
	bool m_is_win;

	// key press
	bool m_moving_right;
	bool m_moving_left;
	bool m_moving_up;
	bool m_moving_down;

	// color
	float m_color_change;
	float m_direction_change;

	// TODO -- wall collision
	bool m_wall_up;
	bool m_wall_down;
	bool m_wall_left;
	bool m_wall_right;

	bool m_dash;

public:
	bool init();
	void destroy();
	void update(float ms);
	void draw(const mat3 &projection) override;

	// collision
	bool collision(vec2 pos, vec2 box);
	bool collides_with(const Spotter &spotter);
	bool collides_with(const Wanderer &wanderer);
	bool collides_with(const Trophy &trophy);

	// wall collision
	void set_wall_collision(char direction, bool value);
	bool get_wall_collision();

	// collision
	vec2 get_position() const;
	vec2 get_bounding_box() const;

	// movement
	void move(vec2 off);

	void set_rotation(float radians);

	void set_direction(char direction, bool value);

	void change_color(float c);

	float get_color_change() const;

	void change_direction(float c);

	float get_direction_change() const;

	// alive
	bool is_alive() const;
	void kill();

	// goal
	bool is_win() const;
	void win();

	bool collides_with_wall();

	char get_direction();

	void dash();
	void set_dash(bool value);
	bool get_dash();
};