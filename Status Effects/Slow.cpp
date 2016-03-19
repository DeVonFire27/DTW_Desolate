/***************************************************************
|	File:		Slow.cpp
|	Author:		Michael Mozdzierz
|	Date:		04/11/2014
|
***************************************************************/

#include "globals.h"
#include "Slow.h"

Slow::Slow(Character* effector)
{
	affectors.emplace(StatType::moveSpeed, 0.5f);
	SetEffector(effector);


	duration = 1.f;
	SetEmitter(GameplayState::GetInstance()->CreateEmitter("stunEffect"));
}

Slow::~Slow(void)
{
	SetEffector(nullptr);
	SetEmitter(nullptr);
}