/***************************************************************
|	File:		Slow.h
|	Author:		Michael Mozdzierz
|	Date:		04/11/2014
|
***************************************************************/

#pragma once

#include "StatusEffect.h"

class Character;

class Slow : public StatusEffect
{
public:
	Slow(Character* effector);
	~Slow(void);

	virtual StatEffectType GetType(void) const override { return StatEffectType::Slow; }
};