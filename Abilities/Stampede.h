#pragma once
#include "Ability.h"
#include "../../SGD Wrappers/SGD_Geometry.h"

class Stampede : public Ability
{
private:
	bool firstHit = false;
	bool fromTheLeft = true;
	Emitter* trail = nullptr;
	float slowTimer = 0.0f;
	SGD::HAudio gallop = SGD::INVALID_HANDLE;

public:
	Stampede();
	virtual ~Stampede();

	virtual void Activate() final;
	virtual void Update(float dt) final;
	virtual void Render(void) final;
	virtual void HandleCollision(const IEntity* other) final;
	bool isFirstHit() const { return firstHit; }
	void FirstHit() { firstHit = true; }
	void FromTheLeft(bool left) { fromTheLeft = left; }
	void HandleSpecial();
	virtual int GetAbilityType() const { return AbilityType::RADICORN_ACT; }

	virtual SGD::Rectangle GetRect(void) const final;

};

