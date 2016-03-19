/***************************************************************
|	File:		HealthPickup.cpp
|	Author:		Michael Mozdzierz
|	Date:		04/14/2014
|
***************************************************************/

#include "globals.h"
#include "HealthPickup.h"

HealthPickup::HealthPickup(SGD::Point pos)
{
	m_hImage = SGD::GraphicsManager::GetInstance()->LoadTexture("resources\\graphics\\Medkit.png");
	m_szSize = { 50, 50 };
	m_ptPosition = pos;
}

HealthPickup::~HealthPickup()
{
	SGD::GraphicsManager::GetInstance()->UnloadTexture(m_hImage);
}

void HealthPickup::Update(float dt)
{
	timer -= dt;

	if (timer < 0)
	{
		DestroyEntityMsg* msg = new DestroyEntityMsg(this);
		SGD::MessageManager::GetInstance()->QueueMessage(msg);
	}
}

/*virtual*/ void HealthPickup::HandleCollision(const IEntity* other) /*override*/
{
	const Character* temp = dynamic_cast<const Character*>(other);

	if (temp && !temp->GetIsEnemy())
	{
		DestroyEntityMsg* msg = new DestroyEntityMsg(this);
		SGD::MessageManager::GetInstance()->QueueMessage(msg);
	}
}