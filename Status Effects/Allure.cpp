/***************************************************************
|	File:		Allure.cpp
|	Author:		Michael Mozdzierz
|	Date:		04/11/2014
|
***************************************************************/

#include "globals.h"
#include "Allure.h"

Allure::Allure(Character* effector, Character* tgt)
{
	SetEffector(effector);

	this->tgt = tgt;
	duration = 2.5f;

	SetEmitter(GameplayState::GetInstance()->CreateEmitter("allure"));
}

Allure::~Allure(void)
{
	SetEffector(nullptr);
	DestroyEntityMsg* msg = new DestroyEntityMsg(emitter);
	msg->QueueMessage();
}

/*virtual*/ void Allure::Update(float dt) /*override*/
{
	float direction = atan2(tgt->GetPosition().y - effector->GetPosition().y, tgt->GetPosition().x - effector->GetPosition().x);

	effector->SetVelocity(SGD::Vector { cos(direction), sin(direction) } *effector->GetStat(StatType::moveSpeed) * 0.5f);

	StatusEffect::Update(dt);
}