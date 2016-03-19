/***************************************************************
|	File:		CreateBullet.cpp
|	Author:		Michael Mozdzierz
|	Date:		04/16/2014
|
***************************************************************/

#include "globals.h"
#include "CreateBulletMsg.h"
#include "../Weapons/Weapon.h"
#include "../Weapons/Bullet.h"

CreateBulletMsg::CreateBulletMsg(Weapon* own) : SGD::Message(MSG_CREATEBULLET)
{
	if (own->GetOwner())
	{
		this->direction = own->GetRotation() + own->GetSwayRad();

		this->damage = own->GetDamage();
		owner = own;

		if (own->GetGunType() == Weapon::GunType::Shotgun)
			speed = 500.f + float((rand() % 50) - 25);
		else
			speed = 1000.0f;
	}
}

CreateBulletMsg::CreateBulletMsg(Bullet* b, Character* e) : SGD::Message(MSG_CREATEBULLET)
{
	//Calculating direction to mouse
	SGD::Vector orientation = { 0, -1 }; // Default orientation

	this->damage = b->GetOwner()->GetDamage();
	owner = b->GetOwner();
	this->direction = owner->GetRotation() + owner->GetSwayRad();

	split = true;

	if (b->GetOwner()->GetGunType() == Weapon::GunType::Shotgun)
		speed = 500.f + float((rand() % 50) - 25);
	else
		speed = 1000.0f;
	enemy = e;

}
