#include "MyTexture.h"
#include "Segment.h"
#include <vector>

const int SCREEN_WIDTH = 1024;
const int SCREEN_HEIGHT = 1024;
const int SCREEN_BORDER = 64;

class GameManager
{
public:

	void SetGameOverState(bool game_over) { this->game_over = game_over; }
	void SetHeadCurMoveDir(MoveDirection dir);
	void SetSpeed(int speed) { this->speed = speed; }

	void IsHeadHitWall(MyTexture& body);
	void IsHeadHitBody();
	void StartGame(SDL_Renderer* renderer, TTF_Font* font, MyTexture& background, MyTexture& coin, MyTexture& score, MyTexture& head, MyTexture& body, MyTexture& game_over);
	void AddNewSegment(Segment& segment);
	void MoveSnakeSegments();
	void UpdateSegPos();
	void GenerateNewCoin(MyTexture& coin);
	void RenderBackgroundSnakeCoinScore(SDL_Renderer* renderer, TTF_Font* font, MyTexture& background, MyTexture& coin, MyTexture& score, MyTexture& head, MyTexture& body);
	void RenderGameOverScreen(SDL_Renderer* renderer, MyTexture& game_over);

	auto& GetSnake() { return this->snake; }
	int GetScore() { return this->score; }
	bool IsGameOver() { return this->game_over; }
	bool IsCoinCollected();
	int GetSpeed() { return this->speed; }

private:
	std::vector<Segment> snake;
	
	SDL_Color font_color = { 255, 255, 255 };

	int coin_position_x = 160,
		coin_position_y = 160;

	int step = 32,
		score = 0,
		speed = 200;

	double angle = 0;

	bool game_over = false;
};

void GameManager::IsHeadHitWall(MyTexture& body)
{
	if (snake[0].GetPosition().y < SCREEN_BORDER
		|| snake[0].GetPosition().y > SCREEN_HEIGHT - body.GetHeight() - SCREEN_BORDER
		|| snake[0].GetPosition().x < SCREEN_BORDER
		|| snake[0].GetPosition().x > SCREEN_WIDTH - body.GetWidth() - SCREEN_BORDER)
		this->game_over = true;
}

void GameManager::IsHeadHitBody()
{
	for (int i = 1; i < this->snake.size(); ++i)
		if (this->snake[0].GetPosition().x == this->snake[i].GetPosition().x && this->snake[0].GetPosition().y == this->snake[i].GetPosition().y)
		{
			this->game_over = true;
			return;
		}
}

void GameManager::SetHeadCurMoveDir(MoveDirection dir)
{
	if (this->snake.size() == 1)
		this->snake[0].SetMoveDirection(dir); //If snake have only head, set head move direction to current move direction
	else
		if ((this->snake[0].GetMoveDirection() == MOVE_LEFT && dir != MOVE_RIGHT)
			|| (this->snake[0].GetMoveDirection() == MOVE_RIGHT && dir != MOVE_LEFT)
			|| (this->snake[0].GetMoveDirection() == MOVE_UP && dir != MOVE_DOWN)
			|| (this->snake[0].GetMoveDirection() == MOVE_DOWN && dir != MOVE_UP)) //Checking if head move direction isn't the opposite with previous move direction
			this->snake[0].SetMoveDirection(dir);
}

void GameManager::AddNewSegment(Segment& segment)
{
	snake.push_back(segment); //Adding new segment to body
}

void GameManager::MoveSnakeSegments()
{
	//For each snake segment changing position according to move direction 
	for (auto& i : this->snake)
	{
		if (i.GetMoveDirection() == MOVE_UP)
		{
			i.SetPosition(i.GetPosition().x, i.GetPosition().y - this->step);
			i.SetAngle(0.0);
			i.IsHorFlip(false);
			i.IsVerFlip(false);
		}
		if (i.GetMoveDirection() == MOVE_DOWN)
		{
			i.SetPosition(i.GetPosition().x, i.GetPosition().y + this->step);
			i.SetAngle(0.0);
			i.IsHorFlip(false);
			i.IsVerFlip(true);
		}
		if (i.GetMoveDirection() == MOVE_LEFT)
		{
			i.SetPosition(i.GetPosition().x - this->step, i.GetPosition().y);
			i.SetAngle(270);
			i.IsHorFlip(true);
			i.IsVerFlip(false);
		}
		if (i.GetMoveDirection() == MOVE_RIGHT)
		{
			i.SetPosition(i.GetPosition().x + this->step, i.GetPosition().y);
			i.SetAngle(90);
			i.IsHorFlip(true);
			i.IsVerFlip(false);
		}
	}
}

