/***************************************************************
|	File:		Burning.h
|	Author:		Michael Mozdzierz
|	Date:		04/11/2014
|
***************************************************************/

#pragma once
#include "StatusEffect.h"

class Character;


class Burning : public StatusEffect
{
public:
	Burning(Character* effector);
	~Burning(void);

	virtual void Update(float dt) override;

	virtual StatEffectType GetType(void) const override { return StatEffectType::Burning; }
};