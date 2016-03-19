/***************************************************************
|	File:		EventProc.h
|	Author:		Michael Mozdzierz
|	Date:		04/16/2014
|
***************************************************************/

#pragma once

class IObserver;


#include <vector>
#include <map>
#include <string>
#include "Signal.h"

using namespace std;

class EventProc
{
	map<string, vector<IObserver*>> observers;

	/////////////////////////<-Private- Constructor>///////////////////////////////////////////////////
	// Keep constructor private for singleton

	EventProc(void);
public:
	/////////////////////////<Singleton Accessor>///////////////////////////////////////////////////

	static EventProc* GetInstance(void);

	/////////////////////////<public functions>///////////////////////////////////////////////////

	void Dispatch(const char* name);
	void Dispatch(const char* name, void* prams, bool killMemory = false);
	void Flush(void);
	void Subscribe(IObserver* observer, char* eventType);
	void UnSubscribe(IObserver* observer, char* eventType);
};