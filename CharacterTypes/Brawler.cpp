/***************************************************************
|	File:		Brawler.cpp
|	Author:		Michael Mozdzierz
|	Date:		04/11/2014
|
***************************************************************/

#include "globals.h"
#include "Brawler.h"
#include "../Abilities/Chainsaw.h"
#include "../EventProc/EventProc.h"
#include "../GameplayState.h"
#include "../Game.h"
#include "../Weapons/MeleeWeapon.h"
#include "../Animation System/AnimationSystem.h"
#include "../Abilities/Chainsaw.h"
#include "../Messages/CreateAbility.h"
#include "../../SGD Wrappers/SGD_MessageManager.h"
#include "../Agents/Player.h"
#include <sstream>

Brawler::Brawler(void)
{

	color = SGD::Color::Red;
	ability = new Chainsaw();
	ability->SetOwner(this);

	charAnim.SetCurrAnimation("BrawlerWalk");

	dialogueIcon = SGD::GraphicsManager::GetInstance()->LoadTexture("resources/graphics/brawlerDialogueIcon.png");

	cooldown = 8.0f;
	passiveDmg = 0;
	baseDmg = 0;
	healthRegen = 0;
}

Brawler::~Brawler(void)
{
	// unsubscribe to events
	EventProc::GetInstance()->UnSubscribe(this, "Music Town 1");
	EventProc::GetInstance()->UnSubscribe(this, "Music Exploration 1");

	// Unload texture
	SGD::GraphicsManager::GetInstance()->UnloadTexture(dialogueIcon);
	SGD::GraphicsManager::GetInstance()->UnloadTexture(m_hImage);

}

Weapon* Brawler::SetStartingWeapon()
{
	Weapon* startWeapon = nullptr;
	if (this == GameplayState::GetInstance()->player->GetCharacter())
	{
		int diff = Game::GetInstance()->playerDifficulty;
		startWeapon = Game::GetInstance()->CreateWeapon(m_ptPosition, Weapon::GunType::meleeWeapon, diff, false, 1);
		startWeapon->SetPrefix((GunPrefix)diff);

	}
	else
	{
		int randWeapon = rand() % 3;
		int chance = (rand() % 50 + 1) + (level * 2);
		startWeapon = Game::GetInstance()->CreateWeapon(m_ptPosition, Weapon::GunType::meleeWeapon, GetStartingPrefix(chance), false, randWeapon);

	}

	return startWeapon;
}

void Brawler::UseWeapon(void)
{
	if (weapon->GetGunType() == Weapon::GunType::meleeWeapon && !swingWeapon && weapon->GetFireCoolDown() <= 0)
	{
		swingWeapon = true;
		charAnim.SetCurrAnimation("BrawlerAttack");
		Character::UseWeapon();
	}
	else
		Character::UseWeapon();
}

std::stringstream Brawler::GetActiveDiscription()
{
	std::stringstream line;
	switch (GetAbility()->GetSkillLvl())
	{
	case 1:
		line << " Chainsaw Slash: Swings his chainsaw arm in front of him\n damaging and shooting 3 buzzsaws,\n causing bleeding damage.";
		break;
	case 2:
		line << " Chainsaw Dismemberment:\n Swings a chainsaw throwing 3 buzzsaws in a cone\n damaging foes, causingbleeding damage and\n stunning enemies,healing with each hit!";
		break;
	case 3:
		line << " Chainsaw of Bloody Dismemberment:\n Slashes his chainsaw arm, damaging and shooting\n 3 buzzsaws. Causes bleeding and stun,healing\n and reducing the cooldown for each foe hit!!";
		break;
	}

	return line;
}


std::stringstream Brawler::GetPassiveDiscription()
{
	std::stringstream line;
	line << "Stubborn: Takes reduce damage from all sources.";

	if (level >= 6)
		line << "\n\nAdrenaline High: Slowly regenerate health over time.";
	if (level >= 15)
		line << "\n\nWeapon Arm: Does increase damage with melee damage.";
	if (level >= 18)
		line << "\n\nChainsaw Arm: Knockback bash applies Bleeding.";
	if (level >= 24)
		line << "\n\nSavage Excitement: Heals a small amount every time you\nkill a enemy.";

	return line;
}


/*virtual*/ void Brawler::UseAbility(void) /*override*/
{
	if (!cooldownTimerStarted && currHealth > 0)
	{
		CreateAbility* msg = new CreateAbility(ability);
		SGD::MessageManager::GetInstance()->QueueMessage(msg);
		charAnim.SetCurrAnimation("BrawlerActive");
		Character::UseAbility();
		chainsawArm = true;
	}
}

/*virtual*/ void Brawler::HandlePassive(void) /*override*/
{

}

void Brawler::Update(float dt)
{
	// health regen @ level 6+

	if (GetLevel() >= 6)
	{
		if (currHealth < health)
			ModifyHealth((health * .015f) * dt);
	}

	if (ability->GetDuration() < 0 && chainsawArm)
	{
		chainsawArm = false;
		swingWeapon = false;
		charAnim.SetCurrAnimation("BrawlerWalk");
	}

	if (swingWeapon && charAnim.HasEnded())
	{
		swingWeapon = false;
		chainsawArm = false;
		charAnim.SetCurrAnimation("BrawlerWalk");
	}

	// call default update
	Character::Update(dt);

	if (isMoving && charAnim.GetCurrAnimation() != "BrawlerWalk" && !swingWeapon && !chainsawArm)
		charAnim.SetCurrAnimation("BrawlerWalk");
	else if (!isMoving && !swingWeapon && !chainsawArm)
		charAnim.SetCurrAnimation("BrawlerIdle");
}

void Brawler::ModifyHealth(float amount)
{
	if (amount < 0)
	{
		// damage reduction @ level 1+ (5% + level up to 30% max)
		float tempAmount = -amount;
		float dmgReduction = 10.0f + (float)GetLevel();
		if (dmgReduction > 25)
			dmgReduction = 25;
		dmgReduction *= 0.01f;
		
		// if brawler is losing health, reduce amount lost
		tempAmount *= dmgReduction;
		amount += tempAmount;
	}

	// modifyHealth by currently modified/unmodified amount
	Character::ModifyHealth(amount);
}