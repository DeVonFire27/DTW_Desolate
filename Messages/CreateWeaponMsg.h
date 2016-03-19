/***************************************************************
|	File:		CreateWeapon.h
|	Author:		Michael Mozdzierz
|	Date:		04/16/2014
|
***************************************************************/

#pragma once

#include "../../SGD Wrappers/SGD_Message.h"

class Weapon;

class CreateWeaponMsg : public SGD::Message
{
	Weapon* weapon = nullptr;

public:
	CreateWeaponMsg(Weapon* weapon);

	Weapon* GetWeapon(void) const { return weapon; }
};