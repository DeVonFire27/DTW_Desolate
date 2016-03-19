/***************************************************************
|	File:		CreateEnemy.cpp
|	Author:		Michael Mozdzierz
|	Date:		04/16/2014
|
***************************************************************/

#include "globals.h"
#include "CreateEnemyMsg.h"


CreateEnemyMsg::CreateEnemyMsg(NPC* npc) : SGD::Message(MSG_CREATE_ENEMY)
{
	// ensure npc is an enemy
	npc->getCharacter()->SetIsEnemy(true);
	// set npc pointer
	this->objToCreate = npc;
}