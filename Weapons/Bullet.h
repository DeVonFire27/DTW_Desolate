/***************************************************************
|	File:		Bullet.h
|	Author:		Michael Mozdzierz
|	Date:		04/23/2014
|
***************************************************************/

#pragma once

#include "../../source/Entity.h"

class TileLayer;

class Bullet : public Entity
{
	friend class GameplayState;
	friend class Weapon;	

	float damage, tempDamage;
	bool critPassive = false;

	Weapon* owner = nullptr;
	float bulletDropOff = 128;
	bool isEnemyAtk = false;
	bool spit = false;

	bool gunActive = false;
	bool visible = true;
	SGD::Size bulletSize = SGD::Size{ 20, 20 };
	float dist = 1.0f;

	TileLayer* collisionLayer = nullptr;
	SGD::HTexture saw = SGD::INVALID_HANDLE;
	SGD::HTexture spittle = SGD::INVALID_HANDLE;
	SGD::Point startPoint = SGD::Point{};

public:
	Bullet(Weapon* own);
	~Bullet(void);

	/////////////////////////<Interface>///////////////////////////////////////////////////
	virtual void Update(float dt) override;
	virtual void Render() override;
	virtual void HandleCollision(const IEntity* other) override;
	bool alreadyMultiplied = false;
	//handles any gun damage modifications like passive bonuses and crits
	void DamageModifier();
	void SetDamage(float dmg) { damage = dmg; }
	/////////////////////////<accessors>///////////////////////////////////////////////////
	Weapon* GetOwner() const { return owner; }
	float GetDamage(void) const { return damage; }
	bool IsFoeAtk() const { return isEnemyAtk; }
	void SetIsEnemyAtk(bool b) { isEnemyAtk = b; }
	void SetStartPoint(SGD::Point point);
	bool GetCritPassive() const { return critPassive; }
	bool deflectBullet = false;
	bool hasBeenDeflected = false;
	bool CheckTileCollision();
	bool GetGunActive() const { return gunActive; }
	float GetDistance() const { return dist; }
};