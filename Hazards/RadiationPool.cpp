/***************************************************************
|	File:		RadiationPool.cpp
|	Author:		Michael Mozdzierz
|	Date:		04/12/2014
|
***************************************************************/

#include "globals.h"
#include "RadiationPool.h"
#include "../Status Effects/StatusEffect.h"

RadiationPool::RadiationPool(void)
{
	m_hImage = SGD::GraphicsManager::GetInstance()->LoadTexture("resources\\graphics\\radiationPoolImg.png");
	color = SGD::Color::Green;
	m_szSize = { 50.f, 50.f };
}

RadiationPool::~RadiationPool(void)
{
}

/*virtual*/ void RadiationPool::Update(float dt) /*override*/
{

}