bool GameManager::IsCoinCollected()
{
	if (this->snake[0].GetPosition().x == this->coin_position_x && this->snake[0].GetPosition().y == this->coin_position_y) //Cecking if snake head is on coin position
	{
		this->score++;

		this->coin_position_x = 1;
		this->coin_position_y = 1;

		if (this->score % 5 == 0) //Speed up every 5 score points
			this->speed /= 1.2;

		return true;
	}

	return false;
}

void GameManager::UpdateSegPos()
{
	//Update move direction of segments
	if (this->snake.size() > 1)
		for (int i = this->snake.size() - 1; i > 0; --i)
			this->snake[i].SetPosition(this->snake[i - 1].GetPosition().x, this->snake[i - 1].GetPosition().y);
}

void GameManager::GenerateNewCoin(MyTexture& coin)
{
	while (this->coin_position_x % this->step != 0 || this->coin_position_y % this->step != 0) //Generate new coin
	{
		this->coin_position_x = (rand() % (SCREEN_WIDTH - 2 * coin.GetWidth() - SCREEN_BORDER) + SCREEN_BORDER);
		this->coin_position_y = (rand() % (SCREEN_HEIGHT - 2 * coin.GetHeight() - SCREEN_BORDER) + SCREEN_BORDER);
		for (auto& i : this->snake)
		{
			if (i.GetPosition().x == this->coin_position_x)
				this->coin_position_x = 1;
			if (i.GetPosition().y == this->coin_position_y)
				this->coin_position_y = 1;
		}
	}
}

void GameManager::RenderBackgroundSnakeCoinScore(SDL_Renderer* renderer, TTF_Font* font, MyTexture& background, MyTexture& coin, MyTexture& score, MyTexture& head, MyTexture& body)
{
	//Render background and coin
	background.Render(renderer, 0, 0, false, false, 0.0);
	coin.Render(renderer, this->coin_position_x, this->coin_position_y, false, false, 0.0);

	//Render snake
	for (int i = 1; i < this->snake.size(); ++i)
		body.Render(renderer, this->snake[i].GetPosition().x, this->snake[i].GetPosition().y, this->snake[i].GetHorFlip(), this->snake[i].GetVerFlip(), this->snake[i].GetAngle());
	head.Render(renderer, this->snake[0].GetPosition().x, this->snake[0].GetPosition().y, this->snake[0].GetHorFlip(), this->snake[0].GetVerFlip(), this->snake[0].GetAngle());

	score.LoadFromRenderedText(renderer, font, std::to_string(this->score), this->font_color);
	score.Render(renderer, (SCREEN_WIDTH - score.GetWidth()) / 2, -32, false, false, 0.0); //Render score

	SDL_RenderPresent(renderer);
}

void GameManager::RenderGameOverScreen(SDL_Renderer* renderer, MyTexture& game_over)
{
	//If game is over, render game_over text
	game_over.Render(renderer, (SCREEN_WIDTH - game_over.GetWidth()) / 2, (SCREEN_HEIGHT - game_over.GetHeight()) / 2, false, false, 0.0);
	SDL_RenderPresent(renderer);
}

void GameManager::StartGame(SDL_Renderer* renderer, TTF_Font* font, MyTexture& background, MyTexture& coin, MyTexture& score, MyTexture& head, MyTexture& body, MyTexture& game_over)
{
	SetGameOverState(false);
	this->snake.clear();
	this->score = 0;

	this->snake.push_back(Segment((SCREEN_WIDTH - head.GetWidth()) / 2 - 16, (SCREEN_HEIGHT - head.GetHeight()) / 2 - 16, STOP, false, false, 0.0)); //Add snake head to snake vector

	game_over.LoadFromRenderedText(renderer, font, "GAME OVER", this->font_color); //Loading game_over text

	score.LoadFromRenderedText(renderer, font, std::to_string(GetScore()),this->font_color); //Loading score text


	background.Render(renderer, 0, 0, NULL, false, false, NULL); //Background render
	head.Render(renderer, GetSnake()[0].GetPosition().x, GetSnake()[0].GetPosition().y, NULL, false, false, NULL); //Snake head render
	coin.Render(renderer, this->coin_position_x, this->coin_position_y, false, false, 0.0); //Coin render

	SDL_RenderPresent(renderer);
}

