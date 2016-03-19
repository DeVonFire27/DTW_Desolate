/***************************************************************
|	File:		Radiation.h
|	Author:		Michael Mozdzierz
|	Date:		04/11/2014
|
***************************************************************/

#pragma once

#include "StatusEffect.h"

class Character;

class Radiation : public StatusEffect
{
	int stacks;

public:
	Radiation(Character* effector);
	~Radiation(void);

	/////////////////////////<Accessors>///////////////////////////////////////////////////

	int GetStacks(void) const { return stacks; }

	virtual StatEffectType GetType(void) const override { return StatEffectType::Radiation; }

	/////////////////////////<Mutators>///////////////////////////////////////////////////

	void SetStacks(int value);

	/////////////////////////<Interface>///////////////////////////////////////////////////

	virtual void Update(float dt) final;

};