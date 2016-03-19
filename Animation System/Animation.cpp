
#include "globals.h"
#include "Animation.h"
Animation::Animation(SGD::HTexture image, std::string name, bool loop, std::vector<Frame> f)
{
	aniImg = image;
	aniName = name;

	isLooping = loop;
	frames = f;
}

Animation::~Animation()
{
	frames.clear();
}