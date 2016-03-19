#include "globals.h"
#include "Objective.h"

Objective::Objective(string displayString, string compareString, int locX, int locY)
{
	stringToDisplay = displayString;
	stringToCompare = compareString;
	this->locX = locX;
	this->locY = locY;
}