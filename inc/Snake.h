#ifndef SNAKE_H
#define SNAKE_H

#include <utility>
#include <vector>

class Snake
{
	
public:
   Snake(int screen_width, int screen_height,
         std::pair<int, int> start_coordinates,
         int start_length,
         int direction);
   ~Snake();
	
   void update_coordinates();
   void extend_snake();
   void set_head_coordinates(int, int);
   
   std::vector<std::pair<int, int> > get_coordinates();
   std::pair<int, int> get_head_coordinates();

   bool check_for_collision(Snake* other_player);
   bool coordinate_in_snake(std::pair<int, int> const &);
   void set_direction(int);
   int get_direction();
   int get_score();
   void freeze_snake();
   bool is_frozen();
private:
   int screen_width;
   int screen_height;
   std::pair<int, int> head_coordinates;
   int direction;
   std::vector<std::pair<int, int> > coordinates;
   int score{0};
   bool frozen{false};
};

#endif
