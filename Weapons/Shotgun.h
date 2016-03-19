#pragma once
#include "Weapon.h"


class Shotgun : public Weapon
{
public:
	enum ShotgunTypes { DoubleBarrel, Spas12, SawedOff };
private:
	ShotgunTypes st;
	
	int numPellets;
public:

	Shotgun(ShotgunTypes st);
	~Shotgun(void);

	/////////////////////////<public functions>///////////////////////////////////////////////////

	virtual GunType GetGunType() const { return GunType::Shotgun; };

	virtual char* GetName(void) override;
	virtual float GetSwayRad() override;
	virtual void BuffWeapon() override;

	/////////////////////////<Interface>///////////////////////////////////////////////////
	ShotgunTypes GetShotgunType() { return st; }
	void SetNumPellets(int pellets) { numPellets = pellets; }
	int GetNumPellets() { return numPellets; }
	virtual bool Use(void) override;

	virtual void Attack(void) override;

};
