/***************************************************************
|	File:		Transfusion.h
|	Author:		Michael Mozdzierz
|	Date:		04/11/2014
|
***************************************************************/

#pragma once

#include "StatusEffect.h"

class Character;

class Transfusion : public StatusEffect
{
public:
	Transfusion(Character* effector);
	~Transfusion(void);

	virtual StatEffectType GetType(void) const override { return StatEffectType::Transfusion; }

	virtual void Update(float dt) override;

	virtual bool Complete(void) override { return true; } // transfusion happens instantly, no need to wait up!
};