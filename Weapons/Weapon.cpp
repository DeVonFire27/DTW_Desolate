/***************************************************************
|	File:		Weapon.cpp
|	Author:		Michael Mozdzierz
|	Date:		04/23/2014
|
***************************************************************/

#include "globals.h"
#include "../Agents/Character.h"
#include "../Messages/DestroyEntityMsg.h"
#include "../Animation System/AnimationSystem.h"
#include "Weapon.h"	
#include "../Abilities/Ability.h"
#include "../Math.h"
#include "../Agents/Player.h"

Weapon::Weapon(void)
{
	prefix = GunPrefix::standard;
	damageDealt = 0;
}

Weapon::~Weapon(void)
{
	SetOwner(nullptr);
	if (m_hImage != SGD::INVALID_HANDLE)
		SGD::GraphicsManager::GetInstance()->UnloadTexture(m_hImage);
	if (heldImg != SGD::INVALID_HANDLE)
		SGD::GraphicsManager::GetInstance()->UnloadTexture(heldImg);
}

Weapon& Weapon::operator= (const Weapon& w)	// assignment operator
{
	rateOfFire = w.rateOfFire;
	damage = w.damage;
	m_hImage = w.m_hImage;
	m_szSize = w.m_szSize;

	return *this;
}

SGD::Point Weapon::GetAnchor()
{
	return 	AnimationSystem::GetInstance()->GetAnchorPoint(owner->GetAnimation(), owner->GetPosition(), owner->GetRotation());
}

SGD::Point Weapon::GetWeaponPoint()
{
	return 	AnimationSystem::GetInstance()->GetWeaponPoint(owner->GetAnimation(), owner->GetPosition(), owner->GetRotation());
}

void Weapon::SetOwner(Character* own)
{
	if (owner != nullptr)
	{
		owner->Release();
		//owner = nullptr;
	}
	owner = own;
	if (own != nullptr)
	{
		
		owner->AddRef();
	}
}
SGD::Rectangle Weapon::GetRect() const
{
	return SGD::Rectangle(m_ptPosition.x, m_ptPosition.y, m_ptPosition.x + m_szSize.height, m_ptPosition.y + m_szSize.width);
}
void Weapon::Attack(void)
{
	// create a message to create a bullet
	//CreateBulletMsg* msg = new CreateBulletMsg(m_ptPosition, 0, m_fRotation + GetSwayRad(), 3000.f, 10.f);
	if (fireCoolDown <= 0 && GetOwner())
	{
		// if 3 seconds have passed since last shot, reset passiveTimer
		if (GetOwner()->shotTimer >= 3.0f && GetOwner()->GetClass() == ClassType::Gunslinger)
			GetOwner()->passiveTimer = 1.5f;

		CreateBulletMsg* msg = new CreateBulletMsg(this);

		// dispatch create bullet message
		SGD::MessageManager::GetInstance()->QueueMessage(msg);

		// level 15+, increase rate of fire 10% (only once, its not a machine gun)
		if (GetGunType() == GunType::Pistol && owner->GetLevel() >= 15 && !rofIncreased)
		{
			rateOfFire *= 1.1f;
			rofIncreased = true;
		}

		// reset fire cooldown
		float atkMod = (100 - owner->GetStat(StatType::dexterity))*0.01f;
		float tempROF = 1 / rateOfFire;
		fireCoolDown = ((tempROF * 2) + atkMod) / 3;

		// if Gunslinger hasn't shot for 3 seconds, reduce attack cooldown by 50% for 1.5 sec. (shotTimer)
		if (GetOwner()->GetClass() == ClassType::Gunslinger && GetOwner()->passiveTimer > 0)
		{
			fireCoolDown *= .5f;
		}
	}
}
/*virtual*/ void Weapon::Update(float dt) /*override*/
{
	// decrament 'fire cool down' by delta time
	fireCoolDown -= dt;

	Entity::Update(dt);

	// call the inherited method
	if (notWorld)
		deadTimer -= dt;

	if (deadTimer < 0)
	{
		// allocate a message to destroy this instance if you make it through the entire vector of bullet types and don't find fmj or fmjftw
		DestroyEntityMsg* msg = new DestroyEntityMsg(this);
		// dispatch destroy message
		SGD::MessageManager::GetInstance()->QueueMessage(msg);
	}
}

