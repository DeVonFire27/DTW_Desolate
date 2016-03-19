/***************************************************************
|	File:		BearTrap.cpp
|	Author:		Michael Mozdzierz
|	Date:		04/12/2014
|
***************************************************************/

#include "globals.h"
#include "BearTrap.h"

BearTrap::BearTrap(void)
{
	m_hImage = SGD::GraphicsManager::GetInstance()->LoadTexture("resources\\graphics\\bearTrapImage.png");
	m_szSize = { 50.f, 50.f };
}

BearTrap::~BearTrap(void)
{
}