/***************************************************************
|	File:		Brawler.h
|	Author:		Michael Mozdzierz
|	Date:		04/11/2014
|
***************************************************************/

#pragma once

#include "../Agents/Character.h"
#include "../../SGD Wrappers/SGD_GraphicsManager.h"


class Brawler : public Character
{
private:
	bool chainsawArm = false;

public:
	Brawler(void);
	~Brawler(void);
	float passiveDmg = 0;
	float baseDmg;
	float healthRegen;
	/////////////////////////<Interface>///////////////////////////////////////////////////

	virtual void UseAbility(void) override;
	virtual void HandlePassive(void) override;
	virtual std::stringstream GetActiveDiscription() override;
	virtual std::stringstream GetPassiveDiscription() override;
	virtual void UseWeapon(void)override;
	virtual int		GetType(void)	const			override	{ return ENT_BRAWLER; }
	virtual Weapon* SetStartingWeapon() override;

	virtual ClassType GetClass(void) const override { return ClassType::Brawler; }

	HTexture dialogueIcon;
	HTexture& GetDialogueIcon() { return dialogueIcon;}

	void Update(float dt);

	void ModifyHealth(float amount);
};