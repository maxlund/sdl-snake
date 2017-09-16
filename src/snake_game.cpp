#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "Snake.h"
#include "Food.h"
#include <stdio.h>
#include <string>
#include <cmath>
#include <vector>
#include <utility>
#include <iostream>
#include <algorithm>
#include <random>
#include <memory>

using namespace std;

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

// Directions constants, north=0, south=1..etc
enum dirs
{
	north = 0,
	south,
	west,
	east
};

//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

//Frees media and shuts down SDL
void close();

//Loads individual image as texture
SDL_Texture* loadTexture(std::string path);

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The window renderer
SDL_Renderer* gRenderer = NULL;

bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		//Set texture filtering to linear
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1" ))
		{
			printf("Warning: Linear texture filtering not enabled!");
		}

		//Create window
		gWindow = SDL_CreateWindow("Snake!", SDL_WINDOWPOS_UNDEFINED,
									SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH,
									SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (gWindow == NULL)
		{
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			//Create renderer for window
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
			if (gRenderer == NULL)
			{
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if (!(IMG_Init(imgFlags) & imgFlags))
				{
					printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
					success = false;
				}
			}
		}
	}

	return success;
}

bool loadMedia()
{
	//Loading success flag
	bool success = true;

	//Nothing to load
	return success;
}

void close()
{
	//Destroy window	
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}

SDL_Texture* loadTexture(std::string path)
{
	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == NULL)
	{
		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
	}
	else
	{
		//Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );
		if (newTexture == NULL)
		{
			printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		}

		//Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}

	return newTexture;
}

void move_snake(Snake* snake)
{
	switch(snake->get_direction())
	{
	case north:
		snake->set_head_coordinates(0, -10);
		break;
	case south:
		snake->set_head_coordinates(0, 10);
		break;
	case east:
		snake->set_head_coordinates(10, 0);
		break;
	case west:
		snake->set_head_coordinates(-10, 0);
		break;
	default:
		break;		  
	}
}

