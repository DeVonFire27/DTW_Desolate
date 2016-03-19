/***************************************************************
|	File:		Stat.cpp
|	Author:		Michael Mozdzierz
|	Date:		04/14/2014
|
***************************************************************/

#include "globals.h"
#include "Stat.h"
#include <memory>
#include "../Math.h"


Stat::Stat(void)
{
	increace = 0.f;
	base = 0.f;
	type = StatType(0);
}

Stat::Stat(float increace, float base, StatType type)
{
	// copy-in data members
	this->increace = increace;
	this->type = type;
	this->base = base;
}

Stat::~Stat(void)
{
}

float Stat::operator [](int levelIndex)
{
	// find what value the stat should be at this level, and return it
	float value = base + increace * (float)(levelIndex);
	// find how much should be deducted for ailments
	float lost = value * affliction;

	// return total value
	return value - lost;
}