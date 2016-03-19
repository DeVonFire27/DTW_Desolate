#include "globals.h"
#include "CreateAbility.h"
#include "../Abilities/Ability.h"

CreateAbility::CreateAbility(Ability* act) : SGD::Message(MSG_CREATEABILITY)
{
	// store the weapon we want to create
	skill = act;
	// ref count dat shit
	act->AddRef();
}

/*virtual*/ CreateAbility::~CreateAbility(void)
{
	skill->Release();
}