/***************************************************************
|	File:		CreateNPC.cpp
|	Author:		Michael Mozdzierz
|	Date:		04/16/2014
|
***************************************************************/

#include "globals.h"
#include "CreateNPCMsg.h"

CreateNPCMsg::CreateNPCMsg(NPC* objToCreate) : SGD::Message(MSG_CREATENPC)
{
	// store objToCreate
	this->objToCreate = objToCreate;
}