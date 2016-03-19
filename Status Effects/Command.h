#pragma once
#include "StatusEffect.h"
class Command : public StatusEffect
{
private:

public:
	Command(Character* effector);
	virtual ~Command();
};

