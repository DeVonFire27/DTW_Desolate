/***************************************************************
|	File:		Transfusion.cpp
|	Author:		Michael Mozdzierz
|	Date:		04/11/2014
|
***************************************************************/

#include "globals.h"
#include "Transfusion.h"

Transfusion::Transfusion(Character* effector)
{
	SetEffector(effector);


	duration = 10.0f;

	//eventual particles
	SetEmitter(GameplayState::GetInstance()->CreateEmitter("transfusion"));
}

Transfusion::~Transfusion(void)
{

	SetEffector(nullptr);

	DestroyEntityMsg* msg = new DestroyEntityMsg(emitter);
	SGD::MessageManager::GetInstance()->QueueMessage(msg);
}

void Transfusion::Update(float dt) /*override*/
{
	StatusEffect::Update(dt);
}