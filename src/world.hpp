#pragma once

// internal
#include "char.hpp"
#include "common.hpp"
#include "spotter.hpp"
#include "map.hpp"
#include "wanderer.hpp"
#include "start_screen.hpp"
#include "control_screen.hpp"
#include "complete_screen.hpp"
#include "cutscene.hpp"
#include "hud.hpp"
#include "particles.hpp"
#include "shooter.hpp"
#include "overlay.hpp"
#include "level_screen.hpp"

// stlib
#include <vector>
#include <random>

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_mixer.h>

// constants
// game state
#define START_SCREEN 0
#define CONTROL_SCREEN 1
#define LEVEL_SCREEN 2
#define QUIT 3
#define STORY_SCREEN 4
#define WIN_SCREEN 5
#define LEVEL_1 1000
#define LEVEL_2 2000
#define LEVEL_3 3000
#define LEVEL_4 4000
#define LEVEL_5 5000
#define LEVEL_TUTORIAL 6000
#define LEVEL_1_CUTSCENE 1500
#define LEVEL_2_CUTSCENE 2500
#define LEVEL_3_CUTSCENE 3500
#define LEVEL_4_CUTSCENE 4500
#define LEVEL_5_CUTSCENE 5500

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
	CompleteScreen m_complete_screen;
	LevelScreen m_level_screen;
	Cutscene m_cutscene;
	Hud m_hud;

	// TO REMOVE -- need to fix bug where story screen shrinks upon winning
	// story screen handle
	bool m_show_story_screen;

	Map m_map;
	Overlay m_overlay;

	// movement control
	unsigned int m_control; // 0: wasd, 1: arrow keys

	// game state
	unsigned int m_game_state;

	// current game state
	unsigned int m_current_game_state;

	// current level state
	unsigned int m_current_level_state;

	// entities
	Char m_char;
	std::vector<Spotter> m_spotters;
	std::vector<Wanderer> m_wanderers;
	std::vector<Shooter> m_shooters;
	Particles m_particles_emitter;

	// variables
	float m_current_speed;

	// sound
	Mix_Music *m_background_music;
	Mix_Chunk *m_char_dead_sound;
	Mix_Chunk *m_char_green_sound;
	Mix_Chunk *m_char_win_sound;

	// c++ rng
	std::default_random_engine m_rng;
	std::uniform_real_distribution<float> m_dist; // default 0..1

	bool recent_dash = false;

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
	{};

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

	bool init(vec2 screen);
	void destroy();
	bool update(float ms);
	void draw();
	bool is_over() const;

	bool alert_mode;

private:
	bool spawn_spotter();
	bool spawn_shooter();

	bool spawn_wanderer(std::vector<vec2> path);

	void on_key(GLFWwindow *, int key, int, int action, int mod);
	void on_mouse_move(GLFWwindow *window, double xpos, double ypos);

	bool is_char_detectable(Map m_map);

	mat3 calculateProjectionMatrix(int width, int height);

	// reset
	void reset_game();
};
