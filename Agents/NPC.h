/***************************************************************
|	File:		NPC.h
|	Author:		Alec Kennedy
|	Date:		04/11/2014
|
***************************************************************/

#pragma once

#include "../GameplayState.h"
#include "Character.h"
#include "../EventProc/IObserver.h"

enum class State { IDLE, ROAM,  FOLLOW, ATTACK, };

enum class AttackStratagy { AGGRESSIVE, DEFENSIVE, };

enum class BossType {GRUNT, GATLING_GUN, BUZZSAW, JANE, RADZILLA};

class Entity;
class Player;

class Dialogue;

class NPC : public IObserver
{
	friend class GameplayState; //uhhhhhhhhhhhhhhhhhhhhh..... (shhhh...)

	int RepRequirement = 0;
	//References to characters
	vector<NPC*> const * npcs = nullptr;
	Player *player = nullptr;

	bool isDead;

	State state = State::IDLE;
	AttackStratagy attackStrat = AttackStratagy::AGGRESSIVE;

	Entity* anchor = nullptr;
	Character* character = nullptr;
	Character *target = nullptr;
	
	BossType boss = BossType::GRUNT;

	// Dialogue
	Dialogue* dialogue = nullptr;

	//Hold points
	SGD::Point walkTo;
	SGD::Vector followOffset;

	//Cooldowns
	float idleSince = 0.0f;
	float switchTargetCooldown = 0.0f;
	float switchStrafeCooldown = 0.0f;
	float walkTimeOut = 0.0f;
	float abilityCheckTimer = 3.0;

	//Other vars
	float followDistance = 250.f;
	float attackDistance = 250.f;
	float whenRoam = 0.0f;
	int strafeDir = 0;

	
	//Internal functions
	void RotateTowards(SGD::Point);
	void EnterFollow();
	void EnterRoam();
	void AttackAggressive();
	void AttackDefensive();

	bool CanSee(Entity *target);
	bool CanSee(SGD::Vector target);
	
public:
	NPC(Character *character, Player * const player, vector <NPC*> const * const npcs, Entity *anchor = nullptr);
	virtual ~NPC(void);
	float Distance(SGD::Point a, SGD::Point b);
	bool beenShot = false;

	//Interface
	void Update(float dt);
	virtual void HandleEvent(string name, void* args = nullptr) override;
	void SetNearestTarget(bool now = false);

	void SetBossType(BossType type);
	BossType GetBossType() const { return boss; }

	void AOEBuff();

	/////////////////////////<accessors>///////////////////////////////////////////////////
	Character* getCharacter() { return character; }

	bool GetIsEnemy(void) const;
	int GetRepRequirement(void) const;
	State GetState(void) const { return state; }
	AttackStratagy GetAttackStratagy(void) const { return attackStrat; }
	Dialogue* GetDialogue() const { return dialogue; }
	bool GetIsDead() { return isDead; }
	Entity* GetTarget() { return target; }

	/////////////////////////<Mutators>///////////////////////////////////////////////////

	void SetIsEnemy(bool value);
	void SetAnchor(Entity *set);
	void SetCharacter(Character *set);
	void SetRepRequirement(int set){ RepRequirement = set; }
	bool SetWalkTo(SGD::Point);
	void SetIsDead(bool value) { isDead = value; }
	void setTarget(Character* tgt);
};