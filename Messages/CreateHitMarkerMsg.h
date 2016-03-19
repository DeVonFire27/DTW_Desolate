/***************************************************************
|	File:		CreateHitMarkerMsg.h
|	Author:		Michael Mozdzierz
|	Date:		05/20/2014
|
***************************************************************/

#pragma once

#include "../../SGD Wrappers/SGD_Geometry.h"
#include "../../SGD Wrappers/SGD_Message.h"
#include "../../SGD Wrappers/SGD_Color.h"

class CreateHitMarkerMsg : public SGD::Message
{
	int damage;

	float direction;

	SGD::Point position;

	SGD::Color color;

public:
	CreateHitMarkerMsg(int damage, SGD::Point& position, float direction, SGD::Color color);
	virtual ~CreateHitMarkerMsg(void);

	/////////////////////////<Accessors/Mutators>///////////////////////////////////////////////////

	int GetDamage(void) const { return damage; }

	float GetDirection(void) const { return direction; }

	SGD::Color GetColor() const { return color; }

	SGD::Point GetPosition(void) const { return position; }
};