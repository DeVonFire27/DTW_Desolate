#include "globals.h"

#include "Mortar.h"
#include "../Agents/NPC.h"
#include "../GameplayState.h"
#include "../Messages/DestroyEntityMsg.h"
#include "../../SGD Wrappers/SGD_InputManager.h"
#include "../Animation System/AnimationSystem.h"

Mortar::Mortar()
{
	action.SetCurrAnimation("MortarShot");
	radPool.SetCurrAnimation("Radiation");
}


Mortar::~Mortar()
{
}

void Mortar::Activate()
{
	AnimationSystem::GetInstance()->ResetAnimation(action);
	AnimationSystem::GetInstance()->ResetAnimation(radPool);

	duration = 6.5f;
	damage = 255.0f;
	dropTimer = 2.0f;
	hasHit = play = false;
	m_ptPosition = FindControllingNPC(owner)->GetTarget()->GetPosition();
	m_szSize = SGD::Size{ 20, 20 };
}

void Mortar::Update(float dt)
{
	duration -= dt;
	if (dropTimer > 0)
		dropTimer -= dt;

	if (!action.HasEnded())
		AnimationSystem::GetInstance()->Update(action, dt);

	if (!hasHit && dropTimer < 0.5f && dropTimer > 0.0f)
		m_szSize = SGD::Size{ 30, 30 };
	else if (dropTimer < 0)
	{
		m_szSize = SGD::Size{ 60, 60 };
		AnimationSystem::GetInstance()->Update(radPool, dt);

		if (!play)
		{
			SGD::AudioManager::GetInstance()->PlayAudio(GameplayState::GetInstance()->mortar_sfx);
			play = true;
		}
	}
	if (radPool.HasEnded() || owner == nullptr)
	{
		//delete ambulance message
		DestroyEntityMsg* die = new DestroyEntityMsg(this);
		die->QueueMessage();
	}
}

void Mortar::Render(void)
{
	//eventually animation
	//SGD::Rectangle rect;
	//rect.top = m_ptPosition.y;
	//rect.left = m_ptPosition.x;
	if (dropTimer < 0)
	{
		//rect.Resize(SGD::Size(60, 60));
		//GraphicsManager::GetInstance()->DrawRectangle(rect, SGD::Color(0, 255, 0));
		AnimationSystem::GetInstance()->Render(radPool, m_ptPosition, 0, SGD::Color{ 0, 255, 0 });
		collide = AnimationSystem::GetInstance()->ActiveRect(radPool, m_ptPosition);
	}
	else
		collide = AnimationSystem::GetInstance()->ActiveRect(action, m_ptPosition);
	//else if (!hasHit && dropTimer < 0.5f && dropTimer > 0.0f)
	//{
	//	rect.Resize(SGD::Size(30, 30));
	//	GraphicsManager::GetInstance()->DrawRectangle(rect, SGD::Color(255, 0, 0));
	//}
	//else
	//{
	//	rect.Resize(SGD::Size(20, 20));
	//	GraphicsManager::GetInstance()->DrawRectangle(rect, SGD::Color(255, 0, 255));
	//}

	if (!action.HasEnded())
		AnimationSystem::GetInstance()->Render(action, m_ptPosition);
}

SGD::Rectangle Mortar::GetRect(void) const
{
	//SGD::Rectangle rect;
	//rect.top = m_ptPosition.y;
	//rect.left = m_ptPosition.x;
	//rect.Resize(SGD::Size(20, 20));
	//return rect;

	return collide;
}

void Mortar::HandleCollision(const IEntity* other)
{
	const Character* hit = reinterpret_cast<const Character*>(other);

}