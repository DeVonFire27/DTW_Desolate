
/***************************************************************
|	File:		MeleeWeapon.cpp
|	Author:		Michael Mozdzierz
|	Date:		04/18/2014
|
***************************************************************/

#include "globals.h"
#include "MeleeWeapon.h"
#include "../Game.h"
#include "../EventProc/EventProc.h"
#include "../GameplayState.h"
#include "../Messages/CreateBulletMsg.h"
#include "../../SGD Wrappers/SGD_MessageManager.h"
#include "../Animation System/AnimationSystem.h"


MeleeWeapon::MeleeWeapon(meleeWeaponTypes mwt)
{
	this->mwt = mwt;
	SetSize({ 50, 50 });
	if (mwt == meleeWeaponTypes::sharpPointyStick)
	{
		damage = 20;
		rateOfFire = 3.0f;
		m_hImage = SGD::GraphicsManager::GetInstance()->LoadTexture("resources\\graphics\\weaponPickups\\sharpPointyStick.png");
		meleeWeaponSound = GameplayState::GetInstance()->pointyStickSwinging;
		heldImg = SGD::GraphicsManager::GetInstance()->LoadTexture("resources\\graphics\\weaponsHeld\\sharpPointyStick_top.png");
		m_szSize = SGD::Size{9, 45};
	}
	else if (mwt == meleeWeaponTypes::knife)
	{
		damage = 40;
		rateOfFire = 2.0f;
		m_hImage = SGD::GraphicsManager::GetInstance()->LoadTexture("resources\\graphics\\weaponPickups\\knife.png");
		meleeWeaponSound = GameplayState::GetInstance()->knifeSwinging;
		heldImg = SGD::GraphicsManager::GetInstance()->LoadTexture("resources\\graphics\\weaponsHeld\\knife_top.png");
		m_szSize = SGD::Size{16, 54};
	}
	else if (mwt == meleeWeaponTypes::sledgehammer)
	{
		damage = 60;
		rateOfFire = 1.25f;
		m_hImage = SGD::GraphicsManager::GetInstance()->LoadTexture("resources\\graphics\\weaponPickups\\sledgehammer.png");
		meleeWeaponSound = GameplayState::GetInstance()->sledgehammerSwinging;
		heldImg = SGD::GraphicsManager::GetInstance()->LoadTexture("resources\\graphics\\weaponsHeld\\sledgeHammer_top.png");
		m_szSize = SGD::Size{26, 77};
	}
}

MeleeWeapon::~MeleeWeapon()
{
	SGD::AudioManager::GetInstance()->UnloadAudio(meleeWeaponSound);
}

/*virtual*/ char* MeleeWeapon::GetName(void) /*override*/
{
	switch (this->mwt)
	{
	case meleeWeaponTypes::knife:
		return "Knife";
	case meleeWeaponTypes::sharpPointyStick:
		return "Sharp Pointy Stick";
	case meleeWeaponTypes::sledgehammer:
		return "Sledgehammer";
	default:
		return nullptr;
	}
}

/*virtual*/ void MeleeWeapon::Attack(void) /*override*/
{
	//EventProc::GetInstance()->Dispatch("meleeWeaponUsed", this);
	//// reset fire cooldown
	//fireCoolDown = 1.f / rateOfFire;
	SGD::AudioManager::GetInstance()->PlayAudio(meleeWeaponSound);

	CreateBulletMsg* msg = new CreateBulletMsg(this);
	SGD::MessageManager::GetInstance()->QueueMessage(msg);

	// set the cool-down timer
	if (owner)
	{
		float atkMod = (100 - owner->GetStat(StatType::dexterity))*0.01f;
		float tempROF = 1 / rateOfFire;
		fireCoolDown = ((tempROF * 2) + atkMod) / 3;
	}
}

/*virtual*/ bool MeleeWeapon::Use(void) /*override*/
{
	if (fireCoolDown <= 0.f)
	{
		Attack();
		return true;
	}

	return false;
}

void MeleeWeapon::BuffWeapon()
{

}

float MeleeWeapon::GetSwayRad()
{
	return 0.0f;
}

/*virtual*/ float MeleeWeapon::GetDamage(void) /*override*/
{
	// if Brawler, increase damage @ level 15+
	if (owner)
	{
		if (this->GetOwner()->GetClass() == ClassType::Brawler && GetOwner()->GetLevel() >= 15)
		{
			damageDealt = (damage + (damage * owner->GetStat(StatType::strength) * 0.01f)) * 1.15f;
			return (damageDealt);
		}

		// if not, return normal damage
		damageDealt = damage + (damage * owner->GetStat(StatType::strength) * 0.01f);
	}

	return damageDealt;
}