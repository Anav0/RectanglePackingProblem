#pragma once

#include <vector>
#include "Renderer.h"

struct Rect {
	float x, y, w, h;
	int pos;
};

struct Grid {
	float x, y, w, h, line_thickness, gap;
	int pos, col_n, row_n;
	std::vector<Rect> lines;
};

class EntityManager
{
public:
	Renderer *renderer;

	std::vector<Rect> Rects;
	std::vector<Grid> Grids;

	int total_number_of_rects = 0;

	void AddRectToVertices(Rect* rect);
	void UpdateVertexData(Rect* rect);
	void SetRectDimentions(float TL_x, float TL_y, float BR_x, float BR_y, Rect* rect);
};