#include "globals.h"
#include "Ability.h"
#include "../Animation System/AnimationSystem.h"

Ability::Ability()
{

}

Ability::~Ability()
{
	SetOwner(nullptr);
}

void Ability::Activate()
{
}

void Ability::Update(float dt)
{

}

void Ability::Render()
{

}

SGD::Rectangle Ability::GetRect() const
{
	return SGD::Rectangle{};
}

void Ability::HandleCollision(const IEntity* other)
{

}

bool Ability::GetIsFoe() const
{
	return owner->GetIsEnemy();
}

void Ability::SetOwner(Character* own)
{
	if (owner != nullptr)
	{
		owner->Release();
		owner = nullptr;
	}
	if (own)
	{
		owner = own;
		owner->AddRef();
	}
}
