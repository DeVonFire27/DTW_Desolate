#pragma once
#include "../../SGD Wrappers/SGD_Handle.h"
#include "../../SGD Wrappers/SGD_Color.h"
#include "../../SGD Wrappers/SGD_Geometry.h"

class Particle
{
public:
	Particle() = default;
	~Particle() = default;

	SGD::Vector position;

	float lifetime;
	float sinceSpawn = 0.0f;

	float direction;
	float startVelocity;
	float endVelocity;

	SGD::Color currentColor;
	SGD::Color startColor;
	SGD::Color endColor;

	float currentSize;
	float startSize;
	float endSize;

	float currentRotation;
	float rotationsSecondStart;
	float rotationsSecondEnd;

	void Update(float elapsedTime);
};

//Helper functions
float Lerp(float begin, float end, float scale);
unsigned char Lerp(unsigned char begin, unsigned char end, float scale);
SGD::Vector Lerp(SGD::Vector begin, SGD::Vector end, float scale);
SGD::Color Lerp(SGD::Color begin, SGD::Color end, float scale);
