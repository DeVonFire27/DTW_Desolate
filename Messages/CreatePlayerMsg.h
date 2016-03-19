/***************************************************************
|	File:		CreatePlayer.h
|	Author:		Michael Mozdzierz
|	Date:		04/16/2014
|
***************************************************************/

#pragma once

#include "../../SGD Wrappers/SGD_Message.h"
#include "../../SGD Wrappers/SGD_Geometry.h"

class CreatePlayerMsg : public SGD::Message
{
	SGD::Point position;

public:
	CreatePlayerMsg(SGD::Point& position);

	SGD::Point GetPosition(void) const { return position;}
};