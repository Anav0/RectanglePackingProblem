#pragma once

bool isPointInRect(Rect* r, float x, float y) {
	float max_x = r->x + r->w;
	float min_x = r->x;

	float max_y = r->y;
	float min_y = r->y - r->h;

	return x <= max_x && x >= min_x && y >= min_y && y <= max_y;
}

bool isPointInRect(float x, float y, float w, float h, float p_x, float p_y) {
	float max_x = x + w;
	float min_x = x;

	float max_y = y;
	float min_y = y - h;

	return p_x <= max_x && p_x >= min_x && p_y >= min_y && y <= max_y;
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

bool AreOverlapping(float x_1, float y_1, float h_1, float w_1, float x_2, float y_2, float h_2, float w_2) {
	float rect_x1 = x_1;
	float rect_x2 = x_1 + w_1;
	float rect_y1 = y_1 - h_1;
	float rect_y2 = y_1;
	
	float r_x1 = x_2;
	float r_x2 = x_2 + w_2;
	float r_y1 = y_2 - h_2;
	float r_y2 = y_2;

	if (rect_x1 < r_x2 && rect_x2 > r_x1 &&
		rect_y1 < r_y2 && rect_y2 > r_y1)
		return true;

	return false;
}

//==============================================
//================ GRID RELATED ================
//==============================================

Grid* GetGridMouseIsIn(std::vector<Grid>* grids, float mouse_x, float mouse_y) {
	for (int i = 0; i < grids->size(); i++) {
		Grid* grid = &grids->at(i);
		if (isPointInRect(grid->x, grid->y, grid->w, grid->h, mouse_x, mouse_y)) {
			return grid;
		}
	}
	return nullptr;
}

void GetCellTopLeftCornerPos(Grid* grid, int row, int col, float* cell_x, float* cell_y) {
	*cell_x = grid->x + (grid->cell_w * col);
	*cell_y = grid->y + (grid->cell_h * row * -1);
}

void SnapRectToGridCell(Grid* grid, Rect* rect, int row, int col) {
	float cell_x, cell_y;
	GetCellTopLeftCornerPos(grid, row, col, &cell_x, &cell_y);

	rect->x = cell_x;
	rect->y = cell_y;
}

Rect DrawRectOnGrid(Grid* grid, EntityManager* em, int startRow, int startCol, int span_h, int span_v, Color color) {
	Rect rect{};
	rect.color = color;

	//Set x and y to start cells x and y
	float cell_x, cell_y;
	GetCellTopLeftCornerPos(grid, startRow, startCol, &cell_x, &cell_y);

	rect.x = cell_x;
	rect.y = cell_y;

	//Calculate opposite cell and get its BR coords
	float br_x = cell_x + span_v * grid->cell_w;
	float br_y = cell_y + span_h * grid->cell_h;

	//Assign width and height of rect from those BR data
	em->SetRectDimentions(rect.x, rect.y, br_x, br_y, &rect);

	return rect;
}