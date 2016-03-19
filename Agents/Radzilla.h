#pragma once 

#include "Razard.h"

class Radzilla : public Razard
{
public:
	Radzilla();
	~Radzilla();
	void UseAbility() override;
	void Update(float dt);
	void Render() override;
	virtual MutantType GetMutantType() { return MutantType::RADZILLA; }
	void HandlePassive() override;
	float activeLifetime = 6.0f;
	bool isUsingActive = false;
};