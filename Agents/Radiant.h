#pragma once

#include "Mutant.h"

class Radiant : public Mutant
{
public:
	Radiant() = default;
	Radiant(int lvl);
	~Radiant(void);
	void HandlePassive() override;
	void SetStats() override;
	void Render() override;
	void Update(float dt) override;
	virtual MutantType GetMutantType() { return MutantType::RADIANT; }
};