/***************************************************************
|	File:		Gunslinger.h
|	Author:		Michael Mozdzierz
|	Date:		04/11/2014
|
***************************************************************/

#pragma once

#include "../Agents/Character.h"
#include "../../SGD Wrappers/SGD_GraphicsManager.h"

class Gunslinger : public Character
{
public:
	Gunslinger(void);
	virtual ~Gunslinger(void);

	/////////////////////////<Interface>///////////////////////////////////////////////////

	virtual void UseAbility(void) override;
	virtual void HandlePassive(void) override;
	void Update(float dt) override;
	virtual std::stringstream GetActiveDiscription() override;
	virtual std::stringstream GetPassiveDiscription() override;
	virtual void UseWeapon(void) override;
	virtual Weapon* SetStartingWeapon() override;


	virtual int		GetType(void)	const			override { return ENT_GUNSLINGER; }

	virtual ClassType GetClass(void) const override { return ClassType::Gunslinger; }

	HTexture dialogueIcon;
	HTexture& GetDialogueIcon() { return dialogueIcon;}
};