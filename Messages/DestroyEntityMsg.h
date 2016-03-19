/***************************************************************
|	File:		DestroyEntity.h
|	Author:		Michael Mozdzierz
|	Date:		04/16/2014
|
***************************************************************/

#pragma once

#include "../../SGD Wrappers/SGD_Message.h"

class Entity;

class DestroyEntityMsg : public SGD::Message
{
	Entity* EntityToDestroy = nullptr;

public:
	DestroyEntityMsg(Entity* entityToDestroy);

	Entity* GetEntity(void) const { return EntityToDestroy; }
};