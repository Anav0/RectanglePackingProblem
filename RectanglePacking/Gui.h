#pragma once

#include "Shader.h"
#include "Renderer.h"

#include <string>
#include <map>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <ft2build.h>

#include FT_FREETYPE_H

struct Character {
	unsigned int TextureID; // ID handle of the glyph texture
	glm::ivec2   Size;      // Size of glyph
	glm::ivec2   Bearing;   // Offset from baseline to left/top of glyph
	unsigned int Advance;   // Horizontal offset to advance to next glyph
};

class Gui
{
public:
	std::map<GLchar, Character> Characters;
	
	Shader* shader;
	Renderer* renderer;

	bool init(int fontSize);
	void UseShader(Shader* shader);
	void DrawLabel(std::string text, float x, float y, float scale, glm::vec3 color, unsigned int* VBO, unsigned int* VAO);
};