void Weapon::Render()
{
	//If too far then dont render
	if ((m_ptPosition - GameplayState::GetInstance()->player->GetPosition()).ComputeLength() > Game::GetInstance()->GetScreenWidth()) return;

	if (heldImg != SGD::INVALID_HANDLE && owner != nullptr)
	{
		if ((owner->GetClass() == ClassType::Sniper || owner->GetClass() == ClassType::Brawler) && owner->GetAbility()->GetDuration() > 0)
			return;

		if (owner != nullptr && owner->GetClass() != ClassType::Mutant)
		{
			m_ptPosition = AnimationSystem::GetInstance()->GetWeaponPoint(owner->GetAnimation(), owner->GetPosition(), owner->GetRotation());
			m_fRotation = owner->GetRotation();
		}

		SGD::Point anchor = AnimationSystem::GetInstance()->GetAnchorPoint(owner->GetAnimation(), owner->GetPosition(), owner->GetRotation());
		SGD::Vector offset = { anchor.x, anchor.y };
		offset.x -= m_ptPosition.x;
		offset.y -= m_ptPosition.y;

		m_ptPosition.y -= (m_szSize.height*0.85f);
		offset.y += (m_szSize.height*0.85f);
		m_ptPosition.x -= (m_szSize.width*0.5f);
		offset.x += (m_szSize.width*0.5f);
		SGD::GraphicsManager::GetInstance()->DrawTexture(heldImg, m_ptPosition, m_fRotation, offset, color);
	}

	else if (owner == nullptr)
		Entity::Render();
}

float Weapon::GetSwayRad()
{
	float temp = 0;
	if(GetGunType() == GunType::Shotgun || GetGunType() == GunType::flameThrower) 
		temp = Math::to_radians((rand() % int(sway) - sway / 2.f) + ((rand() % 20) - 10));
	return temp;
}

/*virtual*/ bool Weapon::Use(void)
{
	return false;
}

/*virtual*/ float Weapon::GetDamage(void)
{
	// store a value indicating what effect the weapons prefix should have on its damage
	float prefix_modifier = 1.f;

	// switch case prefix
	switch (prefix)
	{
	case GunPrefix::rusty:
		// rusty weapons do 70% damage
		prefix_modifier = 0.7f;
		break;
	case GunPrefix::antique:
		// antique weapons do 85% damage
		prefix_modifier = 0.8f;
		break;
	case GunPrefix::standard:
		// standard weapons do 100% damage, skip this case
		break;
	case GunPrefix::highQuality:
		// high quality weapons do 115% damage
		prefix_modifier = 1.15f;
		break;
	case GunPrefix::futuristic:
		// futuristic weapons do 130% damage
		prefix_modifier = 1.3f;
		break;
	}

	if (owner)
	{
		float tempdamage = damage * prefix_modifier + (damage * owner->GetStat(StatType::accuracy) * 0.01f);
		return tempdamage;
	}

	return damage * prefix_modifier;
}

float Weapon::GetCompareDamage()
{
	// store a value indicating what effect the weapons prefix should have on its damage
	float prefix_modifier = 1.f;

	// switch case prefix
	switch (prefix)
	{
	case GunPrefix::rusty:
		// rusty weapons do 70% damage
		prefix_modifier = 0.7f;
		break;
	case GunPrefix::antique:
		// antique weapons do 85% damage
		prefix_modifier = 0.8f;
		break;
	case GunPrefix::standard:
		// standard weapons do 100% damage, skip this case
		break;
	case GunPrefix::highQuality:
		// high quality weapons do 115% damage
		prefix_modifier = 1.15f;
		break;
	case GunPrefix::futuristic:
		// futuristic weapons do 130% damage
		prefix_modifier = 1.3f;
		break;
	}

	return damage * prefix_modifier + (GameplayState::GetInstance()->player->GetCharacter()->GetStat(StatType::accuracy) * damage * 0.01f);
}

char* Weapon::GetPrefixName(void) /*override*/
{
	switch (prefix)
	{
	case GunPrefix::rusty:
		return "Rusty";
	case GunPrefix::antique:
		return "Antique";
	case GunPrefix::standard:
		return "Standard";
	case GunPrefix::highQuality:
		return "High Quality";
	case GunPrefix::futuristic:
		return "Futuristic";
	}

	return nullptr;
}
