#pragma once

// internal
#include "common.hpp"
#include "map.hpp"
#include "spotter.hpp"
#include "trophy.hpp"
#include "wanderer.hpp"
#include "bullets.hpp"

// stlib
#include <vector>

class Spotter;
class Wanderer;
class Trophy;
class Bullets;

class Char : public Entity
{
	// shared texture
	static Texture char_texture;

private:
	// config
	const float config_scale = 0.03f;

	bool m_is_alive;

	// key press
	bool m_moving_right;
	bool m_moving_left;
	bool m_moving_up;
	bool m_moving_down;

	vec2 m_correction;

	// color
	int m_color;

	// TODO -- wall collision
	bool m_wall_up;
	bool m_wall_down;
	bool m_wall_left;
	bool m_wall_right;

	// dash
	bool m_dash;
	int m_direction_change;

	// ANIMATION
	int sprite_switch = 1;
	float sprite_countdown = 200.f;
	int flip_in_x = 1;

public:
	bool init();
	void destroy();
	void update(float ms);
	void draw(const mat3 &projection) override;

	// alive
	bool is_alive() const;
	void kill();

	// collision
	bool collision(vec2 pos, vec2 box);
	bool is_colliding(const Spotter &spotter);
	bool is_colliding(const Wanderer &wanderer);
	bool is_colliding(const Trophy &trophy);
	bool is_colliding(const Bullets &bullets);
	vec2 get_bounding_box() const;

	// wall collision
	void set_wall_collision(char direction, bool value);
	void change_correction(vec2 c);
	vec2 get_correction() const;
	bool is_wall_collision() const;

	// movement
	void set_direction(char direction, bool value);
	void change_position(vec2 off);
	vec2 get_position() const;
	float get_speed() const;
	vec2 get_velocity();
	bool is_moving() const;

	// dash
	void change_direction(int c);
	int get_direction() const;

	// color change
	void set_color(int color);
	int get_color() const;

	// dash
	void set_dash(bool value);
	bool is_dashing();

	void set_rotation(float radians);
	void flip_char();
};