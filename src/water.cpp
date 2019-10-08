#include "water.hpp"

#include <iostream>

bool Water::init() {
	glGetIntegerv(GL_VIEWPORT, view_port);

	m_dead_time = -1;

	// since we are not going to apply transformation to this screen geometry
	// the coordinates are set to fill the standard openGL window [-1, -1 .. 1, 1]
	// make the size slightly larger then the screen to crop the boundary.
	static const GLfloat screen_vertex_buffer_data[] = {
		-1.05f, -1.05f, 0.0f,
		1.05f, -1.05f, 0.0f,
		-1.05f,  1.05f, 0.0f,
		-1.05f,  1.05f, 0.0f,
		1.05f, -1.05f, 0.0f,
		1.05f,  1.05f, 0.0f,
	};

	// clear errors
	gl_flush_errors();

	// vertex buffer creation
	glGenBuffers(1, &mesh.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(screen_vertex_buffer_data), screen_vertex_buffer_data, GL_STATIC_DRAW);

	if (gl_has_errors())
		return false;

	// load shaders
	if (!effect.load_from_file(shader_path("water.vs.glsl"), shader_path("water.fs.glsl")))
		return false;

	return true;
}

// release all graphics resources
void Water::destroy() {
	glDeleteBuffers(1, &mesh.vbo);

	glDeleteShader(effect.vertex);
	glDeleteShader(effect.fragment);
	glDeleteShader(effect.program);
}

void Water::draw(const mat3& projection) {

	// enable alpha channel for textures
	glEnable(GL_BLEND); 
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// depth
	glEnable(GL_DEPTH_TEST);

	// set shaders
	glUseProgram(effect.program);

	// set screen_texture sampling to texture unit 0
	// set clock
	GLuint screen_text_uloc = glGetUniformLocation(effect.program, "screen_texture");
	GLuint time_uloc = glGetUniformLocation(effect.program, "time");
	GLuint dead_timer_uloc = glGetUniformLocation(effect.program, "dead_timer");

	GLuint window_width_uloc = glGetUniformLocation(effect.program, "window_width");
	GLuint window_height_uloc = glGetUniformLocation(effect.program, "window_height");

	glUniform1i(screen_text_uloc, 0);
	glUniform1f(time_uloc, (float)(glfwGetTime() * 10.0f));
	glUniform1f(dead_timer_uloc, (m_dead_time > 0) ? (float)((glfwGetTime() - m_dead_time) * 10.0f) : -1);
	glUniform1i(window_width_uloc, view_port[2]);
	glUniform1i(window_height_uloc, view_port[3]);

	// draw the screen texture on the quad geometry
	// set vertices
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);

	// bind to attribute 0 (in_position) as in the vertex shader
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	// draw
	glDrawArrays(GL_TRIANGLES, 0, 6); // 2*3 indices starting at 0 -> 2 triangles
	glDisableVertexAttribArray(0);
}

void Water::set_char_dead() {
	m_dead_time = glfwGetTime();
}

void Water::reset_char_dead_time() {
	m_dead_time = -1;
}

float Water::get_char_dead_time() const {
	return glfwGetTime() - m_dead_time;
}