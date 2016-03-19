#include "globals.h"
#include "BulletStorm.h"
#include "../GameplayState.h"
#include "../Animation System/AnimationSystem.h"

BulletStorm::BulletStorm()
{
	storm = SGD::AudioManager::GetInstance()->LoadAudio("resources/audio/Sound Effects/Storm_Lvl2.wav");
	m_hImage = SGD::GraphicsManager::GetInstance()->LoadTexture("resources/graphics/whirlwind.png");
}


BulletStorm::~BulletStorm()
{
	SGD::AudioManager::GetInstance()->UnloadAudio(storm);
	SGD::GraphicsManager::GetInstance()->UnloadTexture(m_hImage);
}

void BulletStorm::Activate()
{
	duration = 2.5f;
	m_ptPosition = owner->GetPosition();
	m_ptPosition.x -= 36;
	m_ptPosition.y -= 40;

	if (owner->GetLevel() >= 11)
		SkillLvl = 2;
	if (owner->GetLevel() >= 21)
		SkillLvl = 3;

	if (SkillLvl != 1)
		SGD::AudioManager::GetInstance()->PlayAudio(storm, true);
}

void BulletStorm::Update(float dt)
{
	duration -= dt;
	if (duration < 0 || owner->GetCurrHealth() < 0)
	{
		owner->GetWeapon()->ToggleGunActive();
		if (SkillLvl != 1)
			SGD::AudioManager::GetInstance()->StopAudio(storm);
		//delete ambulance message
		DestroyEntityMsg* die = new DestroyEntityMsg(this);
		die->QueueMessage();
	}

	m_fRotation += 0.75f;

	if (SkillLvl >= 2)
		owner->AOEPull(owner->GetPosition(), 400, 3.0f, 0.4f);

	Entity::Update(dt);
}

void BulletStorm::Render(void)
{
	if (SkillLvl > 1)
	{
		SGD::Point temp = SGD::Point(128,128);
		SGD::GraphicsManager::GetInstance()->DrawTexture(m_hImage, m_ptPosition, m_fRotation, SGD::Vector{ temp.x, temp.y }, SGD::Color{200, 255, 255,255}, SGD::Size{ 0.5f, 0.5f });
	}
}

void BulletStorm::HandleCollision(const IEntity* other)
{

}

SGD::Rectangle BulletStorm::GetRect(void) const
{
	return collide;
}