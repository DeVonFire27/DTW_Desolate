/***************************************************************
|	File:		Sniper.h
|	Author:		Michael Mozdzierz
|	Date:		04/11/2014
|
***************************************************************/

#pragma once

#include "../Agents/Character.h"
#include "../../SGD Wrappers/SGD_GraphicsManager.h"

class Sniper : public Character
{
public:
	Sniper(void);
	~Sniper(void);

	/////////////////////////<Interface>///////////////////////////////////////////////////

	virtual void UseAbility(void) override;
	virtual void HandlePassive(void) override;
	virtual void Update(float deltaTime) override;
	virtual void UseWeapon(void) override;

	virtual int		GetType(void)	const			override	{ return ENT_SNIPER; }
	virtual std::stringstream GetActiveDiscription() override;
	virtual std::stringstream GetPassiveDiscription() override;
	virtual Weapon* SetStartingWeapon() override;
	virtual ClassType GetClass(void) const override { return ClassType::Sniper; }
	HTexture dialogueIcon;

	HTexture& GetDialogueIcon() { return dialogueIcon;}
};