#include "globals.h"
#include "Frame.h"

Frame::Frame(SGD::Rectangle rendRect, SGD::Rectangle colRect, 
	SGD::Rectangle actRect, SGD::Point anchor, SGD::Point weapon, std::string eve, float dur)
{
	renderRect = rendRect;
	collideRect = colRect;
	activeRect = actRect;

	anchorPoint = anchor;
	weaponOffset = weapon;

	eventMessage = eve;

	duration = dur;
}