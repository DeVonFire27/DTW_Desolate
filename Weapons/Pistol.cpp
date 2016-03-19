#include "globals.h"
#include "Pistol.h"
#include "Weapon.h"
#include "../Game.h"
#include "../Math.h"
#include "../Agents/Mutant.h"

Pistol::Pistol(PistolType pt)
{
	this->pt = pt;

	SetSize({ 65 * .6f, 43 * .6f });
	if (pt == PistolType::ninemm)
	{
		rateOfFire = 2.63f;
		damage = 20;
		sway = 25.f;
		m_hImage = SGD::GraphicsManager::GetInstance()->LoadTexture("resources\\graphics\\weaponPickups\\9mmPistol.png");
		heldImg = SGD::GraphicsManager::GetInstance()->LoadTexture("resources\\graphics\\weaponsHeld\\9mmPistol_top.png");
		m_szSize = SGD::Size{12, 47};
	}
	else if (pt == PistolType::revolver)
	{
		damage = 40;
		rateOfFire = 1.44f;
		sway = 35.5f;
		m_hImage = SGD::GraphicsManager::GetInstance()->LoadTexture("resources\\graphics\\weaponPickups\\357Magnum.png");
		heldImg = SGD::GraphicsManager::GetInstance()->LoadTexture("resources\\graphics\\weaponsHeld\\357Magnum_top.png");
		m_szSize = SGD::Size{10, 43};

	}
	else if (pt == PistolType::deserteagle)
	{
		damage = 32;
		rateOfFire = 2.3f;
		sway = 15.f;
		m_hImage = SGD::GraphicsManager::GetInstance()->LoadTexture("resources\\graphics\\weaponPickups\\desertEagle.png");
		heldImg = SGD::GraphicsManager::GetInstance()->LoadTexture("resources\\graphics\\weaponsHeld\\desertEagle_top.png");
		m_szSize = SGD::Size{18, 52};

	}
	else if (pt == PistolType::spittle)
	{
		damage = (10 + 5.0f * (int)GameplayState::GetInstance()->player->GetCharacter()->GetLevel());
		rateOfFire = 2.0f;
		sway = 25.0f;
	}
}

/*virtual*/ bool Pistol::Use(void) /*override*/
{
	if (fireCoolDown <= 0.f && owner)
	{
		Attack();
		if (GetPistolType() != PistolType::spittle)
			SGD::AudioManager::GetInstance()->PlayAudio(GameplayState::GetInstance()->pistolFiring);
		else
			SGD::AudioManager::GetInstance()->PlayAudio(GameplayState::GetInstance()->spitAttack);

		// Default velocity vector
		SGD::Vector velocity = SGD::Vector(0, -1);

		// Rotating the vector to match the character's rotation
		velocity.Rotate(owner->GetRotation());

		// Inverting it
		velocity = -velocity;

		// Applying a recoil to the character who used this weapon
		owner->weaponRecoil += velocity * 5;

		return true;
	}

	return false;
}

void Pistol::BuffWeapon()
{
	if (GetPrefix() == GunPrefix::futuristic)
	{
		if (GetPistolType() == Pistol::PistolType::ninemm)
		{
			SetRateOfFire(6.66f);
		}
		else if (GetPistolType() == Pistol::PistolType::revolver)
		{
			bulType.push_back(BulletType::FMJFTW);
		}
		else //desert eagle
		{
			//+25% critical hit chance
			bulType.push_back(BulletType::critChance);
		}
	}
}

/*virtual*/ char* Pistol::GetName(void) /*override*/
{
	switch (pt)
	{
	case PistolType::deserteagle:
		return "Desert Eagle";
	case PistolType::ninemm:
		return "9mm Pistol";
	case PistolType::revolver:
		return ".357 Magnum";
	}

	return nullptr;
}
