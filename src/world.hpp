#pragma once

// internal
#include "common.hpp"
#include "constants.hpp"

#include "char.hpp"
#include "complete_screen.hpp"
#include "control_screen.hpp"
#include "cutscene.hpp"
#include "hud.hpp"
#include "level_screen.hpp"
#include "map.hpp"
#include "overlay.hpp"
#include "particles.hpp"
#include "shooter.hpp"
#include "spotter.hpp"
#include "start_screen.hpp"
#include "wanderer.hpp"

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
	float m_screen_scale;

	// screen texture
	GLuint m_frame_buffer;
	Texture m_screen_tex;

	// sound
	Mix_Music *m_background_music;
	Mix_Chunk *m_sfx_alert;
	Mix_Chunk *m_sfx_get_trophy;

	// c++ rng
	std::default_random_engine m_rng;
	std::uniform_real_distribution<float> m_dist; // default 0..1

	// screens
	StartScreen m_start_screen;
	ControlScreen m_control_screen;
	LevelScreen m_level_screen;
	Cutscene m_cutscene;
	CompleteScreen m_complete_screen;
	Hud m_hud;

	// entities
	Char m_char;
	Map m_map;
	Overlay m_overlay;
	Particles m_particles_emitter;
	std::vector<Shooter> m_shooters;
	std::vector<Spotter> m_spotters;
	std::vector<Wanderer> m_wanderers;

	// movement control
	unsigned int m_control; // 0: wasd, 1: arrow keys

	// game state
	unsigned int m_game_state;

	// current game state
	unsigned int m_current_game_state;

	// current level state
	unsigned int m_current_level_state;

	// variables
	bool m_alert_mode;
	int m_alert_mode_cooldown;
	int m_cooldown;
	float m_current_speed;
	bool m_recent_dash;
	bool m_spawn_particles;

	// wanderer checkpoint
	vector<vector<vec2>> wanderer_paths =
	{ {{6,6}, {6,2}, {1,2}, {1,6}},
	{{8,6}, {8,2}, {16,2}, {16,6}},
	{{6,11}, {1,11}, {1,9}, {6,9}},
	{{8,11}, {8,9}, {13,9}, {13,11}},
	{{1,14}, {16,14}},
	{{1,17}, {12,18}},
	{{20,18}, {31,25}, {41,16}, {32,25}},
	{{18,22}, {31,28}, {34,28}, {42,20}, {34,28}, {30,28}},
	{{19,38}, {19,27}, {11,27}, {11,38}},
	{{22,38}, {58,37}},
	{{45,26}, {45,18}, {52,18}, {52,26}},
	{{19,2}, {19,15}},
	{{23,2}, {48,3}},
	{{39,15}, {39,7}, {51,7}, {51,15}},
	{{35,12}, {23,12}, {23,15}, {29,20}, {33,20}, {36,15}} };

	vector<vector<vec2>> wanderer_paths_2 =
	{
		{{1,8}, {10,8}},
		{{1,11}, {10,11}},
		{{1,14}, {10,14}},
		{{1,17}, {10,17}},
		{{1,20}, {10,20}},
		{{1,23}, {10,23}},
		{{1,26}, {10,26}},
		{{1,29}, {10,29}},
		{{1,32}, {10,32}},
		{{1,35}, {10,35}},
		{{12,9}, {22,9}},
		{{12,12}, {22,12}},
		{{12,15}, {22,15}},
		{{12,18}, {22,18}},
		{{12,21}, {22,21}},
		{{12,24}, {22,24}},
		{{12,27}, {22,27}},
		{{12,30}, {22,30}},
		{{12,33}, {22,33}},
		{{23,36}, {23,38}, {16,38}, {16,36}},
		{{24,2}, {24,16}},
		{{34,8}, {26,8}, {26,6}, {34,6}},
		{{24,17}, {32,17}, {32,23}, {24,23}},
		{{41,27}, {41,32}},
		{{43, 8}, {43, 14}},
		{{45,36}, {53,36}, {53,38}, {45,38}},
		{{58,36}, {58,10}},
		{{38,2}, {46,2}, {46,11}, {49,11}, {49,16}, {49,11}, {46,11}, {46,2}}
	};

	vector<vector<vec2>> wanderer_paths_3 =
	{ { {1, 6}, { 12,7 }},
	{{1,10}, {10,11}, {11,17}, {10,11}},
	{{1,18}, {1,23}, {8,23}, {8,18}},
	{{14,2}, {14,10}, {18,10}, {18,2}},
	{{20,10}, {21,3}},
	{{30,5}, {42,2}},
	{{44,2}, {56,3}},
	{{40,14}, {41,7}, {58,6}, {41,7}},
	{{55,28}, {55,19}, {58,19}, {58,28}},
	{{45,35}, {58,38}, {58,35}, {52,35}},
	{{27,38}, {41,37}},
	{{24,30}, {41,29}},
	{{24,25}, {35,26}},
	{{19,22}, {33,21}, {34,17}, {33,21}},
	{{15,25}, {15,29}, {22,29}, {22,25}},
	{{4,35}, {14,37}, {16,33}, {19,32}, {16,33}, {14,37}} };

public:
	World();
	~World();

	bool init();
	void destroy();
	bool update(float ms);
	void draw();
	bool is_over() const;

private:
	bool spawn_spotter();
	bool spawn_shooter();

	bool spawn_wanderer(std::vector<vec2> path);

	void on_key(GLFWwindow *, int key, int, int action, int mod);
	void on_mouse_move(GLFWwindow *window, double xpos, double ypos);

	bool is_char_detectable();

	mat3 calculateProjectionMatrix(int width, int height);

	// cutscene caller
	void advance_to_cutscene();

	// reset
	void reset_game();
};
