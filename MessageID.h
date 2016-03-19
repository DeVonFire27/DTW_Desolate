/***************************************************************
|	File:		MessageID.h
|	Author:		
|	Course:		
|	Purpose:	MessageID enum declares the message enumerators
***************************************************************/

#ifndef MESSAGEID_H
#define MESSAGEID_H


/**************************************************************/
// MessageID enum
//	- enumerated list of message identifiers

enum MessageID
{
	MSG_DESTROYENTITY,
	MSG_CREATEPLAYER,
	MSG_CREATEBULLET,
	MSG_CREATENPC,
	MSG_CREATEHEALTHPICKUP,
	MSG_CREATEWEAPON,
	MSG_CREATE_ENEMY,
	MSG_CREATEABILITY,
	MSG_CREATELASER,
	MSG_CREATEHITMARKER,
};


#endif //MESSAGEID_H