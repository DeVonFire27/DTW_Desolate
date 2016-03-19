/***************************************************************
|	File:		Sniper.cpp
|	Author:		Michael Mozdzierz
|	Date:		04/11/2014
|
***************************************************************/

#include "globals.h"
#include "Sniper.h"
#include "../Abilities/Laser.h"
#include "../Messages/CreateAbility.h"
#include "../EventProc/EventProc.h"
#include "../Weapons/SniperRifle.h"
#include "../Game.h"
#include "../GameplayState.h"
#include "../Agents/Player.h"
#include "../Agents/Character.h"
#include <sstream>


Sniper::Sniper(void)
{

	ability = new Laser();
	ability->SetOwner(this);

	charAnim.SetCurrAnimation("SniperWalk");

	dialogueIcon = SGD::GraphicsManager::GetInstance()->LoadTexture("resources/graphics/sniperDialogueIcon.png");
	color = SGD::Color::Orange;
	cooldown = 22;

}

Sniper::~Sniper(void)
{
	// unsubscribe to events
	EventProc::GetInstance()->UnSubscribe(this, "Music Town 1");
	EventProc::GetInstance()->UnSubscribe(this, "Music Exploration 1");

	// Unload textures
	SGD::GraphicsManager::GetInstance()->UnloadTexture(m_hImage);
	SGD::GraphicsManager::GetInstance()->UnloadTexture(dialogueIcon);

	delete ability;
	ability = nullptr;
}

Weapon* Sniper::SetStartingWeapon()
{
	Weapon* startWeapon = nullptr;
	if (this == GameplayState::GetInstance()->player->GetCharacter())
	{
		int diff = Game::GetInstance()->playerDifficulty;
		startWeapon = Game::GetInstance()->CreateWeapon(m_ptPosition, Weapon::GunType::SniperRifle, diff, false, 1);
		startWeapon->SetPrefix((GunPrefix)diff);
		//startWeapon->SetOwner(this);
	}
	else
	{
		int randWeapon = rand() % 3;
		int chance = (rand() % 50 + 1) + (level*2);
		startWeapon = Game::GetInstance()->CreateWeapon(m_ptPosition, Weapon::GunType::SniperRifle, GetStartingPrefix(chance), false, randWeapon);
		//startWeapon->SetOwner(this);
	}

	return startWeapon;
}

void Sniper::UseWeapon(void)
{
	if (weapon->GetGunType() == Weapon::GunType::meleeWeapon && !swingWeapon && weapon->GetFireCoolDown() <= 0)
	{
		swingWeapon = true;
		charAnim.SetCurrAnimation("SniperAttack");
		Character::UseWeapon();
	}
	else
		Character::UseWeapon();
}


std::stringstream Sniper::GetActiveDiscription()
{
	std::stringstream line;
	switch (GetAbility()->GetSkillLvl())
	{
	case 1:
		line << "Laser: Channels a beam damaging and slowing all that it hits.";
		break;
	case 2:
		line << "RAD Laser: Channels a beam that damages, slows and radiates enemies hit.";
		break;
	case 3:
		line << "Mutation Disintegration Ray:\n Channels a powerful blast that damages, slows and radiates enemies, heals Sniper with each hit!";
		break;
	}

	return line;
}

std::stringstream Sniper::GetPassiveDiscription()
{
	std::stringstream line;
	line << "Critical Hit: Each shot has a chance to do double damage.";

	if (level >= 6)
		line << "\n\nPiercing Critical: Critical shots applies Bleeding.";
	if (level >= 15)
		line << "\n\nBest Gun: Does increase damage with rifles.";
	if (level >= 18)
		line << "\n\nCrippling Critical: Critical shots applies a mini stun.";
	if (level >= 24)
		line << "\n\nBlood'splosion: Killing enemies cause a explosion of blood, slowing nearby enemies.";
	
	return line;
}

void Sniper::Update(float deltaTime)
{

	if (swingWeapon && charAnim.HasEnded())
	{
		swingWeapon = false;
		charAnim.SetCurrAnimation("SniperWalk");
	}

	Character::Update(deltaTime);

	if (isMoving && charAnim.GetCurrAnimation() != "SniperWalk" && !swingWeapon)
		charAnim.SetCurrAnimation("SniperWalk");
	else if (!isMoving && !swingWeapon)
		charAnim.SetCurrAnimation("SniperIdle");
}

/*virtual*/ void Sniper::UseAbility(void) /*override*/
{
	if (!cooldownTimerStarted && currHealth > 0)
	{
		CreateAbility* msg = new CreateAbility(ability);
		SGD::MessageManager::GetInstance()->QueueMessage(msg);
		Character::UseAbility();
	}
}

/*virtual*/ void Sniper::HandlePassive(void) /*override*/
{
	// set critchance
	SetCritHitChance((int)(10 + (GetLevel() * 2.6f)));
}