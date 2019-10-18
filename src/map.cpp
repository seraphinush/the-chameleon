// Header
#include "map.hpp"

#include <cmath>

Texture Map::wall_texture;
Texture Map::corridor_texture;

// 800 * 1200 Map of Level 1
// 31 for the \n of all chars
char level_1[40][61] = {
	"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW",
	"WCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCW",
	"WCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCW",
	"WCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCW",
	"WCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCW",
	"WCCCCCWWWWWWWWWWWWWWWWWWWWCCCCCWWWWWWWWWWWCCCCCWWWWWWWCCCCCW",
	"WCCCCCWWWWWWWWWWWWWWWWWWWWCCCCCWWWWWWWWWWWCCCCCWWWWWWWCCCCCW",
	"WCCCCCWWWWWWWWWWWWWWWWWWWWCCCCCWWWWWWWWWWWCCCCCWWWWWWWCCCCCW",
	"WCCCCCWWWWWWWWWWWWWWWWWWWWCCCCCWWWWWWWWWWWCCCCCWWWWWWWCCCCCW",
	"WCCCCCWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWCCCCCWWWWWWWCCCCCW",
	"WCCCCCWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWCCCCCWWWWWWWCCCCCW",
	"WCCCCCWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWCCCCCWWWWWWWCCCCCW",
	"WCCCCCWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWCCCCCWWWWWWWCCCCCW",
	"WCCCCCWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWCCCCCWWWWWWWCCCCCW",
	"WCCCCCCCCCCCCCCCCCCCCCWWWWWWWWWWWWWCCCCCCCCCCCCCCCCCCCCCCCCW",
	"WCCCCCCCCCCCCCCCCCCCCCWWWWWWWWWWWWWCCCCCCCCCCCCCCCCCCCCCCCCW",
	"WCCCCCCCCCCCCCCCCCCCCCWWWWWWWWWWWWWCCCCCCCCCCCCCCCCCCCCCCCCW",
	"WCCCCCWWWWWWWWWWWWWCCCWWWWWWWWWWWWWCCCCCCCCCCCCCCCCCCCCCCCCW",
	"WCCCCCWWWWWWWWWWWWWCCCWWWWWWWWWWWWWCCCCCCCCCCCCCCCCCCCCCCCCW",
	"WCCCCCWWWWWWWWWWWWWCCCWWWWWWWWWWWWWCCCCCCCCCCCCCCCCWWWCCCCCW",
	"WCCCCCWWWWWWWWWWWWWCCCWWWWWWWWWWWWWCCCCCCCCCCCCCCCCWWWCCCCCW",
	"WCCCCCWWWWWWWWWWWWWCCCWWWWWWWWWWWWWCCCCCCCCCCCCCCCCWWWCCCCCW",
	"WCCCCCWWWWWWWWWWWWWCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCWWWCCCCCW",
	"WCCCCCWWWWWWWWWWWWWCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCWWWCCCCCW",
	"WCCCCCWWWWWWWWWWWWWCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCWWWCCCCCW",
	"WCCCCCWWWWWWWWWWWWWWWWWWCCCCCCCCCCCCCCCCCCCCCCCCCCCWWWCCCCCW",
	"WCCCCCWWWWWWWWWWWWWWWWWWCCCCCCCCCCCCCCCCCCCCCCCCCCCWWWCCCCCW",
	"WCCCCCWWWWWWWWWWWWWWWWWWCCCCCCCCCCCCCCCCCCCCCCCCCCCWWWCCCCCW",
	"WCCCCCWWWWWWWWWWWWWWWWWWCCCCCCCCCCCCCCCCCCCCCCCCCCCWWWCCCCCW",
	"WCCCCCWWWWWWWWWWWWWWWWWWCCCCCCCCCCCCCCCCCCCCCCCCCCCWWWCCCCCW",
	"WCCCCCWWWWWWWWWWWWWWWWWWCCCCCCCCCCCCCCCCCCCCCCCCCCCWWWCCCCCW",
	"WCCCCCWWWWWWWWWWWWWWWWWWCCCCCCCCCCCWWWWWWWWWWWWWWWWWWWCCCCCW",
	"WCCCCCWWWWWWWWWWWWWWWWWWCCCCCCCCCCCWWWWWWWWWWWWWWWWWWWCCCCCW",
	"WCCCCCWWWWWWWWWWWWWWWWWWCCCCCCCCCCCWWWWWWWWWWWWWWWWWWWCCCCCW",
	"WCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCW",
	"WCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCW",
	"WCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCW",
	"WCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCW",
	"WCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCW",
	"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW"
};

bool Map::init()
{
	// Load shared texture
	if (!wall_texture.is_valid())
	{
		if (!wall_texture.load_from_file(textures_path("wall_tile.png")))
		{
			fprintf(stderr, "Failed to load wall texture!");
			return false;
		}
	}

	// Load shared texture
	if (!corridor_texture.is_valid())
	{
		if (!corridor_texture.load_from_file(textures_path("corridor_tile.png")))
		{
			fprintf(stderr, "Failed to load corridor texture!");
			return false;
		}
	}

	// Vertex Buffer in local coordinates
	TexturedVertex vertices[4];
	// Top Left
	vertices[0].position = { 0.f, 20.f, 0.f };
	vertices[0].texcoord = { 0.f, 1.f };
	// Top Right
	vertices[1].position = { 20.f, 20.f, 0.f };
	vertices[1].texcoord = { 1.f, 1.f };
	// Bottom Right
	vertices[2].position = { 20.f, 0.f, 0.f };
	vertices[2].texcoord = { 1.f, 0.f };
	// Bottom Left
	vertices[3].position = { 0.f, 0.f, 0.f };
	vertices[3].texcoord = { 0.f, 0.f };

	// Counterclockwise as it's the default opengl front winding direction
	uint16_t indices[] = { 0, 3, 1, 1, 3, 2 };

	// Clearing errors
	gl_flush_errors();

	// Vertex Buffer creation
	glGenBuffers(1, &mesh.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TexturedVertex) * 4, vertices, GL_STATIC_DRAW);

	// Index Buffer creation
	glGenBuffers(1, &mesh.ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * 6, indices, GL_STATIC_DRAW);

	// Vertex Array (Container for Vertex + Index buffer)
	glGenVertexArrays(1, &mesh.vao);

	// Clearing errors
	gl_flush_errors();

	if (gl_has_errors())
		return false;

	// Loading shaders
	if (!effect.load_from_file(shader_path("map.vs.glsl"), shader_path("map.fs.glsl")))
		return false;

	// Setting initial values, scale is negative to make it face the opposite way
	// 1.0 would be as big as the original texture.
	physics.scale = { 1.0f, 1.0f };

	return true;
}

