// header
#include "world.hpp"

// stlib
#include <string.h>
#include <cassert>
#include <sstream>
#include <iostream>

namespace
{
const size_t MAX_SPOTTERS = 5;
const size_t MAX_SHOOTERS = 5;
const size_t MAX_WANDERERS = 10;
const size_t SPOTTER_DELAY_MS = 800;

const size_t MAX_ALERT_MODE_COOLDOWN = 100;
int alert_mode_cooldown = MAX_ALERT_MODE_COOLDOWN;

// TODO
vec2 spotter_loc[5];

vec2 shooter_loc[5];

namespace
{
void glfw_err_cb(int error, const char *desc)
{
	fprintf(stderr, "%d: %s", error, desc);
}
} // namespace
} // namespace

World::World() : 
	m_control(0),
	m_current_game_state(0),
	m_game_state(START_SCREEN),
	m_next_wanderer_spawn(0.f),
	m_show_story_screen(true)
{
	// send rng with random device
	m_rng = std::default_random_engine(std::random_device()());
}

World::~World()
{
}

// initialization
bool World::init(vec2 screen)
{
	// TODO
	spotter_loc[0] = {100, 100};
	spotter_loc[1] = {screen.x - 100, 100};
	spotter_loc[2] = {100, screen.y - 100};
	spotter_loc[3] = {screen.x - 100, screen.y - 100};
	spotter_loc[4] = {800, 500};

	// TODO
	shooter_loc[0] = { 100 + 100, 100 + 50 };
	shooter_loc[1] = { screen.x - 50, 100 + 50 };
	shooter_loc[2] = { 150, screen.y - 150 };
	shooter_loc[3] = { screen.x - 50, screen.y - 50 };
	shooter_loc[4] = { 850, 550 };


	// GLFW / OGL Initialization
	// Core Opengl 3.
	glfwSetErrorCallback(glfw_err_cb);
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW");
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, 1);
#if __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	glfwWindowHint(GLFW_RESIZABLE, 0);
	m_window = glfwCreateWindow((int)screen.x, (int)screen.y, "The Chameleon", nullptr, nullptr);
	if (m_window == nullptr)
		return false;

	glfwMakeContextCurrent(m_window);
	glfwSwapInterval(1); // vsync

	// load OpenGL function pointers
	gl3w_init();

	// set callbacks to member functions (that's why the redirect is needed)
	// input is handled using GLFW, for more info see
	// http://www.glfw.org/docs/latest/input_guide.html
	glfwSetWindowUserPointer(m_window, this);
	auto key_redirect = [](GLFWwindow *wnd, int _0, int _1, int _2, int _3) { ((World *)glfwGetWindowUserPointer(wnd))->on_key(wnd, _0, _1, _2, _3); };
	auto cursor_pos_redirect = [](GLFWwindow *wnd, double _0, double _1) { ((World *)glfwGetWindowUserPointer(wnd))->on_mouse_move(wnd, _0, _1); };
	glfwSetKeyCallback(m_window, key_redirect);
	glfwSetCursorPosCallback(m_window, cursor_pos_redirect);

	// create a frame buffer
	m_frame_buffer = 0;
	glGenFramebuffers(1, &m_frame_buffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_frame_buffer);

	// for some high DPI displays (ex. Retina Display on Macbooks)
	// https://stackoverflow.com/questions/36672935/why-retina-screen-coordinate-value-is-twice-the-value-of-pixel-value
	int fb_width, fb_height;
	glfwGetFramebufferSize(m_window, &fb_width, &fb_height);
	m_screen_scale = static_cast<float>(fb_width) / screen.x;

	// initialize the screen texture
	m_screen_tex.create_from_screen(m_window);

	// load music and sounds
	if (SDL_Init(SDL_INIT_AUDIO) < 0)
	{
		fprintf(stderr, "Failed to initialize SDL Audio");
		return false;
	}

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1)
	{
		fprintf(stderr, "Failed to open audio device");
		return false;
	}

	m_background_music = Mix_LoadMUS(audio_path("music.wav"));
	m_char_dead_sound = Mix_LoadWAV(audio_path("char_dead.wav"));
	m_char_green_sound = Mix_LoadWAV(audio_path("green_sound.wav"));
	m_char_win_sound = Mix_LoadWAV(audio_path("char_win.wav"));

	if (m_background_music == nullptr || m_char_dead_sound == nullptr || m_char_win_sound == nullptr || m_char_green_sound == nullptr)
	{
		fprintf(stderr, "Failed to load sounds\n %s\n %s\n %s\n %s\n make sure the data directory is present",
				audio_path("music.wav"),
				audio_path("char_dead.wav"),
				audio_path("green_sound.wav"),
				audio_path("char_win.wav"));
		return false;
	}

	// play background music
	Mix_PlayMusic(m_background_music, -1);
	fprintf(stderr, "Loaded music\n");

	m_current_speed = 1.f;

	return m_start_screen.init() &&
		   m_control_screen.init() &&
		   m_story_screen.init() &&
		   m_map.init() &&
		   m_char.init() &&
		   m_trophy.init() &&
		   m_complete_screen.init() &&
		   m_overlay.init(alert_mode);
}

