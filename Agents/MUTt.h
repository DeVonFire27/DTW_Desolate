#pragma once

#include "Mutant.h"

class MUTt : public Mutant
{
public:
	MUTt() = default;
	MUTt(int lvl);
	void UseAbility() override;
	virtual void Update(float dt) override;
	virtual MutantType GetMutantType() { return MutantType::MUTT; }

	void HandlePassive() override;
	void SetStats() override;
};