/***************************************************************
|	File:		NPC.cpp
|	Author:		Alec Kennedy
|	Date:		04/11/2014
|
***************************************************************/

#include "globals.h"
#include "NPC.h"
#include "../Math.h"
#include "../EventProc/EventProc.h"
#include "Player.h"
#include "Jane.h"
#include "../Abilities/Ability.h"
#include "../Weapons/Weapon.h"

#include "../Status Effects/StatusEffect.h"
#include "../Status Effects/Stun.h"
#include "../Status Effects/Radiation.h"
#include "../Status Effects/Bleeding.h"
#include "../Status Effects/Rage.h"
#include "../Status Effects/Command.h"
#include "../Status Effects/Regen.h"

#include "../Tile System/TileLayer.h"
#include "../Tile System/Tile.h"
#include "../EntityManager.h"
#include <assert.h>

#include "../Dialogue System/Dialogue.h"

#include "Radicorn.h"

#define LENIANCY 10.0f
#define GIVEUPTIMER 4.0f
#define TOOFAR 700.0f

// NPC
//	- Construct an NPC.  This is only a controller for a character that is in the Entity Manager.
//
NPC::NPC(Character *character, Player * const player, vector<NPC*> const * const npcs, Entity *anchor)
{
	EventProc::GetInstance()->Subscribe(this, "NPC_DEAD");
	EventProc::GetInstance()->Subscribe(this, "GainExp");

	this->npcs = npcs;
	SetAnchor(anchor);
	dialogue = new Dialogue(character);
	SetCharacter(character);
	EventProc::GetInstance()->Subscribe(this, "PushBack");

	if (target && ((Character *)target)->GetCurrHealth() <= 0) setTarget(nullptr);

	RepRequirement = 50;
	isDead = false;
	// If the character is a MUTt
	if (character->GetClass() == ClassType::Mutant)
	{

		if (((Mutant*)(character))->GetMutantType() == MutantType::MUTT)
		{
			dialogue->AddPopUpDialogue("Grrr...");
			dialogue->AddPopUpDialogue("Woof woof");
		}
		// If the character is a RadiAnt
		else if (((Mutant*)(character))->GetMutantType() == MutantType::RADIANT)
		{
			dialogue->AddPopUpDialogue("Crrriickkk");
		}
		// If the character is a Radicorn
		else if (((Mutant*)(character))->GetMutantType() == MutantType::RADICORN)
		{
			dialogue->AddPopUpDialogue("Neeeiiiggh");
		}
		// If the character is a Radzilla
		else if (((Mutant*)(character))->GetMutantType() == MutantType::RADZILLA)
		{
			dialogue->AddPopUpDialogue("RAAAAAAAAAAAAAAAAAGH!");
		}
		// If the character is a Razard
		else if (((Mutant*)(character))->GetMutantType() == MutantType::RAZARD)
		{
			dialogue->AddPopUpDialogue("Fsss....");
		}
	}
	// Give Jane specific dialogue
	else if (character->GetClass() == ClassType::Jane)
	{
		dialogue->AddPopUpDialogue("You'll never beat me!");
		dialogue->AddPopUpDialogue("Bandits, defend me!");
		dialogue->AddPopUpDialogue("HAHAHAHAHAHAHAAHAHAHAHA!");
		dialogue->AddPopUpDialogue("You're pathetic!");
	}
	// If the character is an enemy, give them enemy-like things to say
	else if (character->GetIsEnemy())
	{
		// Give them popup dialogue
		dialogue->AddPopUpDialogue("Get 'em!");
		dialogue->AddPopUpDialogue("Attack!");
		dialogue->AddPopUpDialogue("Git off mah lawn!");
		dialogue->AddPopUpDialogue("You'll regret this!");
		dialogue->AddPopUpDialogue("FOR JANE!");
		dialogue->AddPopUpDialogue("You'll never beat Jane!");
		//
		dialogue->AddPopUpDialogue("Just go away!");
		dialogue->AddPopUpDialogue("YOU'RE the bad one!");
		dialogue->AddPopUpDialogue("I can't lose!");
		dialogue->AddPopUpDialogue("HA! What a tiny gun!");
		dialogue->AddPopUpDialogue("heh..heh...HAHAHAHA!");
	}
	// Else if the character is a friendly, give them friendly things to say
	else
	{
		// Give them popup dialogue
		dialogue->AddPopUpDialogue("Such a nice day today!");
		dialogue->AddPopUpDialogue("So sleeeeepyyy...");
		dialogue->AddPopUpDialogue("Ugh.. It's so hot!");
		dialogue->AddPopUpDialogue("Damn those bandits...");
		dialogue->AddPopUpDialogue("Jane's going to kill us all...");
		dialogue->AddPopUpDialogue("Time sure flies these days!");
		dialogue->AddPopUpDialogue("Lets hang out some time.");
		//
		dialogue->AddPopUpDialogue("It's my birthday!");
		dialogue->AddPopUpDialogue("Shh.. SHHHHH....");
		dialogue->AddPopUpDialogue("Oh, my.. That's a large gun.");
		dialogue->AddPopUpDialogue("..He's such a nag!");
		dialogue->AddPopUpDialogue("Almost done!");
		dialogue->AddPopUpDialogue("*giggle*");
	}
}

