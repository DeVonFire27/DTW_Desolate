/***************************************************************
|	File:		Pitfal.cpp
|	Author:		Michael Mozdzierz
|	Date:		04/12/2014
|
***************************************************************/

#include "globals.h"
#include "Pitfall.h"
#include "../Agents/Character.h"

Pitfall::Pitfall(void)
{
	m_hImage = SGD::GraphicsManager::GetInstance()->LoadTexture("resources\\graphics\\pitfallImg.png");
	m_szSize = { 50.f, 50.f };
}

Pitfall::~Pitfall(void)
{
	SGD::GraphicsManager::GetInstance()->UnloadTexture(m_hImage);
}

/*virtual*/ void Pitfall::Update(float dt) /*override*/
{
	// decrament timer if it's not zero
	if (duration > 0.f)
		duration -= dt;

	// call inherited function
	Entity::Update(dt);
}