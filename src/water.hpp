#pragma once

#include "common.hpp"

class Water : public Entity
{
public:
	int view_port[4];

	// create all the associated render resources and default transform
	bool init();

	// release all associated resources
	void destroy();

	// render the water
	void draw(const mat3& projection)override;

	// char dead time getters and setters
	void set_char_dead();

	void reset_char_dead_time();
	
	float get_char_dead_time() const;

private:
	// When char is alive, the time is set to -1
	float m_dead_time;
};
