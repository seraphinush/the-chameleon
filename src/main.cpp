// internal
#include "common.hpp"
#include "world.hpp"

#define GL3W_IMPLEMENTATION
#include <gl3w.h>

// stlib
#include <chrono>
#include <iostream>

using Clock = std::chrono::high_resolution_clock;

// Global 
World world;
const int width = 1200;
const int height = 800;

const float TICK = 40.f;

// Entry point
int main(int argc, char* argv[])
{
	// Initializing world (after renderer.init().. sorry)
	if (!world.init({ (float)width, (float)height }))
	{
		// Time to read the error message
		std::cout << "Press any key to exit" << std::endl;
		std::cin.get();
		return EXIT_FAILURE;
	}

	auto t = Clock::now();

	float avg_ms = 0.f;
	float dt = 0.f;

	// variable timestep loop
	while (!world.is_over())
	{
		// Processes system messages, if this wasn't present the window would become unresponsive
		glfwPollEvents();

		// Calculating elapsed times in milliseconds from the previous iteration
		auto now = Clock::now();
		float ms = (float)(std::chrono::duration_cast<std::chrono::microseconds>(now - t)).count() / 1000;

		avg_ms = (avg_ms + ms) / 2;
		//fprintf(stderr, "cur:%f, avg:%f\n", ms, avg_ms);

		if (ms < TICK)
		{
			Sleep(TICK - ms);
		}

		t = now;

		world.update(ms - dt);
		world.draw();
		
		dt = 0.f;
	}

	world.destroy();

	return EXIT_SUCCESS;
}