#include "globals.h"
#include "Radicorn.h"
#include "NPC.h"
#include "Player.h"

Radicorn::Radicorn()
{
	m_szSize = { 80, 64 };
	m_hImage = SGD::GraphicsManager::GetInstance()->LoadTexture("resources/graphics/radicorn.png");

	health = 1500;
	currHealth = health;
	accuracy = 0;
	dexterity = 65;
	defense = 50;
	moveSpeed = 350;

	rateOfAttack = (100 - dexterity) * 0.01f;
	attackTimer = rateOfAttack;
}

void Radicorn::SetStats()
{

}

void Radicorn::Render()
{
	SGD::GraphicsManager::GetInstance()->DrawTextureSection(m_hImage, m_ptPosition,
	{ 0, 0, 80, 64 }, m_fRotation - SGD::PI/2, m_szSize / 2);

	if (isEnemy && currHealth < health)
	{
		SGD::GraphicsManager::GetInstance()->DrawRectangle({ GetPosition().x, GetPosition().y - 15.0f, GetPosition().x + 50.0f, GetPosition().y - 5.0f }, { 25, 25, 25 });
		SGD::GraphicsManager::GetInstance()->DrawRectangle({ GetPosition().x, GetPosition().y - 15.0f, GetPosition().x + ((currHealth / health * 50)), GetPosition().y - 5.0f }, { 25, 255, 25 });
	}
}

void Radicorn::UseAbility()
{
	currCooldown--;
	if (currCooldown < 0)
		currCooldown = 0;
	//radicorn dashes forwards, stops at the first collidable object (tile or character)
	//hit from the dash apply bleed for a number of seconds and 5 radiation stacks
	if (currCooldown == 0)
	{
		dashing = true;
		targetPosition.x = FindControllingNPC(this)->GetTarget()->GetPosition().x;
		targetPosition.y = FindControllingNPC(this)->GetTarget()->GetPosition().y;
	}
}

void Radicorn::DashTowards()
{
	//TODO: make it keep going rather that stopping once it hits the target location if it hasn't anything yet!
	SGD::Vector toTarget = (Point(targetPosition.x, targetPosition.y)) - m_ptPosition;
	toTarget.Normalize();
	targetPosition = toTarget;
	toTarget *= (float)GetMoveSpeed();
	
	m_vtVelocity += toTarget;
	isMoving = true;
}
void Radicorn::HandlePassive()
{
	//60% critical hit chance
	critHitChance = 60;
}