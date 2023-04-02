#include "Gui.h"
#include "Shader.h"

bool Gui::init(int fontSize = 24) {

	//glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	FT_Library ft;
	if (FT_Init_FreeType(&ft))
	{
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
		return false;
	}

	std::string font_name = "d:\\Projects\\RectanglePacking\\fonts\\Rubik-Regular.ttf";

	FT_Face face;
	if (FT_New_Face(ft, font_name.c_str(), 0, &face)) {
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
		return false;
	}

	FT_Set_Pixel_Sizes(face, 0, fontSize);

	// disable byte-alignment restriction
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	for (unsigned char c = 0; c < 128; c++)
	{
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}

		Character character = {
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			static_cast<unsigned int>(face->glyph->advance.x)
		};

		Characters.insert(std::pair<char, Character>(c, character));

		// generate texture
		//unsigned int texture;
		//glGenTextures(1, &texture);
		//glBindTexture(GL_TEXTURE_2D, texture);
		//glTexImage2D(
		//	GL_TEXTURE_2D,
		//	0,
		//	GL_RED,
		//	face->glyph->bitmap.width,
		//	face->glyph->bitmap.rows,
		//	0,
		//	GL_RED,
		//	GL_UNSIGNED_BYTE,
		//	face->glyph->bitmap.buffer
		//);
		//// set texture options
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// now store character for later use

	}

	//glBindTexture(GL_TEXTURE_2D, 0);
	// destroy FreeType once we're finished
	FT_Done_Face(face);
	FT_Done_FreeType(ft);
}

void Gui::UseShader(Shader* shader)
{
	this->shader = shader;
	this->shader->use();
}

void Gui::BindVertexArray(unsigned int* texture, unsigned int* VAO, unsigned int* VBO, glm::vec3 color)
{
	glUniform3f(glGetUniformLocation(shader->ID, "textColor"), color.x, color.y, color.z);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(*VAO);

	glBindTexture(GL_TEXTURE_2D, *texture);

	glBindBuffer(GL_ARRAY_BUFFER, *VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), &vertices);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Gui::Draw() {
	glDrawElements(GL_TRIANGLES, vertices.size(), GL_UNSIGNED_INT, 0);
}

void Gui::DrawLabel(std::string text, float x, float y, float scale, glm::vec3 color, unsigned int* VBO, unsigned int* VAO) {
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = Characters[*c];

		float xpos = x + ch.Bearing.x * scale;
		float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

		float w = ch.Size.x * scale;
		float h = ch.Size.y * scale;

		//1
		vertices.push_back(xpos);
		vertices.push_back(ypos + h);
		vertices.push_back(0.0f);
		vertices.push_back(0.0f);

		//2
		vertices.push_back(xpos);
		vertices.push_back(ypos);
		vertices.push_back(0.0f);
		vertices.push_back(1.0f);

		//3
		vertices.push_back(xpos + w);
		vertices.push_back(ypos);
		vertices.push_back(1.0f);
		vertices.push_back(1.0f);

		//4
		vertices.push_back(xpos);
		vertices.push_back(ypos + h);
		vertices.push_back(0.0f);
		vertices.push_back(0.0f);

		//5
		vertices.push_back(xpos + w);
		vertices.push_back(ypos);
		vertices.push_back(1.0f);
		vertices.push_back(1.0f);

		//6
		vertices.push_back(xpos + w);
		vertices.push_back(ypos + h);
		vertices.push_back(1.0f);
		vertices.push_back(0.0f);

		unsigned int texture;
		BindVertexArray(&texture, VAO, VBO, color);

		x += (ch.Advance >> 6) * scale;
	}
	//glBindVertexArray(0);
	//glBindTexture(GL_TEXTURE_2D, 0);
}