#include "globals.h"
#include "Tile.h"

Tile::Tile(int tileNumber, int X, int Y, bool isPassable, string event)
{
	this->tileNumber = tileNumber;
	this->isPassable = isPassable;
	this->event = event;
	this->X = X;
	this->Y = Y;
};