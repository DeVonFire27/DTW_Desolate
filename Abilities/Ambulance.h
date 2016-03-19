/***************************************************************
|	File:		Ambulance.h
|	Author:		Michael Mozdzierz
|	Date:		04/11/2014
|
***************************************************************/

#pragma once

#include "Ability.h"
#include "../../SGD Wrappers/SGD_Geometry.h"

using namespace SGD;

class Emitter;

class Ambulance : public Ability
{
private:
	bool hitFirst = false;
	float slowTimer = 0.0f;
	Emitter* trail = nullptr;
	SGD::HAudio siren = SGD::INVALID_HANDLE;
public:
	Ambulance(void);
	virtual ~Ambulance(void);

	/////////////////////////<Interface>//////////////////////////////////////////////////

	virtual void Activate() final;
	virtual void Update(float dt) final;
	virtual void Render(void) final;
	virtual void HandleCollision(const IEntity* other) final;
	bool isFirstHit() const { return hitFirst; }
	void FirstHit() { hitFirst = true; }
	void HandleSpecial(bool heals);

	virtual int GetAbilityType() const { return AbilityType::MEDIC_ACT; }

	virtual SGD::Rectangle GetRect(void) const final;	
};