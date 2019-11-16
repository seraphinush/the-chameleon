#pragma once

// internal
#include "common.hpp"

#include "char.hpp"
#include "map.hpp"

#include <vector>

class Char;
class Map;

struct path_construction
{
	std::vector<vec2> path;
	int heuristic;
	int expected_total;
};

// guard type 1 : wanderer
class Wanderer : public Entity
{
	// shared texture
	static Texture wanderer_texture;

private:
	// config
	const float config_scale = 0.30f;
	const float config_speed = 30.f;
	
	// direction
	vec2 direction = { 1, 0 };

	// animation
	int sprite_switch = 1;
	float sprite_countdown = 200.f;
	int flip_in_x = 1;

	// pathing ai
	Map* m_map;
	Char* m_player;
	std::vector<vec2> m_path;
	std::vector<vec2> immediate_path;
	int current_goal_index;
	int current_immediate_goal_index;
	bool alert_mode = false;
	int chase_refresh_timer;

private:
	// pathing ai
	void calculate_immediate_path(vec2 goal, int limit_search);
	bool check_goal_arrival(vec2 goal);
	void move_towards_goal(vec2 goal, float ms);
	std::vector<path_construction> find_paths_from(path_construction origin, vec2 goal);
	std::vector<path_construction> merge_in_order(std::vector<path_construction> p1, std::vector<path_construction> p2);
	bool tile_is_accessible(vec2 origin, int x_delta, int y_delta);

public:
	bool init(std::vector<vec2> path, Map& map, Char& player);
	void destroy();
	void update(float ms);
	void draw(const mat3& projection) override;

	// movement
	void set_position(vec2 position);
	vec2 get_position() const;

	// collision
	vec2 get_bounding_box() const;

	// alert
	void set_alert_mode(bool val);
};