// Update
//	- This handles switching between the different AI states, moving the character and shooting.
//
void NPC::Update(float dt)
{
	player = GameplayState::GetInstance()->player;

	//Fix things
	if (dt <= 0 || !character || !player || GameplayState::GetInstance()->GetFuckTheseStupidFuckingNPCs()) return;
	if (!anchor)
		SetAnchor(character);

	//If too far away then dont update
	if (Distance(character->GetPosition(), player->GetPosition()) > Game::GetInstance()->GetScreenWidth() * 2.5f) return;

	//Release target is dead or if on same team.
	if (target && (target->GetIsEnemy() == character->GetIsEnemy() || target->GetCurrHealth() <= 0.0f)) setTarget(nullptr);

	//Mob mentality.  If anchor has taget then I too should have a target.
	if (!target && anchor && anchor->IsACharacter())
	{
		NPC *found = FindControllingNPC((Character *)anchor);
		if (found) found->SetNearestTarget();
	}

	// assign janes bosstype
	if (getCharacter()->GetClass() == ClassType::Jane)
		boss = BossType::JANE;

	//Cooldowns
	if (switchTargetCooldown > 0.0f) switchTargetCooldown -= dt;
	if (switchStrafeCooldown > 0.0f) switchStrafeCooldown -= dt;

	float distanceFromAnchor = Distance(character->GetPosition(), anchor->GetPosition());

	//When to switch cases?
	// only teleport if you are not an enemy
	if (distanceFromAnchor > TOOFAR && character->GetIsEnemy() == false)
	{
		character->spawnCollision = true;
		character->SetPosition(anchor->GetPosition());	//Should probably do some smart stuff to tele in open spot off-screen.  For now this will make it less game breaking.
	}
	else if (distanceFromAnchor > followDistance * 2.5f + LENIANCY)
		EnterFollow();
	else if (target)
		state = State::ATTACK;
	else if (distanceFromAnchor > followDistance + LENIANCY)
		EnterFollow();
	else if (idleSince > whenRoam)
		EnterRoam();

	if (target)
	{
		float myHealth = character->GetCurrHealth() / character->GetHealth();
		float anchorHealth = (anchor->IsACharacter()) ? ((Character *)anchor)->GetCurrHealth() / ((Character *)anchor)->GetHealth() : 0.0f;
		if (anchorHealth - myHealth > 20.0f)
			attackStrat = AttackStratagy::DEFENSIVE;
		else if (myHealth - anchorHealth > 20.0f)
			attackStrat = AttackStratagy::AGGRESSIVE;
	}

	// setup Jane strategies (used in 'when to switch cases: if(target) - above && switch(state): attack - below)
	if (getCharacter()->GetClass() == ClassType::Jane && target != nullptr)
	{
		Jane* tempJane = dynamic_cast<Jane*>(getCharacter());

		if (tempJane)
		{
			if (tempJane->GetCurrHealth() > tempJane->GetHealth() * .75f || (tempJane->GetCurrHealth() < tempJane->GetHealth() * .5f && tempJane->GetCurrHealth() > tempJane->GetHealth() * .25f))
			{
				attackStrat = AttackStratagy::DEFENSIVE;
				tempJane->SetAbilityNum(2);
			}
			if ((tempJane->GetCurrHealth() < tempJane->GetHealth() * .75f && tempJane->GetCurrHealth() > tempJane->GetHealth() * .50f) || tempJane->GetCurrHealth() < tempJane->GetHealth() * .25f)
			{
				attackStrat = AttackStratagy::AGGRESSIVE;
				tempJane->SetAbilityNum(1);
			}
			if (tempJane->GetCurrHealth() > tempJane->GetHealth() * .75f)// && tempJane->minionsDead >= 10)
			{
				attackStrat = AttackStratagy::AGGRESSIVE;
				tempJane->SetAbilityNum(1);
			}
			if (tempJane->GetCurrHealth() < tempJane->GetHealth() * .25f)
				tempJane->SetAbilityNum(3);

			if (tempJane->GetCurrHealth() > tempJane->GetHealth() * .75f)
			{
				if (tempJane->minionsSpawned < 10 && tempJane->janeMinionTimer <= 0)
				{
					tempJane->SpawnMinion(ClassType(rand() % 7));
					tempJane->minionsSpawned++;

					tempJane->janeMinionTimer = 4.0f;
				}
				else
					tempJane->janeMinionTimer -= dt;

			}
		}
	}


	//If player is too far away to see us moving then dont bother
	if (Distance(player->GetCharacter()->GetPosition(), character->GetPosition()) > TOOFAR) return;

	//Move correctly based on state
	switch (state)
	{
	case State::IDLE:
	{
		idleSince += dt;
	}
		break;
	case State::ROAM:
	{
		//If at pos then set idle
		if (Distance(character->GetPosition(), walkTo) < LENIANCY || walkTimeOut > GIVEUPTIMER)
		{
			state = State::IDLE;
			whenRoam = (rand() % 300 + 200) / 100.0f;
		}
		else
		{
			walkTimeOut += dt;
			RotateTowards(walkTo);
			character->WalkTowards(walkTo);
		}
	}
		break;
	case State::FOLLOW:
	{
		setTarget(nullptr);

		if (Distance(character->GetPosition(), anchor->GetPosition() + followOffset) > LENIANCY)
		{
			RotateTowards(anchor->GetPosition() + followOffset);
			character->WalkTowards(anchor->GetPosition() + followOffset);
		}
		else
			state = State::IDLE;
	}
		break;
	case State::ATTACK:
	{
		if (!target)
		{
			state = State::IDLE;
			break;
		}

		attackStrat == AttackStratagy::AGGRESSIVE ? AttackAggressive() : AttackDefensive();

		if (!(character->GetClass() == ClassType::Gunslinger && character->GetAbility()->GetDuration() > 0))
			RotateTowards(target->GetPosition());
	}
	}

	//See if any enemies to aggro
	SetNearestTarget();

	// if NPC is an Enemy and has a target
	if (target && (GetIsEnemy() || (!GetIsEnemy() && player->autoCast)))
	{
		// create random number for Active chance
		abilityCheckTimer -= dt;

		if (abilityCheckTimer <= 0)
		{
			if (rand() % 100 < 25)
				character->UseAbility();

			abilityCheckTimer = 3.0f;
		}
	}

	if (boss != BossType::GRUNT)
		AOEBuff();

	if (state == State::IDLE)
		character->isMoving = false;
	else
		character->isMoving = true;
}

