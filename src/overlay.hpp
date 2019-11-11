#pragma once

#include "common.hpp"


class Overlay : public Entity
{
public:
	// Creates all the associated render resources and default transform
	bool init();

	// Releases all associated resources
	void destroy();

	// Renders the overlay
	void draw(const mat3& projection)override;

	void oscillation();


private:
	// When salmon is alive, the time is set to -1
	float m_dead_time;
	bool alert_mode;
	float oscillation_value;
};
