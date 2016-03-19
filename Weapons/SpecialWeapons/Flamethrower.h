/***************************************************************
|	File:		Flamethrower.h
|	Author:		Michael Mozdzierz
|	Date:		05/13/2014
|
***************************************************************/

#pragma once

#include "../Weapon.h"

class Emitter;

class Flamethrower : public Weapon
{
	Emitter* emitter;

	float emitTimeout = 0.f;

public:
	Flamethrower(void);
	~Flamethrower(void);

	/////////////////////////<Interface>///////////////////////////////////////////////////

	virtual GunType GetGunType(void) const override { return GunType::flameThrower; }

	virtual char* GetName(void) override { return "Flamethrower"; }

	virtual bool Use(void) override;

	virtual void Update(float dt) override;
};