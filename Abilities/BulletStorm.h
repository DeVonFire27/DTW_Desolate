#pragma once
#include "Ability.h"
#include "../Weapons/Weapon.h"

class BulletStorm : public Ability
{

private:
	float buffFireRate = 1.0f;
	SGD::Rectangle collide = SGD::Rectangle{};
	SGD::HAudio storm = SGD::INVALID_HANDLE;
public:
	BulletStorm(void);
	virtual ~BulletStorm(void);

	/////////////////////////<Interface>//////////////////////////////////////////////////

	virtual void Activate() final;
	virtual void Update(float dt) final;
	virtual void Render(void) final;
	virtual void HandleCollision(const IEntity* other) final;

	virtual SGD::Rectangle GetRect(void) const final;
};

