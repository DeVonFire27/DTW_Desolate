/***************************************************************
|	File:		Medic.cpp
|	Author:		Michael Mozdzierz
|	Date:		04/11/2014
|
***************************************************************/

#include "globals.h"
#include "Medic.h"
#include "../Abilities/Ambulance.h"
#include "../Messages/CreateAbility.h"
#include "../EventProc/EventProc.h"
#include "../Weapons/SniperRifle.h"
#include "../Game.h"
#include "../GameplayState.h"
#include "../Agents/Player.h"
#include "../Agents/Character.h"
#include <sstream>


Medic::Medic(void)
{
	ability = new Ambulance();
	ability->SetOwner(this);
	
	charAnim.SetCurrAnimation("MedicWalk");


	dialogueIcon = SGD::GraphicsManager::GetInstance()->LoadTexture("resources/graphics/medicDialogueIcon.png");

	color = SGD::Color::Yellow;
	cooldown = 30;

}

Medic::~Medic(void)
{	
	// unsubscribe to events
	EventProc::GetInstance()->UnSubscribe(this, "Music Town 1");
	EventProc::GetInstance()->UnSubscribe(this, "Music Exploration 1");

	// Unload textures
	SGD::GraphicsManager::GetInstance()->UnloadTexture(m_hImage);
	SGD::GraphicsManager::GetInstance()->UnloadTexture(dialogueIcon);
}

void Medic::UseWeapon(void)
{
	if (weapon->GetGunType() == Weapon::GunType::meleeWeapon && !swingWeapon && weapon->GetFireCoolDown() <= 0)
	{
		swingWeapon = true;
		charAnim.SetCurrAnimation("MedicAttack");
		Character::UseWeapon();
	}
	else
		Character::UseWeapon();
}

/*virtual*/ void Medic::UseAbility(void) /*override*/
{
	if (!cooldownTimerStarted && currHealth > 0)
	{
		CreateAbility* msg = new CreateAbility(ability);
		SGD::MessageManager::GetInstance()->QueueMessage(msg);
		Character::UseAbility();
	}
}

Weapon* Medic::SetStartingWeapon()
{
	Weapon* startWeapon = nullptr;
	if (this == GameplayState::GetInstance()->player->GetCharacter())
	{
		int diff = Game::GetInstance()->playerDifficulty;
		startWeapon = Game::GetInstance()->CreateWeapon(m_ptPosition, Weapon::GunType::SniperRifle, diff, false, 2);
		startWeapon->SetPrefix((GunPrefix)diff);
		//startWeapon->SetOwner(this);
	}
	else
	{
		int randWeapon = rand() % 3;
		int chance = (rand() % 50 + 1) + (level * 2);
		startWeapon = Game::GetInstance()->CreateWeapon(m_ptPosition, Weapon::GunType::SniperRifle, GetStartingPrefix(chance), false, randWeapon);
		//startWeapon->SetOwner(this);
	}

	return startWeapon;
}

std::stringstream Medic::GetActiveDiscription()
{
	std::stringstream line;
	switch (GetAbility()->GetSkillLvl())
	{
	case 1:
		line << "Emergency: Summons a ambulance at cursor location\n that drives by do massive damage to the\n first foe it, healing allies if it kills. Does\n moderate damage to following foes.";
		break;
	case 2:
		line << "Medical Driveby: Summons a ambulance at cursor location that\n drives by do massive damage to the first foe it,\n healing allies if is killing blow. Does moderate\n damage, stuns and applies Transfusion to following foes!";
		break;
	case 3:
		line << "Phoenix Down: Summons a burning ambulance at cursor location\n that drives by do massive damage to the first foe\n it, healing allies if is killing blow. Does moderate\n damage, stuns, burns and applies Transfusion to\n following foes!!";
		break;
	}

	return line;
}

std::stringstream Medic::GetPassiveDiscription()
{
	std::stringstream line;
	line << "Transfusion Shot: Each shot has a chance to apply Transfusion.";

	if (level >= 6)
		line << "\n\nDrugs: Regenerates health over time.";
	if (level >= 15)
		line << "\n\nThe Good Drugs: Increase the heal gained from medkits.";
	if (level >= 18)
		line << "\n\nMedical Treatment: Knockback heals allies of status ailments.";
	if (level >= 24)
		line << "\n\nBlood Transfusion: Killing a enemies heals nearby allies.";

	return line;
}

void Medic::Update(float deltaTime)
{
	if (GetLevel() >= 6)
		ModifyHealth(GetStat(StatType::health)*0.02f*deltaTime);

	if (swingWeapon && charAnim.HasEnded())
	{
		swingWeapon = false;
		charAnim.SetCurrAnimation("MedicWalk");
	}

	Character::Update(deltaTime);

	if (isMoving && charAnim.GetCurrAnimation() != "MedicWalk" && !swingWeapon)
		charAnim.SetCurrAnimation("MedicWalk");
	else if (!isMoving && !swingWeapon)
		charAnim.SetCurrAnimation("MedicIdle");
}

/*virtual*/ void Medic::HandlePassive(void) /*override*/
{
	
}