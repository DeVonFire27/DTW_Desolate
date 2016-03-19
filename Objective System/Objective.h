#pragma once

#include <iostream>
using namespace std;

class Objective
{
	string stringToDisplay;
	string stringToCompare;
	int locX;
	int locY;

public:
	Objective(string displayString, string compareString, int locX, int locY);

	string GetObjectiveText() { return stringToDisplay; }
	string GetCompareString() { return stringToCompare; }

	int GetLocX() { return locX; }
	int GetLocY() { return locY; }
};