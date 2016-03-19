/***************************************************************
|	File:		HitMarker.h
|	Author:		Michael Mozdzierz
|	Date:		05/20/2014
|
***************************************************************/

#pragma once

#include "../Entity.h"
class HitMarker : public Entity
{
	float alpha = 1.f;

	int damage;

public:
	HitMarker(int damage, SGD::Point& position, float direction, SGD::Color color);
	virtual ~HitMarker(void);

	/////////////////////////<Interface>///////////////////////////////////////////////////

	virtual void Update(float dt) override;
	virtual void Render(void) override;
};