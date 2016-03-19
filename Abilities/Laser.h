/***************************************************************
|	File:		Laser.h
|	Author:		Michael Mozdzierz
|	Date:		04/11/2014
|
***************************************************************/

#pragma once

#include "Ability.h"

using namespace SGD;

class Laser : public Ability
{
private:
	SGD::Color beamColor = SGD::Color{};
	SGD::HAudio laser1 = SGD::INVALID_HANDLE;
	SGD::HAudio laser2 = SGD::INVALID_HANDLE;

public:
	Laser(void);
	virtual ~Laser(void);

	/////////////////////////<Interface>//////////////////////////////////////////////////

	virtual void Activate() final;
	virtual void Update(float dt) final;
	virtual void Render(void) final;
	virtual void HandleCollision(const IEntity* other) final;
	virtual int GetAbilityType() const { return AbilityType::SNIPE_ACT; }

	Character* GetOwner() { return owner; }
	virtual SGD::Rectangle GetRect(void) const final;
};