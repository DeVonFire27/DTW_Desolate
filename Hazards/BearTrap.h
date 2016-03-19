/***************************************************************
|	File:		BearTrap.h
|	Author:		Michael Mozdzierz
|	Date:		04/11/2014
|
***************************************************************/

#pragma once

#include "Hazards.h"
#include "../../SGD Wrappers/SGD_Handle.h"

class BearTrap : public Hazards
{
	SGD::HAudio sound;

	SGD::HTexture closedTex;

public:
	BearTrap(void);
	~BearTrap(void);
	
	HazardType GetHazardType() override { return BEAR_TRAP; }
};