int main(int argc, char* argv[])
{
	// Set speed to default, or user input if present
	int speed{400};
	int number_of_players{1};

	//Main loop flag
	bool quit = false;
	if (argc > 1)
	{
		try
		{
			int input = stoi(argv[1]);
			if (input > 400 || input < 1)
			{
			    cerr << "Error, speed must be between 1-400" << endl
					 << "Usage: " << argv[0] << " <SPEED> <NUMBER OF PLAYERS>" << endl;
				return 1;
			}
			speed -= input;
			
			input = stoi(argv[2]);
			if (input > 3 || input < 1)
			{
			    cerr << "Error, number of players must be between 1-3" << endl
					 << "Usage: " << argv[0] << " <SPEED> <NUMBER OF PLAYERS>" << endl;
				return 1;
			}
			number_of_players = input;
		}
		catch (exception & e)
		{
			cerr << "Usage: " << argv[0] << " <SPEED> <NUMBER OF PLAYERS>" << endl
				 << "Error: " << e.what() << endl;
			return 1;
		}
	}

	//Start up SDL and create window
	if(!init())
	{
		printf( "Failed to initialize!\n" );
	}
	else
	{
		//Load media
		if( !loadMedia() )
		{
			printf( "Failed to load media!\n" );
		}
		else
		{	
			//Event handler
			SDL_Event e;

			// Init players/snakes
			vector<Snake*> snakes;
			int start_length{ 4 };
			int start_pos_x{SCREEN_WIDTH/2 - (number_of_players * 20)};
			int start_pos_y{SCREEN_HEIGHT/2 + (start_length * 10)};
			pair<int, int> start_coordinates{start_pos_x, start_pos_y};
			
			for (int i = 0; i < number_of_players; i++)
			{
				snakes.push_back(new Snake( SCREEN_WIDTH, SCREEN_HEIGHT,
											start_coordinates, start_length, north ));
				start_coordinates.first += 60;
			}

			// Init initial food position
		    unique_ptr<Food> food = make_unique<Food>(SCREEN_WIDTH, SCREEN_HEIGHT);

			// Place food in coordinate without snake
			bool is_occupied{true};
			while(is_occupied)
			{
				for (Snake* s: snakes)
				{
				    is_occupied = s->coordinate_in_snake(food->get_coordinates());
					if (is_occupied)
					{
						food->set_new_coordinates();
						break;
					}
				}
			}
			
		    int counted_frames = 0;
			while(!quit)
			{
				//Handle events on queue
				while(SDL_PollEvent( &e ) != 0)
				{
					//User requests quit
					if(e.type == SDL_QUIT)
					{
						quit = true;
					}
				}
				
				// Move the snake(s)			   
				const Uint8* keystates = SDL_GetKeyboardState(NULL);
			    auto it = begin(snakes);
				Snake* snake = *it;

				// Player 1 key presses
				if (keystates[SDL_SCANCODE_UP] &&
					!keystates[SDL_SCANCODE_LEFT] &&
					!keystates[SDL_SCANCODE_RIGHT] &&
					!keystates[SDL_SCANCODE_DOWN])
				{
					if (snake->get_direction() != south)
						snake->set_direction(north);
				}
				if (keystates[SDL_SCANCODE_DOWN] &&
					!keystates[SDL_SCANCODE_LEFT] &&
					!keystates[SDL_SCANCODE_RIGHT] &&
					!keystates[SDL_SCANCODE_UP])
				{
					if (snake->get_direction() != north)
						snake->set_direction(south);
				}
				if (keystates[SDL_SCANCODE_LEFT] &&
					!keystates[SDL_SCANCODE_UP] &&
					!keystates[SDL_SCANCODE_DOWN] &&
					!keystates[SDL_SCANCODE_RIGHT])
				{
					if (snake->get_direction() != east)
						snake->set_direction(west);
				}
				if (keystates[SDL_SCANCODE_RIGHT] &&
					!keystates[SDL_SCANCODE_UP] &&
					!keystates[SDL_SCANCODE_DOWN] &&
					!keystates[SDL_SCANCODE_LEFT])
				{
					if (snake->get_direction() != west)
						snake->set_direction(east);
				}
				// Player 2 key presses
				if (number_of_players > 1)
				{
					//Increment pointer to get next snake
				    snake = *(++it);
					
					if (keystates[SDL_SCANCODE_W] &&
						!keystates[SDL_SCANCODE_A] &&
						!keystates[SDL_SCANCODE_D] &&
						!keystates[SDL_SCANCODE_S])
					{
						if (snake->get_direction() != south)
							snake->set_direction(north);
					}
					if (keystates[SDL_SCANCODE_S] &&
						!keystates[SDL_SCANCODE_A] &&
						!keystates[SDL_SCANCODE_D] &&
						!keystates[SDL_SCANCODE_W])
					{
						if (snake->get_direction() != north)
							snake->set_direction(south);
					}
					if (keystates[SDL_SCANCODE_A] &&
						!keystates[SDL_SCANCODE_W] &&
						!keystates[SDL_SCANCODE_S] &&
						!keystates[SDL_SCANCODE_D])
					{
						if (snake->get_direction() != east)
							snake->set_direction(west);
					}

					if (keystates[ SDL_SCANCODE_D ] &&
						!keystates[ SDL_SCANCODE_W ] &&
						!keystates[ SDL_SCANCODE_S ] &&
						!keystates[ SDL_SCANCODE_A ])
					{
						if (snake->get_direction() != west)
							snake->set_direction(east);
					}
				}
				// Player 3 key presses
				if (number_of_players > 2)
				{
					snake = *(++it);
					if (keystates[SDL_SCANCODE_I] &&
						!keystates[SDL_SCANCODE_J] &&
						!keystates[SDL_SCANCODE_L] &&
						!keystates[SDL_SCANCODE_K])
					{
						if (snake->get_direction() != south)
							snake->set_direction(north);
					}
					if (keystates[SDL_SCANCODE_K] &&
						!keystates[SDL_SCANCODE_J] &&
						!keystates[SDL_SCANCODE_L] &&
						!keystates[SDL_SCANCODE_I])
					{
						if (snake->get_direction() != north)
							snake->set_direction(south);
					}
					if (keystates[SDL_SCANCODE_J] &&
						!keystates[SDL_SCANCODE_I] &&
						!keystates[SDL_SCANCODE_K] &&
						!keystates[SDL_SCANCODE_L])
					{
						if (snake->get_direction() != east)
							snake->set_direction(west);
					}
					if (keystates[SDL_SCANCODE_L] &&
						!keystates[SDL_SCANCODE_I] &&
						!keystates[SDL_SCANCODE_K] &&
						!keystates[SDL_SCANCODE_J])
					{
						if (snake->get_direction() != west)
							snake->set_direction(east);
					}
				}
				if(counted_frames % speed == 0)
				{
					// Update head positions of all snakes
					for (Snake* s : snakes)
					{
						if (!s->is_frozen())
							move_snake(s);
					}			   
					// Check if a player died
					for (Snake* s : snakes)
					{
					    for(Snake* sn : snakes)
						{
							if (s->check_for_collision(sn))
								s->freeze_snake();
						}	   
					}
					if (all_of(begin(snakes), end(snakes), [](Snake* s) { return s->is_frozen(); }))
					{
						quit = true;
						break;
					}				    
					// Update snake body coordinates
					bool food_got_eaten{false};
					for (Snake* s : snakes)
					{
						if (!s->is_frozen())
						{
							if (s->get_head_coordinates() == food->get_coordinates())
							{
								s->extend_snake();
								food_got_eaten = true;
								speed -= 5;
							}
							else
							{
								s->update_coordinates();
							}
						}
					}							
					// If food was eaten, place new food in unoccupied coordinates
					if (food_got_eaten)
					{
						food->set_new_coordinates();
						is_occupied = true;
						while(is_occupied)
						{
							for (Snake* s : snakes)
							{
								is_occupied = s->coordinate_in_snake(food->get_coordinates());
								if (is_occupied)
								{
									food->set_new_coordinates();
									break;
								}
							}
						}
					}

				}
				
				//Clear screen
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
				SDL_RenderClear(gRenderer);

				it = begin(snakes);
				snake = *it;
				
				//Render blue filled snake
				for (pair<int, int> const & c : snake->get_coordinates())
				{
					SDL_Rect fillRect{ c.first , c.second, 10, 10 };
					SDL_SetRenderDrawColor( gRenderer, 0x00, 0x00, 0xFF, 0xFF );		
					SDL_RenderFillRect( gRenderer, &fillRect );
				}
				//Render green filled snake
				if (number_of_players >= 2)
				{
					snake = *(++it);
					for (pair<int, int> const & c : snake->get_coordinates())
					{
						SDL_Rect fillRect{ c.first , c.second, 10, 10 };
						SDL_SetRenderDrawColor( gRenderer, 0x00, 0xFF, 0x00, 0xFF );		
						SDL_RenderFillRect( gRenderer, &fillRect );
					}
				}
				//Render purple filled snake
				if (number_of_players >= 3)
				{
					snake = *(++it);
					for (pair<int, int> const & c : snake->get_coordinates())
					{
						SDL_Rect fillRect{ c.first , c.second, 10, 10 };
						SDL_SetRenderDrawColor( gRenderer, 0xB6, 0x42, 0xF4, 0xFF );		
						SDL_RenderFillRect( gRenderer, &fillRect );
					}
				}				

				// Render red filled food
				SDL_Rect fillRect{ food->get_coordinates().first, food->get_coordinates().second, 10, 10 };
				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0x00, 0x00, 0xFF );		
				SDL_RenderFillRect( gRenderer, &fillRect );
				
				//Update screen
				SDL_RenderPresent( gRenderer );

				counted_frames++;
			}

			auto it = begin(snakes);
			Snake* snake = *it;
			cout << "*** GAME OVER ***" << endl
				 << "Blue snake score: " << snake->get_score() << endl;
			if (number_of_players > 1)
			{
				snake = *(++it);
				cout << "Green snake score: " << snake->get_score() << endl;
			}
			if (number_of_players > 2)
			{
				snake = *(++it);
				cout << "Purple snake score: " << snake->get_score() << endl;
			}

			for (Snake* s : snakes)
			{
				delete s;
			}
		}
	}

	//Free resources and close SDL
	close();

	return 0;
}
