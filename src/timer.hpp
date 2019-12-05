#pragma once

// internal
#include "common.hpp"

// stdlib
#include <vector>

class Timer : public Entity
{
public:
	bool init();
	void destroy();
	void update(float ms);
	void draw(const mat3 &projection) override;

	void reset();
	void pause(bool value);

private:
	GLuint m_instance_vbo;
	int seconds = 0;
	int minutes = 0;
	int off = 0;
	int pause_time = 0;
	bool paused = false;
};