/***************************************************************
|	File:		GatlingGun.cpp
|	Author:		Michael Mozdzierz
|	Date:		05/13/2014
|
***************************************************************/

#include "globals.h"
#include "GatlingGun.h"
#include "../../Math.h"
#include "../../Agents/Character.h"
#include "../../Status Effects/Slow.h"
#include "../../Status Effects/StatusEffect.h"

GatlingGun::GatlingGun(void)
{
	damage = 8.f;
	rateOfFire = 1.f;
	rof_current = 1.f / rateOfFire;
	rof_affected = 1.f / rateOfFire;
	sway = 45.f;
	m_hImage = SGD::GraphicsManager::GetInstance()->LoadTexture("resources\\graphics\\weaponPickups\\gatlingGun.png");
	m_szSize = { 120.f, 50.f };

	heldImg = SGD::GraphicsManager::GetInstance()->LoadTexture("resources\\graphics\\weaponsHeld\\minigun_top.png");
	m_szSize = SGD::Size{ 34, 88 };
}

GatlingGun::~GatlingGun(void)
{
	SGD::GraphicsManager::GetInstance()->UnloadTexture(m_hImage);
}

/*virtual*/ void GatlingGun::Update(float dt) /*override*/
{
	rof_affected -= dt;
	fireCoolDown = rof_affected;
	
	if (inactiveTimer > 0.f)
		inactiveTimer -= dt;
	else
	{
		rof_affected = rateOfFire;
		rof_current = rateOfFire;
	}

	Weapon::Update(dt);
}

/*virtual*/ bool GatlingGun::Use(void) /*override*/
{
	inactiveTimer = 0.1f;
	if (rof_affected <= 0.f)
	{
		rof_current = Math::Floor(((rof_current * 10.f) * 0.75f) / 10.f, 0.04f);
		rof_affected = rof_current;
		Attack();
		SGD::AudioManager::GetInstance()->PlayAudio(GameplayState::GetInstance()->gatlingGun);

		owner->AddStatusAilment(new Slow(owner));

		// Default velocity vector
		SGD::Vector velocity = SGD::Vector(0, -1);

		// Rotating the vector to match the character's rotation
		velocity.Rotate(owner->GetRotation());

		// Inverting it
		velocity = -velocity;

		// Applying a recoil to the character who used this weapon
		owner->weaponRecoil += velocity * 10;


		return true;
	}

	return false;
}