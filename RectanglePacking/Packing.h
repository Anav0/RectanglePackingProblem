#pragma once
#include "EntityManager.h"
#include "Gameplay.h"
#include <random>

bool ByY(const Rect* a, const Rect* b)
{
	return a->y < b->y;
}

bool ByX(const Rect* a, const Rect* b)
{
	return a->x < b->x;
}

void ConstructPermutationFromRects(std::vector<Rect>* rects, std::vector<int>* PM, std::vector<int>* PP, std::vector<int>* PH) {
	//std::sort(rects->begin(), rects->end(), ByY);


}

inline bool isBeforeInPermutation(int a, int b, std::vector<int>* positionHelper) {
	return positionHelper[a] < positionHelper[b];
}

int CalculateValue(std::vector<int>* PM, std::vector<int>* PP, std::vector<int>* PH, std::vector<double>* lengths) {

	double total = 0;

}

int CalculatePenelty(std::vector<int>* PM, std::vector<int>* PP, std::vector<int>* PH) {}

bool MeetsConstraints(std::vector<int>* PM, std::vector<int>* PP, std::vector<int>* PH, Grid* grid) {}

inline void RandomSwap(std::vector<int>* arr) {
	int random_index_a = (rand() % arr->size());
	int random_index_b = (rand() % arr->size());

	int el_a = arr->at(random_index_a);
	int el_b = arr->at(random_index_b);

	int tmp = el_a;
	arr->at(random_index_a) = el_b;
	arr->at(random_index_b) = tmp;
}

// DOI 10.1007/s10878-015-9973-8
// Handling precedence constraints in scheduling
// problems by the sequence pair representation
// A. Kozik
void SequencePairHorizontal(Grid* grid, std::vector<Rect> rects) {

	srand((unsigned)time(NULL));

	std::vector<int> PP, PM, PH;
	ConstructPermutationFromRects(&rects, &PP, &PM, &PH);

	int iterations = 10000;

	std::vector<int> best_PP;
	std::vector<int> best_PM;
	std::vector<int> best_PH;

	double best_value = DBL_MIN;
	double lowest_penalty = DBL_MAX;

	for (size_t i = 0; i < iterations; i++)
	{
		if (MeetsConstraints(&PP, &PM, &PH, grid)) {
			double current_value = CalculateValue(&PP, &PM, &PH);

			if (current_value > best_value) {
				best_PP = PP;
				best_PM = PM;
				best_value = current_value;
			}
		}
		else {
			double current_penalty = CalculatePenelty(&PP, &PM, &PH);

			if (current_penalty < lowest_penalty) {
				best_PP = PP;
				best_PM = PM;
				lowest_penalty = current_penalty;
			}
		}

		RandomSwap(&PP);
		RandomSwap(&PM);
	}

	printf("Best value found was: %f\n", best_value);
	printf("Lowest penalty found is: %f", lowest_penalty);
}