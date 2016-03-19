#include "globals.h"
#include "../Entity.h"
#include "CreateLaserBullet.h"
#include "../Abilities/Ability.h"

CreateLaserBullet::CreateLaserBullet(Ability* own) : SGD::Message(MSG_CREATELASER)
{
	if (own->GetSkillLvl() != 3 && own->GetAbilityType() == Entity::AbilityType::SNIPE_ACT)
		this->direction = own->GetRotation();
	else if (own->GetAbilityType() == Entity::AbilityType::SNIPE_ACT)
		this->direction = own->GetRotation() - (PI*0.5f);
	else
		this->direction = own->GetRotation() - (PI*0.5f);

	this->damage = own->GetDamage();
	owner = own;
	speed = 3000.0f;
}
