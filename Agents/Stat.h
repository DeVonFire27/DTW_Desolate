/***************************************************************
|	File:	Stat.h
|	Author:	Michael Mozdzierz
|	Date:	04/14/2014
|
***************************************************************/

#pragma once

enum class StatType
{
	health,
	accuracy,
	strength,
	dexterity,
	defense,
	moveSpeed,
};

struct Stat
{
	float increace,
		  base,
		  affliction = 0.f;

	StatType type;

	Stat(void);
	Stat(float increace, float base, StatType type);
	~Stat(void);

	void BuffStat(float amount) { base += amount; }

	float operator [](int levelIndex);
};