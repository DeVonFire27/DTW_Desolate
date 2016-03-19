#pragma once
#include <map>
#include "Animation.h"
#include "AnimationTimeStamp.h"
#include "../../SGD Wrappers/SGD_Geometry.h"
#include "../../SGD Wrappers/SGD_Color.h"
#include "../TinyXML/tinyxml.h"

class AnimationSystem
{
	std::map<std::string, Animation> animations;

	/**********************************************************/
	// SINGLETON (not-dynamically allocated)
	AnimationSystem(void) = default;	// default constructor
	virtual ~AnimationSystem(void) = default;	// destructor

	AnimationSystem(const AnimationSystem&) = delete;	// copy constructor
	AnimationSystem& operator= (const AnimationSystem&) = delete;	// assignment operator

public:
	/***************************************************5*******/
	// Singleton Accessor:
	static AnimationSystem* GetInstance(void);

	void Render(AnimationTimeStamp& aniTS, SGD::Point renderPoint, float rotate = 0.0f, SGD::Color col = {}, SGD::Size scale = {1.0f, 1.0f});
	void Update(AnimationTimeStamp& aniTS, float deltaTime);

	SGD::Rectangle CollisionRect(AnimationTimeStamp& aniTS, SGD::Point position, float rotate = 0.0f);
	SGD::Rectangle ActiveRect(AnimationTimeStamp& aniTS, SGD::Point position, float rotate = 0.0f);
	SGD::Point GetWeaponPoint(AnimationTimeStamp& aniTS, SGD::Point position, float rotate = 0.0f);
	SGD::Point GetAnchorPoint(AnimationTimeStamp& aniTS, SGD::Point position, float rotate = 0.0f);

	void AddAnimation(std::string name, Animation& anim);
	void ResetAnimation(AnimationTimeStamp& aniTS);
	bool LoadAnimation(const char* szFilename);

	void UnloadAnimation();
	bool UnloadSingleAnimation(AnimationTimeStamp& aniTS);
};
