#pragma once

// internal
#include "common.hpp"
#include "salmon.hpp"
#include "spotter.hpp"
#include "water.hpp"
#include "wanderer.hpp"

// stlib
#include <vector>
#include <random>

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_mixer.h>

class World
{
public:
	World();
	~World();

	bool init(vec2 screen);

	void destroy();

	bool update(float ms);

	void draw();

	bool is_over()const;

private:
	bool spawn_spotter();

	bool spawn_wanderer();

	bool spawn_fish();

	void on_key(GLFWwindow*, int key, int, int action, int mod);
	void on_mouse_move(GLFWwindow* window, double xpos, double ypos);

private:
	// screen handle
	GLFWwindow* m_window;
	float m_screen_scale; // Screen to pixel coordinates scale factor

	// screen texture
	GLuint m_frame_buffer;
	Texture m_screen_tex;

	// water effect
	Water m_water;

	// points
	unsigned int m_points;

	// entities
	Salmon m_salmon;
	std::vector<Spotter> m_spotters;
	std::vector<Wanderer> m_wanderers;

	// variables
	float m_current_speed;
	float m_next_wanderer_spawn;

	// sound
	Mix_Music* m_background_music;
	Mix_Chunk* m_salmon_dead_sound;
	Mix_Chunk* m_salmon_eat_sound;

	// c++ rng
	std::default_random_engine m_rng;
	std::uniform_real_distribution<float> m_dist; // default 0..1
};
