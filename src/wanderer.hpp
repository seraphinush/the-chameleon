#pragma once

// internal
#include "common.hpp"
#include "map.hpp"

#include <vector>

class Map;

struct path_construction
{
	std::vector<vec2> path;
	int heuristic;
	int expected_total;
};

//class Map;
// guard type 1 : wanderer
class Wanderer : public Entity
{
	// shared texture
	static Texture wanderer_texture;

private:
	// config
	const float config_scale = 0.30f;
	const float config_speed = 30.f;

	// ALERT MODE COOL-DOWN
	float cool_down;

	// TO REMOVE - placeholder for randomize path wall collision
	bool m_wall_up;
	bool m_wall_down;
	bool m_wall_left;
	bool m_wall_right;
	
	// direction
	vec2 direction = { 1, 0 };

	// animation
	int sprite_switch = 1;
	float sprite_countdown = 200.f;
	int flip_in_x = 1;

	// Pathing AI
	Map* m_map;
	std::vector<vec2> m_path;
	std::vector<vec2> immediate_path;
	int current_goal_index;
	int current_immediate_goal_index;

private:

	// Pathing AI
	void calculate_immediate_path(vec2 goal);
	bool check_goal_arrival(vec2 goal);
	void move_towards_goal(vec2 goal, float ms);
	std::vector<path_construction> find_paths_from(path_construction origin, vec2 goal);
	std::vector<path_construction> merge_in_order(std::vector<path_construction> p1, std::vector<path_construction> p2);

public:
	bool init(std::vector<vec2> path, Map& map);
	void destroy();
	void update(float ms);
	void draw(const mat3& projection) override;

	void set_position(vec2 position);
	vec2 get_position() const;

	// collision
	void set_wall_collision(char direction, bool value);
	vec2 get_bounding_box() const;

	void alert_wanderer();

	// ALERT WANDERER METHOD
	bool alert_mode;
};