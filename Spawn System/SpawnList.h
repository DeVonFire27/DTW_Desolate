#pragma once

#include <vector>
using namespace std;

class Spawn;
class NPC;

class SpawnList
{
	vector<Spawn*> spawns;

	SpawnList() = default;
	~SpawnList() = default;

public:
	static SpawnList* GetInstance();

	void AddSpawn(Spawn* newSpawn) { spawns.push_back(newSpawn); }
	void DeleteAllSpawns();
	void RespawnAll();
	vector<Spawn*>& GetSpawns() { return spawns; }
	void SearchAndKill(NPC* npcToKill);

	void Update(float dt);
};