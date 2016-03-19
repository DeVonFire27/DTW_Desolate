#include "globals.h"
#include "Emitter.h"
#include "Particle.h"
#include "../../SGD Wrappers/SGD_GraphicsManager.h"
#include <cassert>

#define RANDMODIFIER 1000
#define PARTICLECHUNKS 100

/**************************************************************/
// Setup
//	- Set up emitter variables
Emitter::Emitter(SHAPE shape, float lifeTime, float spawnRate, float spawnRateRand, float particleLife, float particleLifeRand, float radius, float angle, float rotation, float rotationRand, bool running)
{
	this->shape = shape;
	this->running = running;
	this->lifeTime = lifeTime;
	this->spawnRate = spawnRate;
	this->spawnRateRand = spawnRateRand;
	this->particleLifeTime = particleLife;
	this->particleLifeTimeRand = particleLifeRand;
	this->radius = radius;
	this->angle = angle;
	this->m_fRotation = rotation;
	this->rotationRand = rotationRand;
}
void Emitter::SetParticleRotation(float start, float end, float startRand, float endRand)
{
	this->particleRotationsSecondStart = start;
	this->particleRotationsSecondEnd = end;
	this->particleRotationsSecondStartRand = startRand;
	this->particleRotationsSecondEndRand = endRand;
}
void Emitter::SetParticleSize(float start, float end, float startRand, float endRand)
{
	this->particleStartSize = start;
	this->particleEndSize = end;
	this->particleStartSizeRand = startRand;
	this->particleEndSizeRand = endRand;
}
void Emitter::SetParticleColor(SGD::Color start, SGD::Color end, float startRand, float endRand)
{
	this->particleStartColor = start;
	this->particleEndColor = end;
	this->particleStartColorRand = startRand;
	this->particleEndColorRand = endRand;
}
void Emitter::SetParticleVelocity(float start, float end, float startRand, float endRand)
{
	this->particleStartVelocity = start;
	this->particleEndVelocity = end;
	this->particleStartVelocityRand = startRand;
	this->particleEndVelocityRand = endRand;
}

