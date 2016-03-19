/***************************************************************
|	File:		Bleeding.h
|	Author:		Michael Mozdzierz
|	Date:		04/11/2014
|
***************************************************************/

#pragma once
#include "StatusEffect.h"


class Bleeding : public StatusEffect
{
public:
	Bleeding(Character* effector);
	~Bleeding(void);

	virtual void Update(float dt) override;

	virtual StatEffectType GetType(void) const override { return StatEffectType::Bleeding; }
};