#include "globals.h"
#include "Spawn.h"
#include "../Agents/NPC.h"
#include "../Messages/CreateEnemyMsg.h"
#include "../../SGD Wrappers/SGD_MessageManager.h"
#include "../EntityManager.h"
#include "../Agents/Player.h"
#include "../Agents/Mutant.h"


Spawn::Spawn(NPC* toBeSpawned, int x, int y)
{
	this->toBeSpawned = toBeSpawned;
	this->x = x;
	this->y = y;

	toBeSpawned->getCharacter()->spawnCollision = true;

	isDead = true;
	this->toBeSpawned->SetIsDead(true);
	sinceDeath = 30.0f;
}

Spawn::~Spawn()
{
	delete toBeSpawned;
	toBeSpawned = nullptr;
}

void Spawn::Update(float dt)
{
	if (sinceDeath < 30.0f)
	{
		if ((x + toBeSpawned->getCharacter()->GetSize().width) < -World::GetInstance()->GetCamera()->GetPosition().x ||
			x > -World::GetInstance()->GetCamera()->GetPosition().x + Game::GetInstance()->GetScreenWidth() ||
			(y + toBeSpawned->getCharacter()->GetSize().height) < -World::GetInstance()->GetCamera()->GetPosition().y ||
			y > -World::GetInstance()->GetCamera()->GetPosition().y + Game::GetInstance()->GetScreenHeight() &&
			isDead)
			sinceDeath += dt;
	}

	if (!isDead)
		toBeSpawned->Update(dt);

	// If the spawn is dead for a certain time, respawn it
	if (isDead && sinceDeath >= 30.0f)
	{
		ReviveNPC();
	}
}

void Spawn::KillNPC()
{
	isDead = true;
	toBeSpawned->SetIsDead(true);
	sinceDeath = 0.0f;
}

void Spawn::ReviveNPC()
{
	// If the character is NOT dead, don't revive
	if (!isDead) return;

	toBeSpawned->getCharacter()->SetPosition(SGD::Point((float)x, (float)y));

	toBeSpawned->getCharacter()->SetCurrHealth(toBeSpawned->getCharacter()->GetHealth());

	toBeSpawned->setTarget(nullptr);
	CreateEnemyMsg * newMsg = new CreateEnemyMsg(toBeSpawned);
	SGD::MessageManager::GetInstance()->QueueMessage(newMsg);

	isDead = false;
	toBeSpawned->SetIsDead(false);
}