/**************************************************************/
// Update
//	- move the entity's position by its velocity
/*virtual*/ void Emitter::Update(float elapsedTime)
{
	// if we're not active, GTFO
	if (!active) return;

	//If too far away then dont update
	if (GameplayState::GetInstance()->player && (m_ptPosition - GameplayState::GetInstance()->player->GetPosition()).ComputeLength() > Game::GetInstance()->GetScreenWidth() * 2) return;

	//Update emitter variables
	m_ptPosition += m_vtVelocity * elapsedTime;
	sinceSpawn += elapsedTime;
	sinceEmitterSpawn += elapsedTime;
	if (sinceEmitterSpawn > lifeTime && lifeTime >= 0.f) running = false;

	//Update pos to follow
	if (follow)
	{
		SGD::Vector change = follow->GetPosition() - lastPos;
		m_ptPosition += change;
		lastPos = follow->GetPosition();
	}

	//Spawn new?
	if (sinceSpawn > spawnRate && ( maxParticles < 0 || particles.size() < (unsigned int)maxParticles ) && running)
	{
		//Set new spawn rate using rand
		spawnRate = Randomizer(spawnRate, spawnRateRand);
		sinceSpawn = 0.0f;

		//Make sure there are some dead ones to spawn in
		if (deadParticles.size() <= 0)
		{
			deadParticles.reserve(PARTICLECHUNKS);

			for (unsigned int i = 0; i < PARTICLECHUNKS; i++)
			{
				deadParticles.push_back(new Particle);
				//OutputDebugStringA((std::to_string(deadParticles.size()) + "\n").c_str());
			}
		}

		//Ressurect 1 particle
		Particle *newParticle = deadParticles.back();
		deadParticles.pop_back();
		particles.push_back(newParticle);
		newParticle->sinceSpawn = 0.0f;

		switch (shape)
		{
		default:
		case Emitter::RADIAL:
			{
				//Set up the spawn angle
				float rot = RandomizeBased(angle / 2.0f) + m_fRotation;

				//Set up spawn position
				SGD::Vector direction = SGD::Vector{ 0.0f, -1.0f };
				direction.Rotate(rot);
				direction *= Randomizer(radius / 2.0f, 1.0f);
				newParticle->position = { m_ptPosition.x + direction.x, m_ptPosition.y + direction.y };

				newParticle->direction = rot;
			}
			break;
		case Emitter::RECT:
			{
				//Position is center of RECT
				int minX = int(m_ptPosition.x - radius / 2);
				int minY = int(m_ptPosition.y - angle / 2);
				int maxX = int(m_ptPosition.x + radius / 2);
				int maxY = int(m_ptPosition.y + angle / 2);


				int diffX = maxX - minX;
				int diffY = maxY - minY;

				if (diffX == 0)
					diffX = 1;
				if (diffY == 0)
					diffY = 1;

				newParticle->position = {
					(float)minX + rand() % diffX,
					(float)minY + rand() % diffY
				};

				//Start direction away from center
				SGD::Vector offset = { 
					newParticle->position.x - m_ptPosition.x, 
					newParticle->position.y - m_ptPosition.y 
				};

				newParticle->direction = SGD::Vector{ 0.0f, 1.0f }.ComputeAngle(offset.ComputeNormalized());
			}
			break;
		case Emitter::LINE:
			{
				assert(radius != 0.0f && "Radius is the length of the spawn section.");

				//Radius is how long the line is.  Angle is slope of line
				int length = rand() % (int)radius;
				SGD::Vector offset = { 0.f, 1.f };
				offset.Rotate(angle);
				offset *= (float)length;

				newParticle->position = {
					m_ptPosition.x + offset.x,
					m_ptPosition.y + offset.y
				};

				//Direction is tangent of line (raining)
				newParticle->direction = angle + 3 * (SGD::PI / 2);
			}
			break;
		}

		//Set up other variables
		newParticle->lifetime = Randomizer(particleLifeTime, particleLifeTimeRand);

		newParticle->startVelocity = Randomizer(particleStartVelocity, particleStartVelocityRand);
		newParticle->endVelocity = Randomizer(particleEndVelocity, particleEndVelocityRand);

		newParticle->startColor = Randomizer(particleStartColor, particleStartColorRand);
		newParticle->endColor = Randomizer(particleEndColor, particleEndColorRand);
		newParticle->currentColor = newParticle->startColor;

		newParticle->rotationsSecondStart = Randomizer(particleRotationsSecondStart, particleRotationsSecondStartRand)*Randomizer(.001f, 800.0f);
		newParticle->rotationsSecondEnd = Randomizer(particleRotationsSecondEnd, particleRotationsSecondEndRand)*Randomizer(.001f, 800.0f);
		newParticle->currentRotation = newParticle->rotationsSecondStart;

		newParticle->startSize = Randomizer(particleStartSize, particleStartSizeRand);
		newParticle->endSize = Randomizer(particleEndSize, particleEndSizeRand);
		newParticle->currentSize = newParticle->startSize;
	}

	//Update particles
	for (unsigned int i = 0; i < particles.size(); i++)
	{
		//Update it
		particles[i]->Update(elapsedTime);

		//Kill it if dead.
		if (particles[i]->sinceSpawn > particles[i]->lifetime)
		{
			deadParticles.push_back(particles[i]);
			particles.erase(particles.begin() + i);
			i--;
			continue;
		}

	}
}


