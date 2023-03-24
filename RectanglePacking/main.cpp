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

void AddGrid(double x, double y, double thickness, double gap, const unsigned int maxWidth, const unsigned int maxHeight) {

	std::vector<float>* vertices       = &RENDERER.vertices;
	std::vector<unsigned int>* indices = &RENDERER.indices;

	float tp = (thickness + gap) / maxWidth;
	int howManyHorizontalLines = (maxHeight / (thickness + gap))*2;
	int howManyVerticalLines   = (maxWidth / (thickness + gap))*2;
	int vertical_n = 0;
	float normalized_height = static_cast<float>(maxHeight) / WINDOW_MGR.SCR_HEIGHT;
	float normalized_width  = static_cast<float>(maxWidth) / WINDOW_MGR.SCR_WIDTH;
	while (vertical_n <= howManyVerticalLines) {
		Rect r{};
		r.x = x;
		r.y = y;
		r.h = normalized_height;
		r.w = thickness / maxWidth;
		r.pos = vertices->size();
		ENTITY_MGR.Grid.push_back(r);
		ENTITY_MGR.AddRectToVertices(&r);
		x += tp;
		vertical_n++;
	}
	int horizontal_n = 0;
	tp = (thickness + gap) / maxHeight;
	while (horizontal_n <= howManyHorizontalLines) {
		Rect r{};
		r.y = y;
		r.x = x;
		r.w = normalized_width;
		r.h = thickness / maxWidth;
		r.pos = vertices->size();
		ENTITY_MGR.Grid.push_back(r);
		ENTITY_MGR.AddRectToVertices(&r);
		y += tp;
		horizontal_n++;
	}
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
	//AddGrid(0.0, 0.0, 2.0, STEP, WINDOW_MGR.SCR_WIDTH, WINDOW_MGR.SCR_HEIGHT);
	AddGrid(0.0, 0.0, 2.0, STEP, 200, 200);

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