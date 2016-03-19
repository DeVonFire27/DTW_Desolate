/***************************************************************
|	File:		IObserver.h
|	Author:		Michael Mozdzierz
|	Date:		04/16/2014
|
***************************************************************/

#pragma once

#include <string>

using namespace std;

class IObserver
{
public:
	virtual ~IObserver(void) = 0 {}

	virtual void HandleEvent(string name, void* args = nullptr) = 0 {}
};