#pragma once
#include "Weapon.h"

class SniperRifle : public Weapon
{
public:
	enum SniperRifleTypes { Barrett, Dragunov, Intervention };
private:
	SniperRifleTypes srt;
public:

	SniperRifle(SniperRifleTypes srt);

	/////////////////////////<Accessors>///////////////////////////////////////////////////

	virtual GunType GetGunType() const { return GunType::SniperRifle; };
	virtual void BuffWeapon() override;

	/////////////////////////<Interface>///////////////////////////////////////////////////

	SniperRifleTypes GetSniperType() { return srt; }
	virtual bool Use(void) override;
	virtual char* GetName(void) override;
};

