/***************************************************************
|	File:		CreateHitMarkerMsg.cpp
|	Author:		Michael Mozdzierz
|	Date:		05/20/2014
|
***************************************************************/

#include "globals.h"
#include "CreateHitMarkerMsg.h"

CreateHitMarkerMsg::CreateHitMarkerMsg(int damage, SGD::Point& position, float direction, SGD::Color color) : SGD::Message(MSG_CREATEHITMARKER)
{
	this->damage = damage;
	this->position = position;
	this->direction = direction;
	this->color = color;
}

/*virtual*/ CreateHitMarkerMsg::~CreateHitMarkerMsg(void)
{
}