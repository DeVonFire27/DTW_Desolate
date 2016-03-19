#pragma once
#include "StatusEffect.h"
class Regen : public StatusEffect
{
public:
	Regen(Character* effector);
	virtual ~Regen();
	virtual void Update(float dt) override;
};

