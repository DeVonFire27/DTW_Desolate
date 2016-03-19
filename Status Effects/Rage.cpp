#include "globals.h"
#include "Rage.h"


Rage::Rage(Character* effector)
{
	affectors.emplace(StatType::strength, -0.2f);
	affectors.emplace(StatType::defense, -0.2f);
	SetEffector(effector);


	SetEmitter(GameplayState::GetInstance()->CreateEmitter("rage2"));

	duration = 1.0f;
}


Rage::~Rage()
{
	SetEffector(nullptr);
	SetEmitter(nullptr);
}
