#pragma once

#include <iostream>
using namespace std;

struct Tile
{
	string event;
	bool isPassable;
	int tileNumber;
	int X;
	int Y;

	Tile(int tileNumber, int X, int Y, bool isPassable, string event);

};