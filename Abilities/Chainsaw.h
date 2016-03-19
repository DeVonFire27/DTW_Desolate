/***************************************************************
|	File:		Chainsaw.h
|	Author:		Michael Mozdzierz
|	Date:		04/11/2014
|
***************************************************************/

#pragma once

#include "Ability.h"
#include <vector>
using namespace SGD;

class Character;
class Weapon;

class Chainsaw : public Ability
{
private:
	vector<const Character*> swingVictim;
	Weapon* buzz = nullptr;
	bool fired = false;
	bool reset = false;
	SGD::HAudio rev = SGD::INVALID_HANDLE;

public:
	Chainsaw(void);
	virtual ~Chainsaw(void);

	/////////////////////////<Interface>//////////////////////////////////////////////////

	virtual void Activate() final;
	virtual void Update(float dt) final;
	virtual void Render(void) final;
	virtual void HandleCollision(const IEntity* other) final;
	virtual int GetAbilityType() const { return AbilityType::BRAWL_ACT; }
	virtual AnimationTimeStamp GetAnimTS() override;
	Character* GetOwner() { return owner; }
	bool WasHit(const Character* hit);

	virtual SGD::Rectangle GetRect(void) const final;
};