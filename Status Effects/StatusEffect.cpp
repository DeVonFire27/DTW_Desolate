/***************************************************************
|	File:		StatusEffect.cpp
|	Author:		Michael Mozdzierz
|	Date:		04/11/2014
|
***************************************************************/

#include "globals.h"
#include "StatusEffect.h"
#include "../Particle System/Emitter.h"
#include "../EventProc/EventProc.h"
#include "../Messages/DestroyEntityMsg.h"

StatusEffect::StatusEffect(void)
{
	EventProc::GetInstance()->Subscribe(this, "NPC_DEAD");
}
/*virtual*/ StatusEffect::~StatusEffect(void)
{
	EventProc::GetInstance()->UnSubscribe(this, "NPC_DEAD");

	// destroy the effect of this status ailment, if we have one
	SetEmitter(nullptr);
	// release the reference to the effector
	SetEffector(nullptr);
}

void StatusEffect::SetEffector(Character *set)
{
	if (effector)
	{
		effector->Release();
		effector = nullptr;
	}

	effector = set;

	if (effector)
	{
		effector->AddRef();
	}
}

void StatusEffect::SetEmitter(Emitter *set)
{
	if (emitter)
	{
		//emitter->Release();

		DestroyEntityMsg *msg = new DestroyEntityMsg(emitter);
		msg->QueueMessage();

		emitter = nullptr;
	}

	emitter = set;

	//if (emitter)
	//{
	//	emitter->AddRef();
	//}
}	//

/*virtual*/ void StatusEffect::Update(float dt)
{
	duration -= dt;
	// incrament stored time
	time += dt;

	if (emitter && effector && !end)
		emitter->SetPosition(SGD::Point(effector->GetPosition().x + effector->GetSize().width*0.5f, effector->GetPosition().y + 32));
	if (duration < 0)
		end = true;
}

/*virtual*/ void StatusEffect::HandleEvent(string name, void* args) /*override*/
{
	if (name == "NPC_DEAD")
	{
		if (effector == args)
		{
			SetEffector(nullptr);
			SetEmitter(nullptr);
		}
	}
}