// Releases all graphics resources
void Map::destroy()
{
	glDeleteBuffers(1, &mesh.vbo);
	glDeleteBuffers(1, &mesh.ibo);
	glDeleteBuffers(1, &mesh.vao);

	glDeleteShader(effect.vertex);
	glDeleteShader(effect.fragment);
	glDeleteShader(effect.program);
}

void Map::draw(const mat3& projection) {

	translation_tile = vec2({ 0.0, 0.0 });

	for (int i = 0; i < 40; i++) {
		// Increment the row
		for (int j = 0; j < 61; j++) {
			if (level_1[i][j] == 'W') {
				// Draw a Wall
				draw_wall(projection);
			}
			else if (level_1[i][j] == 'C') {
				// Draw a Corridor
				draw_corridor(projection);
			}

			translation_tile.x += 20.0;
		}
		translation_tile.x = 0.0;
		translation_tile.y += 20.0;
	}
}

void Map::draw_wall(const mat3& projection)
{
	// Transformation code, see Rendering and Transformation in the template specification for more info
	// Incrementally updates transformation matrix, thus ORDER IS IMPORTANT
	transform.begin();
	transform.translate(translation_tile);
	transform.scale(physics.scale);
	transform.end();

	// Setting shaders
	glUseProgram(effect.program);

	// Enabling alpha channel for textures
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Depth
	glEnable(GL_DEPTH_TEST);

	// Getting uniform locations for glUniform* calls
	GLint transform_uloc = glGetUniformLocation(effect.program, "transform");
	GLint color_uloc = glGetUniformLocation(effect.program, "fcolor");
	GLint projection_uloc = glGetUniformLocation(effect.program, "projection");

	// Setting vertices and indices
	glBindVertexArray(mesh.vao);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);

	// Input data location as in the vertex buffer
	GLint in_position_loc = glGetAttribLocation(effect.program, "in_position");
	GLint in_texcoord_loc = glGetAttribLocation(effect.program, "in_texcoord");
	glEnableVertexAttribArray(in_position_loc);
	glEnableVertexAttribArray(in_texcoord_loc);
	glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)0);
	glVertexAttribPointer(in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)sizeof(vec3));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, wall_texture.id);

	// Setting uniform values to the currently bound program
	glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)& transform.out);
	float color[] = { 1.f, 1.f, 1.f };
	glUniform3fv(color_uloc, 1, color);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)& projection);

	// Drawing!
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
}


void Map::draw_corridor(const mat3& projection)
{
	// Transformation code, see Rendering and Transformation in the template specification for more info
	// Incrementally updates transformation matrix, thus ORDER IS IMPORTANT
	transform.begin();
	transform.translate(translation_tile);
	transform.scale(physics.scale);
	transform.end();

	// Setting shaders
	glUseProgram(effect.program);

	// Enabling alpha channel for textures
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Depth
	glEnable(GL_DEPTH_TEST);

	// Getting uniform locations for glUniform* calls
	GLint transform_uloc = glGetUniformLocation(effect.program, "transform");
	GLint color_uloc = glGetUniformLocation(effect.program, "fcolor");
	GLint projection_uloc = glGetUniformLocation(effect.program, "projection");

	// Setting vertices and indices
	glBindVertexArray(mesh.vao);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);

	// Input data location as in the vertex buffer
	GLint in_position_loc = glGetAttribLocation(effect.program, "in_position");
	GLint in_texcoord_loc = glGetAttribLocation(effect.program, "in_texcoord");
	glEnableVertexAttribArray(in_position_loc);
	glEnableVertexAttribArray(in_texcoord_loc);
	glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)0);
	glVertexAttribPointer(in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)sizeof(vec3));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, corridor_texture.id);

	// Setting uniform values to the currently bound program
	glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)& transform.out);
	float color[] = { 1.f, 1.f, 1.f };
	glUniform3fv(color_uloc, 1, color);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)& projection);

	// Drawing!
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
}

vec2 Map::get_position()const
{
	return motion.position;
}

void Map::set_position(vec2 position)
{
	motion.position = position;
}

float Map::collision_with(Char m_char) {
	vec2 char_position = m_char.get_position();
	int char_tile_x = (int) char_position.x / 20;
	int char_tile_y = (int) char_position.y / 20;

	if (level_1[char_tile_y][char_tile_x] == 'W') {
		return 1.0;
	}
	else if (level_1[char_tile_y][char_tile_x] == 'C') {
		return 2.0;
	}
	else {
		return 0.0;
	}
}


vec2 Map::get_bounding_box() const
{
	// Returns the local bounding coordinates scaled by the current size of the turtle 
	// fabs is to avoid negative scale due to the facing direction.
	return { std::fabs(physics.scale.x) * wall_texture.width, std::fabs(physics.scale.y) * wall_texture.height };
}