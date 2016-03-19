#include "globals.h"
#include "Laser.h"
#include "../GameplayState.h"
#include "../Messages/DestroyEntityMsg.h"
#include "../Animation System/AnimationSystem.h"

Laser::Laser(void)
{
	action.SetCurrAnimation("Laser");
	laser1 = SGD::AudioManager::GetInstance()->LoadAudio("resources/audio/Sound Effects/Laser_Lvl1.wav");
	laser2 = SGD::AudioManager::GetInstance()->LoadAudio("resources/audio/Sound Effects/Laser_Lvl3.wav");

}

Laser::~Laser(void)
{
	SGD::AudioManager::GetInstance()->UnloadAudio(laser1);
	SGD::AudioManager::GetInstance()->UnloadAudio(laser2);
}

void Laser::Activate()
{
	AnimationSystem::GetInstance()->ResetAnimation(action);
	duration = 4.0f;

	GameplayState::GetInstance()->GetWorld()->GetCamera()->CameraShake(5, 3.5f);

	damage = owner->GetStat(StatType::accuracy)*0.075f;

	if (owner->GetLevel() >= 21)
	{
		SkillLvl = 3;
		action.SetCurrAnimation("BigBlast");
		beamColor = SGD::Color { 255, 0, 255 };
		GameplayState::GetInstance()->GetWorld()->GetCamera()->CameraShake(15, 3.5f);
	}
	else if (owner->GetLevel() >= 11)
	{
		SkillLvl = 2;
		beamColor = SGD::Color { 0, 255, 0 };
		GameplayState::GetInstance()->GetWorld()->GetCamera()->CameraShake(10, 3.5f);
	}
	

	if (SkillLvl < 3)
		SGD::AudioManager::GetInstance()->PlayAudio(laser1);
	else
		SGD::AudioManager::GetInstance()->PlayAudio(laser2);
}

void Laser::Update(float dt)
{
	duration -= dt;
	if (duration < 0 || owner->GetCurrHealth() < 0)
	{

		if (SkillLvl < 3)
			SGD::AudioManager::GetInstance()->StopAudio(laser1);
		else
			SGD::AudioManager::GetInstance()->StopAudio(laser2);

		//delete ambulance message
		DestroyEntityMsg* die = new DestroyEntityMsg(this);
		die->QueueMessage();
	}
	m_ptPosition = owner->GetPosition();
	m_ptPosition.x += owner->GetSize().width*0.5f;
	if (SkillLvl != 3)
		m_fRotation = owner->GetRotation() - (PI*0.5f);
	else
		m_fRotation = owner->GetRotation();

	AnimationSystem::GetInstance()->Update(action, dt);
	Entity::Update(dt);

	CreateLaserBullet* msg = new CreateLaserBullet(this);
	msg->QueueMessage();
}

void Laser::Render(void)
{
	SGD::Point anchor = AnimationSystem::GetInstance()->GetAnchorPoint(owner->GetAnimation(), m_ptPosition);
	SGD::Vector temp = (anchor - m_ptPosition);
	temp.Rotate(m_fRotation);
	temp.x = (anchor.x + temp.x);
	temp.y = (anchor.y + temp.y);
	AnimationSystem::GetInstance()->Render(action, m_ptPosition, m_fRotation, beamColor);
}

void Laser::HandleCollision(const IEntity* other)
{
	const Character* hit = reinterpret_cast<const Character*>(other);

}

SGD::Rectangle Laser::GetRect(void) const
{
	SGD::Rectangle newRect = SGD::Rectangle();

	return newRect;
}