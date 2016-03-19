/***************************************************************
|	File:		CreateHealthPickup.cpp
|	Author:		Michael Mozdzierz
|	Date:		04/16/2014
|
***************************************************************/

#include "globals.h"
#include "CreateHealthPickupMsg.h"

CreateHealthPickupMsg::CreateHealthPickupMsg(SGD::Point& position) : SGD::Message(MSG_CREATEHEALTHPICKUP)
{
	this->position = position;
}