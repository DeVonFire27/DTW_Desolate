#include "globals.h"
#include "Ambulance.h"
#include "../GameplayState.h"
#include "../Animation System/AnimationSystem.h"
#include "../Agents/NPC.h"
#include "../Agents/Player.h"

#include "../Particle System/Emitter.h"
#include "../Status Effects/Burning.h"
#include <vector>
#include "../Agents/NPC.h"
#include "../Tile System/World.h"

Ambulance::Ambulance(void)
{
	action.SetCurrAnimation("Hit_Run");
	m_vtVelocity.x = -1800;
	m_szSize = SGD::Size(64, 64);
	siren = SGD::AudioManager::GetInstance()->LoadAudio("resources/audio/Sound Effects/Ambul_sfx.wav");

}

Ambulance::~Ambulance(void)
{
	SGD::AudioManager::GetInstance()->UnloadAudio(siren);
}


void Ambulance::Activate()
{
	AnimationSystem::GetInstance()->ResetAnimation(action);
	duration = 3.0f;
	hitFirst = false;

	damage = owner->GetStat(StatType::accuracy) + 55;

	SGD::Point spawnPoint;
	spawnPoint.x = owner->GetPosition().x + 900;

	if (owner == GameplayState::GetInstance()->player->GetCharacter())
		spawnPoint.y = -GameplayState::GetInstance()->GetWorld()->GetCamera()->GetPosition().y + InputManager::GetInstance()->GetMousePosition().y;
	else
	{
		NPC* temp = FindControllingNPC(owner);
		if (temp && temp->GetTarget() == nullptr)
			spawnPoint.y = -GameplayState::GetInstance()->GetWorld()->GetCamera()->GetPosition().y + InputManager::GetInstance()->GetMousePosition().y;
		else
			spawnPoint.y = temp->GetTarget()->GetPosition().y;
	}

	this->SetPosition(spawnPoint);
	if (owner->GetLevel() >= 21)
	{
		SkillLvl = 3;
		action.SetCurrAnimation("PhoenixDown");
		trail = GameplayState::GetInstance()->CreateEmitter("pheonix_Down");
		trail->AddRef();
	}
	else if (owner->GetLevel() >= 11)
	{
		SkillLvl = 2;
		action.SetCurrAnimation("Driveby");
		trail = GameplayState::GetInstance()->CreateEmitter("medical_driveby");
		trail->AddRef();
	}


	SGD::AudioManager::GetInstance()->PlayAudio(siren, true);
}

void Ambulance::Update(float dt)
{
	duration -= dt;
	if (duration < 0 || owner->GetCurrHealth() < 0)
	{
		SGD::AudioManager::GetInstance()->StopAudio(siren);
		//delete ambulance message
		DestroyEntityMsg* die = new DestroyEntityMsg(this);
		die->QueueMessage();

		if (trail)
			trail->Release();
	}
	if (slowTimer > 0)
		slowTimer -= dt;
	else
		GameplayState::GetInstance()->SetTimeStep(1.0f);

	AnimationSystem::GetInstance()->Update(action, dt);
	collide = AnimationSystem::GetInstance()->CollisionRect(action, m_ptPosition);
	Entity::Update(dt);

	if (SkillLvl > 1 && trail != nullptr)
		trail->SetPosition(AnimationSystem::GetInstance()->GetWeaponPoint(action, m_ptPosition));

	if (SkillLvl == 3 && trail != nullptr)
	{
		SGD::Rectangle burnRect = AnimationSystem::GetInstance()->ActiveRect(action, m_ptPosition);

		vector<NPC*> hits = GameplayState::GetInstance()->npcs;
		for (unsigned int x = 0; x < hits.size(); x++)
		{
			if (hits[x]->getCharacter()->GetPosition().IsWithinRectangle(burnRect))
				hits[x]->getCharacter()->AddStatusAilment(new Burning(hits[x]->getCharacter()));
		}
	}
}

void Ambulance::Render(void)
{
	//SGD::GraphicsManager::GetInstance()->DrawRectangle(collide, SGD::Color(255, 0, 0));
	AnimationSystem::GetInstance()->Render(action, m_ptPosition);
}

void Ambulance::HandleCollision(const IEntity* other)
{
	const Character* hit = reinterpret_cast<const Character*>(other);
}

void Ambulance::HandleSpecial(bool heals)
{
	if (!hitFirst)
	{
		hitFirst = true;
		GameplayState::GetInstance()->SetTimeStep(0.05f);
		slowTimer = 0.1f;

		//drop blood particles when someone loses health
		Emitter *e = GameplayState::GetInstance()->CreateEmitter("bloodEffect");
		e->SetPosition(this->GetPosition());
		e->SetFollowTarget(this);
		e->SetParticleSize(1.0f, 5.0f);
		//HEALS!
		if (heals)
		{
			Player* tempPlayer = GameplayState::GetInstance()->player;
			SGD::AudioManager::GetInstance()->PlayAudio(GameplayState::GetInstance()->heals);

			for (int x = 0; x < 4; x++)
			{
				Character* tempChar = tempPlayer->GetPartyMember(x);
				if (tempChar != nullptr)
				{
					tempChar->ModifyHealth(tempChar->GetHealth()*0.25f);
					Emitter* heals = GameplayState::GetInstance()->CreateEmitter("heals", tempChar);
					heals->lifeTime = 0.25f;
				}
			}
			//spawn blood splatter 
		}

	}
}

SGD::Rectangle Ambulance::GetRect(void) const
{
	SGD::Rectangle camera = GameplayState::GetInstance()->GetWorld()->GetCamera()->GetRect();
	if (m_ptPosition.IsWithinRectangle(camera))
		return collide;

	return SGD::Rectangle{};
}