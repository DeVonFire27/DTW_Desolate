#include "globals.h"
#include "Command.h"
#include "../Agents/Character.h"
#include "../EntityManager.h"

Command::Command(Character* effector)
{
	affectors.emplace(StatType::dexterity, -0.2f);
	affectors.emplace(StatType::accuracy, -0.2f);
	SetEffector(effector);

	SetEmitter(GameplayState::GetInstance()->CreateEmitter("command2"));

	duration = 1.0f;
}


Command::~Command()
{

	SetEffector(nullptr);
	SetEmitter(nullptr);
}
