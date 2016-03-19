/***************************************************************
|	File:		Cyborg.cpp
|	Author:		Michael Mozdzierz
|	Date:		04/11/2014
|
***************************************************************/

#include "globals.h"
#include "Cyborg.h"
#include "../Animation System/AnimationSystem.h"
#include "../GameplayState.h"
#include "../Abilities/Nuke.h"
#include "../GameplayState.h"
#include "../Particle System/Emitter.h"
#include "../Agents/Player.h"
#include "../Agents/Character.h"

#include "../Game.h"
#include "../Weapons/Shotgun.h"
#include "../EventProc/EventProc.h"
#include <sstream>


Cyborg::Cyborg(void)
{

	//m_hImage = SGD::GraphicsManager::GetInstance()->LoadTexture("resources//graphics//normalv2.png");
	ability = new Nuke();
	ability->SetOwner(this);

	charAnim.SetCurrAnimation("CyborgWalk");


	dialogueIcon = SGD::GraphicsManager::GetInstance()->LoadTexture("resources/graphics/cyborgDialogueIcon.png");

	Energyshield = GameplayState::GetInstance()->CreateEmitter("cyborg_shield");

	SGD::Point offset = AnimationSystem::GetInstance()->GetAnchorPoint(GetAnimation(), m_ptPosition);

	Energyshield->SetPosition(m_ptPosition);
	shieldHP = GetStat(StatType::health)*0.25f;
	Energyshield->SetParticleSize(1.0f, 3.0f);

	color = SGD::Color::Blue;
	cooldown = 26;
}

Cyborg::~Cyborg(void)
{
	// unsubscribe to events
	EventProc::GetInstance()->UnSubscribe(this, "Music Town 1");
	EventProc::GetInstance()->UnSubscribe(this, "Music Exploration 1");

	// Unload texture
	SGD::GraphicsManager::GetInstance()->UnloadTexture(m_hImage);
	SGD::GraphicsManager::GetInstance()->UnloadTexture(dialogueIcon);

}

Weapon* Cyborg::SetStartingWeapon()
{
	Weapon* startWeapon = nullptr;
	if (this == GameplayState::GetInstance()->player->GetCharacter())
	{
		int diff = Game::GetInstance()->playerDifficulty;
		startWeapon = Game::GetInstance()->CreateWeapon(m_ptPosition, Weapon::GunType::Shotgun, diff, false, 2);
		startWeapon->SetPrefix((GunPrefix)diff);
		//startWeapon->SetOwner(this);
	}
	else
	{
		int randWeapon = rand() % 3;
		int chance = (rand() % 50 + 1) + (level * 2);
		startWeapon = Game::GetInstance()->CreateWeapon(m_ptPosition, Weapon::GunType::Shotgun, GetStartingPrefix(chance), false, randWeapon);
		//startWeapon->SetOwner(this);
	}

	return startWeapon;
}

std::stringstream Cyborg::GetActiveDiscription()
{
	std::stringstream line;
	switch (GetAbility()->GetSkillLvl())
	{
	case 1:
		line << " Nuke Launcher: Fires a mini nuke that explodes, stunning and\n doing massive damage to the first foe hit while\n damaging and radiate those in the blast radius.";
		break;
	case 2:
		line << " Friendly Mutation Launcher:\n Shoots a nuke that does massive damage and stuns\nthe first foe hit, it damages and radiates foes in the\n blast radius but heals allies!";
		break;
	case 3:
		line << " Implosion Explosion Launcher:\n Shoots a nuke doing massive damage and stuns the first foe hit, pulling in nearby foes into the blast\n radius to damage and radiate them while healing allies!!";
		break;
	}

	return line;
}

void Cyborg::UseAbility(void)
{
	if (!cooldownTimerStarted && currHealth > 0)
	{
		CreateAbility* msg = new CreateAbility(ability);
		SGD::MessageManager::GetInstance()->QueueMessage(msg);
		Character::UseAbility();
	}
}

std::stringstream Cyborg::GetPassiveDiscription()
{
	std::stringstream line;
	line << "Energy Shield: Has a shield that blocks damage. Shield resets after\n10 seconds of not taking damage.";

	if (level >= 6)
		line << "\n\nRadiation Pulse: Knockback applies Radiation.";
	if (level >= 15)
		line << "\n\nSpray'n Pray: Increase damage with shotguns.";
	if (level >= 18)
		line << "\n\nChain Lightning: Killing a enemy damage 3 nearby enemies.";
	if (level >= 24)
		line << "\n\nStatic Storm: Chain Lightning stuns and applies Radiation.";

	return line;
}

void Cyborg::ModifyHealth(float value)
{
	if (shieldHP > 0 && value < 0)
	{
		float tempDmg = -value;
		float tempShield = shieldHP;
		tempShield -= tempDmg;
		shieldTimer = 10.0f;

		if (tempShield < 0)
		{
			shieldHP = 0;
			if (!isEnemy)
				SGD::AudioManager::GetInstance()->PlayAudio(GameplayState::GetInstance()->shield);
		}
		else
			shieldHP = tempShield;

		float shieldPercent = shieldHP / (GetStat(StatType::health)*0.2f);

		Energyshield->SetParticleSize(shieldPercent, shieldPercent*3.0f);

	}
	else if (value < 0)
	{
		shieldTimer = 10.0f;
		Energyshield->active = false;
		Character::ModifyHealth(value);
	}
	else
		Character::ModifyHealth(value);

}

void Cyborg::UseWeapon(void)
{
	if (weapon->GetGunType() == Weapon::GunType::meleeWeapon && !swingWeapon && weapon->GetFireCoolDown() <= 0)
	{
		swingWeapon = true;
		charAnim.SetCurrAnimation("CyborgAttack");
		Character::UseWeapon();
	}
	else
		Character::UseWeapon();
}

void Cyborg::Update(float deltaTime)
{
	if (shieldTimer > 0)
	{
		shieldTimer -= deltaTime;
		recharge = true;
	}

	if (swingWeapon && charAnim.HasEnded())
	{
		swingWeapon = false;
		charAnim.SetCurrAnimation("CyborgWalk");
	}

	Character::Update(deltaTime);

	if (isMoving &&  charAnim.GetCurrAnimation() != "CyborgWalk" && !swingWeapon)
		charAnim.SetCurrAnimation("CyborgWalk");
	else if (!isMoving && !swingWeapon)
		charAnim.SetCurrAnimation("CyborgIdle");

	SGD::Point offset = m_ptPosition;
	offset.x += (m_szSize.width*0.5f);
	offset.y += (m_szSize.height*0.5f);

	Energyshield->SetPosition(offset);
}

void Cyborg::HandlePassive(void)
{
	if (shieldTimer <= 0 && recharge)
	{
		shieldHP = GetStat(StatType::health)*0.25f;
		recharge = false;
		Energyshield->active = true;
		Energyshield->SetParticleSize(1.0f, 3.0f);
		SGD::AudioManager::GetInstance()->PlayAudio(GameplayState::GetInstance()->shieldFull);

	}
}
