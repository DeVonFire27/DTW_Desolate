/***************************************************************
|	File:		Burning.cpp
|	Author:		Michael Mozdzierz
|	Date:		04/11/2014
|
***************************************************************/

#include "globals.h"
#include "Burning.h"

Burning::Burning(Character* effector)
{
	affectors.emplace(StatType::defense, 0.75f);
	SetEffector(effector);

	duration = 6.f;
	SetEmitter(GameplayState::GetInstance()->CreateEmitter("fire"));

}

Burning::~Burning(void)
{

	SetEffector(nullptr);
	SetEmitter(nullptr);
}

/*virtual*/ void Burning::Update(float dt) /*override*/
{
	if (time > 1.f)
	{
		float damage = effector->GetHealth() * 0.05f;
		damage += (effector->GetLevel() * 2.5f);
		effector->ModifyHealth(-damage);
		// reset the time
		time = 0.f;
	}
	// call inherited function
	StatusEffect::Update(dt);
}