// release all the associated resources
void World::destroy()
{
	glDeleteFramebuffers(1, &m_frame_buffer);

	if (m_background_music != nullptr)
		Mix_FreeMusic(m_background_music);
	if (m_char_dead_sound != nullptr)
		Mix_FreeChunk(m_char_dead_sound);
	if (m_char_green_sound != nullptr)
		Mix_FreeChunk(m_char_green_sound);
	if (m_char_win_sound != nullptr)
		Mix_FreeChunk(m_char_win_sound);

	Mix_CloseAudio();

	for (auto &spotter : m_spotters)
		spotter.destroy();
	for (auto &wanderer : m_wanderers)
		wanderer.destroy();
	m_wanderers.clear();
	m_spotters.clear();
	m_trophy.destroy();
	m_char.destroy();
	m_map.destroy();
	m_overlay.destroy();

	glfwDestroyWindow(m_window);
}

// update our game world
bool World::update(float elapsed_ms)
{
	int w, h;
	glfwGetFramebufferSize(m_window, &w, &h);
	vec2 screen = {(float)w / m_screen_scale, (float)h / m_screen_scale};

	m_start_screen.update(m_current_game_state);
	m_control_screen.update(m_current_game_state);
	m_story_screen.update(m_current_game_state);
	m_complete_screen.update(m_current_game_state);

	//////////////////////
	// COOLDOWN
	//////////////////////
	if (!(alert_mode_cooldown >= MAX_ALERT_MODE_COOLDOWN))
	{
		alert_mode_cooldown++;
	}
	else
	{
		alert_mode = false;
		// update spotters
		for (auto& spotter : m_spotters) {
			if (alert_mode) {
				spotter.alert_mode = false;
			}
		}

		// update wanderers
		for (auto& wanderer : m_wanderers) {
			wanderer.update(elapsed_ms* m_current_speed);
			wanderer.alert_mode = false;
		}

		// update shooters
		for (auto& shooter : m_shooters) {
			if (alert_mode) {
				shooter.alert_mode = false;
			}
		}
	}

	// IF ALERT MODE OVERLAY
	m_overlay.update_alert_mode(alert_mode);
	if (alert_mode) {
		m_overlay.oscillation();
	}

	if (m_game_state == LEVEL_1)
	{
		//////////////////////
		// COLLISION
		//////////////////////

		// collision, char-wall
		m_map.is_wall_collision(m_char);
		
		// TO REMOVE - placeholder for randomize path wall collision
		// collision, wanderer-wall
		for (auto &wanderer : m_wanderers)
			m_map.is_wall_collision(wanderer);

		// collision, char-spotter
		for (const auto &spotter : m_spotters)
			if (m_char.is_colliding(spotter) && is_char_detectable(m_map))
			{
				if (m_char.is_alive())
				{
					Mix_PlayChannel(-1, m_char_dead_sound, 0);
					m_map.set_char_dead();
				}
				m_char.kill();
				break;
			}

		// proximity, spotter
		for (auto& spotter : m_spotters)
			if (spotter.collision_with(m_char) && is_char_detectable(m_map))
			{
				if (m_char.is_alive())
				{
					alert_mode = true;
					spotter.alert_mode = true;
					alert_mode_cooldown = 0;
				}
				break;
			}


		// collision, char-wanderer
		for (const auto &wanderer : m_wanderers)
		{
			if (m_char.is_colliding(wanderer) && is_char_detectable(m_map))
			{
				if (m_char.is_alive())
				{
					Mix_PlayChannel(-1, m_char_dead_sound, 0);
					m_map.set_char_dead();
				}
				m_char.kill();
				break;
			}
		}

		// proximity, char-shooter
		for (auto& shooter : m_shooters)
		{
			if ((shooter.collision_with(m_char)) && is_char_detectable(m_map))
			{
				if (m_char.is_alive())
				{
					alert_mode = true;
					alert_mode_cooldown = 0;
					// ROTATE SHOOTER TO POINT AT M_CHAR
					float angle_to_char = atan((m_char.get_position().y - shooter.get_position().y) / (m_char.get_position().x - shooter.get_position().x));
					if (angle_to_char < 0) {
						if (m_char.get_position().y > shooter.get_position().y) {
							angle_to_char += 3.14;
						}
					}
				
					else if (m_char.get_position().x < shooter.get_position().x) {
						angle_to_char += 3.14;
					}
					shooter.set_rotation(angle_to_char);

					// SHOOTING AND COOLDOWN
					shooter.is_shooting = true;
					shooter.bullets.cooldown -= 15.f;
					if (shooter.bullets.cooldown < 0.f) {
						shooter.bullets.spawn_bullet(shooter.get_position(), angle_to_char);
						shooter.bullets.cooldown = 1500.f;
					}
				}
				break;
			}
		}



		// collision, char-trophy
		if (m_char.is_colliding(m_trophy))
		{
			if (m_char.is_alive())
			{
				Mix_PlayChannel(-1, m_char_win_sound, 0);
				m_map.set_char_dead();
				m_game_state = WIN_SCREEN;
			}
			m_char.kill();
		}

		//////////////////////
		// UPDATE
		//////////////////////

		// update char
		m_char.update(elapsed_ms);

		// update spotters
		for (auto& spotter : m_spotters) {
			if (alert_mode) {
				spotter.alert_mode = true;
			}
			spotter.update(elapsed_ms * m_current_speed);
		}

		// update wanderers
		for (auto& wanderer : m_wanderers) {
			wanderer.update(elapsed_ms* m_current_speed);
			// setting it to false : SET TRUE WHEN JOSE IS READY WITH CHASE AI
			wanderer.alert_mode = false;
		}

		// update shooter
		for (auto& shooter : m_shooters) {
			if (alert_mode) {
				shooter.alert_mode = true;
			}
			shooter.update(elapsed_ms * m_current_speed);
			if (shooter.is_shooting) {
				shooter.bullets.update(elapsed_ms * m_current_speed);
				if (m_char.is_colliding(shooter.bullets)) {
					m_char.set_color(1);
				}
			}
		}

	

		//////////////////////
		// DYNAMIC SPAWN
		//////////////////////

		// spawn spotter
		if (m_spotters.size() < MAX_SPOTTERS)
		{
			if (!spawn_spotter())
				return false;

			Spotter &new_spotter = m_spotters.back();

			new_spotter.set_position(spotter_loc[m_spotters.size() - 1]);
		}

		// spawn shooter
		if (m_shooters.size() < MAX_SHOOTERS)
		{
			if (!spawn_shooter())
				return false;

			Shooter& new_shooter = m_shooters.back();

			new_shooter.set_position(shooter_loc[m_shooters.size() - 1]);
		}

		// spawn wanderer
		m_next_wanderer_spawn -= elapsed_ms * m_current_speed;
		if (m_wanderers.size() < MAX_WANDERERS && m_next_wanderer_spawn < 0.f)
		{
			if (!spawn_wanderer())
				return false;

			Wanderer &new_wanderer = m_wanderers.back();

			// set initial position
			new_wanderer.set_position({screen.x - 50, 100 + m_dist(m_rng) * (screen.y - 100)});

			// next spawn
			m_next_wanderer_spawn = (SPOTTER_DELAY_MS / 2) + m_dist(m_rng) * (SPOTTER_DELAY_MS / 2);
		}

		//////////////////////
		// CONSEQUENCES
		//////////////////////

		// yellow
		if (m_map.get_flash_time() > 2)
		{
			m_map.reset_flash_time();
			m_map.set_flash(0);
		}

		// red
		if (m_char.is_dashing())
			if (m_char.is_wall_collision())
				m_char.set_dash(false);

		//////////////////////
		// RESET LEVEL
		//////////////////////

		if (!m_char.is_alive() && m_map.get_char_dead_time() > 2)
		{
			reset_game();
		}
		return true;
	}

	return true;
}

