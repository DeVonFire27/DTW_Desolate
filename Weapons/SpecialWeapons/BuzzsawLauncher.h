/***************************************************************
|	File:		BuzzsawLauncher.h
|	Author:		Michael Mozdzierz
|	Date:		05/13/2014
|
***************************************************************/

#pragma once

#include "../Weapon.h"

class BuzzsawLauncher : public Weapon
{
private:
	bool isBrawlActive = false;

public:
	BuzzsawLauncher(void);

	/////////////////////////<Interface>///////////////////////////////////////////////////

	virtual GunType GetGunType(void) const override { return GunType::chainsawLauncher; }

	virtual char* GetName(void) override { return "Buzzsaw Launcher"; }

	virtual bool Use(void) override;
	bool GetisBrawlActive() const { return isBrawlActive; }
	void SetisBrawlActive(const bool is) { isBrawlActive = is; }

	virtual float GetSwayRad() override;

};