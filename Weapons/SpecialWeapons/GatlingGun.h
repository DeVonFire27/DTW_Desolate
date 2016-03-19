/***************************************************************
|	File:		GatlingGun.h
|	Author:		Michael Mozdzierz
|	Date:		05/13/2014
|
***************************************************************/

#pragma once

#include "../Weapon.h"

class GatlingGun : public Weapon
{
	float rof_affected = 0.f;
	float rof_current = 0.f;
	float inactiveTimer = 0.f;

public:
	GatlingGun(void);
	~GatlingGun(void);

	/////////////////////////<Interface>///////////////////////////////////////////////////

	virtual GunType GetGunType(void) const override { return GunType::gatlingGun; }

	virtual char* GetName(void) override { return "Mini Gun"; }

	virtual void Update(float dt) override;

	virtual bool Use(void) override;
};