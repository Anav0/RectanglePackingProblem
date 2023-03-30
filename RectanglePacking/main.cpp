#include <iostream>
#include <vector>
#include <math.h>
#include <algorithm>
#include <filesystem>
#include <map>

#include "EntityManager.h"
#include "Renderer.h"
#include "GlfwWindow.h"
#include "Palettes.h"
#include "Gameplay.h"
#include "Shader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <ft2build.h>

#include FT_FREETYPE_H

Renderer RENDERER{};
EntityManager ENTITY_MGR{};
Color defaultRectColor{ 1.0f, 0.43f, 0.20f };
Color defaultGridColor{ 0.05f, 0.69f, 0.78f };
GlfwWindow WINDOW_MGR{};

bool isDragging = false;
Grid* gridMouseWasIn = nullptr;
int colorPickIndex = 0;
const int FONT_SIZE = 24;

struct Character {
	unsigned int TextureID; // ID handle of the glyph texture
	glm::ivec2   Size;      // Size of glyph
	glm::ivec2   Bearing;   // Offset from baseline to left/top of glyph
	unsigned int Advance;   // Horizontal offset to advance to next glyph
};

std::map<GLchar, Character> Characters;

bool initFonts() {

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
	else {
		FT_Set_Pixel_Sizes(face, 0, FONT_SIZE);

		// disable byte-alignment restriction
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		// load first 128 characters of ASCII set
		for (unsigned char c = 0; c < 128; c++)
		{
			// Load character glyph 
			if (FT_Load_Char(face, c, FT_LOAD_RENDER))
			{
				std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
				continue;
			}
			// generate texture
			unsigned int texture;
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_RED,
				face->glyph->bitmap.width,
				face->glyph->bitmap.rows,
				0,
				GL_RED,
				GL_UNSIGNED_BYTE,
				face->glyph->bitmap.buffer
			);
			// set texture options
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			// now store character for later use
			Character character = {
			   texture,
			   glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			   glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			   static_cast<unsigned int>(face->glyph->advance.x)
			};
			Characters.insert(std::pair<char, Character>(c, character));
		}
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	// destroy FreeType once we're finished
	FT_Done_Face(face);
	FT_Done_FreeType(ft);
}

