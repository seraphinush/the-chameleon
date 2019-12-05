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
	// fprintf(stderr, "Update timer");
  if((int) glfwGetTime() == 0){
    seconds = 0;
	  minutes = 0;
  }

  int secs_in_min = 5;
  if (!paused){
    seconds = (int) glfwGetTime() - off;
    if(minutes > 0)
        seconds = seconds - (minutes * secs_in_min);
    if(seconds > (secs_in_min -1)){
        minutes += 1;
        seconds = seconds % secs_in_min;
    }
  }
  fprintf(stderr, "Update timer - %02i:%02i \n",minutes, seconds);
}

void Timer::draw(const mat3 &projection)
{
	// fprintf(stderr, "Draw timer");

	
}
void Timer::reset()
{
	// fprintf(stderr, "Draw timer");
  glfwSetTime(0);
}

void Timer::pause(bool value)
{
	// fprintf(stderr, "Draw timer");
  paused = value;
  if(value && pause_time == 0)
    pause_time = (int) glfwGetTime();
  else if (pause_time !=0 && !value){
      off += (int) glfwGetTime() - pause_time;
      pause_time = 0;
  }
}

