#define _USE_MATH_DEFINES

// header
#include "timer.hpp"

// stdlib
#include <cmath>
#include <iostream>
#include <ft2build.h>
#include FT_FREETYPE_H


bool Timer::init()
{
  seconds = 0;
	minutes = 0;
	
	return true;
}

// release all graphics resources
void Timer::destroy()
{
	glDeleteBuffers(1, &mesh.vbo);
	glDeleteBuffers(1, &m_instance_vbo);

	effect.release();
}

void Timer::update(float ms)
{
	// TODO Update timer
  if((int) glfwGetTime() == 0){
    seconds = 0;
	  minutes = 0;
  }

  int secs_in_min = 10;
  seconds = (int) glfwGetTime() ;
  minutes = seconds / secs_in_min;
  seconds = seconds % secs_in_min;

  // fprintf(stderr, " timer - %02i:%02i \n",minutes, seconds);
}

void Timer::draw(const mat3 &projection)
{
	// fprintf(stderr, "Draw timer");

	
}
