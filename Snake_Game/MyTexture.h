#pragma once
#include <iostream>
#include <string>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

class MyTexture
{
public:
	MyTexture();
	~MyTexture();

	bool LoadFromRenderedText(SDL_Renderer* renderer, TTF_Font* font, std::string textureText, SDL_Color textColor);
	bool LoadFromFile(SDL_Renderer* renderer, std::string path);
	void Clear();
	void Render(SDL_Renderer* renderer, int x, int y, bool horizontal_flip, bool vertical_flip, double angle, SDL_Rect* clip);
	int GetWidth();
	int GetHeight();
	SDL_Point& GetPosition();

private:
	SDL_Texture* texture;
	int width;
	int height;
	SDL_Point position;
};
MyTexture::MyTexture()
{
	this->texture = NULL;
	this->width = 0;
	this->height = 0;
}

MyTexture::~MyTexture()
{
	Clear();
}

bool MyTexture::LoadFromRenderedText(SDL_Renderer* renderer, TTF_Font* font, std::string textureText, SDL_Color textColor)
{
	Clear();

	SDL_Surface* textSurface = TTF_RenderText_Solid(font, textureText.c_str(), textColor);
	if (textSurface == NULL)
		std::cout << "Unable to render text surface! SDL_ttf Error:" << TTF_GetError() << std::endl;
	else
	{
		this->texture = SDL_CreateTextureFromSurface(renderer, textSurface);
		if (this->texture == NULL)
			std::cout << "Unable to create texture from rendered text! SDL Error:" << SDL_GetError() << std::endl;
		else
		{
			this->width = textSurface->w;
			this->height = textSurface->h;
		}
		SDL_FreeSurface(textSurface);
	}
	return this->texture != NULL;
}

bool MyTexture::LoadFromFile(SDL_Renderer* renderer, std::string path)
{
	Clear();
	SDL_Texture* newTexture = NULL;

	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == NULL)
		std::cout << "Unable to load image! SDL_image Error: " << IMG_GetError() << std::endl;
	else
	{
		newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
		if (newTexture == NULL)
			std::cout << "Unable to create texture from! SDL Error : " << SDL_GetError() << std::endl;
		else
		{
			this->width = loadedSurface->w;
			this->height = loadedSurface->h;
		}

		SDL_FreeSurface(loadedSurface);
	}

	this->texture = newTexture;
	return this->texture != NULL;
}

void MyTexture::Render(SDL_Renderer* renderer, int x, int y, bool horizontal_flip, bool vertical_flip, double angle, SDL_Rect* clip = NULL)
{
	SDL_Rect renderQuad = { x, y, this->width, this->height };

	this->position.x = x;
	this->position.y = y;

	if (clip != NULL)
	{
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}

	if (horizontal_flip)
		SDL_RenderCopyEx(renderer, this->texture, clip, &renderQuad, angle, NULL, SDL_FLIP_HORIZONTAL);
	else if (vertical_flip)
		SDL_RenderCopyEx(renderer, this->texture, clip, &renderQuad, angle, NULL, SDL_FLIP_VERTICAL);
	else
		SDL_RenderCopyEx(renderer, this->texture, clip, &renderQuad, NULL, NULL, SDL_FLIP_NONE);

}

void MyTexture::Clear()
{
	if (this->texture != NULL)
	{
		SDL_DestroyTexture(this->texture);
		this->texture = NULL;
		this->width = 0;
		this->height = 0;
		this->position.x = 0;
		this->position.y = 0;
	}
}

int MyTexture::GetWidth()
{
	return this->width;
}

int MyTexture::GetHeight()
{
	return this->height;
}

SDL_Point& MyTexture::GetPosition()
{
	return this->position;
}