void NPC::AOEBuff()
{
	vector<NPC*> allies = GameplayState::GetInstance()->npcs;
	float distance = 0.0f;
	for (unsigned int x = 0; x < npcs->size(); x++)
	{
		distance = (allies[x]->getCharacter()->GetPosition() - character->GetPosition()).ComputeLength();
		if (distance < 1024 && allies[x]->getCharacter()->GetIsEnemy())
		{
			Character* buff = allies[x]->getCharacter();
			if (boss == BossType::GATLING_GUN)
			{
				//increase accuracy and dexterity
				buff->AddStatusAilment(new Command(buff));
			}
			else if (boss == BossType::BUZZSAW)
			{
				//increase strength and defense
				buff->AddStatusAilment(new Rage(buff));
			}
			else if (boss == BossType::JANE)
			{
				//increase accruacy, strength, defense and regen
				//buff->AddStatusAilment(new Command(buff));
				//buff->AddStatusAilment(new Rage(buff));
				buff->AddStatusAilment(new Regen(buff));
			}
		}
	}
}

// RotateTowards
//	- Rotates the character to face a given point.
//
void NPC::RotateTowards(SGD::Point place)
{
	//Create a vector for the current orientation
	SGD::Vector orientation = { 0, -1 };

	//Rotate the vector to the players current rotation
	orientation.Rotate(character->GetRotation());

	////Calculate the angle between the vectors
	float angle = orientation.ComputeAngle(place - character->GetPosition());

	//rotate the character towards the cursor
	if (orientation.ComputeSteering(place - character->GetPosition()) < 0.0f)
		character->SetRotation(character->GetRotation() - angle);
	else
		character->SetRotation(character->GetRotation() + angle);
}

