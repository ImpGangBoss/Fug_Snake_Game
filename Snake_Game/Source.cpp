#include "MyTexture.h"
#include <vector>

enum MoveDirection
{
	STOP = 0,
	MOVE_UP = 1,
	MOVE_DOWN = 2,
	MOVE_LEFT = 3,
	MOVE_RIGHT = 4,
};

class Segment
{
public:
	Segment();
	Segment(int x, int y, MoveDirection direction, bool horizontal_flip, bool vertical_flip, double angle);
	Segment(SDL_Point position, MoveDirection direction, bool horizontal_flip, bool vertical_flip, double angle);

	void SetPosition(int x, int y) { this->position.x = x; this->position.y = y; }
	void SetMoveDirection(MoveDirection direction) { this->direction = direction; }
	void IsHorFlip(bool horizontal_flip) { this->horizontal_flip = horizontal_flip; }
	void IsVerFlip(bool vertical_flip) { this->vertical_flip = vertical_flip; }
	void SetAngle(double angle) { this->angle = angle; }

	SDL_Point GetPosition() { return this->position; }
	MoveDirection GetMoveDirection() { return this->direction; }
	bool GetHorFlip() { return this->horizontal_flip; }
	bool GetVerFlip() { return this->vertical_flip; }
	double GetAngle() { return this->angle; }

private:
	SDL_Point position;
	MoveDirection direction = STOP;
	bool horizontal_flip = false;
	bool vertical_flip = false;
	double angle = 0.0;
};

Segment::Segment(int x, int y, MoveDirection direction, bool horizontal_flip, bool vertical_flip, double angle)
{
	this->position.x = x;
	this->position.y = y;
	this->direction = direction;
	this->horizontal_flip = horizontal_flip;
	this->vertical_flip = vertical_flip;
}

Segment::Segment(SDL_Point position, MoveDirection direction, bool horizontal_flip, bool vertical_flip, double angle)
{
	this->position = position;
	this->direction = direction;
	this->horizontal_flip = horizontal_flip;
	this->vertical_flip = vertical_flip;
}

const int SCREEN_WIDTH = 1024;
const int SCREEN_HEIGHT = 1024;
const int SCREEN_BORDER = 64;

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
TTF_Font* font = NULL;

MyTexture background_texture,
snake_head_texture,
snake_body_texture,
coin_texture,
score_texture,
game_over_texture;

bool Init();
bool LoadMedia();
void Kill();

//Creating Window, Renderer and initialization SDL_TTF and SDL_IMG
bool Init()
{
	bool success = true;

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		std::cout << "SDL could not initialize! SDL Error: " << SDL_GetError() << std::endl;
		success = false;
	}
	else
	{
		window = SDL_CreateWindow("Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (window == NULL)
		{
			std::cout << "Window could not be created! SDL Error: " << SDL_GetError() << std::endl;
			success = false;
		}
		else
		{
			renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
			if (renderer == NULL)
			{
				std::cout << "Renderer could not be created! SDL Error: " << SDL_GetError() << std::endl;
				success = false;
			}
			else
			{
				SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

				int imgFlags = IMG_INIT_PNG;
				if (!(IMG_Init(imgFlags) & imgFlags))
				{
					std::cout << "SDL_image could not initialize! SDL_mage Error: " << IMG_GetError() << std::endl;
					success = false;
				}

				if (TTF_Init() == -1)
				{
					std::cout << "SDL_ttf could not initialize! SDL_ttf Error:" << TTF_GetError() << std::endl;
					success = false;
				}
			}
		}
	}
	return success;
}

//Loading assets: sprites and font
bool LoadMedia()
{
	bool success = true;

	if (!background_texture.LoadFromFile(renderer, "Assets/background.png"))
	{
		std::cout << "Failed to load background texture!" << std::endl;
		success = false;
	}
	if (!snake_head_texture.LoadFromFile(renderer, "Assets/head.png"))
	{
		std::cout << "Failed to load head texture!" << std::endl;
		success = false;
	}
	if (!snake_body_texture.LoadFromFile(renderer, "Assets/body.png"))
	{
		std::cout << "Failed to load body texture!" << std::endl;
		success = false;
	}
	if (!coin_texture.LoadFromFile(renderer, "Assets/coin.png"))
	{
		std::cout << "Failed to load coin texture!" << std::endl;
		success = false;
	}
	

	font = TTF_OpenFont("Assets/GreatBold.ttf", 110);
	if (font == NULL)
	{
		std::cout << "Failed to load font! SDL_ttf Error:" << TTF_GetError() << std::endl;
		success = false;
	}

	return success;
}

