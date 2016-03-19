/***************************************************************
|	File:		Stun.h
|	Author:		Michael Mozdzierz
|	Date:		04/11/2014
|
***************************************************************/

#pragma once

#include "StatusEffect.h"

class Stun : public StatusEffect
{
public:
	Stun(Character* effector);
	~Stun(void);

	virtual StatEffectType GetType(void) const override { return StatEffectType::Stun; }

	virtual bool Complete(void) override;
};