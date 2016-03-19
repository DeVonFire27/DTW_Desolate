#pragma once

class NPC;

class Spawn
{
	NPC * toBeSpawned = nullptr;

	// Where its initially spawned and should be respawned
	int x;
	int y;

	bool isDead;

	// Internal float to keep track of time since death
	float sinceDeath;

public:
	Spawn(NPC* toBeSpawned, int x, int y);
	~Spawn();

	void KillNPC();
	void ReviveNPC();

	void Update(float dt);

	bool GetIsDead() { return isDead; }
	void SetIsDead(bool nowDead) { isDead = nowDead; }

	NPC* GetNPCToBeSpawned() { return toBeSpawned; }
	void SetNPCToBeSpawned(NPC* setToBeSpawned) { toBeSpawned = setToBeSpawned; }

	void GetPosition(int &x, int &y) { x = this->x; y = this->y; }
	void SetPosition(int x, int y) { this->x = x; this->y = y; }
};