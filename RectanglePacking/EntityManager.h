#pragma once

#include <vector>

struct Rect {
	float x, y, w, h;
	int pos;
};

class EntityManager
{
public:
	std::vector<Rect> Rects;
};