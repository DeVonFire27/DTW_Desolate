/***************************************************************
|	File:		Allure.h
|	Author:		Michael Mozdzierz
|	Date:		04/11/2014
|
***************************************************************/

#pragma once

#include "StatusEffect.h"

class Character;

class Allure : public StatusEffect
{
	Character* tgt;
public:
	Allure(Character* effector, Character* tgt);
	~Allure(void);

	virtual StatEffectType GetType(void) const override { return StatEffectType::Allure; }

	virtual void Update(float dt) override;
};