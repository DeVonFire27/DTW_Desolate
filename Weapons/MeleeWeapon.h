#pragma once
#include "Weapon.h"

class MeleeWeapon : public Weapon
{
public:
	enum meleeWeaponTypes { sharpPointyStick, knife, sledgehammer };

private:
	meleeWeaponTypes mwt;
	SGD::HAudio meleeWeaponSound;

public:
	SGD::HTexture knifePic;
	SGD::HTexture sledghammerPic;

	virtual void Attack(void) override;
	virtual bool Use(void) override;
	virtual void BuffWeapon() override;


	MeleeWeapon(meleeWeaponTypes mwt);
	~MeleeWeapon();
	virtual char* GetName(void) override;
	virtual GunType GetGunType() const  { return GunType::meleeWeapon; };
	meleeWeaponTypes GetMeleeWeaponType() { return mwt; }
	virtual float GetSwayRad() override;

	virtual float GetDamage(void) override;
};