void NPC::setTarget(Character* tgt)
{
	if (target)
	{
		target->Release();
		target = nullptr;
	}

	target = tgt;

	if (target) target->AddRef();
}

// SetNearestTarget
//	- Attempts to attack the nearest enemy if there is any.
//	- To qualify and enemy must be within the anchor's follow distance.
//
void NPC::SetNearestTarget(bool now)
{
	float aggroRangeMultiplier = 1.25f;
	if (now) aggroRangeMultiplier = 2.0f;

	//Check if there are any enemies to aggro
	if (switchTargetCooldown <= 0.0f || now)
	{
		unsigned int i = 0;
		for (; i < npcs->size(); i++)
		{
			//If not on same team
			if (character->GetIsEnemy() != (*npcs)[i]->GetIsEnemy() && !(*npcs)[i]->GetIsDead())
			{
				//Check if there is a character
				if (!(*npcs)[i]->getCharacter()) continue;

				//If in range (of anchor)
				float distanceToPotentialEnemy;
				if (anchor && anchor->IsACharacter() && FindControllingNPC((Character *)anchor) != this)
					distanceToPotentialEnemy = Distance((*npcs)[i]->getCharacter()->GetPosition(), character->GetPosition());
				else
					distanceToPotentialEnemy = Distance((*npcs)[i]->getCharacter()->GetPosition(), anchor->GetPosition());

				if (distanceToPotentialEnemy < followDistance * aggroRangeMultiplier)
				{
					//Continue if current target is closer
					if (target && Distance(target->GetPosition(), character->GetPosition()) < distanceToPotentialEnemy) continue;

					//Else then mi casa es tu casa
					setTarget((*npcs)[i]->getCharacter());

					if (target == player->GetCharacter())
					{
						if (GameplayState::GetInstance()->currentTutorialObjective == Tutorial::Active)
						{
							GameplayState::GetInstance()->currentTutorialObjective++;
							GameplayState::GetInstance()->sinceLastTutorial = 0.0f;
						}
					}

					//Tell the anchor to attack too
					if (anchor && anchor != character && anchor->IsACharacter())
					{
						NPC *found = FindControllingNPC((Character *)anchor);
						if (found) found->SetNearestTarget();
					}

					switchTargetCooldown = 0.4f;
					attackDistance = rand() % ((int)followDistance - 100) + 100.0f;
				}
			}
		}

		//Also check against player
		if (!player || !player->GetCharacter() || !GetIsEnemy() || switchTargetCooldown > 0.0f) return;
		float distanceToPlayer = Distance(player->GetCharacter()->GetPosition(), anchor->GetPosition());

		if (distanceToPlayer < followDistance * aggroRangeMultiplier)
		{
			if (target && Distance(target->GetPosition(), character->GetPosition()) < distanceToPlayer) return;

			setTarget(player->GetCharacter());
			switchTargetCooldown = 0.4f;
			attackDistance = rand() % ((int)followDistance - 100) + 100.0f;
		}
	}
}

