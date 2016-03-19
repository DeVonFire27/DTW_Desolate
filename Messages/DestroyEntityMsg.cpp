/***************************************************************
|	File:		DestroyEntity.cpp
|	Author:		Michael Mozdzierz
|	Date:		04/16/2014
|
***************************************************************/

#include "globals.h"
#include "DestroyEntityMsg.h"

DestroyEntityMsg::DestroyEntityMsg(Entity* EntityToDestroy) : SGD::Message(MSG_DESTROYENTITY)
{
	this->EntityToDestroy = EntityToDestroy;
}