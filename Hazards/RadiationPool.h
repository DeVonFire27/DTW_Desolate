/***************************************************************
|	File:		RadiationPool.h
|	Author:		Michael Mozdzierz
|	Date:		04/11/2014
|
***************************************************************/

#pragma once

#include "Hazards.h"

class RadiationPool : public Hazards
{
public:
	RadiationPool(void);
	~RadiationPool(void);

	virtual void Update(float dt) override;

	HazardType GetHazardType() override { return RADIATION_POOL; }
};