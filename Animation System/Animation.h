#pragma once
#include "Frame.h"
#include <string>
#include <vector>
#include "../../SGD Wrappers/SGD_Handle.h"

class Animation
{
	SGD::HTexture aniImg = SGD::INVALID_HANDLE;
	std::string aniName;
	bool isLooping = true;
public:
	Animation() = default;
	Animation(SGD::HTexture image, std::string name, bool loop, std::vector<Frame> f);
	~Animation(void);

	SGD::HTexture GetImage() const { return aniImg; }
	std::string GetAniName() const { return aniName; }
	bool GetLooping() const { return isLooping; }

	std::vector<Frame> frames;

};