// render
// http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-14-render-to-texture/
void World::draw()
{
	// clear error buffer
	gl_flush_errors();

	// get size of window
	int w, h;
	glfwGetFramebufferSize(m_window, &w, &h);

	// update window title with points
	std::stringstream title_ss;
	title_ss << "The Chameleon";
	glfwSetWindowTitle(m_window, title_ss.str().c_str());

	// first render to the custom framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// clear backbuffer
	glViewport(0, 0, w, h);
	glDepthRange(0.00001, 10);
	const float clear_color[3] = {0.3f, 0.3f, 0.3f};
	glClearColor(clear_color[0], clear_color[1], clear_color[2], 1.0);
	glClearDepth(1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mat3 projection_2D = calculateProjectionMatrix(w, h);

	// game state
	switch (m_game_state)
	{
	case START_SCREEN:
		m_start_screen.draw(projection_2D);
		break;
	case CONTROL_SCREEN:
		m_control_screen.draw(projection_2D);
		break;
	case STORY_SCREEN:
		m_story_screen.draw(projection_2D);
		break;
	case LEVEL_1:
		// draw map
		m_map.draw(projection_2D);


		// draw entities
		for (auto &spotter : m_spotters)
			spotter.draw(projection_2D);
		for (auto &wanderer : m_wanderers)
			wanderer.draw(projection_2D);
		for (auto& shooter : m_shooters) {
			shooter.draw(projection_2D);
			if (shooter.is_shooting) {
				shooter.bullets.draw(projection_2D);
			}
		}
		m_trophy.draw(projection_2D);
		m_char.draw(projection_2D);

		m_overlay.draw(projection_2D);

		// bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_screen_tex.id);
		break;
	case WIN_SCREEN:
		m_complete_screen.draw(projection_2D);
		break;
	}

	// present
	glfwSwapBuffers(m_window);
}

mat3 World::calculateProjectionMatrix(int width, int height)
{
	float left = 0.f; // *-0.5;
	float top = 0.f;  // (float)h * -0.5;
	float right = 0.f;
	float bottom = 0.f;

	if (m_game_state != LEVEL_1)
	{
		right = (float)width / m_screen_scale;   // *0.5;
		bottom = (float)height / m_screen_scale; // *0.5;
	}
	else
	{
		left = m_char.get_position().x - ((float)width / (4 * m_screen_scale));
		top = m_char.get_position().y - ((float)height / (4 * m_screen_scale));
		right = m_char.get_position().x + ((float)width / (4 * m_screen_scale));
		bottom = m_char.get_position().y + ((float)height / (4 * m_screen_scale));
	}
	float sx = 2.f / (right - left);
	float sy = 2.f / (top - bottom);
	float tx = -(right + left) / (right - left);
	float ty = -(top + bottom) / (top - bottom);
	return {{sx, 0.f, 0.f}, {0.f, sy, 0.f}, {tx, ty, 1.f}};
}

bool World::is_over() const
{
	return glfwWindowShouldClose(m_window);
}

// spawn spotter
bool World::spawn_spotter()
{
	Spotter spotter;
	if (spotter.init())
	{
		m_spotters.emplace_back(spotter);
		return true;
	}
	fprintf(stderr, "Failed to spawn spotter");
	return false;
}

// spawn spotter
bool World::spawn_shooter()
{
	Shooter shooter;
	if (shooter.init())
	{
		shooter.bullets.init();
		m_shooters.emplace_back(shooter);
		return true;
	}
	fprintf(stderr, "Failed to spawn spotter");
	return false;
}

// spawn wanderer
bool World::spawn_wanderer()
{
	Wanderer wanderer;
	if (wanderer.init())
	{
		m_wanderers.emplace_back(wanderer);
		return true;
	}
	fprintf(stderr, "Failed to spawn wanderer");
	return false;
}

// key callback function
void World::on_key(GLFWwindow *, int key, int, int action, int mod)
{
	// start screen, control screen, story screen
	if (m_game_state != LEVEL_1)
	{
		if (action == GLFW_PRESS && key == GLFW_KEY_DOWN)
			if (m_current_game_state < 2)	m_current_game_state++;

		if (action == GLFW_PRESS && key == GLFW_KEY_UP)
			if (m_current_game_state > 0) m_current_game_state--;

		if (action == GLFW_PRESS && key == GLFW_KEY_ENTER)
		{
			if (m_game_state == STORY_SCREEN) m_game_state= LEVEL_1;
			else if (m_game_state == WIN_SCREEN) m_game_state = START_SCREEN;
			else if (m_current_game_state == 0)
			{
				// TO REMOVE -- need to fix bug where story screen shrinks upon winning
				m_show_story_screen ? m_game_state = STORY_SCREEN : m_game_state= LEVEL_1;
				m_show_story_screen = false;
			}
			else if (m_game_state == CONTROL_SCREEN) m_game_state = START_SCREEN;
			else m_game_state = m_current_game_state;
		}
	}

	// ESC: return to start screen
	if (action == GLFW_PRESS && key == GLFW_KEY_ESCAPE)
	{
		m_current_game_state = 0;
		m_game_state = START_SCREEN;
	}

	// movement, set movement
	if (action == GLFW_PRESS && m_game_state == LEVEL_1)
	{
		if ((key == GLFW_KEY_D && m_control == 0) || (key == GLFW_KEY_RIGHT && m_control == 1)) {
			m_char.set_direction('R', true);
			m_char.flip_char();
		}
		else if ((key == GLFW_KEY_A && m_control == 0) || (key == GLFW_KEY_LEFT && m_control == 1)) {
			m_char.set_direction('L', true);
			m_char.flip_char();
		}
		else if ((key == GLFW_KEY_W && m_control == 0) || (key == GLFW_KEY_UP && m_control == 1))
			m_char.set_direction('U', true);
		else if ((key == GLFW_KEY_S && m_control == 0) || (key == GLFW_KEY_DOWN && m_control == 1))
			m_char.set_direction('D', true);
	}

	// color, set color, consequences
	if (action == GLFW_PRESS && m_game_state == LEVEL_1)
	{
		// red
		if (((key == GLFW_KEY_UP && m_control == 0) || (key == GLFW_KEY_W && m_control == 1)) && m_char.get_color() != 1)
		{
			if (m_char.is_dashing()) return;
			m_char.set_color(1);
			m_char.set_dash(true);
		}
		// green
		else if (((key == GLFW_KEY_DOWN && m_control == 0) || (key == GLFW_KEY_S && m_control == 1)) && m_char.get_color() != 2)
		{
			if (m_char.is_dashing()) return;
			Mix_PlayChannel(-1, m_char_green_sound, 0);
			m_char.set_color(2);
			alert_mode = true;
			alert_mode_cooldown = 0;
		}
		// blue
		else if (((key == GLFW_KEY_LEFT && m_control == 0) || (key == GLFW_KEY_A && m_control == 1)) && m_char.get_color() != 3)
		{
			if (m_char.is_dashing()) return;
			m_char.set_color(3);
		}
		// yellow
		else if (((key == GLFW_KEY_RIGHT && m_control == 0) || (key == GLFW_KEY_D && m_control == 1)) && m_char.get_color() != 4)
		{
			if (m_char.is_dashing()) return;
			m_char.set_color(4);
			m_map.set_flash(1);
			alert_mode = true;
			alert_mode_cooldown = 0;
		}
	}

	// remove movement
	if (action == GLFW_RELEASE && m_game_state == LEVEL_1)
	{
		if ((key == GLFW_KEY_D && m_control == 0) || (key == GLFW_KEY_RIGHT && m_control == 1))
			m_char.set_direction('R', false);
		else if ((key == GLFW_KEY_A && m_control == 0) || (key == GLFW_KEY_LEFT && m_control == 1))
			m_char.set_direction('L', false);
		else if ((key == GLFW_KEY_W && m_control == 0) || (key == GLFW_KEY_UP && m_control == 1))
			m_char.set_direction('U', false);
		else if ((key == GLFW_KEY_S && m_control == 0) || (key == GLFW_KEY_DOWN && m_control == 1))
			m_char.set_direction('D', false);
	}

	// game mode
	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_1)
			m_control = 0;
		else if (key == GLFW_KEY_2)
			m_control = 1;
	}

	// reset
	if (action == GLFW_RELEASE && key == GLFW_KEY_R)
	{
		reset_game();
	}

	// game current speed
	if (action == GLFW_RELEASE && (mod & GLFW_MOD_SHIFT) && key == GLFW_KEY_COMMA)
		m_current_speed -= 0.1f;
	if (action == GLFW_RELEASE && (mod & GLFW_MOD_SHIFT) && key == GLFW_KEY_PERIOD)
		m_current_speed += 0.1f;

	m_current_speed = fmax(0.f, m_current_speed);
}

void World::on_mouse_move(GLFWwindow *window, double xpos, double ypos)
{
	//
}

bool World::is_char_detectable(Map m_map)
{
	return m_char.is_moving() || (m_map.get_tile(m_char) != m_char.get_color() + 1);
}

void World::reset_game()
{
	m_char.destroy();
	m_trophy.destroy();
	m_char.init();
	m_trophy.init();
	m_spotters.clear();
	m_wanderers.clear();
	m_shooters.clear();
	m_map.reset_char_dead_time();
	m_current_speed = 1.f;
	m_overlay.destroy();	
	alert_mode = false;
	m_overlay.init(alert_mode);
	
	// reset direction for every spotter
	for (auto& spotter : m_spotters) 
	{
		spotter.reset_direction();
	}
}