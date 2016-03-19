/***************************************************************
|	File:		HealthPickup.h
|	Author:		Michael Mozdzierz
|	Date:		04/14/2014
|
***************************************************************/
#pragma once
#include "Entity.h"
#include "../SGD Wrappers/SGD_GraphicsManager.h"
#include "../SGD Wrappers/SGD_Geometry.h"

class HealthPickup : public Entity
{
private:
	float timer = 10.0f;
public:
	HealthPickup(SGD::Point pos);
	~HealthPickup();
	virtual void HandleCollision(const IEntity* other) override;
	virtual void Update(float dt) override;

	virtual int		GetType(void)	const			override	{ return ENT_MEDKIT; }

};