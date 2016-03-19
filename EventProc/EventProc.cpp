/***************************************************************
|	File:		EventProc.cpp
|	Author:		Michael Mozdzierz
|	Date:		04/16/2014
|
***************************************************************/

#include "globals.h"
#include "EventProc.h"
#include "IObserver.h"

#include <string>

using namespace std;

EventProc::EventProc(void)
{
}

/*static*/ EventProc* EventProc::GetInstance(void)
{
	// lazy inilitize pointer
	static EventProc instance;
	// return pointer
	return &instance;
}

void EventProc::Dispatch(const char* name)
{
	// for everyone listing to this event
	for (unsigned int ii = 0; ii < observers[name].size(); ii++)
		observers[name][ii]->HandleEvent(name);
}

void EventProc::Dispatch(const char* name, void* prams, bool killMemory)
{	
	// for everyone listing to this event
	for (unsigned int ii = 0; ii < observers[name].size(); ii++)
		observers[name][ii]->HandleEvent(name, prams);

	// if the user wants to clean up memory after-process, do so
	if (killMemory)
		delete prams;
}

void EventProc::Subscribe(IObserver* observer, char* eventType)
{
	// add the observer to the new even type
	observers[eventType].push_back(observer);
}

void EventProc::UnSubscribe(IObserver* observer, char* eventType)
{
	// store which bucket we need to look through
	vector<IObserver*>& bucket = observers[eventType];
	// for all the observers in this bucket
	for (unsigned int i = 0; i < bucket.size(); i++)
	{
		// if these are the droids we're looking for
		if (bucket[i] == observer)
		{
			// erase this observer
			bucket.erase(bucket.begin() + i);
			// break out of this function
			return;
		}
	}
}