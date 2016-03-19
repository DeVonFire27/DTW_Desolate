#pragma once

#include "Weapon.h"

class Pistol : public Weapon
{
public:
	enum PistolType { ninemm, revolver, deserteagle, spittle };
private:
	PistolType pt;

public:

	Pistol(PistolType pt);

	/////////////////////////<Interface>///////////////////////////////////////////////////

	virtual bool Use(void) override;

	virtual char* GetName(void) override;
	PistolType GetPistolType() { return pt; }
	virtual void BuffWeapon() override;

	/////////////////////////<public functions>///////////////////////////////////////////////////

	virtual GunType GetGunType() const { return GunType::Pistol; };
};