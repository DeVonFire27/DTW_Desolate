#include "globals.h"
#include "MutantAttack.h"
#include "../Game.h"
#include "../EventProc/EventProc.h"
#include "../GameplayState.h"
#include "../Messages/CreateBulletMsg.h"
#include "../../SGD Wrappers/SGD_MessageManager.h"
#include "../Animation System/AnimationSystem.h"

MutantAttack::MutantAttack(MutantAtkType atk)
{
	mwt = atk;
	SetSize({ 50, 50 });
	damage = 15.0f;
	rateOfFire = 2.0f;
}


MutantAttack::~MutantAttack()
{
	
}

void MutantAttack::Attack()
{

	CreateBulletMsg* msg = new CreateBulletMsg(this);
	SGD::MessageManager::GetInstance()->QueueMessage(msg);

	// set the cool-down timer
	if (owner)
	{
		float atkMod = (100 - owner->GetStat(StatType::dexterity))*0.01f;
		float tempROF = 1 / rateOfFire;
		fireCoolDown = ((tempROF * 2) + atkMod) / 3;
	}
}

bool MutantAttack::Use(void)
{
	if (fireCoolDown <= 0.f)
	{
		Attack();
		return true;
	}

	return false;
}

float MutantAttack::GetDamage(void) /*override*/
{
	// if Brawler, increase damage @ level 15+
	float tempDmg = damage;
	if (owner)
	{
		// if not, return normal damage
		tempDmg += (damage * owner->GetStat(StatType::strength) * 0.01f);
	}

	return tempDmg;
}