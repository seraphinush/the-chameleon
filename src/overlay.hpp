#pragma once

#include "common.hpp"


class Overlay : public Entity
{
public:
	int view_port[4];

	// Creates all the associated render resources and default transform
	bool init(bool in_alert_mode, int max_cooldown);

	// Releases all associated resources
	void destroy();

	// Renders the overlay
	void draw(const mat3& projection)override;

	void oscillation();

	void set_cooldown(int value);

	void update_alert_mode(bool in_alert_mode);


private:
	// When salmon is alive, the time is set to -1
	float m_dead_time;
	bool alert_mode;
	float oscillation_value;
	int cooldown;
	int MAX_COOLDOWN;
};
