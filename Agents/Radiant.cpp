/***************************************************************
|	File:		Radiant.cpp
|	Author:		Ethan Mills
|	Date:		04/13/2014
|
***************************************************************/

#include "globals.h"
#include "Radiant.h"
#include "../Game.h"
#include "../Weapons/Weapon.h"
#include "../Weapons/Pistol.h"

Radiant::Radiant(int lvl)
{
	//m_hImage = SGD::GraphicsManager::GetInstance()->LoadTexture("resources/graphics/radiant.png");
	m_szSize = {  64, 64 };
	level = lvl;
	rateOfAttack = (100 - dexterity) * 0.01f;
	attackTimer = rateOfAttack;
	SetStats();
	charAnim.SetCurrAnimation("Rad_Idle");

	SetWeapon(new Pistol(Pistol::PistolType::spittle));	
	weapon->Release();
}

Radiant::~Radiant(void)
{
	SetWeapon(nullptr);
}

void Radiant::SetStats()
{
	health = 70 + 55.0f * level;
	currHealth = health;
	strength = 4 + 4.0f * level;
	accuracy = 11 + 7.5f * level;
	dexterity = 55.0f + level;
	defense = 65 + 11.0f * level;
	moveSpeed = 360;
}

void Radiant::HandlePassive()
{
	//immune to radiation, chance of dodging (maybe deflects bullets instead of dodging? Making Melee the best way of defeating them)
	
	
}

void Radiant::Update(float dt)
{
	Mutant::Update(dt);

	if (isMoving &&  charAnim.GetCurrAnimation() != "Rad_Walk")
		charAnim.SetCurrAnimation("Rad_Walk");
	else if (!isMoving)
		charAnim.SetCurrAnimation("Rad_Idle");

}

void Radiant::Render()
{
	Character::Render();
	if (isEnemy && currHealth < health)
	{
		SGD::GraphicsManager::GetInstance()->DrawRectangle({ GetPosition().x, GetPosition().y - 15.0f, GetPosition().x + 50.0f, GetPosition().y - 5.0f }, { 25, 25, 25 });
		SGD::GraphicsManager::GetInstance()->DrawRectangle({ GetPosition().x, GetPosition().y - 15.0f, GetPosition().x + ((currHealth / health * 50)), GetPosition().y - 5.0f }, { 25, 255, 25 });
	}
}