#include "globals.h"
#include "Particle.h"

void Particle::Update(float elapsedTime)
{
	sinceSpawn += elapsedTime;
	if (lifetime == 0.0f) return; //Divide by zero bad.

	float scale = sinceSpawn / lifetime;

	//Set up the velocity
	SGD::Vector currentVelocity = { 0.f, 1.f };
	currentVelocity.Rotate(direction);
	currentVelocity *= Lerp(startVelocity, endVelocity, scale);

	//Update current values
	position += currentVelocity*elapsedTime;
	currentColor = Lerp(startColor, endColor, scale);
	currentSize = Lerp(startSize, endSize, scale);
	currentRotation += Lerp(rotationsSecondStart, rotationsSecondEnd, scale)*elapsedTime;
}

//Helper functions
float Lerp(float begin, float end, float scale)
{
	return begin + (end - begin)*scale;
}
unsigned char Lerp(unsigned char begin, unsigned char end, float scale)
{
	return begin + unsigned char((end - begin)*scale);
}
SGD::Vector Lerp(SGD::Vector begin, SGD::Vector end, float scale)
{
	return begin + (end - begin)*scale;
}
SGD::Color Lerp(SGD::Color begin, SGD::Color end, float scale)
{
	SGD::Color newColor;
	newColor.red = Lerp(begin.red, end.red, scale);
	newColor.green = Lerp(begin.green, end.green, scale);
	newColor.blue = Lerp(begin.blue, end.blue, scale);
	newColor.alpha = Lerp(begin.alpha, end.alpha, scale);

	return newColor;
}
