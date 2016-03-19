/****************************************************
[	Author:		Michael Mozdzierz
[	Date:		01/30/2014
[	File:		Math.cpp
[	Course:		SGD
[
****************************************************/

#include "globals.h"
#include "Math.h"
#include <stdlib.h>
#include "Tile System\TileLayer.h"
#include <memory>
#include "Tile System\Tile.h"

const float Math::PI = 3.14159f;

bool _recurLineTest(Tile* tile, SGD::Point& at, SGD::Point& end, TileLayer* tiles, SGD::Vector step)
{
	// store a value for the tile width
	int tile_width = (int)tiles->GetTileSize().width;
	// if this tile is not passible, an intersection has been found: return true
	if (!tile->isPassable)
		return true;

	// if we went outside the world, return false
	if (at.x < 0.f || at.y < 0.f || at.x >= tiles->layerColumns || at.y >= tiles->layerRows)
		return false;

	// if we made it to our end point, no intersection was found, return false
	if ((at - end).ComputeLength() < 1.5f)
		return false;

	// move 'at' by step
	at += step;

	// call recursive function
	return _recurLineTest(tiles->tiles[int(at.x) + int(at.y) * tiles->layerColumns], at, end, tiles, step);
}

bool Math::LineTest(SGD::Point& start, SGD::Point& end, TileLayer* tiles)
{
	// store values for the width and height of tiles
	int tile_width = (int)tiles->GetTileSize().width;
	int tile_height = (int)tiles->GetTileSize().height;

	// find the index of the end-point tile
	SGD::Point end_index = SGD::Point(end.x / tile_width, end.y / tile_height);
	// find a the index of the start point
	SGD::Point at = SGD::Point(start.x / tile_width, start.y / tile_height);

	// find the ammout to step by
	SGD::Vector step = end - start;
	step.Normalize();
	step *= 2.f;

	// perform recursive line test
	return _recurLineTest(tiles->tiles[int(at.x) + int(at.y) * tiles->layerColumns], at, end_index, tiles, step);
}

float Math::to_degrees(float radians)
{
	return radians * (180.0f / PI);
}

float Math::to_radians(float degrees)
{
	return degrees * (PI / 180.0f);
}

float Math::distance(float a, float b)
{
	return max(a, b) - min(a, b);
}

float Math::lerp(float low, float high, float weight)
{
	return low + (high - low) * weight;
}

// TO WHOM IT MAY CONCERN:
// 
// If your wondering why the blinking crikey this method takes two void pointers:
// it's because im sick of having to remake 'contains' methods in each course
// for each new 'rectangle' struct/class. So yea, for floats for 'rect', two floats
// for 'point,' she should run like a charm.
bool Math::RectContains(void* rect, void* point)
{
	float rectData[4];
	memcpy(rectData, rect, 16);
	float left = rectData[0],
		top = rectData[1],
		right = rectData[2],
		bottom = rectData[3];

	float pointData[2];
	memcpy(pointData, point, 8);
	float x = pointData[0],
		y = pointData[1];

	if (x < right && x > left &&
		y < bottom && y > top)
		return true;

	return false;
}