#pragma once
#include "Ability.h"

class Mortar : public Ability
{

private:
	float dropTimer = 2.0f;
	AnimationTimeStamp radPool;
	bool play = false;

public:
	Mortar();
	virtual ~Mortar();

	virtual void Activate();
	virtual void Update(float dt) override;
	virtual void Render(void) override;

	virtual SGD::Rectangle GetRect(void) const override;
	virtual int GetAbilityType() const { return AbilityType::MORTAR_ACT; }

	virtual void HandleCollision(const IEntity* other) override;
	float GetDropTimer() const { return dropTimer; }
	bool HasHit() const { return hasHit; }
	bool hasHit = false;

};