/**************************************************************/
// Render
//	- draw the entity's image at its position
/*virtual*/ void Emitter::Render(void)
{
	if (!active) return;

	//If too far away then dont render
	if (GameplayState::GetInstance()->player && (m_ptPosition - GameplayState::GetInstance()->player->GetPosition()).ComputeLength() > Game::GetInstance()->GetScreenWidth()) return;

	SGD::GraphicsManager *graphics = SGD::GraphicsManager::GetInstance();

	for (unsigned int i = 0; i < particles.size(); i++)
	{
		if (particles[i]->sinceSpawn > particles[i]->lifetime) continue;

		//Draw all particles according to specification
		graphics->DrawTexture(
			m_hImage,
			{ particles[i]->position.x - ((m_szSize.width*particles[i]->currentSize) / 2), particles[i]->position.y - ((m_szSize.height*particles[i]->currentSize) / 2) },
			particles[i]->currentRotation,
			{ m_szSize.width / 2, m_szSize.height / 2 },
			particles[i]->currentColor,
			{ particles[i]->currentSize, particles[i]->currentSize }
		);
	}
}


Emitter::Emitter(const Emitter& other)
{
	active = other.active;
	running = other.running;

	particles.clear();
	deadParticles.clear();
	maxParticles = other.maxParticles; //Negative means no max.

	//Follow an entity
	follow = nullptr;

	//How long the emitter sticks around
	lifeTime = other.lifeTime;
	sinceEmitterSpawn = 0.0f;

	//Spawn rate
	spawnRate = other.spawnRate;
	spawnRateRand = other.spawnRateRand;
	sinceSpawn = 100.0f;

	//Particle lives
	particleLifeTime = other.particleLifeTime;
	particleLifeTimeRand = other.particleLifeTimeRand;

	//Where to spawn them
	shape = other.shape;
	radius = other.radius;
	angle = other.angle;
	rotationRand = other.rotationRand;

	//All other variables
	particleStartVelocity = other.particleStartVelocity;
	particleStartVelocityRand = other.particleStartVelocityRand;
	particleEndVelocity = other.particleEndVelocity;
	particleEndVelocityRand = other.particleEndVelocityRand;

	particleStartColor = other.particleStartColor;
	particleStartColorRand = other.particleStartColorRand;
	particleEndColor = other.particleEndColor;
	particleEndColorRand = other.particleEndColorRand;

	particleStartSize = other.particleStartSize;
	particleStartSizeRand = other.particleStartSizeRand;
	particleEndSize = other.particleEndSize;
	particleEndSizeRand = other.particleEndSizeRand;

	particleRotationsSecondStart = other.particleRotationsSecondStart;
	particleRotationsSecondStartRand = other.particleRotationsSecondStartRand;
	particleRotationsSecondEnd = other.particleRotationsSecondEnd;
	particleRotationsSecondEndRand = other.particleRotationsSecondEndRand;

	texFilename = other.texFilename;
	m_hImage = SGD::GraphicsManager::GetInstance()->LoadTexture(other.texFilename.c_str());
	m_szSize = other.m_szSize;

	m_unRefCount = 1;	// calling 'new' gives the prime reference

	*this = other;
}

/**************************************************************/
// Assignment operator
//	
Emitter& Emitter::operator =(const Emitter& other)
{
	if (&other != this)
	{
		active = other.active;
		running = other.running;

		particles.clear();
		deadParticles.clear();
		maxParticles = other.maxParticles; //Negative means no max.

		//Follow an entity
		follow = nullptr;

		//How long the emitter sticks around
		lifeTime = other.lifeTime;
		sinceEmitterSpawn = 0.0f;

		//Spawn rate
		spawnRate = other.spawnRate;
		spawnRateRand = other.spawnRateRand;
		sinceSpawn = 100.0f;

		//Particle lives
		particleLifeTime = other.particleLifeTime;
		particleLifeTimeRand = other.particleLifeTimeRand;

		//Where to spawn them
		shape = other.shape;
		radius = other.radius;
		angle = other.angle;
		rotationRand = other.rotationRand;

		//All other variables
		particleStartVelocity = other.particleStartVelocity;
		particleStartVelocityRand = other.particleStartVelocityRand;
		particleEndVelocity = other.particleEndVelocity;
		particleEndVelocityRand = other.particleEndVelocityRand;

		particleStartColor = other.particleStartColor;
		particleStartColorRand = other.particleStartColorRand;
		particleEndColor = other.particleEndColor;
		particleEndColorRand = other.particleEndColorRand;

		particleStartSize = other.particleStartSize;
		particleStartSizeRand = other.particleStartSizeRand;
		particleEndSize = other.particleEndSize;
		particleEndSizeRand = other.particleEndSizeRand;

		particleRotationsSecondStart = other.particleRotationsSecondStart;
		particleRotationsSecondStartRand = other.particleRotationsSecondStartRand;
		particleRotationsSecondEnd = other.particleRotationsSecondEnd;
		particleRotationsSecondEndRand = other.particleRotationsSecondEndRand;

		texFilename = other.texFilename;
		m_hImage = SGD::GraphicsManager::GetInstance()->LoadTexture(other.texFilename.c_str());
		m_szSize = other.m_szSize;

		m_unRefCount = 1;	// calling 'new' gives the prime reference
	}

	return *this;
}
/**************************************************************/
// SetFollowTarget
//	- Sets an Entity to follow
void Emitter::SetFollowTarget(Entity *follow)
{
	if (follow) follow->AddRef();
	if (this->follow) this->follow->Release();
	this->follow = follow;
	
	if(follow) lastPos = follow->GetPosition();
}

