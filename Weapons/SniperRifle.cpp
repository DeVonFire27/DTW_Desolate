#include "globals.h"
#include "../Game.h"
#include "SniperRifle.h"
#include "../Agents/Character.h"

SniperRifle::SniperRifle(SniperRifleTypes srt)
{
	// store what type of sniper rifle this is
	this->srt = srt;

	SetSize({ 80 * .6f, 50 * .6f });
	if (srt == SniperRifleTypes::Barrett)
	{
		damage = 44;
		rateOfFire = 1.25f;
		sway = 10.f;
		m_hImage = SGD::GraphicsManager::GetInstance()->LoadTexture("resources\\graphics\\weaponPickups\\barret50Cal.png");
		heldImg = SGD::GraphicsManager::GetInstance()->LoadTexture("resources\\graphics\\weaponsHeld\\barret50Cal_top.png");
		m_szSize = SGD::Size{21, 91};
	}
	else if (srt == SniperRifleTypes::Dragunov)
	{
		damage = 38;
		rateOfFire = 1.45f;
		sway = 7.5f;
		m_hImage = SGD::GraphicsManager::GetInstance()->LoadTexture("resources\\graphics\\weaponPickups\\dragunov.png");
		heldImg = SGD::GraphicsManager::GetInstance()->LoadTexture("resources\\graphics\\weaponsHeld\\dragonov_top.png");
		m_szSize = SGD::Size{9, 86};
	}
	else if (srt == SniperRifleTypes::Intervention)
	{
		damage = 51;
		rateOfFire = 1.35f;
		sway = 5.0f;
		m_hImage = SGD::GraphicsManager::GetInstance()->LoadTexture("resources\\graphics\\weaponPickups\\intervention.png");
		heldImg = SGD::GraphicsManager::GetInstance()->LoadTexture("resources\\graphics\\weaponsHeld\\intervention_top.png");
		m_szSize = SGD::Size{18, 90};
	}
}

/*virtual*/ char* SniperRifle::GetName(void) /*override*/
{
	switch (srt)
	{
	case SniperRifleTypes::Barrett:
		return "Barret .50 cal";
	case SniperRifleTypes::Dragunov:
		return "Dragunov";
	case SniperRifleTypes::Intervention:
		return "Intervention";
	}

	return nullptr;
}

void SniperRifle::BuffWeapon()
{
	if (GetPrefix() == GunPrefix::futuristic)
	{
		if (GetSniperType() == SniperRifle::SniperRifleTypes::Barrett)
		{
			//apply 3 stacks of radiation, fire and transfusion status effects
			bulType.push_back(BulletType::fire);
			bulType.push_back(BulletType::lifesteal);
			bulType.push_back(BulletType::radiation);
		}
		else if (GetSniperType() == SniperRifle::SniperRifleTypes::Dragunov)
		{
			//apply an extra 50% damage
			SetDamage(65 * 1.5f);
		}
		else
		{
			//bullets peirce enemies and apply 5 stacks of radiation
			bulType.push_back(BulletType::fmj);
			bulType.push_back(BulletType::radiation);
		}
	}
}

/*virtual*/ bool SniperRifle::Use(void) /*override*/
{
	if (fireCoolDown <= 0.f)
	{
		Attack();
		SGD::AudioManager::GetInstance()->PlayAudio(GameplayState::GetInstance()->sniperRifleFiring);

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