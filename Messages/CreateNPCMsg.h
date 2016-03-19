/***************************************************************
|	File:		CreateNPC.h
|	Author:		Michael Mozdzierz
|	Date:		04/16/2014
|
***************************************************************/

#pragma once

#include "../../SGD Wrappers/SGD_Message.h"

class NPC;

class CreateNPCMsg : public SGD::Message
{
	NPC* objToCreate = nullptr;

public:
	CreateNPCMsg(NPC* npcToCreate);

	NPC* GetNpc(void) { return objToCreate; }
};