/**************************************************************/
// Randomizer
//	- randomizes a value by a percentage
float RandomizeBased(float max)
{
	float minValue = max*-1;
	float maxValue = max;

	if (minValue >= maxValue) return max;
	float random = ((float)rand()) / (float)RAND_MAX;

	return (random*(maxValue - minValue)) + minValue;
}
float Randomizer(float value, float percentage)
{
	if (percentage < 0.0001f) return value;

	float minValue = value - percentage*value;
	float maxValue = value + percentage*value;

	if (minValue >= maxValue) return value;
	float random = ((float)rand()) / (float)RAND_MAX;

	return (random*(maxValue - minValue)) + minValue;
}
unsigned char Randomizer(unsigned char value, float percentage)
{
	if (percentage < 0.0001f) return value;

	float minValue = value - percentage*value;
	float maxValue = value + percentage*value;

	if (minValue >= maxValue) return value;
	float random = ((float)rand()) / (float)RAND_MAX;

	return (unsigned char)((random*(maxValue - minValue)) + minValue);
}
SGD::Vector Randomizer(SGD::Vector value, float percentage)
{
	SGD::Vector newVal;
	newVal.x = Randomizer(value.x, percentage);
	newVal.y = Randomizer(value.y, percentage);

	return newVal;
}
SGD::Color Randomizer(SGD::Color value, float percentage)
{
	value.red = Randomizer(value.red, percentage);
	value.blue = Randomizer(value.blue, percentage);
	value.green = Randomizer(value.green, percentage);
	value.alpha = Randomizer(value.alpha, percentage);

	return value;
}


/**************************************************************/
// GetRect
//	- calculate the entity's bounding rectangle
/*virtual*/ SGD::Rectangle Emitter::GetRect(void) const
{
	//Make it not exist
	return SGD::Rectangle{ 0.0f, 0.0f, 0.0f, 0.0f };
}


/**************************************************************/
// HandleCollision
//	- respond to collision between entities
/*virtual*/ void Emitter::HandleCollision(const IEntity* pOther)
{
	/* DO NOTHING */
	(void)pOther;		// unused parameter
}

/**************************************************************/
// Destructor
//	- Cleans up particles.
Emitter::~Emitter()
{
	for (unsigned int i = 0; i < particles.size(); i++)
	{
		delete particles[i];
	}

	for (unsigned int i = 0; i < deadParticles.size(); i++)
	{
		delete deadParticles[i];
	}
	
	SGD::GraphicsManager::GetInstance()->UnloadTexture(GetImage());
}


void Emitter::SetImage(const char* filename)
{
	if (m_hImage != SGD::INVALID_HANDLE)
		SGD::GraphicsManager::GetInstance()->UnloadTexture(m_hImage);
	m_hImage = SGD::GraphicsManager::GetInstance()->LoadTexture(filename);
	texFilename = filename;
}
