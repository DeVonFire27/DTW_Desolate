/***************************************************************
|	File:		EventColliderTest.h
|	Author:		Michael Mozdzierz
|	Date:		04/16/2014
|
***************************************************************/

#include "globals.h"
#include "EventColliderTest.h"
#include "EventProc.h"

EventColliderTest::EventColliderTest(void)
{
	m_hImage = SGD::GraphicsManager::GetInstance()->LoadTexture("resources\\graphics\\hazard.png");
}


/*virtual*/ void EventColliderTest::HandleCollision(const IEntity* other) /*override*/
{
	EventProc::GetInstance()->Dispatch("OnChangeColor", new SGD::Point(0, 0), true);
}