#pragma once

// internal
#include "common.hpp"

// start screen
class Cutscene : public Entity
{
	// shared texture
	static Texture dialogue_texture;
	static Texture left_dialogue_texture;
	static Texture right_dialogue_texture;

    unsigned int dialogue_counter;

	vec2 left_dialogue_position;
	vec2 right_dialogue_position;

	vec2 left_scale = {0.08f, 0.7f};
	vec2 right_scale = {0.08f, 0.8f};

	int right_dialogues[20] = {2, 4, 5, 8, 9, 11, 12, 13, 16, 18, 21, 22, 23, 24, 25};
	int left_dialogues[20] = {1, 3, 6, 7, 10, 14, 15, 17, 19, 20, 26, 27};

	unsigned int current_cutscene_state;

public:
	bool init();
	void destroy();
	void update(unsigned int game_state);
	void draw(const mat3& projection) override;
    bool dialogue_done(unsigned int cutscene_state);
    void increment_dialogue_counter(unsigned int cutscene_state);
	bool is_left_dialogue();
};