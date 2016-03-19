/***************************************************************
|	File:		CreateEnemy.h
|	Author:		Michael Mozdzierz
|	Date:		04/16/2014
|
***************************************************************/

#pragma once

#include "../../SGD Wrappers/SGD_Message.h"

class NPC;

class CreateEnemyMsg : public SGD::Message
{
	NPC* objToCreate = nullptr;

public:
	CreateEnemyMsg(NPC* npcToCreate);
	
	NPC* GetNpc(void) const { return objToCreate; }
};