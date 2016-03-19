/***************************************************************
|	File:		ColliderTest.cpp
|	Author:		Michael Mozdzierz
|	Date:		04/11/2014
|
***************************************************************/
#include "globals.h"
#include "ColliderTest.h"

ColliderTest::ColliderTest(void)
{
	m_hImage = SGD::GraphicsManager::GetInstance()->LoadTexture("resource//graphics//normal.png");
}

/*virtual*/ void ColliderTest::Update(float dt) /*final*/
{
	isColliding = false;

	// call inherited function
	Entity::Update(dt);
}

/*virtual*/ void ColliderTest::HandleCollision(const IEntity* other) /*final*/
{
	isColliding = true;
}	

/*virtual*/ void ColliderTest::Render(void) /*final*/
{
	SGD::GraphicsManager::GetInstance()->DrawTexture(m_hImage, m_ptPosition, 0.f, SGD::Vector(0.f, 0.f), isColliding ? SGD::Color::Red : SGD::Color::White);
}
