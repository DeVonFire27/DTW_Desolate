/***************************************************************
|	File:		ColliderTest.h
|	Author:		Michael Mozdzierz
|	Date:		04/11/2014
|
***************************************************************/

#pragma once

#include "../Entity.h"
#include "../../SGD Wrappers/SGD_Geometry.h"

class ColliderTest : public Entity
{
	bool isColliding;

public:
	ColliderTest(void);

	/////////////////////////<Interface>///////////////////////////////////////////////////

	virtual void Update(float dt) final;
	virtual void HandleCollision(const IEntity* other) final;
	virtual void Render(void) final;

	virtual SGD::Rectangle GetRect(void) const override { return SGD::Rectangle(m_ptPosition.x - 50.f, m_ptPosition.y - 50.f, m_ptPosition.x + 50.f, m_ptPosition.y + 50.f); }
};