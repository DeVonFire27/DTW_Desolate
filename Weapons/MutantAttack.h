#pragma once
#include "Weapon.h"
class MutantAttack : public Weapon
{
public:
	enum MutantAtkType { MUTTS, RAZARD };

private:
	MutantAtkType mwt;
	SGD::HAudio atkSound;

public:
	MutantAttack(MutantAtkType atk);
	virtual ~MutantAttack();

	virtual void Attack(void) override;
	virtual bool Use(void) override;
	virtual GunType GetGunType() const  { return GunType::MutantAtk; };
	MutantAtkType GetMutantAtkType() { return mwt; }

	virtual float GetDamage(void) override;

};

