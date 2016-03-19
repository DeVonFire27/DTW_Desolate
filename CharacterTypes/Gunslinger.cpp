/***************************************************************
|	File:		Gunslinger.cpp
|	Author:		Michael Mozdzierz
|	Date:		04/11/2014
|
***************************************************************/

#include "globals.h"
#include "Gunslinger.h"
#include "../Abilities/BulletStorm.h"

#include "../GameplayState.h"
#include "../Agents/Player.h"
#include "../Agents/Character.h"
#include "../EventProc/EventProc.h"
#include "../Weapons/Pistol.h"
#include "../Game.h"

#include "../MessageID.h"
#include "../Messages/DestroyEntityMsg.h"
#include "../Messages/CreateAbility.h"

#include "../Status Effects/Radiation.h"
#include "../Hazards/RadiationPool.h"
#include "../Status Effects/StatusEffect.h"
#include "../Spawn System/SpawnList.h"
#include <sstream>


Gunslinger::Gunslinger(void)
{
	m_hImage = SGD::GraphicsManager::GetInstance()->LoadTexture("resources//graphics//normalv2.png");
	color = SGD::Color::Green;
	ability = new BulletStorm();
	ability->SetOwner(this);


		charAnim.SetCurrAnimation("GunWalk");

	dialogueIcon = SGD::GraphicsManager::GetInstance()->LoadTexture("resources/graphics/gunslingerDialogueIcon.png");

	cooldown = 18;
}

Gunslinger::~Gunslinger(void)
{
	// unsubscribe to events
	EventProc::GetInstance()->UnSubscribe(this, "Music Town 1");
	EventProc::GetInstance()->UnSubscribe(this, "Music Exploration 1");

	// Unloading textures
	SGD::GraphicsManager::GetInstance()->UnloadTexture(m_hImage);
	SGD::GraphicsManager::GetInstance()->UnloadTexture(dialogueIcon);
}

void Gunslinger::UseWeapon(void)
{
	if (weapon->GetGunType() == Weapon::GunType::meleeWeapon && !swingWeapon && weapon->GetFireCoolDown() <= 0)
	{
		swingWeapon = true;
		charAnim.SetCurrAnimation("GunAttack");
		Character::UseWeapon();
	}
	else
		Character::UseWeapon();
}

Weapon* Gunslinger::SetStartingWeapon()
{
	Weapon* startWeapon = nullptr;
	if (this == GameplayState::GetInstance()->player->GetCharacter())
	{
		int diff = Game::GetInstance()->playerDifficulty;
		startWeapon = Game::GetInstance()->CreateWeapon(m_ptPosition, Weapon::GunType::Pistol, diff, false, 1);
		//startWeapon->SetOwner(this);
	}
	else
	{
		int randWeapon = rand() % 3;
		int chance = (rand() % 50 + 1) + (level * 2);
		startWeapon = Game::GetInstance()->CreateWeapon(m_ptPosition, Weapon::GunType::Pistol, GetStartingPrefix(chance), false, randWeapon);
		//startWeapon->SetOwner(this);
	}

	return startWeapon;
}

std::stringstream Gunslinger::GetActiveDiscription()
{
	std::stringstream line;
	switch (GetAbility()->GetSkillLvl())
	{
	case 1:
		line << "Bulletstorm: Spins firing in all directions at intense speed\n with increase damage.";
		break;
	case 2:
		line << "Bullet Hurricane: Spins at incredible speed, pulling in foes\n and firing in all directions at increase speed\n and power!";
		break;
	case 3:
		line << "Bullet Catastrophe: Spins and pull nearby foes at intense\n speed, firing in all directions with increase fire\n rate and power, apply bleed to all foes hit!!";
		break;
	}

	return line;
}

std::stringstream Gunslinger::GetPassiveDiscription()
{
	std::stringstream line;
	line << "Quick Draw: After 3 seconds of not firing, you'll have a massive \nincrease in fire rate.";

	if (level >= 6)
		line << "\n\nCritical Chance: Each shot has a chance to do double damage.";
	if (level >= 15)
		line << "\n\nFavorite Gun: Has increase damage and fire rate with pistols.";
	if (level >= 18)
		line << "\n\nReload: Immediately gains Quick Draw after using Active.";
	if (level >= 24)
		line << "\n\nTrick Shot: Bullets have a chance to ricochet to other nearby\nenemies after hitting.";

	return line;
}

