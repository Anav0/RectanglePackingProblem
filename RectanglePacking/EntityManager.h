#pragma once

#include <vector>
#include <string>

#include "Renderer.h"

struct Color {
	float r, g, b;
};

struct Rect {
	float x, y, w, h;
	int pos;
	Color color;
};

struct Grid {
	float x, y, w, h, line_thickness, gap;
	int pos, col_n, row_n;
	float cell_h, cell_w;
	std::vector<Rect> lines_v;
	std::vector<Rect> lines_h;
	std::vector<Rect> rects_on_this_grid;
};

class EntityManager
{
public:
	Renderer *renderer;

	std::vector<Rect> Rects;
	std::vector<Grid> Grids;

	int total_number_of_rects = 0;

	void RegisterRect(Rect rect);
	void RegisterRect(Rect rect, Grid* grid);
	void AddRectToVertices(Rect* rect);
	void UpdateVertexData(Rect* rect);
	void SetRectDimentions(float TL_x, float TL_y, float BR_x, float BR_y, Rect* rect);
};