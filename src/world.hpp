#pragma once

// internal
#include "char.hpp"
#include "common.hpp"
#include "spotter.hpp"
#include "wanderer.hpp"
#include "start_screen.hpp"
#include "control_screen.hpp"
#include "story_screen.hpp"
#include "complete_screen.hpp"
#include "map.hpp"
#include "trophy.hpp"

// stlib
#include <vector>
#include <random>

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_mixer.h>

class World
{
private:
	// screen handle
	GLFWwindow *m_window;
	float m_screen_scale; // Screen to pixel coordinates scale factor

	// screen texture
	GLuint m_frame_buffer;
	Texture m_screen_tex;

	// screens
	StartScreen m_start_screen;
	ControlScreen m_control_screen;
	StoryScreen m_story_screen;
	CompleteScreen m_complete_screen;

	// TO REMOVE -- need to fix bug where story screen shrinks upon winning
	// story screen handle
	bool m_show_story_screen;

	Map m_map;

	// movement control
	unsigned int m_control; // 0: wasd, 1: arrow keys

	// game state
	unsigned int m_game_state; // 0: start, 1: controls, 2: quit, 3: level 1,

	// current game state
	unsigned int m_current_game_state;

	// entities
	Char m_char;
	std::vector<Spotter> m_spotters;
	std::vector<Wanderer> m_wanderers;
	Trophy m_trophy;

	// variables
	float m_current_speed;
	float m_next_wanderer_spawn;

	// sound
	Mix_Music *m_background_music;
	Mix_Chunk *m_char_dead_sound;
	Mix_Chunk *m_char_green_sound;
	Mix_Chunk *m_char_win_sound;

	// c++ rng
	std::default_random_engine m_rng;
	std::uniform_real_distribution<float> m_dist; // default 0..1

	bool recent_dash = false;

public:
	World();
	~World();

	bool init(vec2 screen);

	void destroy();

	bool update(float ms);

	void draw();

	bool is_over() const;

private:
	bool spawn_spotter();

	bool spawn_wanderer();

	bool spawn_trophy();

	void on_key(GLFWwindow *, int key, int, int action, int mod);
	void on_mouse_move(GLFWwindow *window, double xpos, double ypos);

	bool is_char_detectable(Map m_map);

	mat3 calculateProjectionMatrix(int width, int height);
};
