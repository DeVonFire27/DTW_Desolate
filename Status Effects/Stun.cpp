/***************************************************************
|	File:		Stun.cpp
|	Author:		Michael Mozdzierz
|	Date:		04/11/2014
|
***************************************************************/

#include "globals.h"
#include "Stun.h"

Stun::Stun(Character* effector)
{
	duration = 4.0f;
	SetEffector(effector);


	// add particle effect
	SetEmitter(GameplayState::GetInstance()->CreateEmitter("stunEffect"));

}

Stun::~Stun(void)
{

	SetEffector(nullptr);

	DestroyEntityMsg* msg = new DestroyEntityMsg(emitter);
	msg->QueueMessage();
}

/*virtual*/ bool Stun::Complete(void) /*override*/
{
	if (StatusEffect::Complete())
	{
		effector->SetStunned(false);
		return true;
	}

	return false;
}