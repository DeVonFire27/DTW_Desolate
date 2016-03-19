/***************************************************************
|	File:		CreateHealthPickup.h
|	Author:		Michael Mozdzierz
|	Date:		04/16/2014
|
***************************************************************/

#pragma once

#include "../../SGD Wrappers/SGD_Message.h"
#include "../../SGD Wrappers/SGD_Geometry.h"

class CreateHealthPickupMsg : public SGD::Message
{
	SGD::Point position;

public:
	CreateHealthPickupMsg(SGD::Point& position);

	SGD::Point GetPosition(void) const { return position; }
};