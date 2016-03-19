/***************************************************************
|	File:		Ability.h
|	Author:		Michael Mozdzierz
|	Date:		04/11/2014
|
***************************************************************/

#pragma once

#include "../../SGD Wrappers/SGD_Handle.h"
#include "../../SGD Wrappers/SGD_Geometry.h"
#include "../Animation System/AnimationTimeStamp.h"
#include "../../source/Entity.h"
#include "../Agents/Character.h"

using namespace SGD;

class Ability : public Entity
{
protected:
	float damage = 0;
	float duration = 0;
	int SkillLvl = 1;
	SGD::Rectangle collide = SGD::Rectangle{};
	Character* owner = nullptr;

	AnimationTimeStamp action;

public:
	Ability(void);
	virtual ~Ability(void) = 0;

	/////////////////////////<Public functions>///////////////////////////////////////////////////

	virtual void Activate() = 0;

	/////////////////////////<Interface>///////////////////////////////////////////////////

	virtual void Update(float dt) override;
	virtual void Render(void) override;

	virtual SGD::Rectangle GetRect(void) const override;
	virtual int	GetType(void)	const override	{ return ENT_ABILITY; }

	virtual void HandleCollision(const IEntity* other) override;
	void SetOwner(Character* own);
	void SetSkillLvl(int lvl) { SkillLvl = lvl; }
	Character* GetOwner() const { return owner; }
	/////////////////////////<Accessors>///////////////////////////////////////////////////

	float GetDamage(void) const { return damage; }
	float GetDuration(void) const { return duration; }
	bool GetIsFoe() const ;
	virtual AnimationTimeStamp GetAnimTS() { return action; }
	int GetSkillLvl() const { return SkillLvl; }
};