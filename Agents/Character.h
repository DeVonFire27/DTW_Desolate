/***************************************************************
|	File:		Character.h
|	Author:		Michael Mozdzierz
|	Date:		04/11/2014
|
***************************************************************/

#pragma once

#include "../../source/Entity.h"
#include "../Animation System/AnimationTimeStamp.h"
#include "../EventProc/IObserver.h"
#include <map>
#include <list>

#include "../Status Effects/StatusEffect.h"

using namespace std;

#include "../Agents/Stat.h"
#include "../Tile System/World.h"
#include "../GameplayState.h"
#include "IMinimapVisible.h"

class BitmapFont;
class Weapon;
class Ability;
class TileLayer;
struct Tile;
class StatusEffect;

enum class ClassType
{
	Brawler,
	Cyborg,
	Gunslinger,
	Sniper,
	Medic,
	Jane,
	Mutant
};

struct PushBack
{
	SGD::Point pos = SGD::Point{};
	bool bonusKB = false;
	ClassType owner;
};


class Character : public Entity, public IObserver, public IMinimapVisible
{
	bool characterCollision = false;
	bool healthDebug = false;

	bool inParty = false;

	string eventString;

protected:
	float health = 100.f,
		currHealth = 100.f,
		cooldown = 0.0f,
		currCooldown = 0.0f,
		lastCooldown = 0.0f;

	SGD::Point knockbackVector = SGD::Point(0, 0);

	vector<StatusEffect*>  effects;

	Weapon* weapon = nullptr;
	vector<Tile*> collidingTiles;
	int level = 1,
		expNeeded = 0,
		currExp = 0;

	bool cooldownTimerStarted = false;
	bool collisionLoaded = false;
	bool swingWeapon = false;

	Ability* ability = nullptr;
	SGD::Rectangle collide = SGD::Rectangle{};

	bool isEnemy = false;
	bool stunned = false;

	AnimationTimeStamp charAnim;

	map<StatType, Stat> stats;
	SGD::Rectangle futureLocation;

	float ambulHit = 0.0f;
	float hitTimer = 0.0f;
	unsigned int critHitChance = 0;

	bool alreadyHit = false;
	bool contactWithRadiation = false;
	float radiationStackTimer = 0.f;

	float radiationSoundTimer = 0.f;
	float bearTrapSoundTimer = 0.f;
	float pitfallSoundTimer = 0.f;
	float bullPushTimer = 0.0f;

public:
	float currDamage = 0;
	float shotTimer = 0.0f;
	float passiveTimer = 0.0f;
	bool isMoving = false;
	float move = 0.0f;
	bool spawnCollision = false;
	float pushTimer = 0.0f;

	SGD::Point weaponRecoil = SGD::Point(0, 0);

	SGD::Vector plusBulletVector = SGD::Vector(0, 0);

	Character(void);
	virtual ~Character();
	bool recruitEmitterCreated = false;
	/////////////////////////<Public Functions>///////////////////////////////////////////////////
	TileLayer* collisionLayer = GameplayState::GetInstance()->GetWorld()->GetTileLayers()[1];
	int timer = 0;
	bool GetCharacterCollision() const { return characterCollision; }
	void CheckTileCollision();
	virtual void UseWeapon(void);
	virtual void UseAbility(void);
	virtual void HandlePassive(void);
	virtual void LevelUp(void);
	virtual void ModifyHealth(float value);
	void LoadStats(const char* filename);
	virtual void HandleCollision(const IEntity* other) override;
	void WalkTowards(SGD::Point target);
	void WalkTowards(Entity *target);
	void WalkDirection(SGD::Vector target);
	virtual void AddStatusAilment(StatusEffect* effect);
	void RemoveStatusAilment(StatEffectType type);
	string GetClassString();

	bool GetInParty() { return inParty; }
	void SetInParty(bool newInParty) { inParty = newInParty; }

	virtual SGD::Rectangle GetRect(void) const override;


	string GetEvent() { return eventString; }
	void SetEvent(string newEvent) { eventString = newEvent; }

	virtual void HandleEvent(string name, void* args = nullptr) override;

	HTexture dialogueIcon;
	virtual HTexture& GetDialogueIcon() { return dialogueIcon; }
	virtual std::stringstream GetActiveDiscription();
	virtual std::stringstream GetPassiveDiscription();

	bool CheckCharacterCollision(SGD::Rectangle futureLoacation);
	virtual int		GetType(void)	const			override	{ return ENT_BASE; }
	void Update(float dt) override;
	void Render() override;
	virtual void RenderMiniMap(void) override;
	/////////////////////////<Accessors>///////////////////////////////////////////////////
	bool GetAlreadyHit() const { return alreadyHit; }
	void SetAlreadyHit(bool ah) { alreadyHit = ah; }
	float GetHealth(void) const { return health; }
	float GetCurrHealth(void) const { return currHealth; }

	float GetCoolDown(void) const { return cooldown; }
	float GetCurrCoolDown(void) const { return currCooldown; }
	float GetLastCoolDown(void) const { return lastCooldown; }
	bool GetIsEnemy(void) const { return isEnemy; }
	bool GetStunned(void) const;
	int GetLevelUpExp(void) const { return (int)(4 + (level - 1) * 9.5f); }
	AnimationTimeStamp GetAnimation() { return charAnim; }

	void SetWeapon(Weapon* w);
	Weapon* GetWeapon() { return weapon; }
	virtual float GetStat(StatType type) { return stats[type][level]; }
	Stat GetStatType(StatType type) { return stats[type]; }

	int GetCritHitChance() { return critHitChance; }
	void SetCritHitChance(int crit) { critHitChance = crit; }
	virtual Weapon* SetStartingWeapon() { return nullptr; }
	int GetStartingPrefix(int chance);

	int GetLevel(void) const { return level; }
	int GetExpNeeded(void) const { return expNeeded; }
	int GetCurrExp(void) const { return currExp; }
	Ability* GetAbility(void) const { return ability; }

	virtual ClassType GetClass(void) const { return ClassType::Brawler; }
	void ClearStatusEffects();
	void AOEPull(SGD::Point pos, float range, float pullSpeed, float roationSpeed = 0.0f);
	float DistanceToPlayer();

	/////////////////////////<Mutators>///////////////////////////////////////////////////

	void SetIsEnemy(bool value) { isEnemy = value; }	//to be altered, need it to call its time stamp's collision
	void SetCurrHealth(float ch) { currHealth = ch; }
	void SetHealth(float value) { health = value; }
	void SetExp(int exp);
	void SetLevel(int lvl) { level = lvl; }
	void SetStunned(bool value) { stunned = value; }
	void SetCurrCooldown(float cool);
};

NPC *FindControllingNPC(Character *character);

