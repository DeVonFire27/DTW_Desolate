#include "globals.h"
#include "Regen.h"


Regen::Regen(Character* effector)
{
	affectors.emplace(StatType::strength, -0.2f);
	affectors.emplace(StatType::defense, -0.2f);
	affectors.emplace(StatType::dexterity, -0.2f);
	affectors.emplace(StatType::accuracy, -0.2f);
	SetEffector(effector);
	duration = 1.0f;
}


Regen::~Regen()
{
	SetEffector(nullptr);
}

void Regen::Update(float dt) /*override*/
{

	effector->ModifyHealth(effector->GetStat(StatType::health)*0.01f*dt);

	// call the inherited function
	StatusEffect::Update(dt);
}