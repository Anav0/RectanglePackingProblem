#pragma once

#include <vector>
#include "Renderer.h"

struct Rect {
	float x, y, w, h;
	int pos;
};

class EntityManager
{
public:
	Renderer *renderer;

	std::vector<Rect> Rects;
	std::vector<Rect> Grid;

	int total_number_of_rects = 0;

	void AddRectToVertices(Rect* rect);
	void UpdateVertexData(Rect* rect);
	void SetRectDimentions(float TL_x, float TL_y, float BR_x, float BR_y, Rect* rect);
};