void Gunslinger::Update(float dt)
{
	if (level < 6)
		critHitChance = 0;

	// reduce shotTimer (reset during bullet creation)
	if (passiveTimer == 0)
		shotTimer += dt;

	if (GameplayState::GetInstance()->player && this == dynamic_cast<Gunslinger*>(GameplayState::GetInstance()->player->GetCharacter()))
	{
		if (passiveTimer < 0)
			passiveTimer = 0;
	}

	if (passiveTimer > 0)
		passiveTimer -= dt;

	// using ability = can't move
	if (cooldownTimerStarted && ability->GetDuration() > 0)
	{
		m_fRotation = ability->GetRotation();
		m_vtVelocity = SGD::Vector(0, 0);
		UseWeapon();

		if (weapon)
			weapon->Update(dt*10.0f);
		if (ambulHit > 0)
			ambulHit -= dt;
		if (hitTimer > 0)
			hitTimer -= dt;
		if (pushTimer > 0)
			pushTimer -= dt;

		// if we have enough exp, level up
		if (currExp > GetLevelUpExp() && level < 25)
		{
			LevelUp();
			currExp = 0;
		}

		// if we ded...
		if (currHealth <= 0)
		{
			// signal gain of experience if this is an enemy
			if (isEnemy)
				EventProc::GetInstance()->Dispatch("GainExp", &level);

			ClearStatusEffects();

			int dropAnything = rand() % 100 + 1;
			if (dropAnything < 60)
			{
				int healthKitOrWep = rand() % 2;

				if (healthKitOrWep == 1 && weapon != nullptr)
				{
					CreateWeaponMsg* msg = new CreateWeaponMsg(weapon);
					SGD::MessageManager::GetInstance()->QueueMessage(msg);
				}
				else
				{
					CreateHealthPickupMsg* msg = new CreateHealthPickupMsg(m_ptPosition);

					SGD::MessageManager::GetInstance()->QueueMessage(msg);
				}
			}
			if (GameplayState::GetInstance()->player)
			{
				for (auto i = GameplayState::GetInstance()->player->party.begin(); i != GameplayState::GetInstance()->player->party.end(); i++)
				{
					if (this == (*i)->getCharacter())
					{
						(*i)->getCharacter()->SetInParty(false);
						GameplayState::GetInstance()->player->party.erase(i);
						break;
					}
				}
			}

			DestroyEntityMsg* msg = new DestroyEntityMsg(this);
			SGD::MessageManager::GetInstance()->QueueMessage(msg);

			SpawnList::GetInstance()->SearchAndKill(FindControllingNPC(this));

			EventProc::GetInstance()->Dispatch("NPC_DEAD", this);

			cooldownTimerStarted = false;
			currCooldown = 0;
		}

		// apply each status ailment in the status effects list
		for (unsigned int i = 0; i < effects.size(); i++)
		{
			effects[i]->Update(dt);
			if (effects[i]->Complete())
			{
				effects.erase(effects.begin() + i);
				i--;
			}
		}
	}
	else
	{
		if (swingWeapon && charAnim.HasEnded())
		{
			swingWeapon = false;
			charAnim.SetCurrAnimation("GunWalk");
		}
		Character::Update(dt);

		if (isMoving && charAnim.GetCurrAnimation() != "GunWalk" && !swingWeapon)
			charAnim.SetCurrAnimation("GunWalk");
		else if (!isMoving && !swingWeapon)
			charAnim.SetCurrAnimation("GunIdle");
	}
}


/////////////////////////<Interface>///////////////////////////////////////////////////

/*virtual*/ void Gunslinger::UseAbility(void) /*override*/
{
	if (!cooldownTimerStarted && currHealth > 0)
	{
		CreateAbility* msg = new CreateAbility(ability);
		SGD::MessageManager::GetInstance()->QueueMessage(msg);
		Character::UseAbility();
		weapon->ToggleGunActive();

		// if level 18+, gain 2x shot speed after ability
		if (GetLevel() >= 18)
			passiveTimer = 1.5f;
	}
}

/*virtual*/ void Gunslinger::HandlePassive(void) /*override*/
{
	// set critchance
	if (level >= 6)
		SetCritHitChance(10 + (GetLevel() * 2));
}
