#include "globals.h"
#include "Shotgun.h"
#include "../Game.h"
#include "../Math.h"

Shotgun::Shotgun(ShotgunTypes st)
{
	// store what kind of shotgun this is
	this->st = st;
	// set the size
	SetSize({ 125 * .6f, 50 * .6f });
	if (st == ShotgunTypes::DoubleBarrel)
	{
		numPellets = 4;
		damage = 30;
		rateOfFire = 1.15f;
		sway = 55.f;
		m_hImage = SGD::GraphicsManager::GetInstance()->LoadTexture("resources\\graphics\\weaponPickups\\doubleBarrelShotgun.png");
		heldImg = SGD::GraphicsManager::GetInstance()->LoadTexture("resources\\graphics\\weaponsHeld\\doubleBarrelShotgun_top.png");
		m_szSize = SGD::Size{15, 93};

	}
	else if (st == ShotgunTypes::Spas12)
	{
		damage = 35;
		rateOfFire = 1.25f;
		sway = 45.f;
		numPellets = 3;
		m_hImage = SGD::GraphicsManager::GetInstance()->LoadTexture("resources\\graphics\\weaponPickups\\spas12.png");
		heldImg = SGD::GraphicsManager::GetInstance()->LoadTexture("resources\\graphics\\weaponsHeld\\spas12_top.png");
		m_szSize = SGD::Size{8, 77};

	}
	else if (st == ShotgunTypes::SawedOff)
	{
		damage = 30;
		rateOfFire = 0.9f;
		numPellets = 5;
		sway = 65.f;
		m_hImage = SGD::GraphicsManager::GetInstance()->LoadTexture("resources\\graphics\\weaponPickups\\sawedOffShotgun.png");
		heldImg = SGD::GraphicsManager::GetInstance()->LoadTexture("resources\\graphics\\weaponsHeld\\sawedOfflShotgun_top.png");
		m_szSize = SGD::Size{17, 55};

	}
}

Shotgun::~Shotgun(void)
{
	SGD::GraphicsManager::GetInstance()->UnloadTexture(m_hImage);
}

/*virtual*/ bool Shotgun::Use(void) /*override*/
{
	if (fireCoolDown <= 0.f)
	{
		Attack();		
		
		// Default velocity vector
		SGD::Vector velocity = SGD::Vector(0, -1);

		// Rotating the vector to match the character's rotation
		velocity.Rotate(owner->GetRotation());

		// Inverting it
		velocity = -velocity;

		// Applying a recoil to the character who used this weapon
		owner->weaponRecoil += velocity * 20;

		SGD::AudioManager::GetInstance()->PlayAudio(GameplayState::GetInstance()->shotgunFiring);

		return true;
	}

	return false;
}

/*virtual*/ void Shotgun::Attack(void) /*override*/
{
	if (fireCoolDown <= 0 && GetOwner())
	{
		// if 3 seconds have passed since last shot, reset passiveTimer
		if (GetOwner()->shotTimer >= 3.0f && GetOwner()->GetClass() == ClassType::Gunslinger)
			GetOwner()->passiveTimer = 3.0f;

		for (int i = 0; i < numPellets; i++)
		{
			// create a 'create bullet' message
			//CreateBulletMsg* msg = new CreateBulletMsg(m_ptPosition, 0, m_fRotation + GetSwayRad(), 800.f + float((rand() % 50) - 25), damage);
			CreateBulletMsg* msg = new CreateBulletMsg(this);

			// dispatch message
			SGD::MessageManager::GetInstance()->QueueMessage(msg);
		}

		// set the cool-down timer
		float atkMod = 0;
		if (owner)
			atkMod = (100 - owner->GetStat(StatType::dexterity))*0.01f;
		float tempROF = 1 / rateOfFire;

		fireCoolDown = ((tempROF * 2) + atkMod) / 3;
	}
}

void Shotgun::BuffWeapon()
{
	if (GetPrefix() == GunPrefix::futuristic)
	{
		if (GetShotgunType() == Shotgun::ShotgunTypes::DoubleBarrel)
		{
			//double number of pellets per shot
			SetNumPellets(8);
		}
		else if (GetShotgunType() == Shotgun::ShotgunTypes::Spas12)
		{
			//pass in the multiplyingShots type and when you check collision with a bullet of that type you can tell it how to act
			bulType.push_back(BulletType::multiplyingShots);
		}
		else // sawed-off
		{
			//make shotgun do fire damage
			bulType.push_back(BulletType::fire);
		}
	}
}

float Shotgun::GetSwayRad()
{
	return Math::to_radians((rand() % int(sway) - sway / 2.f) + ((rand() % 20) - 10));
}

/*virtual*/ char* Shotgun::GetName(void) /*override*/
{
	switch (st)
	{
	case ShotgunTypes::DoubleBarrel:
		return "Double Barrel Shotgun";
	case ShotgunTypes::SawedOff:
		return "Sawed Off Shotgun";
	case ShotgunTypes::Spas12:
		return "Spas 12";
	}

	return nullptr;
}