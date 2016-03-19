/***************************************************************
|	File:		CreateWeapon.cpp
|	Author:		Michael Mozdzierz
|	Date:		04/16/2014
|
***************************************************************/

#include "globals.h"
#include "CreateWeaponMsg.h"
#include "../Weapons/Weapon.h"

CreateWeaponMsg::CreateWeaponMsg(Weapon* weapon) : SGD::Message(MSG_CREATEWEAPON)
{
	// store the weapon we want to create
	this->weapon = weapon;
	// add a reference to the weapon
	this->weapon->AddRef();
}