// EnterFollow
//	- If not already following then switches to follow mode.
//	- Also selects an offset from the player to make more interesting flocking.
//
void NPC::EnterFollow()
{
	if (state == State::FOLLOW) return; //Dont enter more than once

	state = State::FOLLOW;
	idleSince = 0.0f;

	setTarget(nullptr);

	//Pick a vector from anchor to follow to
	SGD::Vector toAnchor = anchor->GetPosition() - character->GetPosition();
	float rotation = rand() % 100 / 300.0f + 0.2f;
	if (rand() % 2) rotation = -rotation;
	toAnchor.Rotate(rotation);

	followOffset = (character->GetPosition() + toAnchor) - anchor->GetPosition();

	//Todo:  Make less wonky.  Possibly rotate the follow offset by player's rotation.
}

// EnterRoam
//	- If not in roam then set it and update cooldowns.
//	- SetWalkTo is retried a number of times to try and get valid location.
//
void NPC::EnterRoam()
{
	if (state == State::ROAM) return;

	int tries = 0;

	SGD::Vector toVec;
	do
	{
		tries++;
		toVec = { 0, -1 };
		toVec.Rotate(rand() % 628 / 62.8f);
		toVec *= rand() % (int)(followDistance - 30.0f) + 30.0f - LENIANCY;
	} while (!SetWalkTo(anchor->GetPosition() + toVec) || tries < 15);

	state = State::ROAM;
	idleSince = 0.0f;
}

// AttackAggressive
//	- Called to move toward target and attack.
//	- Strafing can be clockwise, counter clockwise and hold(0).
//
void NPC::AttackAggressive()
{
	float distance = Distance(character->GetPosition(), target->GetPosition());
	float targetDistance = attackDistance * 2.0f;

	if (dynamic_cast<Radicorn*>(character) && ((Radicorn*)character)->GetDashing())
	{
		//if the radicorn is dashing double its movement speed, and make it continue in a straight line until it hits something.
		((Radicorn*)character)->SetMoveSpeed(700);
		((Radicorn*)character)->DashTowards();
	}
	//Brawler changes...
	//assert(character->GetWeapon() && "NPC::AttackAgressive - Weapon is null dummy!");
	if (character->GetWeapon() && character->GetWeapon()->GetGunType() == Weapon::GunType::meleeWeapon || character->GetClass() == ClassType::Mutant)
		targetDistance = 50.0f;

	if (abs(targetDistance - distance) < LENIANCY)
	{
		//In range (strafe)
		if (switchStrafeCooldown <= 0.0f)
		{
			strafeDir = rand() % 3 - 1;

			if (strafeDir == 0) switchStrafeCooldown = (rand() % 50 + 100) / 100.0f;
			else switchStrafeCooldown = (rand() % 200 + 50) / 100.0f;
		}

		//Move in direction
		if (strafeDir)
		{
			SGD::Vector enemyToChar = character->GetPosition() - target->GetPosition();
			enemyToChar.Rotate(strafeDir * 0.1f);

			character->WalkTowards(target->GetPosition() + enemyToChar);
		}
	}
	else
	{
		//Need to get in range
		if (distance > targetDistance)
		{
			//if the character is a radicorn, check to see if it can use its dash ability, if so, do it
			if (dynamic_cast<Radicorn*>(character) && character->GetCurrCoolDown() == 0)
			{
				((Radicorn*)character)->UseAbility();
			}
			else
				character->WalkTowards(target->GetPosition());
		}
		else if (distance < targetDistance)
			character->WalkDirection(character->GetPosition() - target->GetPosition());
	}
	if (character->GetCurrHealth() > 0)
		character->UseWeapon();
}

