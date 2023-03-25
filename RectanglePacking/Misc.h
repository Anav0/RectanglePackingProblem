#pragma once
#include <math.h>

inline void convertToOpenglCoordSystem(double x, double y, double* ox, double* oy, int maxWidth, int maxHeight) {
	*ox = 2 * x / maxWidth - 1;
	*oy = -2 * y / maxHeight + 1;
}