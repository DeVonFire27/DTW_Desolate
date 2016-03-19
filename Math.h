/****************************************************
[	Author:		Michael Mozdzierz
[	Date:		01/30/2014
[	File:		Math.h
[	Course:		SGD
[
****************************************************/

#pragma once

struct Tile;
class TileLayer;

#include "../SGD Wrappers/SGD_Geometry.h"
#include <vector>

using namespace std;

namespace Math
{
	extern const float PI;

	template <typename T> 
	T Clamp(T value, T min, T max);

	// returns 'value' clamped to the 'floor' lower-bound
	template <typename T>
	T Floor(T value, T floor);

	// returns 'value' clamped to the 'ceiling' upper-bound
	template <typename T>
	T Ceiling(T value, T ceiling);

	float lerp(float low, float high, float weight);

	float to_radians(float);
	float to_degrees(float);

	float distance(float, float);

#undef min
#undef max
	template <typename T>
	T min(T& a, T& b);

	template <typename T>
	T max(T& a, T& b);

	bool RectContains(void* rect, void* point);

	bool LineTest(SGD::Point& start, SGD::Point& end, TileLayer* tiles);
}

template <typename T>
T Math::Clamp(T value, T min, T max)
{
	if (value < min)
		return min;
	if (value > max)
		return max;

	return value;
}

template <typename T>
T Math::min(T& a, T& b)
{
	return a < b ? a : b;
}

template <typename T>
T Math::max(T& a, T& b)
{
	return a > b ? a : b;
}

template <typename T>
T Math::Floor(T value, T floor)
{
	if (value < floor)
		return floor;

	return value;
}

template <typename T>
T Math::Ceiling(T value, T ceiling)
{
	if (value > ceiling)
		return ceiling;

	return value;
}