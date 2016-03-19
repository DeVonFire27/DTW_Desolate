/***************************************************************
|	File:		
.h
|	Author:		Michael Mozdzierz
|	Date:		04/16/2014
|
***************************************************************/

#pragma once

#include "../../SGD Wrappers/SGD_Message.h"
#include "../../SGD Wrappers/SGD_Geometry.h"

class Weapon;
class Character;
class Bullet;

struct CreateBulletMsg : SGD::Message
{
	float direction = 0.f,
			speed = 0.f,
			damage = 0.f;
	bool split = false;

	Weapon* owner = nullptr;
	Character* enemy = nullptr;
	Bullet* b = nullptr;
	CreateBulletMsg(Weapon* own);
	CreateBulletMsg(Bullet* b, Character* e);
};