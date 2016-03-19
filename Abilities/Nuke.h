/***************************************************************
|	File:		Nuke.h
|	Author:		Michael Mozdzierz
|	Date:		04/11/2014
|
***************************************************************/

#pragma once

#include "Ability.h"
#include "../../SGD Wrappers/SGD_Geometry.h"

class Emitter;

class Nuke : public Ability
{
private:
	bool notExplode = true;
	SGD::Color blastColor = SGD::Color{};
	SGD::HAudio nuke = SGD::INVALID_HANDLE;
	SGD::HTexture missile = SGD::INVALID_HANDLE;
	Emitter* fireFlare = nullptr;

public:
	Nuke(void);
	virtual ~Nuke(void);

	/////////////////////////<Interface>//////////////////////////////////////////////////

	virtual void Activate() final;
	virtual void Update(float dt) final;
	virtual void Render(void) final;
	virtual void HandleCollision(const IEntity* other) final;
	virtual int GetAbilityType() const { return AbilityType::CYBORG_ACT; }

	virtual SGD::Rectangle GetRect(void) const final;
	bool HasExplode() const { return notExplode; }
};