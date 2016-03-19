/***************************************************************
|	File:		BuzzsawLauncher.cpp
|	Author:		Michael Mozdzierz
|	Date:		05/13/2014
|
***************************************************************/

#include "globals.h"
#include "BuzzsawLauncher.h"
#include "../../Math.h"

BuzzsawLauncher::BuzzsawLauncher(void)
{
	rateOfFire = 2.85f;
	damage = 25.f;
	sway = 10.f;
	m_hImage = SGD::GraphicsManager::GetInstance()->LoadTexture("resources\\graphics\\weaponPickups\\buzzsawLauncher.png");

	heldImg = SGD::GraphicsManager::GetInstance()->LoadTexture("resources\\graphics\\weaponsHeld\\buzzSawLauncher_top.png");
	m_szSize = SGD::Size{ 16, 53 };
}


/*virtual*/ bool BuzzsawLauncher::Use(void) /*override*/
{
	if (fireCoolDown <= 0.f)
	{
		Attack();
		SGD::AudioManager::GetInstance()->PlayAudio(GameplayState::GetInstance()->chainSawLauncher);

		// Default velocity vector
		SGD::Vector velocity = SGD::Vector(0, -1);

		// Rotating the vector to match the character's rotation
		velocity.Rotate(owner->GetRotation());

		// Inverting it
		velocity = -velocity;

		// Applying a recoil to the character who used this weapon
		owner->weaponRecoil += velocity * 12;

		return true;
	}

	return false;
}

float BuzzsawLauncher::GetSwayRad()
{
	if (isBrawlActive)
		return Math::to_radians((rand() % int(45.0f) - 45.0f / 2.f) + ((rand() % 20) - 10));
	return Weapon::GetSwayRad();
}