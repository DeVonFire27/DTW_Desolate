#pragma once
#include "../../SGD Wrappers/SGD_Message.h"
#include "../../SGD Wrappers/SGD_Geometry.h"

class Ability;

struct CreateLaserBullet : SGD::Message
{
	float direction = 0.f;
	float speed = 0.f;
	float damage = 0.f;

	Ability* owner = nullptr;

	CreateLaserBullet(Ability* own);
};

