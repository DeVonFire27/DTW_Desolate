#include "globals.h"
#include "Nuke.h"
#include "../GameplayState.h"
#include "../Animation System/AnimationSystem.h"
#include "../Tile System/Tile.h"
#include "../Tile System/TileLayer.h"
#include "../Particle System/Emitter.h"
#include "../Math.h"

Nuke::Nuke(void)
{
	action.SetCurrAnimation("MiniNuke");
	nuke = SGD::AudioManager::GetInstance()->LoadAudio("resources/audio/Sound Effects/Nuke_sfx.wav");
	missile = SGD::GraphicsManager::GetInstance()->LoadTexture("resources/graphics/missile.png");
	fireFlare = GameplayState::GetInstance()->CreateEmitter("fire");
	fireFlare->AddRef();
	m_szSize = { 32, 64 };
}

Nuke::~Nuke(void)
{
	fireFlare->Release();
	SGD::AudioManager::GetInstance()->UnloadAudio(nuke);
	SGD::GraphicsManager::GetInstance()->UnloadTexture(missile);
}


void Nuke::Activate()
{
	AnimationSystem::GetInstance()->ResetAnimation(action);
	duration = 6.5f;
	notExplode = true;
	damage = (owner->GetStat(StatType::accuracy) + 50)*1.75f;
	fireFlare->active = true;

	m_ptPosition = AnimationSystem::GetInstance()->GetWeaponPoint(owner->GetAnimation(), owner->GetPosition(), owner->GetRotation());
	m_vtVelocity = SGD::Vector(0, -1);
	m_vtVelocity.Rotate(owner->GetRotation());
	m_vtVelocity *= 640;

	if (owner->GetLevel() >= 11)
	{
		SkillLvl = 2;
		blastColor = SGD::Color{ 153, 255, 51 };
	}
	if (owner->GetLevel() >= 21)
	{
		SkillLvl = 3;
		action.SetCurrAnimation("Boom");
		blastColor = SGD::Color{};
	}
}

void Nuke::Update(float dt)
{
	duration -= dt;
	if (duration < 0 || owner == nullptr || action.HasEnded())
	{
		SGD::AudioManager::GetInstance()->StopAudio(nuke);
		DestroyEntityMsg* die = new DestroyEntityMsg(this);
		die->QueueMessage();

		fireFlare->active = false;
	}

	if (!notExplode)
	{
		damage = owner->GetStat(StatType::accuracy) * dt * 1.25f;
		AnimationSystem::GetInstance()->Update(action, dt);
		collide = AnimationSystem::GetInstance()->ActiveRect(action, m_ptPosition, m_fRotation);
	}
	else
	{
		//SGD::Rectangle rect;
		//rect.top = m_ptPosition.y;
		//rect.left = m_ptPosition.x;
		//rect.Resize(SGD::Size(20, 20));
		fireFlare->SetPosition(SGD::Point(m_ptPosition.x + 8.0f, m_ptPosition.y + 16.0f));
		fireFlare->SetRotation(owner->GetRotation());
		collide = Entity::GetRect();
	}
	Entity::Update(dt);

	TileLayer* collisionLayer = GameplayState::GetInstance()->GetWorld()->GetTileLayers()[1];
	const SGD::Point ref_position = { m_ptPosition.x + m_szSize.width*0.5f, m_ptPosition.y + m_szSize.height*0.5f };
	const int tileSize_width = (int)collisionLayer->GetTileSize().width;
	const int tileSize_height = (int)collisionLayer->GetTileSize().height;
	const int tilesWide = collisionLayer->layerColumns - 1;
	const int tilesHigh = collisionLayer->layerRows - 1;
	SGD::Point index = { Math::Clamp((ref_position.x / (float)tileSize_width), 0.f, (float)tilesWide), Math::Clamp(ref_position.y / (float)tileSize_height, 0.f, (float)tilesHigh) };


	if (!collisionLayer->GetTileAt((int)index.x, (int)index.y)->isPassable)
	{
		if (collisionLayer->GetTileAt((int)index.x, (int)index.y)->event != "bp")
		{
			//the implosion before the explosion!! :D
			if (SkillLvl >= 3 && notExplode)
				owner->AOEPull(m_ptPosition, 400.0f, 5.0f);
			else if (SkillLvl >= 3)
				owner->AOEPull(m_ptPosition, 150.0f, 0.5f);

			m_vtVelocity = SGD::Vector(0, 0);
			if (notExplode)
			{
				SGD::AudioManager::GetInstance()->PlayAudio(nuke);
				if (SkillLvl != 2)
					GameplayState::GetInstance()->GetWorld()->GetCamera()->CameraShake(20, 3.0f);
				else
					GameplayState::GetInstance()->GetWorld()->GetCamera()->CameraShake(30, 3.0f);
				notExplode = false;
				fireFlare->active = true;
			}
		}
	}
}

void Nuke::Render(void)
{
	if (!notExplode)
		AnimationSystem::GetInstance()->Render(action, m_ptPosition, m_fRotation, blastColor);
	else
	{
		//SGD::Rectangle rect;
		//rect.top = m_ptPosition.y;
		//rect.left = m_ptPosition.x;
		//rect.Resize(SGD::Size(20, 20));
		//GraphicsManager::GetInstance()->DrawRectangle(rect, SGD::Color(65, 255, 65));

		SGD::GraphicsManager::GetInstance()->DrawTexture(missile, m_ptPosition, owner->GetRotation(), SGD::Vector{ 16, 32 }, {}, SGD::Size{ 0.5f, 0.5f });
	}
}

void Nuke::HandleCollision(const IEntity* other)
{
	const Character* hit = reinterpret_cast<const Character*>(other);
	if (hit->GetIsEnemy() != owner->GetIsEnemy())
	{
		//the implosion before the explosion!! :D
		if (SkillLvl >= 3 && notExplode)
			owner->AOEPull(m_ptPosition, 300.0f, 5.0f);
		else if (SkillLvl >= 3)
			owner->AOEPull(m_ptPosition, 150.0f, 0.5f);

		m_vtVelocity = SGD::Vector(0, 0);
		if (notExplode)
		{
			SGD::AudioManager::GetInstance()->PlayAudio(nuke);
			if (SkillLvl != 2)
				GameplayState::GetInstance()->GetWorld()->GetCamera()->CameraShake(20, 3.0f);
			else
				GameplayState::GetInstance()->GetWorld()->GetCamera()->CameraShake(30, 3.0f);

			notExplode = false;
			fireFlare->active = true;
		}
	}

}

SGD::Rectangle Nuke::GetRect(void) const
{
	return collide;
}