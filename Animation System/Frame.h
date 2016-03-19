#pragma once
#include "../../SGD Wrappers/SGD_Geometry.h"
#include <string>

class Frame
{
	float duration = 0.5f;

	SGD::Rectangle renderRect;
	SGD::Rectangle collideRect;
	SGD::Rectangle activeRect;

	SGD::Point anchorPoint;
	SGD::Point weaponOffset;

	std::string eventMessage;
public:

	Frame(SGD::Rectangle rendRect, SGD::Rectangle colRect, SGD::Rectangle actRect, SGD::Point anchor, SGD::Point weapon, std::string eve, float dur);

	float GetDuration() const{ return duration; }
	SGD::Rectangle GetRenderRect() const { return renderRect; }
	SGD::Rectangle GetCollideRect() const { return collideRect; }
	SGD::Rectangle GetActiveRect() const { return activeRect; }
	SGD::Point GetAnchorPoint() const { return anchorPoint; }
	SGD::Point GetWeapon() const { return weaponOffset; }
	std::string GetEvent() const { return eventMessage; }

};
