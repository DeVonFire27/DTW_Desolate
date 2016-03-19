/***************************************************************
|	File:		Weapon.h
|	Author:		Michael Mozdzierz
|	Date:		04/23/2014
|
***************************************************************/

#pragma once

#include "../../source/Entity.h"
#include "../../SGD Wrappers/SGD_GraphicsManager.h"
#include "Bullet.h"
#include <vector>
using namespace std;

enum class GunPrefix { rusty, antique, standard, highQuality, futuristic };
enum BulletType
{
	normal, fire, lifesteal, radiation, multiplyingShots, fmj, FMJFTW, stunChance, critChance
};
class Character;

class Weapon : public Entity
{
protected:
	float rateOfFire = 1.0f,
		fireCoolDown = 0.0f,
		damage = 0.0f,
		damageDealt = 0.0f;

	bool rofIncreased = false;
	int weaponSpawnLevel = 0;
	HTexture heldImg = SGD::INVALID_HANDLE;

	// the ammount of sway, in degrees, that the bullet
	// directions that are fired by this gun are offset by
	float sway = 0.f;
	float deadTimer = 10.0f;
	bool notWorld = false;
	Character* owner = nullptr;
	GunPrefix prefix;

	bool gunActive = false;

public:
	Weapon(void);
	virtual ~Weapon(void);
	Weapon& operator= (const Weapon&);	// assignment operator

	enum class GunType { baseweapon, meleeWeapon, Pistol, SniperRifle, Shotgun, chainsawLauncher, gatlingGun, flameThrower, MutantAtk };
	char* Weapon::GetPrefixName(void);
	vector<BulletType> bulType;

	/////////////////////////<Public Functions>///////////////////////////////////////////////////
	SGD::Rectangle GetRect() const;

	virtual void Attack(void);
	virtual void BuffWeapon() {};
	// Used for different fire modes (simi-auto, full-auto, 
	// bolt action). Returns a value indicating if the weapon 
	// can actually be used at this time
	virtual bool Use(void);

	virtual char* GetName(void) { return "Weapon"; }
	SGD::Point GetAnchor();
	SGD::Point GetWeaponPoint();

	/////////////////////////<Accessors>///////////////////////////////////////////////////
	virtual GunType GetGunType() const { return GunType::baseweapon; }
	virtual float GetFireCoolDown() const { return fireCoolDown; }
	float GetRateOfFire(void) const { return rateOfFire; }
	virtual float GetDamage(void);
	float GetCompareDamage();
	virtual int		GetType(void)	const			override	{ return ENT_WEAPON; }
	void SetOwner(Character* newOwner);
	Character* GetOwner() const { return owner; }
	GunPrefix GetPrefix() { return prefix; }
	void SetPrefix(GunPrefix gp) { prefix = gp; }
	void SetRateOfFire(float rof) { rateOfFire = rof; }
	void SetDamage(float d) { damage = d; }
	// protected accessor for finding the sway in radians. 
	// returns a value, in radians, plus or minus a random 
	// value between zero and 'sway'
	virtual float GetSwayRad();
	void SetSpawnLevel(int newWeaponSpawnLevel) { weaponSpawnLevel = newWeaponSpawnLevel; }
	int GetSpawnLevel() { return weaponSpawnLevel; }

	bool GetNotWorld() { return notWorld; }
	void SetNotWorld(bool set) { notWorld = set; }

	void ToggleGunActive() { gunActive = !gunActive; }
	bool GetGunActive() { return gunActive; }
	/////////////////////////<Interface>///////////////////////////////////////////////////


	virtual void Update(float dt) override;
	virtual void Render() override;
};