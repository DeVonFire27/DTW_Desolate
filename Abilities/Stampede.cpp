#include "globals.h"
#include "Stampede.h"
#include "../GameplayState.h"
#include "../Animation System/AnimationSystem.h"
#include "../Agents/NPC.h"
#include "../Agents/Player.h"

#include "../Particle System/Emitter.h"
#include "../Agents/NPC.h"
#include "../Tile System/World.h"

Stampede::Stampede()
{
	action.SetCurrAnimation("Radicorn");
	m_szSize = SGD::Size(64, 64);
}


Stampede::~Stampede()
{
	
}

void Stampede::Activate()
{
	AnimationSystem::GetInstance()->ResetAnimation(action);
	trail = GameplayState::GetInstance()->CreateEmitter("radicorn_mane");
	duration = 3.0f;
	firstHit = false;

	SGD::AudioManager::GetInstance()->PlayAudio(GameplayState::GetInstance()->stampede_sfx, true);

	damage = owner->GetStat(StatType::accuracy) + 35;

	SGD::Point spawnPoint;
	if (fromTheLeft)
	{
		spawnPoint.y = owner->GetPosition().y - 900;
		m_vtVelocity.y = 1200;
	}
	else
	{
		spawnPoint.y = owner->GetPosition().y + 900;
		m_vtVelocity.y = -1200;
		trail->SetRotation(PI);
	}


	NPC* temp = FindControllingNPC(owner);
	spawnPoint.x = temp->GetTarget()->GetPosition().x;

	this->SetPosition(spawnPoint);


}

void Stampede::Update(float dt)
{
	duration -= dt;
	if (duration < 0 || owner->GetCurrHealth() < 0)
	{
		SGD::AudioManager::GetInstance()->StopAudio(GameplayState::GetInstance()->stampede_sfx);

		//delete ambulance message
		DestroyEntityMsg* die = new DestroyEntityMsg(this);
		die->QueueMessage();

		DestroyEntityMsg* msg = new DestroyEntityMsg(trail);
		SGD::MessageManager::GetInstance()->QueueMessage(msg);
	}
	if (slowTimer > 0)
		slowTimer -= dt;
	else
		GameplayState::GetInstance()->SetTimeStep(1.0f);


	AnimationSystem::GetInstance()->Update(action, dt);


	collide = AnimationSystem::GetInstance()->CollisionRect(action, m_ptPosition);

	Entity::Update(dt);
}

void Stampede::HandleSpecial()
{
	if (!firstHit)
	{
		firstHit = true;
		GameplayState::GetInstance()->SetTimeStep(0.05f);
		slowTimer = 0.1f;

		//drop blood particles when someone loses health
		Emitter *e = GameplayState::GetInstance()->CreateEmitter("bloodEffect");
		e->SetPosition(this->GetPosition());
		e->SetFollowTarget(this);
		e->SetParticleSize(1.0f, 5.0f);
	}
}

void Stampede::Render(void)
{
	//SGD::GraphicsManager::GetInstance()->DrawRectangle(Entity::GetRect(), SGD::Color(255, 0, 0));
	
	if (fromTheLeft)
		AnimationSystem::GetInstance()->Render(action, m_ptPosition);
	else
		AnimationSystem::GetInstance()->Render(action, m_ptPosition, PI);

}

void Stampede::HandleCollision(const IEntity* other)
{
	const Character* hit = reinterpret_cast<const Character*>(other);
}

SGD::Rectangle Stampede::GetRect(void) const
{
	SGD::Rectangle camera = GameplayState::GetInstance()->GetWorld()->GetCamera()->GetRect();
	if (m_ptPosition.IsWithinRectangle(camera))
		return collide;

	return SGD::Rectangle{};
}