//Clearing textures. Destroying Window and Renderer. Closing font
void Kill()
{
	background_texture.Clear();
	snake_body_texture.Clear();
	snake_head_texture.Clear();
	coin_texture.Clear();
	score_texture.Clear();
	game_over_texture.Clear();

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	TTF_CloseFont(font);

	window = NULL;
	renderer = NULL;
	font = NULL;

	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

int main(int argc, char* args[])
{
	srand(time(nullptr));

	if (!Init())
		std::cout << "Failed to initialize!" << std::endl;
	else
	{
		if (!LoadMedia())
			std::cout << "Failed to load media!" << std::endl;
		else
		{
			std::vector<Segment> snake;

			bool quit = false;
			SDL_Event e;
			SDL_Color font_color = { 255, 255, 255 };
			SDL_Point current_position;
			MoveDirection current_direction;
			Segment temp_segment(0, 0, STOP, false, false, 0.0);

			int coin_position_x = 160,
				coin_position_y = 160,
				score_position_x = (SCREEN_WIDTH - score_texture.GetWidth()) / 2 - 32,
				score_position_y = -32,
				step = 32,
				score = 0,
				frame_counter = 0,
				speed = 200;

			double angle = 0;

			bool horizontal_flip = false,
				vertical_flip = false,
				add_new_segment = false,
				game_over = false;

			current_position.x = (SCREEN_WIDTH - snake_head_texture.GetWidth()) / 2 - 16; //Set head x position to map center
			current_position.y = (SCREEN_HEIGHT - snake_head_texture.GetHeight()) / 2 - 16; //Set head y position to map center
			current_direction = STOP;
			snake.push_back(Segment(current_position, current_direction, horizontal_flip, vertical_flip, angle)); //Add snake head to snake vector

			game_over_texture.LoadFromRenderedText(renderer, font, "GAME OVER", font_color); //Loading game_over text
			score_texture.LoadFromRenderedText(renderer, font, std::to_string(score), font_color); //Loading score text

			background_texture.Render(renderer, 0, 0, NULL, false, false, NULL); //Background render
			snake_head_texture.Render(renderer, current_position.x, current_position.y, NULL, false, false, NULL); //Snake head render
			coin_texture.Render(renderer, coin_position_x, coin_position_y, false, false, 0.0); //Coin render
			score_texture.Render(renderer, score_position_x, score_position_y, false, false, 0.0); //Score render
			
			SDL_RenderPresent(renderer);
			
			while (!quit)
			{
				while (SDL_PollEvent(&e) != 0)
				{
					switch (e.type)
					{
						case SDL_QUIT:
							quit = true;
						case SDL_KEYDOWN:
						{
							switch (e.key.keysym.sym)
							{
							case SDLK_r:
							{
								if (game_over == true) //If R is pressed and game is over, clear snake and set head postion to map center
								{
									snake.clear();
									game_over = false;

									speed = 200;
									score = 0;

									current_position.x = (SCREEN_WIDTH - snake_head_texture.GetWidth()) / 2 - 16;
									current_position.y = (SCREEN_HEIGHT - snake_head_texture.GetHeight()) / 2 - 16;
									current_direction = STOP;
									snake.push_back(Segment(current_position, current_direction, horizontal_flip, vertical_flip, angle));

									background_texture.Render(renderer, 0, 0, false, false, 0.0);
									snake_head_texture.Render(renderer, current_position.x, current_position.y, horizontal_flip, vertical_flip, angle);

									SDL_RenderPresent(renderer);
								}
								break;
							}
							case SDLK_w:
								current_direction = MOVE_UP;
								break;
							case SDLK_s:
								current_direction = MOVE_DOWN;
								break;
							case SDLK_a:
								current_direction = MOVE_LEFT;
								break;
							case SDLK_d:
								current_direction = MOVE_RIGHT;
								break;
							default:
								break;
							}
						}
						default:
							break;
					}
				}

				if (snake[0].GetPosition().y < SCREEN_BORDER
					|| snake[0].GetPosition().y > SCREEN_HEIGHT - snake_body_texture.GetHeight() - SCREEN_BORDER
					|| snake[0].GetPosition().x < SCREEN_BORDER
					|| snake[0].GetPosition().x > SCREEN_WIDTH - snake_body_texture.GetWidth() - SCREEN_BORDER) //Checing if head hit the wall
					game_over = true;
				
				for (int i = 1; i < snake.size(); ++i)
					if (snake[0].GetPosition().x == snake[i].GetPosition().x && snake[0].GetPosition().y == snake[i].GetPosition().y) //Checking if head hit snake body
						game_over = true;
				
				if (!game_over)
				{
					if (snake.size() == 1)
						snake[0].SetMoveDirection(current_direction); //If snake have only head, set head move direction to current move direction
					else
						if ((snake[0].GetMoveDirection() == MOVE_LEFT && current_direction != MOVE_RIGHT)
							|| (snake[0].GetMoveDirection() == MOVE_RIGHT && current_direction != MOVE_LEFT)
							|| (snake[0].GetMoveDirection() == MOVE_UP && current_direction != MOVE_DOWN)
							|| (snake[0].GetMoveDirection() == MOVE_DOWN && current_direction != MOVE_UP)) //Checking if head move direction isn't the opposite with previous move direction
							snake[0].SetMoveDirection(current_direction);

					if (frame_counter % speed == 0) //Update speed
					{
						if (add_new_segment)
						{
							add_new_segment = false;
							snake.push_back(temp_segment); //Adding new segment to body
						}
						
						if (snake[0].GetPosition().x == coin_position_x && snake[0].GetPosition().y == coin_position_y) //Cecking if snake head is on coin position
						{
							score++;

							coin_position_x = 1;
							coin_position_y = 1;

							add_new_segment = true;

							temp_segment = snake[snake.size() - 1]; //New segment have same properties as a previous

							if (score % 5 == 0) //Speed up every 5 score points
								speed /= 1.2;
						}

						//For each snake segment changing position according to move direction 
						for (auto& i : snake)
						{
							if (i.GetMoveDirection() == MOVE_UP)
							{
								i.SetPosition(i.GetPosition().x, i.GetPosition().y - step);
								i.SetAngle(0.0);
								i.IsHorFlip(false);
								i.IsVerFlip(false);
							}
							if (i.GetMoveDirection() == MOVE_DOWN)
							{
								i.SetPosition(i.GetPosition().x, i.GetPosition().y + step);
								i.SetAngle(0.0);
								i.IsHorFlip(false);
								i.IsVerFlip(true);
							}
							if (i.GetMoveDirection() == MOVE_LEFT)
							{
								i.SetPosition(i.GetPosition().x - step, i.GetPosition().y);
								i.SetAngle(270);
								i.IsHorFlip(true);
								i.IsVerFlip(false);
							}
							if (i.GetMoveDirection() == MOVE_RIGHT)
							{
								i.SetPosition(i.GetPosition().x + step, i.GetPosition().y);
								i.SetAngle(90);
								i.IsHorFlip(true);
								i.IsVerFlip(false);
							}
						}

						//Update move direction of segments
						if (snake.size() > 1)
							for (int i = snake.size() - 1; i > 0; --i)
								snake[i].SetMoveDirection(snake[i - 1].GetMoveDirection());
					}

					while (coin_position_x % step != 0 || coin_position_y % step != 0) //Generate new coin
					{
						coin_position_x = rand() % (SCREEN_WIDTH - 2 * coin_texture.GetWidth() - SCREEN_BORDER) + SCREEN_BORDER;
						coin_position_y = rand() % (SCREEN_HEIGHT - 2 * coin_texture.GetHeight() - SCREEN_BORDER) + SCREEN_BORDER;
						for (auto& i : snake)
						{
							if (i.GetPosition().x == coin_position_x)
								coin_position_x = 1;
							if (i.GetPosition().y == coin_position_y)
								coin_position_y = 1;
						}
					}

					//Render background and coin
					background_texture.Render(renderer, 0, 0, false, false, 0.0);
					score_texture.LoadFromRenderedText(renderer, font, std::to_string(score), font_color);
					
					//Render snake
					for (int i = 1; i < snake.size(); ++i)
						snake_body_texture.Render(renderer, snake[i].GetPosition().x, snake[i].GetPosition().y, snake[i].GetHorFlip(), snake[i].GetVerFlip(), snake[i].GetAngle());
					snake_head_texture.Render(renderer, snake[0].GetPosition().x, snake[0].GetPosition().y, snake[0].GetHorFlip(), snake[0].GetVerFlip(), snake[0].GetAngle());

					score_texture.Render(renderer, score_position_x, score_position_y, false, false, 0.0); //Render score

					frame_counter++;

					SDL_RenderPresent(renderer);
				}
				else
				{
					//If game is over, render game_over text
					game_over_texture.Render(renderer, (SCREEN_WIDTH - game_over_texture.GetWidth()) / 2,
						(SCREEN_HEIGHT - game_over_texture.GetHeight()) / 2,
						false, false, 0.0);
					SDL_RenderPresent(renderer);
				}
			}
		}
	}

	Kill();

	return 0;
}