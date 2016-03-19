/***************************************************************
|	File:		Hazards.h
|	Author:		Michael Mozdzierz
|	Date:		04/11/2014
|
***************************************************************/

#pragma once

#include "../../source/Entity.h"

enum HazardType { BASE_HAZARD, BEAR_TRAP, RADIATION_POOL, PITFALL };

class Hazards : public Entity
{
protected:
	float duration;

public:
	Hazards(void);
	virtual ~Hazards(void);

	virtual HazardType GetHazardType() { return BASE_HAZARD; }

	/////////////////////////<Accessors-Mutatators>///////////////////////////////////////////////////

	float GetDuration(void) const { return duration; }

	void SetDuration(float value) { duration = value; }
};