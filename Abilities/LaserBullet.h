#pragma once
#include "../Entity.h"

class Ability;

class LaserBullet : public Entity
{
	float damage = 0.0f;
	Ability* owner = nullptr;
	bool isEnemyAtk = false;

	float hitTimer = 0.0f;

public:
	LaserBullet(Ability* own);
	virtual ~LaserBullet();

	virtual void Update(float dt) override;
	virtual void Render() override;
	virtual void HandleCollision(const IEntity* other) override;

	virtual int	GetType(void)	const override	{ return ENT_ABILITY; }
	int GetAbilityType() const;


	float GetDamage(void) const { return damage; }
	bool IsFoeAtk() const { return isEnemyAtk; }
	float GetHitTimer() const { return hitTimer; }
	Ability* GetOwner() const { return owner; }

	void SetDamage(const float dmg) { damage = dmg; }
	int GetSkillLvl() const;
};

