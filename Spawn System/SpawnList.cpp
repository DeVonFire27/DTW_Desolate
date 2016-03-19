#include "globals.h"
#include "SpawnList.h"
#include "Spawn.h"

SpawnList* SpawnList::GetInstance()
{
	static SpawnList s_Instance;
	return &s_Instance;
}

void SpawnList::Update(float dt)
{
	for (unsigned int i = 0; i < spawns.size(); i++)
	{
		spawns[i]->Update(dt);
	}
}

void SpawnList::SearchAndKill(NPC * npcToKill)
{
	for (unsigned int i = 0; i < spawns.size(); i++)
	{
		if (spawns[i]->GetNPCToBeSpawned() == npcToKill)
			spawns[i]->KillNPC();
	}
}

void SpawnList::RespawnAll()
{
	for (unsigned int i = 0; i < spawns.size(); i++)
	{
		spawns[i]->ReviveNPC();
	}
}

void SpawnList::DeleteAllSpawns()
{
	for (unsigned int i = 0; i < spawns.size(); i++)
	{

		//find NPC in npcs vector and set to nullptr
		for (auto iter = GameplayState::GetInstance()->npcs.begin(); iter != GameplayState::GetInstance()->npcs.end(); iter++)
		{
			if (spawns[i]->GetNPCToBeSpawned() == (*iter))
			{
				(*iter) = nullptr;
			}
		}

		delete spawns[i];
		spawns[i] = nullptr;
	}

	spawns.clear();
}