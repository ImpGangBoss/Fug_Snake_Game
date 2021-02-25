#pragma once
#include <SDL.h>

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