/***************************************************************
|	File:		Razard.h
|	Author:		Ethan Mills
|	Date:		04/13/2014
|
***************************************************************/

#pragma once

#include "Mutant.h"

class Razard : public Mutant
{
	float razardPassiveTimer = 0;
public:
	Razard() = default;
	Razard(int lvl);
	void Render() override;
	void HandlePassive() override;
	void Update(float dt) override;
	virtual MutantType GetMutantType() { return MutantType::RAZARD; }
	void SetStats() override;

	virtual void AddStatusAilment(StatusEffect* effect) override;

};