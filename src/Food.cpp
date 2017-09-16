#include "Food.h"
#include <utility>
#include <random>
#include <iostream>

using namespace std;

Food::Food(int w, int h) : width{w}, height{h}
{
	set_new_coordinates();
}

Food::~Food() {}

void Food::set_new_coordinates()
{
	 random_device rd;
	 uniform_int_distribution<int> x_coord(0, (width-10) / 10);
	 uniform_int_distribution<int> y_coord(0, (height-10) / 10);
	 coordinates.first = x_coord(rd) * 10;
	 coordinates.second = y_coord(rd) * 10;
}

pair<int, int> Food::get_coordinates()
{
	return coordinates;
}
