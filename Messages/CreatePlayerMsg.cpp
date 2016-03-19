/***************************************************************
|	File:		CreatePlayer.cpp
|	Author:		Michael Mozdzierz
|	Date:		04/16/2014
|
***************************************************************/

#include "globals.h"
#include "CreatePlayerMsg.h"

CreatePlayerMsg::CreatePlayerMsg(SGD::Point& position) : SGD::Message(MSG_CREATEPLAYER)
{
	this->position = position;
}