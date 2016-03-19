#pragma once
#include "Character.h"
#include "Mutant.h"

class Jane : public Character
{

private:
	int abilityNum = 1;
	int numMinions = 0;
	bool ultimate = false;
	Ability* dash = nullptr;

public:
	Jane();
	virtual ~Jane();
	//BossType boss = BossType::JANE;

	virtual int	GetType(void) const override { return ENT_JANE; }
	
	int minionsSpawned = 0;
	float janeMinionTimer = 4.0f;

	// accessors
	int GetAbilityNum() const { return abilityNum; }
	int GetNumMinions() const { return numMinions; }

	// mutators
	void SetAbilityNum(int ability) { abilityNum = ability; }
	void SetNumMinions(int minions) { numMinions = minions; }

	void Update(float dt);
	virtual void UseAbility(void);
	virtual ClassType GetClass(void) const { return ClassType::Jane; }
	void SpawnMinion(ClassType);

};
