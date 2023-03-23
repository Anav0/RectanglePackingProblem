#pragma once
#include <math.h>

inline void convertToOpenglCoordSystem(double x, double y, double* ox, double* oy, int maxWidth, int maxHeight) {
	*ox = 2 * x / maxWidth - 1;
	*oy = -2 * y / maxHeight + 1;
}

inline void MoveToClosestStep(double step, double* x, double* y, int maxWidth, int maxHeight) {
	float normalizedStepW = step / maxWidth;
	float normalizedStepH = step / maxHeight;
	*x = floor(*x / normalizedStepW) * normalizedStepW;
	*y = floor(*y / normalizedStepH) * normalizedStepH;
}