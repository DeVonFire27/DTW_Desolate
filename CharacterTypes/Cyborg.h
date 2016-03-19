/***************************************************************
|	File:		Cyborg.h
|	Author:		Michael Mozdzierz
|	Date:		04/11/2014
|
***************************************************************/

#pragma once

#include "../Agents/Character.h"
#include "../../SGD Wrappers/SGD_GraphicsManager.h"

class Emitter;

class Cyborg : public Character
{
private:
	float shieldTimer = 0.0f;
	float shieldHP = 0.0f;

	Emitter* Energyshield = nullptr;
	float startSize = 1.0f;
	float endSize = 3.0f; 
	bool recharge = true;

public:
	Cyborg(void);
	~Cyborg(void);

	/////////////////////////<Interface>///////////////////////////////////////////////////
	virtual void Update(float deltaTime) override;
	virtual void UseAbility(void) override;
	virtual void HandlePassive(void) override;
	virtual std::stringstream GetActiveDiscription() override;
	virtual std::stringstream GetPassiveDiscription() override;
	virtual void UseWeapon(void) override;
	virtual Weapon* SetStartingWeapon() override;

	virtual void ModifyHealth(float value) override;

	virtual int		GetType(void)	const			override	{ return ENT_CYBORG; }

	virtual ClassType GetClass(void) const override { return ClassType::Cyborg; }

	HTexture dialogueIcon;
	HTexture& GetDialogueIcon() { return dialogueIcon;}
};