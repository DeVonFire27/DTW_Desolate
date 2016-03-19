#pragma once
#include "../Entity.h"
//#include "Particle.h"
#include "../../SGD Wrappers/SGD_Handle.h"
#include "../../SGD Wrappers/SGD_Color.h"
#include <vector>

class Particle;

class Emitter : public Entity
{
public:
	enum SHAPE { RADIAL, RECT, LINE, }; //Radial with 0 radius is point

	//When set to false Emitter will no longer spawn particles and should be removed
	//if all particles gone.
	bool active = true;
	bool running = true;

	std::vector<Particle *> particles;
	std::vector<Particle *> deadParticles;
	int maxParticles = -1; //Negative means no max.

	//Follow an entity
	Entity *follow = nullptr;
	SGD::Point lastPos;

	//How long the emitter sticks around
	float lifeTime;
	float sinceEmitterSpawn = 100.0f;

	//Spawn rate
	float spawnRate;
	float spawnRateRand;
	float sinceSpawn = 0.0f;

	//Particle lives
	float particleLifeTime;
	float particleLifeTimeRand;

	//Where to spawn them
	SHAPE shape;
	float radius;
	float angle;
	float rotationRand;

	//All other variables
	float particleStartVelocity;
	float particleStartVelocityRand;
	float particleEndVelocity;
	float particleEndVelocityRand;

	SGD::Color particleStartColor;
	float particleStartColorRand;
	SGD::Color particleEndColor;
	float particleEndColorRand;

	float particleStartSize;
	float particleStartSizeRand;
	float particleEndSize;
	float particleEndSizeRand;

	float particleRotationsSecondStart;
	float particleRotationsSecondStartRand;
	float particleRotationsSecondEnd;
	float particleRotationsSecondEndRand;

	std::string texFilename;

	unsigned int m_unRefCount = 1;	// calling 'new' gives the prime reference
public:
	/**********************************************************/
	// Interface:
	//	- virtual functions for children classes to override
	virtual void Update(float elapsedTime) override;
	virtual void Render(void) override;

	Emitter(const Emitter&);
	~Emitter();
	Emitter& operator =(const Emitter&);

	virtual SGD::Rectangle GetRect(void) const override;
	virtual int	GetType(void) const override	{ return ENT_EMITTER; }
	virtual void HandleCollision(const IEntity* pOther)	override;

	Emitter() = default;
	Emitter(SHAPE shape, float lifetime, float spawnRate, float spawnRateRand, float particleLife, float particleLifeRand, float radius, float angle, float rotation, float rotationRand, bool running = true);
	void SetParticleRotation(float start, float end, float startRand = 0.0f, float endRand = 0.0f);
	void SetParticleSize(float start, float end, float startRand = 0.0f, float endRand = 0.0f);
	void SetParticleColor(SGD::Color start, SGD::Color end, float startRand = 0.0f, float endRand = 0.0f);
	void SetParticleVelocity(float start, float end, float startRand = 0.0f, float endRand = 0.0f);
	void SetFollowTarget(Entity *follow);
	void SetRunning(bool value) { running = value; }
	void SetImage(const char* filename);
};

//Helper functions
float RandomizeBased(float max);
float Randomizer(float value, float percentage);
SGD::Vector Randomizer(SGD::Vector value, float percentage);
SGD::Color Randomizer(SGD::Color value, float percentage);
unsigned char Randomizer(unsigned char value, float percentage);

