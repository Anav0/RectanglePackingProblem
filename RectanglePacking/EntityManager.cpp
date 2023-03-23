#include "EntityManager.h"

void EntityManager::AddRectToVertices(Rect* rect)
{
	std::vector<float>* vertices = &renderer->vertices;
	std::vector<unsigned int>* indices  = &renderer->indices;

	auto lengthBefore = vertices->size() / 3;

	vertices->push_back(rect->x + rect->w); //TR
	vertices->push_back(rect->y);
	vertices->push_back(0.0f);

	vertices->push_back((rect->x + rect->w)); //BR
	vertices->push_back(rect->y - rect->h);
	vertices->push_back(0.0f);

	vertices->push_back(rect->x); //BL
	vertices->push_back(rect->y - rect->h);
	vertices->push_back(0.0f);

	vertices->push_back(rect->x); //TL
	vertices->push_back(rect->y);
	vertices->push_back(0.0f);

	indices->push_back(lengthBefore);
	indices->push_back(lengthBefore + 1);
	indices->push_back(lengthBefore + 3);
	indices->push_back(lengthBefore + 1);
	indices->push_back(lengthBefore + 2);
	indices->push_back(lengthBefore + 3);
}

void EntityManager::UpdateVertexData(Rect* rect)
{
	std::vector<float>* vertices = &renderer->vertices;
	std::vector<unsigned int>* indices = &renderer->indices;

	int index = rect->pos;

	vertices->at(index++) = rect->x + rect->w; //TR
	vertices->at(index++) = rect->y;
	vertices->at(index++) = 0.0f;
			
	vertices->at(index++) = rect->x + rect->w; //BR
	vertices->at(index++) = rect->y - rect->h;
	vertices->at(index++) = 0.0f;
			
	vertices->at(index++) = rect->x; //BL
	vertices->at(index++) = rect->y - rect->h;
	vertices->at(index++) = 0.0f;
			
	vertices->at(index++) = rect->x; //TL
	vertices->at(index++) = rect->y;
	vertices->at(index++) = 0.0f;
}

void EntityManager::SetRectDimentions(float TL_x, float TL_y, float BR_x, float BR_y, Rect* rect) {

	float left   = std::min(TL_x, BR_x);
	float right  = std::max(TL_x, BR_x);
	float top    = std::min(TL_y, BR_y);
	float bottom = std::max(TL_y, BR_y);

	float width  = right  - left;
	float height = bottom - top;

	rect->w = width;
	rect->h = height;
}
