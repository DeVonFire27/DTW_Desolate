/***************************************************************
|	File:		Pitfal.h
|	Author:		Michael Mozdzierz
|	Date:		04/11/2014
|
***************************************************************/

#pragma once

#include "Hazards.h"
#include "../../SGD Wrappers/SGD_Handle.h"

class Pitfall : public Hazards
{

public:
	Pitfall(void);
	~Pitfall(void);
	/////////////////////////<text>///////////////////////////////////////////////////

	bool GetIsActive(void) const { return duration > 0.f; }

	/////////////////////////<Interface>///////////////////////////////////////////////////

	virtual void Update(float dt) override;
	HazardType GetHazardType() override { return PITFALL; }

};