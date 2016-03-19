#pragma once

#include "../../SGD Wrappers/SGD_Message.h"

class Ability;

class CreateAbility : public SGD::Message
{
private:
	Ability* skill = nullptr;
public:
	CreateAbility(Ability* act);
	virtual ~CreateAbility(void);

	Ability* GetAbility(void) const { return skill; }
};

