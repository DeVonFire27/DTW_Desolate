/***************************************************************
|	File:		Mutant.cpp
|	Author:		Ethan Mills
|	Date:		04/13/2014
|
***************************************************************/

#include "globals.h"
#include "Mutant.h"
#include "../EventProc/EventProc.h"
#include "../GameplayState.h"
#include "../Messages/CreateHealthPickupMsg.h"
#include "Player.h"

Mutant::Mutant(void)
{
	weapon = nullptr;
	mutantPassiveCooldown = 0.f;
	mutantPassiveCooldown = 0.f;
}

void Mutant::Update(float dt)
{
	Character::Update(dt);
	mutantPassiveCooldown += dt;
	mutantActiveCooldown += dt;
	attackTimer -= dt;
}
void Mutant::HandlePassive(void)
{

}

void Mutant::UseAbility(void)
{

}

float Mutant::GetStat(StatType type)
{
	switch (type)
	{
	case StatType::health:
		return health;
		break;
	case StatType::strength:
		return strength;
		break;
	case StatType::accuracy:
		return accuracy;
		break;
	case StatType::dexterity:
		return dexterity;
		break;
	case StatType::defense:
		return defense;
		break;
	case StatType::moveSpeed:
		return (float)moveSpeed;
		break;
	}

	return 0.0f;
}

void Mutant::Render()
{
	Character::Render();
	if (isEnemy && currHealth < health)
	{
		SGD::GraphicsManager::GetInstance()->DrawRectangle({ GetPosition().x, GetPosition().y - 15.0f, GetPosition().x + 50.0f, GetPosition().y - 5.0f }, { 25, 25, 25 });
		SGD::GraphicsManager::GetInstance()->DrawRectangle({ GetPosition().x, GetPosition().y - 15.0f, GetPosition().x + ((currHealth / health * 50)), GetPosition().y - 5.0f }, { 25, 255, 25 });
	}
}