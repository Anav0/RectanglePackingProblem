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

void AddGrid(double thickness, double gap, const unsigned int maxWidth, const unsigned int maxHeight) {

	std::vector<float>* vertices         = &RENDERER.vertices;
	std::vector<unsigned int>* indices = &RENDERER.indices;

	float x = -1;
	float tp = (thickness + gap) / maxWidth;//thickness + gap;

	int n1 = 0, n2 = 0;
	while (x <= maxWidth) {
		Rect r{};
		r.x = x;
		r.y = 1.0;
		r.h = 2;
		r.w = thickness / maxWidth;// thickness;
		r.pos = vertices->size();
		ENTITY_MGR.Rects.push_back(r);
		ENTITY_MGR.AddRectToVertices(&r);
		x += tp;
		n1++;
	}

	float y = -1;
	tp = (thickness + gap) / maxHeight;//thickness + gap;
	while (y <= maxHeight) {
		Rect r{};
		r.y = y;
		r.x = -1.0;
		r.w = 2;
		r.h = thickness / maxHeight;// thickness;
		r.pos = vertices->size();
		ENTITY_MGR.Rects.push_back(r);
		ENTITY_MGR.AddRectToVertices(&r);
		y += tp;
		n2++;
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
	//AddGrid(2.0, STEP, WINDOW_MGR.SCR_WIDTH, WINDOW_MGR.SCR_HEIGHT);

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