// AttackDefensive
//	- Tries to get behind something that will act as cover
//	- Still shoots when enemy is in sight.
//
void NPC::AttackDefensive()
{
	Character *anchorTemp = nullptr;

	for (unsigned int i = 0; i < npcs->size(); i++)
	{
		if (!(*npcs)[i]->getCharacter() || Distance(character->GetPosition(), (*npcs)[i]->getCharacter()->GetPosition()) > TOOFAR) continue; //If no character or too far
		if ((*npcs)[i]->GetIsEnemy() != GetIsEnemy()) continue; //Dont try and hide behind enemies...

		//Calculate the would be safe spot
		SGD::Vector objToTarget = target->GetPosition() - (*npcs)[i]->getCharacter()->GetPosition();
		SGD::Vector objToHideSpot = objToTarget.ComputeNormalized();
		objToHideSpot.Rotate(SGD::PI);
		objToHideSpot *= 60.0f;

		SGD::Point spot = (*npcs)[i]->getCharacter()->GetPosition() + objToHideSpot;

		if (SetWalkTo(spot)) break;
	}

	character->WalkTowards(walkTo);
	character->UseWeapon();
}

int NPC::GetRepRequirement(void) const
{ 
	int required = RepRequirement;
	int bonus = character->GetLevel() - GameplayState::GetInstance()->player->GetCharacter()->GetLevel();
	if (bonus < 0)
		bonus = 0;

	required += (bonus * 2000);
	return required;
}

// SetWalkTo
//	- Attempts to set a walk to location returning true on success.
//	- Failure would be due to invalid location selected.
bool NPC::SetWalkTo(SGD::Point point)
{
	//Todo::  World should have an accessor to check if a tile is passable.

	World *world = World::GetInstance();
	int col = (int)point.x / 64;
	int row = (int)point.y / 64;
	int numCols = world->GetTileLayers()[0]->layerColumns;
	if (col >= numCols)
		col = numCols - 1;
	if (row >= world->GetTileLayers()[0]->layerRows)
		row = world->GetTileLayers()[0]->layerRows - 1;

	//Check all layers
	bool passable = true;
	std::vector<TileLayer *> layers = world->GetTileLayers();
	for (unsigned int layerNum = 0; layerNum < layers.size(); layerNum++)
	{
		if (!layers[layerNum]->tiles[col + row * numCols]->isPassable) passable = false;
	}

	//Check all NPCs
	for (unsigned int i = 0; i < npcs->size(); i++)
	{
		if ((*npcs)[i]->getCharacter()) continue;

		if (Distance((*npcs)[i]->getCharacter()->GetPosition(), point) < 50.0f)
			passable = false;
	}

	//Check player
	if (Distance(player->GetCharacter()->GetPosition(), point) < 50.0f)
		passable = false;

	if (passable)
	{
		walkTimeOut = 0.0f;
		walkTo = point;
		return true;
	}

	return false;
}

void NPC::SetAnchor(Entity *set)
{
	if (anchor)
	{
		anchor->Release();
		anchor = nullptr;
	}
	anchor = set;
	if (anchor)
		anchor->AddRef();
}

