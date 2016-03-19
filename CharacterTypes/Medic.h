/***************************************************************
|	File:		Medic.h
|	Author:		Michael Mozdzierz
|	Date:		04/11/2014
|
***************************************************************/

#pragma once

#include "../Agents/Character.h"
#include "../../SGD Wrappers/SGD_GraphicsManager.h"

class Medic : public Character
{
public:
	Medic(void);
	~Medic(void);

	/////////////////////////<Interface>///////////////////////////////////////////////////

	virtual void UseAbility(void) override;
	virtual void HandlePassive(void) override;
	virtual void UseWeapon(void) override;
	virtual Weapon* SetStartingWeapon() override;

	virtual void Update(float deltaTime) override;
	virtual std::stringstream GetActiveDiscription() override;
	virtual std::stringstream GetPassiveDiscription() override;

	virtual int		GetType(void)	const			override	{ return ENT_MEDIC; }

	virtual ClassType GetClass(void) const override { return ClassType::Medic; }

	HTexture dialogueIcon;
	HTexture& GetDialogueIcon() { return dialogueIcon;}
};