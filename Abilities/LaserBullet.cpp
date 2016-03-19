#include "globals.h"
#include "LaserBullet.h"
#include "../../source/GameplayState.h"
#include "../Animation System/AnimationSystem.h"
#include "Laser.h"
#include "Chainsaw.h"
#include "Ability.h"

LaserBullet::LaserBullet(Ability* own)
{
	owner = own;
	if (owner->GetAbilityType() == AbilityType::SNIPE_ACT)
		m_szSize = { 10.f, 10.f };
	else
		m_szSize = { 30.f, 30.f };
	isEnemyAtk = owner->GetIsFoe();
}

int LaserBullet::GetAbilityType() const
{
	if (owner->GetAbilityType() == AbilityType::SNIPE_ACT)
		return AbilityType::SNIPE_ACT; 

	return AbilityType::BRAWL_ACT;
}
LaserBullet::~LaserBullet()
{
	owner->Release();
}

void LaserBullet::Render()
{
	//SGD::Rectangle rect;
	//rect.top = m_ptPosition.y;
	//rect.left = m_ptPosition.x;
	//rect.Resize(SGD::Size(20, 20));
	//GraphicsManager::GetInstance()->DrawRectangle(rect, SGD::Color(155, 0, 125));
}

int LaserBullet::GetSkillLvl() const
{
	return owner->GetSkillLvl();
}

void LaserBullet::Update(float dt)
{
	// if this bullet goes outside bullet dropoff range...
	SGD::Point temp = AnimationSystem::GetInstance()->GetWeaponPoint(owner->GetAnimTS(), owner->GetPosition(), owner->GetRotation());
	float bulletPos = (m_ptPosition - owner->GetPosition()).ComputeLength();
	float beamPos = (temp - owner->GetPosition()).ComputeLength();
	if (bulletPos >beamPos)
	{
		// create a message to destroy this bullet
		DestroyEntityMsg* msg = new DestroyEntityMsg(this);
		// dispatch the destroy message
		SGD::MessageManager::GetInstance()->GetInstance()->QueueMessage(msg);
	}

	// call the inherited 'Update' function
	hitTimer -= dt;
	Entity::Update(dt);
}

void LaserBullet::HandleCollision(const IEntity* other) /*override*/
{
	if (reinterpret_cast<const Character*>(other) && reinterpret_cast<const Character*>(other)->GetIsEnemy() != isEnemyAtk && hitTimer <= 0.0f)
	{
		if (GetSkillLvl() == 3 && GetAbilityType() == AbilityType::SNIPE_ACT)
		{
			hitTimer = 0.2f;
			Laser* temp = (Laser*)owner;
			if (owner)
				temp->GetOwner()->ModifyHealth(temp->GetOwner()->GetStat(StatType::health)*0.0025f);
		}
		else if (GetSkillLvl() >= 2 && GetAbilityType() == AbilityType::BRAWL_ACT)
		{
			hitTimer = 0.2f;
			Chainsaw* temp = (Chainsaw*)owner;
			if (owner)
				temp->GetOwner()->ModifyHealth(temp->GetOwner()->GetStat(StatType::strength)*0.015f);

			if (GetSkillLvl() == 3 && owner)
			{
				temp->GetOwner()->SetCurrCooldown(temp->GetOwner()->GetCurrCoolDown() - 0.1f);
			}
		}
		
	}
}