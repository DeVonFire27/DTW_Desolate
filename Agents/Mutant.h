/***************************************************************
|	File:		Mutant.h
|	Author:		Ethan Mills
|	Date:		04/13/2014
|
***************************************************************/

#pragma once
#include "Character.h"
#include "../../source/Entity.h"
#include "../Status Effects/StatusEffect.h"
#include <vector>
using namespace std;

enum MutantType {MUTT, RAZARD, RADIANT, RADZILLA, RADICORN};

class Mutant : public Character
{
protected:
	float strength,
		  accuracy,
		  dexterity,
		  defense,
		  rateOfAttack,
		  attackTimer;
	
	int moveSpeed;
public:
	Mutant(void);
	virtual ~Mutant(void) = 0 {}
	void Update(float dt) override;
	void Render() override;
	/////////////////////////<public functions>///////////////////////////////////////////////////
	virtual MutantType GetMutantType() = 0;
	virtual void HandlePassive(void);
	virtual void UseAbility(void);
	virtual void SetStats() = 0;
	float GetStat(StatType type) override;
	virtual ClassType GetClass(void) const { return ClassType::Mutant; }

	/////////////////////////<accessors>///////////////////////////////////////////////////
	float GetStrength(void) const { return strength; }
	float GetAccuracy(void) const { return accuracy; }
	float GetDexterity(void) const { return dexterity; }
	float GetDefense(void) const { return defense; }

	int GetMoveSpeed(void) const { return moveSpeed; }
	void SetMoveSpeed(int ms) { moveSpeed = ms; }

	float GetAttackTimer() { return attackTimer; }
	void SetAttackTimer(float at) { attackTimer = at; }
	float GetRateOfAttack() { return rateOfAttack; }
	float mutantPassiveCooldown = 0.0f;
	float mutantActiveCooldown = 0.0f;
};