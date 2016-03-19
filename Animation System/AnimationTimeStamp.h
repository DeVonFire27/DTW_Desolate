#pragma once
#include <string>

class AnimationTimeStamp
{
	std::string currAnimation;
	int currFrame = 0;
	float timeOnFrame = 0.0f;
	bool hasEnded = false;

public:
	int GetCurrFrame() const { return currFrame; }
	float GetTimeOnFrame() const { return timeOnFrame; }
	std::string GetCurrAnimation() { return currAnimation; }

	bool HasEnded() const { return hasEnded; }
	void SetEnded(bool end) { hasEnded = end; }
	void SetCurrFrame(const int frameNum) { currFrame = frameNum; }
	void SetTimeOnFrame(const float timeNum) { timeOnFrame = timeNum; }
	void SetCurrAnimation(std::string newAni);
};
