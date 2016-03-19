#pragma once

#include "Mutant.h"
#include "../../SGD Wrappers/SGD_Geometry.h"
class Radicorn : public Mutant
{
	bool dashing = false;
	SGD::Vector targetPosition;
public:
	Radicorn();
	void SetStats();
	void Render() override;
	void UseAbility() override;
	void HandlePassive() override;
	virtual MutantType GetMutantType() { return MutantType::RADICORN; }
	SGD::Vector GetTargetPosition() { return targetPosition; }
	bool GetDashing() { return dashing; }
	void SetDashing(bool d) { dashing = d; }
	void Radicorn::DashTowards();

};