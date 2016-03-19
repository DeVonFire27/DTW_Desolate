/***************************************************************
|	File:		Radiation.cpp
|	Author:		Michael Mozdzierz
|	Date:		04/11/2014
|
***************************************************************/

#include "globals.h"
#include "Radiation.h"

Radiation::Radiation(Character* effector)
{
	// ensure no garbage in data member
	stacks = 1;
	// copy-in prameter
	SetEffector(effector);

	// duration is always four seconds
	duration = 5.0f;

	// add particle effect
	SetEmitter(GameplayState::GetInstance()->CreateEmitter("radiationEffect"));

}

Radiation::~Radiation(void)
{
	SetEffector(nullptr);
	SetEmitter(nullptr);
}

void Radiation::SetStacks(int value)
{
	// reset duration
	duration = 5.f;
	// set the passed-in value
	stacks = value;
}

/*virtual*/ void Radiation::Update(float dt) /*final*/
{
	if (time >= 1.f)
	{
		// find how much damage we're going to do this frame
		float ammount = ((effector->GetHealth() - effector->GetCurrHealth()) * 0.005f) + (effector->GetLevel()*1.5f);
		ammount *= (float)stacks;
		// apply the damage to the effector
		effector->ModifyHealth(-ammount);
		// reset time
		time = 0.f;
		stacks++;
	}

	// call the inherited function
	StatusEffect::Update(dt);
}