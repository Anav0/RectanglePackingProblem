#include <iostream>
#include <vector>
#include <math.h>
#include <algorithm>

#include "EntityManager.h"
#include "Renderer.h"
#include "GlfwWindow.h"

Renderer RENDERER{};
EntityManager ENTITY_MGR{};

GlfwWindow WINDOW_MGR{};

bool isDragging = false;
const double STEP = 20;

bool isPointInRect(Rect* r, float x, float y) {
	float max_x = r->x + r->w;
	float min_x = r->x;

	float max_y = r->y;
	float min_y = r->y - r->h;

	return x <= max_x && x >= min_x && y >= min_y && y <= max_y;
}

bool isOverlapping(Rect* rect, std::vector<Rect> rects) {
	float rect_x1 = rect->x;
	float rect_x2 = rect->x + rect->w;
	float rect_y1 = rect->y - rect->h;
	float rect_y2 = rect->y;

	//First element is the one we drag
	for (size_t i = 1; i < rects.size(); i++)
	{
		auto& r = rects[i];
		float r_x1 = r.x;
		float r_x2 = r.x + r.w;
		float r_y1 = r.y - r.h;
		float r_y2 = r.y;

		if (rect_x1 < r_x2 && rect_x2 > r_x1 &&
			rect_y1 < r_y2 && rect_y2 > r_y1)
			return true;
	}

	return false;
}

void AddGrid(double x, double y, int rows_n, int column_n, const unsigned int maxWidth, const unsigned int maxHeight) {

	std::vector<float>* vertices       = &RENDERER.vertices;
	std::vector<unsigned int>* indices = &RENDERER.indices;
	float thickness = 2.0;

	float col_width  = (static_cast<float>(maxWidth) / column_n) / WINDOW_MGR.SCR_WIDTH;
	float row_height = (static_cast<float>(maxHeight) / rows_n) / WINDOW_MGR.SCR_HEIGHT;

	int vertical_n = 0;
	int horizontal_n = 0;

	float normalized_height = static_cast<float>(maxHeight) / WINDOW_MGR.SCR_HEIGHT;
	float normalized_width  = static_cast<float>(maxWidth) / WINDOW_MGR.SCR_WIDTH;

	Grid grid{};
	grid.line_thickness = thickness;
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
		r.x = x_p;
		r.y = y + normalized_height;
		r.h = normalized_height;
		r.w = thickness / maxHeight;
		r.pos = vertices->size();
		grid.lines.push_back(r);
		ENTITY_MGR.AddRectToVertices(&r);
		x_p += col_width;
		vertical_n++;
	}
	
	float y_p = y;
	while (horizontal_n <= rows_n) {
		Rect r{};
		r.y = y_p;
		r.x = x;
		r.w = normalized_width;
		r.h = thickness / maxWidth;
		r.pos = vertices->size();
		grid.lines.push_back(r);
		ENTITY_MGR.AddRectToVertices(&r);
		y_p += row_height;
		horizontal_n++;
	}

	ENTITY_MGR.Grids.push_back(grid);
}

void ReactToStateChanges() {
	if (isDragging) {
		double adjusted_x = WINDOW_MGR.mouse_x;
		double adjusted_y = WINDOW_MGR.mouse_y;
		MoveToClosestStep(STEP, &adjusted_x, &adjusted_y, WINDOW_MGR.SCR_WIDTH, WINDOW_MGR.SCR_HEIGHT);

		ENTITY_MGR.SetRectDimentions(ENTITY_MGR.Rects[0].x, ENTITY_MGR.Rects[0].y, adjusted_x, adjusted_y, &ENTITY_MGR.Rects[0]);
	}

	if (WINDOW_MGR.buttonType == LEFT && WINDOW_MGR.buttonAction == PRESSED) {
		isDragging = true;

		ENTITY_MGR.Rects[0].x = WINDOW_MGR.mouse_x;
		ENTITY_MGR.Rects[0].y = WINDOW_MGR.mouse_y;
	}

	if (WINDOW_MGR.buttonType == LEFT && WINDOW_MGR.buttonAction == RELEASED) {
		isDragging = false;

		if (!isOverlapping(&ENTITY_MGR.Rects[0], ENTITY_MGR.Rects)) {
			Rect r{};
			memcpy(&r, &ENTITY_MGR.Rects[0], sizeof(r));
			r.pos = RENDERER.vertices.size();
			ENTITY_MGR.Rects.push_back(r);
			ENTITY_MGR.AddRectToVertices(&r);
		}

		ENTITY_MGR.Rects[0].x = -1;
		ENTITY_MGR.Rects[0].y = -1;
		ENTITY_MGR.Rects[0].w = 0;
		ENTITY_MGR.Rects[0].h = 0;
	}
}

void RegisterDrawingRectAsFirstElement() {
	ENTITY_MGR.Rects.push_back(Rect{ -1, -2, 0, 0, 0 });
	ENTITY_MGR.AddRectToVertices(&ENTITY_MGR.Rects[0]);
}

int main()
{
	ENTITY_MGR.renderer = &RENDERER;

	if (!WINDOW_MGR.Init() || !RENDERER.BuildShaders()) {
		return -1;
	}
	unsigned int VBO, VAO;

	RegisterDrawingRectAsFirstElement();
	AddGrid(-0.9, -0.9, 20, 20, 1000, 1000);

	while (!WINDOW_MGR.IsClosing())
	{
		WINDOW_MGR.onBeginOfTheLoop();
		glfwPollEvents();

		ReactToStateChanges();

		for (int i = 0; i < ENTITY_MGR.Rects.size(); i++) {
			ENTITY_MGR.UpdateVertexData(&ENTITY_MGR.Rects[i]);
		}

		RENDERER.CreateVertexArray(&VBO, &VAO);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		RENDERER.Render(VAO);

		glfwSwapBuffers(WINDOW_MGR.WINDOW);
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(RENDERER.SHADER_PROGRAM);

	glfwTerminate();
	return 0;
}