void ApplyTextureVertexLayout(unsigned int* VBO, unsigned int* VAO) {
	glGenVertexArrays(1, VAO);
	glGenBuffers(1, VBO);
	glBindVertexArray(*VAO);
	glBindBuffer(GL_ARRAY_BUFFER, *VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void AddGrid(double x, double y, int rows_n, int column_n, const unsigned int maxWidth, const unsigned int maxHeight, Color color) {

	std::vector<float>* vertices = &RENDERER.vertices;
	std::vector<unsigned int>* indices = &RENDERER.indices;
	float thickness = 2.0;

	float col_width = (static_cast<float>(maxWidth) / column_n) / WINDOW_MGR.SCR_WIDTH;
	float row_height = (static_cast<float>(maxHeight) / rows_n) / WINDOW_MGR.SCR_HEIGHT;

	int vertical_n = 0;
	int horizontal_n = 0;

	float normalized_height = static_cast<float>(maxHeight) / WINDOW_MGR.SCR_HEIGHT;
	float normalized_width = static_cast<float>(maxWidth) / WINDOW_MGR.SCR_WIDTH;

	Grid grid{};
	grid.cell_h = row_height;
	grid.cell_w = col_width;
	grid.line_thickness = thickness / maxHeight;
	grid.gap = col_width;
	grid.x = x;
	grid.y = y;
	grid.w = normalized_width;
	grid.h = normalized_height;
	grid.col_n = column_n;
	grid.row_n = rows_n;

	float x_p = x;

	while (vertical_n <= column_n) {
		Rect r{};
		r.color = color;
		r.x = x_p;
		r.y = y;
		r.h = normalized_height;
		r.w = thickness / maxHeight;
		r.pos = vertices->size();
		grid.lines_v.push_back(r);
		ENTITY_MGR.AddRectToVertices(&r);
		x_p += col_width;
		vertical_n++;
	}

	float y_p = y;
	while (horizontal_n <= rows_n) {
		Rect r{};
		r.color = color;
		r.y = y_p - normalized_height;
		r.x = x;
		r.w = normalized_width;
		r.h = thickness / maxWidth;
		r.pos = vertices->size();
		grid.lines_h.push_back(r);
		ENTITY_MGR.AddRectToVertices(&r);
		y_p += row_height;
		horizontal_n++;
	}

	ENTITY_MGR.Grids.push_back(grid);
}

void GetColAndRowUnderCursor(Grid* grid, float mouse_x, float mouse_y, int* row, int* col) {

	*row = 0;
	*col = 0;

	for (int i = 0; i < grid->lines_v.size(); i++) {
		if (grid->lines_v[i].x < mouse_x) *col += 1;
	}

	for (int i = 0; i < grid->lines_h.size(); i++) {
		if (grid->lines_h[i].y > mouse_y) *row += 1;
	}

	if (*row > 0) *row -= 1;
	if (*col > 0) *col -= 1;

}

void ReactToStateChanges() {

	if (isDragging && gridMouseWasIn != NULL) {
		int hovered_row = 0, hovered_col = 0;
		GetColAndRowUnderCursor(gridMouseWasIn, WINDOW_MGR.mouse_x, WINDOW_MGR.mouse_y, &hovered_row, &hovered_col);

		float cell_x, cell_y;
		GetCellTopLeftCornerPos(gridMouseWasIn, hovered_row, hovered_col, &cell_x, &cell_y);

		cell_x += gridMouseWasIn->cell_w - gridMouseWasIn->line_thickness;
		cell_y += (gridMouseWasIn->cell_h - gridMouseWasIn->line_thickness) * -1;

		ENTITY_MGR.SetRectDimentions(ENTITY_MGR.Rects[0].x, ENTITY_MGR.Rects[0].y, cell_x, cell_y, &ENTITY_MGR.Rects[0]);
	}

	if (WINDOW_MGR.buttonType == LEFT && WINDOW_MGR.buttonAction == PRESSED) {
		gridMouseWasIn = GetGridMouseIsIn(&ENTITY_MGR.Grids, WINDOW_MGR.mouse_x, WINDOW_MGR.mouse_y);
		if (gridMouseWasIn != NULL) {
			auto pallett_length = sizeof(PalletteA) / sizeof(Color);
			ENTITY_MGR.Rects[0].color = PalletteA[colorPickIndex % pallett_length];
			colorPickIndex++;
			isDragging = true;
			int hovered_row = 0, hovered_col = 0;
			GetColAndRowUnderCursor(gridMouseWasIn, WINDOW_MGR.mouse_x, WINDOW_MGR.mouse_y, &hovered_row, &hovered_col);
			SnapRectToGridCell(gridMouseWasIn, &ENTITY_MGR.Rects[0], hovered_row, hovered_col);
		}
	}

	if (gridMouseWasIn != NULL && WINDOW_MGR.buttonType == LEFT && WINDOW_MGR.buttonAction == RELEASED) {
		isDragging = false;

		if (!isOverlapping(&ENTITY_MGR.Rects[0], gridMouseWasIn->rects_on_this_grid)) {
			Rect r{};
			memcpy(&r, &ENTITY_MGR.Rects[0], sizeof(r));
			ENTITY_MGR.RegisterRect(r, gridMouseWasIn);
		}

		ENTITY_MGR.Rects[0].x = -1;
		ENTITY_MGR.Rects[0].y = -1;
		ENTITY_MGR.Rects[0].w = 0;
		ENTITY_MGR.Rects[0].h = 0;
	}
}

void RegisterDrawingRectAsFirstElement() {
	Rect r{};
	r.x = -1;
	r.y = -1;
	r.color = defaultRectColor;
	ENTITY_MGR.Rects.push_back(r);
	ENTITY_MGR.AddRectToVertices(&ENTITY_MGR.Rects[0]);
}

void RenderText(Shader& shader, std::string text, float x, float y, float scale, glm::vec3 color, unsigned int* VBO, unsigned int* VAO) {
	shader.use();
	glUniform3f(glGetUniformLocation(shader.ID, "textColor"), color.x, color.y, color.z);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(*VAO);

	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = Characters[*c];

		float xpos = x + ch.Bearing.x * scale;
		float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

		float w = ch.Size.x * scale;
		float h = ch.Size.y * scale;

		float vertices[6][4] = {
			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos,     ypos,       0.0f, 1.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },

			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },
			{ xpos + w, ypos + h,   1.0f, 0.0f }
		};
		glBindTexture(GL_TEXTURE_2D, ch.TextureID);

		glBindBuffer(GL_ARRAY_BUFFER, *VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDrawArrays(GL_TRIANGLES, 0, 6);
		// now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

int main()
{
	ENTITY_MGR.renderer = &RENDERER;

	if (!WINDOW_MGR.Init() || !RENDERER.BuildShaders() || !initFonts()) {
		return -1;
	}

	auto text_vs = "d:\\Projects\\RectanglePacking\\shaders\\text.vs";
	auto text_fs = "d:\\Projects\\RectanglePacking\\shaders\\text.fs";
	Shader shader(text_vs, text_fs);
	glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(WINDOW_MGR.SCR_WIDTH), 0.0f, static_cast<float>(WINDOW_MGR.SCR_HEIGHT));
	shader.use();
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	unsigned int VBO = 0, VAO = 0;
	unsigned int VBO2 = 0, VAO2 = 0;

	RegisterDrawingRectAsFirstElement();
	const int cells = 32;
	AddGrid(-0.9, 0.3, cells, cells, 1280, 1280, defaultGridColor);

	std::vector<DrawInfo> drawInfos = {
		{0, 0, 2, 2},
		{0, 0, 2, 2},
		{5, 5, 4, 4},
	};

	TryDrawingRectsOnGrid(&ENTITY_MGR.Grids[0], &ENTITY_MGR, drawInfos, PalletteA);

	ApplyTextureVertexLayout(&VBO2, &VAO2);
	while (!WINDOW_MGR.IsClosing())
	{
		WINDOW_MGR.onBeginOfTheLoop();
		glfwPollEvents();

		ReactToStateChanges();

		for (int i = 0; i < ENTITY_MGR.Rects.size(); i++) {
			ENTITY_MGR.UpdateVertexData(&ENTITY_MGR.Rects[i]);
		}
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		RenderText(shader, "This is sample text", 50.0f, 50.0f, 1.0f, glm::vec3(1.0, 1.0f, 1.0f), &VBO2, &VAO2);

		RENDERER.CreateVertexArray(&VBO, &VAO);
		RENDERER.Render(VAO);

		glfwSwapBuffers(WINDOW_MGR.WINDOW);
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(RENDERER.SHADER_PROGRAM);

	glfwTerminate();
	return 0;
}