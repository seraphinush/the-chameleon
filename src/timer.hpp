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


private:
	GLuint m_instance_vbo;
	int seconds = 0;
	int minutes = 0;
};