int main(int argc, char* args[])
{
	srand(time(nullptr));

	SDL_Window* window = NULL;
	SDL_Renderer* renderer = NULL;
	TTF_Font* font = NULL;

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		std::cout << "SDL could not initialize! SDL Error: " << SDL_GetError() << std::endl;
	else
	{
		window = SDL_CreateWindow("Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (window == NULL)
			std::cout << "Window could not be created! SDL Error: " << SDL_GetError() << std::endl;
		else
		{
			renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
			if (renderer == NULL)
				std::cout << "Renderer could not be created! SDL Error: " << SDL_GetError() << std::endl;
			else
			{
				SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

				int imgFlags = IMG_INIT_PNG;
				if (!(IMG_Init(imgFlags) & imgFlags))
					std::cout << "SDL_image could not initialize! SDL_mage Error: " << IMG_GetError() << std::endl;

				if (TTF_Init() == -1)
					std::cout << "SDL_ttf could not initialize! SDL_ttf Error:" << TTF_GetError() << std::endl;
			}
		}
	}

	MyTexture background_texture,
		snake_head_texture,
		snake_body_texture,
		coin_texture,
		score_texture,
		game_over_texture;

	if (!background_texture.LoadFromFile(renderer, "Assets/background.png"))
		std::cout << "Failed to load background texture!" << std::endl;
	if (!snake_head_texture.LoadFromFile(renderer, "Assets/head.png"))
		std::cout << "Failed to load head texture!" << std::endl;
	if (!snake_body_texture.LoadFromFile(renderer, "Assets/body.png"))
		std::cout << "Failed to load body texture!" << std::endl;
	if (!coin_texture.LoadFromFile(renderer, "Assets/coin.png"))
		std::cout << "Failed to load coin texture!" << std::endl;
	font = TTF_OpenFont("Assets/GreatBold.ttf", 110);
	if (font == NULL)
		std::cout << "Failed to load font! SDL_ttf Error:" << TTF_GetError() << std::endl;

	GameManager game_manager;

	SDL_Event e;
	MoveDirection current_direction = STOP;
	Segment temp_segment(-100, -100, STOP, false, false, 0.0);

	bool quit = false,
		add_new_segment = false;

	int frame_counter = 0;
			
	game_manager.StartGame(renderer, font, background_texture, coin_texture, score_texture, snake_head_texture, snake_body_texture, game_over_texture);
			
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
						if (game_manager.IsGameOver() == true) //If R is pressed and game is over, clear snake and set head postion to map center
							game_manager.StartGame(renderer, font, background_texture, coin_texture, score_texture, snake_head_texture, snake_body_texture, game_over_texture);
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

		game_manager.IsHeadHitWall(snake_body_texture); //Checing if head hit the wall
				
		game_manager.IsHeadHitBody(); //Checking if head hit snake body
				
		if (!game_manager.IsGameOver())
		{
			game_manager.SetHeadCurMoveDir(current_direction);

			if (frame_counter % game_manager.GetSpeed() == 0) //Update speed
			{
				game_manager.UpdateSegPos();
				game_manager.MoveSnakeSegments();

				if (game_manager.IsCoinCollected())
				{
					game_manager.AddNewSegment(temp_segment);
					temp_segment = game_manager.GetSnake()[game_manager.GetSnake().size() - 1]; //New segment have same properties as a previous
					game_manager.GenerateNewCoin(coin_texture);
				}
			}
			game_manager.RenderBackgroundSnakeCoinScore(renderer, font, background_texture, coin_texture, score_texture, snake_head_texture, snake_body_texture);
			frame_counter++;
		}
		else
			game_manager.RenderGameOverScreen(renderer, game_over_texture);
	}

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

	return 0;
}
