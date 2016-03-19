/***************************************************************
|	File:		Hazards.cpp
|	Author:		Michael Mozdzierz
|	Date:		04/12/2014
|
***************************************************************/

#include "globals.h"
#include "Hazards.h"

Hazards::Hazards(void)
{
	// ensure no garbage in 'duration' value
	duration = 0.f;
}

/*virtual*/ Hazards::~Hazards(void)
{
	SGD::GraphicsManager::GetInstance()->UnloadTexture(m_hImage);
}