void NPC::SetCharacter(Character *set)
{
	if (character)
	{
		character->Release();
		character = nullptr;
		_ASSERTE(_CrtCheckMemory());
		dialogue->SetCharacter(nullptr);
		_ASSERTE(_CrtCheckMemory());
	}
	if (set)
	{
		character = set;
		character->AddRef();
		dialogue->SetCharacter(character);
	}
}

bool NPC::GetIsEnemy(void) const
{
	if (character)
		return character->GetIsEnemy();
	else
		return false; //Returning flase is safer.  Will be exterminated by other enemies and not go rogue on player.
}

void NPC::SetIsEnemy(bool value)
{
	if (character)
		character->SetIsEnemy(value);
}

NPC::~NPC(void)
{
	SetAnchor(nullptr);
	_ASSERTE(_CrtCheckMemory());
	SetCharacter(nullptr);
	if (target)
		target->Release();
	setTarget(nullptr);

	EventProc::GetInstance()->UnSubscribe(this, "NPC_DEAD");
	EventProc::GetInstance()->UnSubscribe(this, "PushBack");
	EventProc::GetInstance()->UnSubscribe(this, "GainExp");

	delete dialogue;
	dialogue = nullptr;
}

float NPC::Distance(SGD::Point a, SGD::Point b)
{
	SGD::Vector diff = b - a;
	return diff.ComputeLength();
}

/*virtual*/ void NPC::HandleEvent(string name, void* args) /*override*/
{
	if (character && name == "PushBack" && (GameplayState::GetInstance()->player->GetCharacter()->GetPosition() - character->GetPosition()).ComputeLength() < 100.f)
	{
		PushBack* push = reinterpret_cast<PushBack*>(args);
		// get the position parameter
		SGD::Point pos = push->pos;
		// find the distance to the point
		float distance = Math::Clamp((pos - character->GetPosition()).ComputeLength(), 0.f, 100.f);
		// find the angle to the point
		float angle = atan2(character->GetPosition().y - pos.y, character->GetPosition().x - pos.x);

		if (character->GetIsEnemy())
		{
			// set the characters velocity
			character->SetVelocity(SGD::Vector{ cos(angle), sin(angle) } *5000);

			if (push->bonusKB)
			{
				if (push->owner == ClassType::Brawler)
				{
					Bleeding* bleed = new Bleeding(character);
					character->AddStatusAilment(bleed);
				}
				else if (push->owner == ClassType::Cyborg)
				{
					Radiation* rad = new Radiation(character);
					rad->SetStacks(5);
					character->AddStatusAilment(rad);
				}
			}

			Stun* stun = new Stun(character);
			stun->SetDuration(0.25f);
			character->AddStatusAilment(stun);
		}

		if (push->bonusKB && push->owner == ClassType::Medic)
			character->ClearStatusEffects();
	}
	if (name == "NPC_DEAD")
	{
		if (target == args)
		{
			//Target died so release the pointer to target and set target to nullptr
			target->Release();
			setTarget(nullptr);
		}
		else if (character == args)
		{
			//"I" died
			//SetCharacter(nullptr);
		}
		else if (anchor == args)
		{
			//Anchor died
			SetAnchor(nullptr);
		}
	}
	if (name == "GainExp" && GameplayState::GetInstance()->player)
	{
		int* bonusXP = reinterpret_cast<int*>(args);
		for (unsigned int i = 0; i < GameplayState::GetInstance()->player->party.size(); i++)
		{
			if (GameplayState::GetInstance()->player->party[i] == this)
			{
				if (bonusXP)
					this->getCharacter()->SetExp(this->getCharacter()->GetCurrExp() + (int)(1.6f*(*bonusXP)*0.75f));
			}
		}
	}
}

void NPC::SetBossType(BossType type)
{
	boss = type;
	if (type != BossType::GRUNT)
	{
		SGD::Size char_size = character->GetSize();
		char_size *= 1.5f;
		character->SetSize(char_size);
	}
}
