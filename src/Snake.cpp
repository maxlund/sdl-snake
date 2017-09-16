#include "Snake.h"
#include "Food.h"
#include <vector>
#include <utility>
#include <algorithm>
#include <iostream>

using namespace std;

Snake::Snake(int width, int height,
			 pair<int,int> start_coordinates,
			 int start_length, int dir)
	: screen_width{width}, screen_height{height}, direction{dir}
{
    for (int i = 0; i < start_length; i++)
	{
		start_coordinates.second -= 10;
	    coordinates.push_back(start_coordinates);
	}
	head_coordinates.first = start_coordinates.first;
	head_coordinates.second = start_coordinates.second;
}

Snake::~Snake() {}

void Snake::update_coordinates()
{
	coordinates.push_back(head_coordinates);
	coordinates.erase(begin(coordinates), ++begin(coordinates));
}

void Snake::extend_snake()
{
	coordinates.push_back(head_coordinates);
	score += 25; // player gets 25 points for each food eaten
}

void Snake::set_head_coordinates(int x_val, int y_val)
{
	head_coordinates.first += x_val;
	head_coordinates.second += y_val;
}

vector<pair<int, int>> Snake::get_coordinates()
{
	return coordinates;
}

pair<int, int> Snake::get_head_coordinates()
{
	return head_coordinates;
}


bool Snake::check_for_collision(Snake* snake)
{
	bool collision{false};
	if ((head_coordinates.first >= screen_width || head_coordinates.first < 0) ||
		(head_coordinates.second >= screen_height || head_coordinates.second < 0))
	{
		collision = true;
	}
	for (pair<int, int> const & c : snake->get_coordinates())
	{
		if (head_coordinates == c)
			collision = true;
	}	
	return collision;
}

bool Snake::coordinate_in_snake(pair<int, int> const & coords)
{
	return find(begin(coordinates), end(coordinates), coords) != end(coordinates);
}

int Snake::get_score()
{
	return score;
}

void Snake::set_direction(int dir)
{
	direction = dir;
}

int Snake::get_direction()
{
	return direction;
}

void Snake::freeze_snake()
{
	frozen = true;
}

bool Snake::is_frozen()
{
	return frozen;
}

