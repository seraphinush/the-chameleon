#pragma once

// internal
#include "common.hpp"
#include "constants.hpp"

// start screen
class Cutscene : public Entity
{
	// shared texture
	static Texture dialogue_texture;
	static Texture left_dialogue_texture;
	static Texture right_dialogue_texture;
	static Texture enemy_texture;

    unsigned int dialogue_counter;

	vec2 left_dialogue_position;
	vec2 right_dialogue_position;

	vec2 left_texture_position;
	vec2 right_texture_position;
	vec2 enemy_texture_position;

	vec2 left_scale = {0.08f, 0.7f};
	vec2 right_scale = {0.08f, 0.8f};
	vec2 enemy_scale = {0.08f, 0.8f};

	int left_dialogues[43] = {
		1, 3, 6, 7, 10, 14, 15, 17, 19, 20, 26, 27, 30, 31, 34,
		37, 39, 41, 43, 46, 47, 51, 53, 55, 57, 59, 61, 63, 67,
		68, 69, 71, 73, 75, 78, 80, 81, 82, 84, 86, 88, 90, 92
	};

	unsigned int current_cutscene_state;

public:
	bool init();
	void destroy();
	void update(unsigned int game_state);
	void draw(const mat3& projection) override;
    bool dialogue_done(unsigned int cutscene_state);
    void increment_dialogue_counter(unsigned int cutscene_state);
	void set_dialogue_counter(unsigned int cutscene_state, unsigned int counter_value);
	bool is_left_dialogue();
};