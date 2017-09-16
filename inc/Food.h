#ifndef FOOD_H
#define FOOD_H

#include <utility>

class Food
{
public:
	Food(int, int);
	~Food();

	void set_new_coordinates();
	std::pair<int, int> get_coordinates();
private:
	std::pair<int, int> coordinates;
	int width;
	int height;
};

#endif
