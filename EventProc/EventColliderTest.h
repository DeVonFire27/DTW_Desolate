/***************************************************************
|	File:		EventColliderTest.h
|	Author:		Michael Mozdzierz
|	Date:		04/16/2014
|
***************************************************************/

#pragma once

#include "../Entity.h"
#include "IObserver.h"

class EventColliderTest : public Entity
{
public:
	EventColliderTest(void);

	/////////////////////////<Interface>///////////////////////////////////////////////////

	virtual void HandleCollision(const IEntity* other) override;

	virtual SGD::Rectangle GetRect(void) const { return SGD::Rectangle(m_ptPosition.x - 50.f, m_ptPosition.y - 50.f, m_ptPosition.x + 50.f, m_ptPosition.y + 50.f); }
};