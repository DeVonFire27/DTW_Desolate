/***************************************************************
|	File:		Bleeding.cpp
|	Author:		Michael Mozdzierz
|	Date:		04/11/2014
|
***************************************************************/

#include "globals.h"
#include "Bleeding.h"

Bleeding::Bleeding(Character* effector)
{
	SetEffector(effector);
	affectors.emplace(StatType::moveSpeed, 0.2f);

	// set duration to 3 seconds
	duration = 5.f;

	// get particle effect
	SetEmitter(GameplayState::GetInstance()->CreateEmitter("bleedingEffect"));

}

Bleeding::~Bleeding(void)
{

	SetEffector(nullptr);
	SetEmitter(nullptr);
}

/*virtual*/ void Bleeding::Update(float dt) /*override*/
{
	if (time >= 1)
	{
		// find how much damage to do in this frame
		float damage = effector->GetCurrHealth() * 0.06f;
		damage += (effector->GetLevel() * 2.5f);
		// apply damage to effector
		effector->ModifyHealth(-damage);
		// reset timer
		time = 0.f;
	}
	// call the inherited function
	StatusEffect::Update(dt);
}