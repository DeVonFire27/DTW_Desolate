#include "globals.h"
#include "AnimationTimeStamp.h"

void AnimationTimeStamp::SetCurrAnimation(std::string newAni)
{
	currAnimation = newAni;
	timeOnFrame = 0.0f;
	currFrame = 0;
	hasEnded = false;
}