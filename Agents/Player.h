/***************************************************************
|	File:		Player.h
|	Author:		Michael Mozdzierz
|	Date:		04/11/2014
|
***************************************************************/

#pragma once

#include <vector>
#include "../../SGD Wrappers/SGD_Geometry.h"
#include "../GameplayState.h"
#include "../Camera.h"
#include "../EventProc/IObserver.h"

using namespace std;

class NPC;
class Character;
class IEntity;
class Weapon;

class Player : public IObserver
{
	//friend GameplayState;

	Character* character = nullptr;

	//This pointer exists so the event system can use the player to find the weapon you are trying to swap with.
	//Once the player presses the interact button on a weapon, that weapon is stored here and then the handle event for
	//swapping weapons is handled which calls the SwapWeapons() function which accesses this pointer.
	Weapon* otherWeapon = nullptr;


	int reputation = 0;
	float switchTargetCooldown = 0.0f;
	NPC *target = nullptr;
public:
	bool autoCast = true;

	Player(void);
	~Player(void);
	/////////////////////////<Public functions>///////////////////////////////////////////////////
	vector<NPC*> party;

	bool AlterParty(void);
	unsigned int killCount = 0;
	SGD::Point GetPosition();
	void SetOtherWeapon(Weapon* w);
	void SetCharacter(Character* c);
	void RenderHUD(void);
	void Update(float dt);
	void Input(void);

	string currentLocation = "";
	float sinceLeft = 0.0f;
	float sinceLocationReached = 0.0f;
	SGD::Vector lastVelocity = { 0, -1 };

	Character* GetCharacter() { return character; }
	void ModifyReputation(Character* who);

	/////////////////////////<Interface>///////////////////////////////////////////////////

	virtual void HandleEvent(string name, void* args) override;

	/////////////////////////<Accessors>///////////////////////////////////////////////////
	SGD::HTexture healthBar, experienceBar;

	int GetReputation(void) const { return reputation; }
	void SetReputation(int newRep) { reputation = newRep; }
	void SetTarget(NPC *set);
	Character* GetPartyMember(int partyIndex); //0 = player

	SGD::Rectangle GetRect();
	void DrawCharacterPortraits();
};