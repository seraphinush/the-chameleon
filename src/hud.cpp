// header
#include "hud.hpp"

// stdlib
#include <algorithm>
#include <cmath>
#include <string>
#include <iostream>

Texture Hud::texture_hud;
Texture Hud::texture_tip;

using namespace std;

bool Hud::init()
{
  // load shared texture
  if (!texture_hud.is_valid())
  {
    if (!texture_hud.load_from_file(textures_path("hud.png")))
    {
      fprintf(stderr, "Failed to load hud texture!");
      return false;
    }
  }

  if (!texture_tip.is_valid())
  {
    if (!texture_tip.load_from_file(textures_path("tooltip_red.png")))
    {
      fprintf(stderr, "Failed to load tooltip texture!");
      return false;
    }
  }

	// the position corresponds to the center of the texture
	float wr = std::max(SCREEN_WIDTH, SCREEN_HEIGHT) * 0.5f;
	float hr = std::max(SCREEN_WIDTH, SCREEN_HEIGHT) * 0.5f;

  TexturedVertex vertices[4];
  vertices[0].position = {-wr, +hr, -0.0f};
  vertices[0].texcoord = {0.f, 1.f};
  vertices[1].position = {+wr, +hr, -0.02f};
  vertices[1].texcoord = {1.f, 1.f};
  vertices[2].position = {+wr, -hr, -0.02f};
  vertices[2].texcoord = {1.f, 0.f};
  vertices[3].position = {-wr, -hr, -0.02f};
  vertices[3].texcoord = {0.f, 0.f};

  uint16_t indices[] = {0, 3, 1, 1, 3, 2};

  // clear errors
  gl_flush_errors();

  // vertex buffer creation
  glGenBuffers(1, &mesh.vbo);
  glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(TexturedVertex) * 4, vertices, GL_STATIC_DRAW);

  // index buffer creation
  glGenBuffers(1, &mesh.ibo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * 6, indices, GL_STATIC_DRAW);

  // vertex array (container for vertex + index buffer)
  glGenVertexArrays(1, &mesh.vao);
  if (gl_has_errors())
    return false;

  // load shaders
  if (!effect.load_from_file(shader_path("textured.vs.glsl"), shader_path("textured.fs.glsl")))
    return false;

  return true;
}

// release all graphics resources
void Hud::destroy()
{
  glDeleteBuffers(1, &mesh.vbo);
  glDeleteBuffers(1, &mesh.ibo);
  glDeleteVertexArrays(1, &mesh.vao);

	effect.release();
}

void Hud::update(unsigned int game_state)
{
  m_game_state = game_state;
}

// TODO -- duplicate usage of program
void Hud::draw(const mat3 &proj) {}

void Hud::draw(const mat3 &proj, vec2 wsize, const vec2 wpoint)
{
  float wscale = (float)SCREEN_WIDTH * ((float)SCREEN_WIDTH / wsize.x);
  
	vec2 hud_scale = vec2({0.f,0.f});
	vec2 hud_trans = vec2({0.f,0.f});

	hud_scale = vec2({(float)(texture_hud.width / 2) / wscale, (float)(texture_hud.height / 2) / wscale});

  if (m_game_state % 1000 == 0)
  {
    hud_trans.x = wpoint.x + wsize.x - wsize.x / 10;
    hud_trans.y = wpoint.y + wsize.y - wsize.y / 10;
  }
  else
  {
    hud_trans.x = (float)SCREEN_WIDTH - (float)(texture_hud.width / 2);
    hud_trans.y = (float)SCREEN_HEIGHT - (float)(texture_hud.height / 2);
  }

  draw_element(proj, texture_hud, hud_trans, hud_scale);
}

void Hud::draw_element(const mat3& proj, const Texture& texture, vec2 pos, vec2 scale)
{
	// transformation
	transform.begin();
	transform.translate(pos);
	transform.scale(scale);
	transform.end();

	// set shaders
	glUseProgram(effect.program);

	// enable alpha channel for textures
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// depth
	glEnable(GL_DEPTH_TEST);

	// get uniform locations for glUniform* calls
	GLint transform_uloc = glGetUniformLocation(effect.program, "transform");
	GLint color_uloc = glGetUniformLocation(effect.program, "fcolor");
	GLint projection_uloc = glGetUniformLocation(effect.program, "projection");

	// set vertices and indices
	glBindVertexArray(mesh.vao);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);

	// input data location as in the vertex buffer
	GLint in_position_loc = glGetAttribLocation(effect.program, "in_position");
	GLint in_texcoord_loc = glGetAttribLocation(effect.program, "in_texcoord");
	glEnableVertexAttribArray(in_position_loc);
	glEnableVertexAttribArray(in_texcoord_loc);
	glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void *)0);
	glVertexAttribPointer(in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void *)sizeof(vec3));

	// enable and binding texture to slot 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture.id);

	// set uniform values to the currently bound program
	glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float *)&transform.out);
	float color[] = {1.f, 1.f, 1.f};
	glUniform3fv(color_uloc, 1, color);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float *)&proj);

	// draw
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
}

// TODO
void Hud::set_tooltip(char color, bool value)
{
  switch (color)
  {
  case 'R':
    show_red_tooltip = value;
    if (value)
    {
      texture_tip.~Texture();
      texture_tip.load_from_file(textures_path("tooltip_red.png"));
    }
    break;
  case 'G':
    show_green_tooltip = value;
    if (value)
    {
      texture_tip.~Texture();
      texture_tip.load_from_file(textures_path("tooltip_green.png"));
    }
    break;
  case 'B':
    show_blue_tooltip = value;
    if (value)
    {
      texture_tip.~Texture();
      texture_tip.load_from_file(textures_path("tooltip_blue.png"));
    }
    break;
  case 'Y':
    show_yellow_tooltip = value;
    if (value)
    {
      texture_tip.~Texture();
      texture_tip.load_from_file(textures_path("tooltip_yellow.png"));
    }
    break;
  }
}
