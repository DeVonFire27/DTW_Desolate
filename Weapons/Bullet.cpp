/***************************************************************
|	File:		Bullet.cpp
|	Author:		Michael Mozdzierz
|	Date:		04/23/2014
|
***************************************************************/

#include "globals.h"
#include "Bullet.h"
#include "Weapon.h"
#include "Pistol.h"
#include "../Agents/Player.h"
#include "MutantAttack.h"
#include "../Math.h"
#include "../../SGD Wrappers/SGD_GraphicsManager.h"
#include "SpecialWeapons\BuzzsawLauncher.h"
#include "../../source/GameplayState.h"

#include <vector>
#include "../Agents/NPC.h"

#include "../Status Effects/Radiation.h"
#include "../Status Effects/Stun.h"
#include "../Status Effects/StatusEffect.h"
#include "../Tile System/TileLayer.h"
#include "../Tile System/Tile.h"
#include "../Math.h"
#include "../Tile System/Tile.h"
#include "../Tile System/TileLayer.h"
#include "../GameplayState.h"
#include "../Tile System/World.h"
#include "../Agents/Mutant.h"
#include "../Agents/Radiant.h"

Bullet::Bullet(Weapon* own)
{
	m_hImage = SGD::GraphicsManager::GetInstance()->LoadTexture("resources\\graphics\\bulletButton.png");
	saw = SGD::GraphicsManager::GetInstance()->LoadTexture("resources\\graphics\\buzzsaw.png");
	spittle = SGD::GraphicsManager::GetInstance()->LoadTexture("resources\\graphics\\spittle.png");

	owner = own;
	owner->AddRef();
	if (owner->GetGunType() == Weapon::GunType::flameThrower)
		visible = false;
	m_szSize = { 10.f, 10.f };
	isEnemyAtk = owner->GetOwner()->GetIsEnemy();
	gunActive = owner->GetGunActive();
	m_fRotation = owner->GetRotation();
	switch (owner->GetGunType())
	{
	case Weapon::GunType::Pistol:
	{
		bulletDropOff *= 3.5f;
		if (reinterpret_cast<Pistol*>(owner)->GetPistolType() == Pistol::PistolType::spittle)
			spit = true;
	}
		break;
	case Weapon::GunType::Shotgun:
		bulletDropOff *= 3.0f;
		break;
	case Weapon::GunType::SniperRifle:
		bulletDropOff *= 4.5f;
		break;
	case Weapon::GunType::flameThrower:
		bulletDropOff *= 2.5f;
		break;
	case Weapon::GunType::chainsawLauncher:
		bulletDropOff *= 3.5f;
		break;
	case Weapon::GunType::gatlingGun:
		bulletDropOff *= 3.5f;
		break;
	case Weapon::GunType::meleeWeapon:
	{
		 bulletDropOff *= 0.3f;
		 m_szSize = { 30.0f, 50.0f };
	}
		break;
	case Weapon::GunType::MutantAtk:
	{
		bulletDropOff *= 0.3f;
		m_szSize = { 30.0f, 50.0f };
	}
		break;
	}

	collisionLayer = GameplayState::GetInstance()->GetWorld()->GetTileLayers()[1];

	owner->GetOwner()->shotTimer = 0.0f;
}

Bullet::~Bullet(void)
{
	SGD::GraphicsManager::GetInstance()->UnloadTexture(m_hImage);
	SGD::GraphicsManager::GetInstance()->UnloadTexture(saw);
	owner->Release();
}

void Bullet::Render()
{
	if (!visible)
		return;
	SGD::Rectangle rect;
	rect.top = m_ptPosition.y;
	rect.left = m_ptPosition.x;
	rect.Resize(bulletSize*dist);

	TileLayer *layer0 = World::GetInstance()->GetTileLayers()[0];
	if (rect.top < 0.0f
		|| rect.left < 0.0f
		|| rect.bottom >= layer0->layerRows * layer0->GetTileSize().width
		|| rect.right >= layer0->layerColumns * layer0->GetTileSize().height
		)
	{
		return;
	}

	//GraphicsManager::GetInstance()->DrawRectangle(GetRect(), SGD::Color(0, 200, 125));
	if (owner->GetGunType() == Weapon::GunType::chainsawLauncher)
		GraphicsManager::GetInstance()->DrawTexture(saw, SGD::Point{ m_ptPosition.x - 32.0f, m_ptPosition.y + 32.0f }, 0, {}, {}, SGD::Size{ 0.25f*dist, 0.25f*dist });
	else if (owner->GetGunType() != Weapon::GunType::meleeWeapon && owner->GetOwner() && owner->GetOwner()->GetClass() != ClassType::Mutant)
		GraphicsManager::GetInstance()->DrawTexture(m_hImage, m_ptPosition, m_fRotation + (PI*0.5f), SGD::Vector(m_szSize.width*0.5f, m_szSize.height*0.5f), {}, SGD::Size{ 0.175f*dist, 0.25f*dist });
	else if (spit)
		GraphicsManager::GetInstance()->DrawTexture(spittle, m_ptPosition, m_fRotation + PI, SGD::Vector(32.0f, 32.0f), {}, SGD::Size{ 0.5f, 0.5f });
}

void Bullet::DamageModifier()
{
	// apply passives at level 15+
	if (owner->GetOwner()->GetLevel() >= 15)
	{
		// if gunslinger, increase damage by 15%
		if (owner->GetGunType() == Weapon::GunType::Pistol && owner->GetOwner()->GetClass() == ClassType::Gunslinger)
			damage *= 1.10f;
		// if sniper, increase damage by 15%
		else if (owner->GetGunType() == Weapon::GunType::SniperRifle && owner->GetOwner()->GetClass() == ClassType::Sniper)
			damage *= 1.15f;
		// if cyborg, increase damage by 15%
		else if (owner->GetGunType() == Weapon::GunType::Shotgun && owner->GetOwner()->GetClass() == ClassType::Cyborg)
			damage *= 1.15f;
	}

	// check characters critChance (modified in handlepassive() of each character)
	if (owner->GetOwner()->GetCritHitChance() > 0)
	{
		// create random crit
		int crit = rand() % 100 + 1;

		// double damage if crit passes
		if (crit < owner->GetOwner()->GetCritHitChance())
		{
			// double current damage
			damage *= 2.0f;
			// mark as true for related passives (see sniper)
			critPassive = true;
		}
		else
			// if crit fails mark as false (see sniper)
			critPassive = false;

		// sniper passives debug code
		if (GameplayState::GetInstance()->player)
		{
			if (GameplayState::GetInstance()->player->GetCharacter()->GetClass() == ClassType::Sniper ||
				GameplayState::GetInstance()->player->GetCharacter()->GetClass() == ClassType::Gunslinger)
			{
				GameplayState::GetInstance()->nMyChance = owner->GetOwner()->GetCritHitChance();
				GameplayState::GetInstance()->nTheOdds = crit;
				GameplayState::GetInstance()->fPassiveDamageDealt = damage;
			}
		}
	}
}

void Bullet::SetStartPoint(SGD::Point point)
{
	startPoint = point;
	m_ptPosition = startPoint;
}

/*virtual*/ void Bullet::Update(float dt) /*override*/
{
	SGD::Rectangle rect;
	rect.top = m_ptPosition.y;
	rect.left = m_ptPosition.x;
	rect.Resize(bulletSize*dist);
	TileLayer *layer0 = World::GetInstance()->GetTileLayers()[0];
	if (rect.top < 0.0f
		|| rect.left < 0.0f
		|| rect.bottom >= layer0->layerRows * layer0->GetTileSize().width
		|| rect.right >= layer0->layerColumns * layer0->GetTileSize().height
		)
	{
		return;
	}

	// if this bullet goes outside bullet dropoff range...
	if (owner->GetOwner() && (m_ptPosition - startPoint).ComputeLength() > bulletDropOff)
	{
		// create a message to destroy this bullet
		DestroyEntityMsg* msg = new DestroyEntityMsg(this);

		// dispatch the destroy message
		SGD::MessageManager::GetInstance()->GetInstance()->QueueMessage(msg);
	}

	if (gunActive)
	{
		dist += 0.075f;
		if (dist > 1.75f)
			dist = 1.75f;
	}

	// if bullet collides with tile, play collision sound
	//if (owner->GetGunType() != Weapon::GunType::meleeWeapon)
		
	// if not, update bullet
	Entity::Update(dt);

	TileLayer* collisionLayer = GameplayState::GetInstance()->GetWorld()->GetTileLayers()[1];
	const SGD::Point ref_position = { m_ptPosition.x + m_szSize.width*0.5f, m_ptPosition.y + m_szSize.height*0.5f };
	const int tileSize_width = (int)collisionLayer->GetTileSize().width;
	const int tileSize_height = (int)collisionLayer->GetTileSize().height;
	const int tilesWide = collisionLayer->layerColumns - 1;
	const int tilesHigh = collisionLayer->layerRows - 1;
	SGD::Point index = { Math::Clamp((ref_position.x / (float)tileSize_width), 0.f, (float)tilesWide), Math::Clamp(ref_position.y / (float)tileSize_height, 0.f, (float)tilesHigh) };

	Tile* tile_at = collisionLayer->GetTileAt(int(index.x), int(index.y));

	if (tile_at && !tile_at->isPassable)
	{
		if (tile_at->event != "bp" && owner->GetGunType() != Weapon::GunType::meleeWeapon && owner->GetGunType() != Weapon::GunType::MutantAtk)
		{
			SGD::AudioManager::GetInstance()->PlayAudio(GameplayState::GetInstance()->bulletImpact);
			DestroyEntityMsg* msg = new DestroyEntityMsg(this);
			msg->QueueMessage();
		}
	}
}

/*virtual*/ void Bullet::HandleCollision(const IEntity* other) /*override*/
{
	const Character* temp = reinterpret_cast<const Character*>(other);

	if (temp && owner && owner->GetOwner() && (temp->GetIsEnemy() != owner->GetOwner()->GetIsEnemy()))
	{
		// create a hit marker to indicate damage
		SGD::Color enemyColor;
		if (temp->GetIsEnemy())
			enemyColor = SGD::Color::Yellow;
		else
			enemyColor = SGD::Color::Blue;

		// dispatch a hit marker message only if the other object actually has health
		if (temp->GetCurrHealth() > 0.f)
		{
			CreateHitMarkerMsg* msg = new CreateHitMarkerMsg((int)(GetDamage()), m_ptPosition, m_fRotation - Math::to_radians(180.f), enemyColor);
			SGD::MessageManager::GetInstance()->QueueMessage(msg);
		}
	}

	// avoid shooting yourself unless its a deflected bullet
	if ((owner->GetOwner() != temp && !deflectBullet) || (owner->GetOwner() == temp && deflectBullet))
	{

#pragma region Brawler
		const Character* testCharacter = reinterpret_cast<const Character*>(other);
		Character* testNonConstCharacter = const_cast<Character*>(testCharacter);

		// player hit by bullet 
		if (GameplayState::GetInstance()->player && testCharacter->GetLevel() >= 1)
		{
			if (testCharacter && testCharacter->GetClass() == ClassType::Brawler && testCharacter == GameplayState::GetInstance()->player->GetCharacter())
			{
				// damage player Takes
				GameplayState::GetInstance()->fBaseDamageTaken = GetDamage();
				GameplayState::GetInstance()->fPassiveDamageTaken = testNonConstCharacter->GetWeapon()->GetDamage();
			}
		}

		if (owner->GetGunType() == Weapon::GunType::chainsawLauncher && owner->GetOwner()->GetClass() == ClassType::Brawler)
		{
			if (reinterpret_cast<BuzzsawLauncher*>(owner)->GetisBrawlActive())
			{
				if (owner->GetOwner()->GetLevel() >= 21)
					owner->GetOwner()->SetCurrCooldown(owner->GetOwner()->GetCurrCoolDown() - 0.5f);
				
				if (owner->GetOwner()->GetLevel() >= 11)
					owner->GetOwner()->ModifyHealth(owner->GetOwner()->GetStat(StatType::strength)*0.35f);
			}
		}
#pragma endregion

		if (temp && owner && owner->GetOwner() && owner->GetOwner()->GetClass() == ClassType::Cyborg && owner->GetOwner()->GetLevel() >= 18 && damage >= temp->GetCurrHealth())
		{
			vector<NPC*> hits = GameplayState::GetInstance()->npcs;
			int lightning = 0;
			for (unsigned int x = 0; x < hits.size() && lightning <= 2; x++)
			{
				float length = (hits[x]->getCharacter()->GetPosition() - temp->GetPosition()).ComputeLength();
				if (length < 300.0f && hits[x]->getCharacter()->GetIsEnemy() != owner->GetOwner()->GetIsEnemy() && hits[x]->getCharacter() != temp)
				{
					hits[x]->getCharacter()->ModifyHealth(-owner->GetOwner()->GetStat(StatType::accuracy)*0.5f);
					lightning++;

					if (owner->GetOwner()->GetLevel() >= 24)
					{
						Stun* stun = new Stun(hits[x]->getCharacter());
						stun->SetDuration(1.0f);
						Radiation* rad = new Radiation(hits[x]->getCharacter());
						rad->SetStacks(5);
						hits[x]->getCharacter()->AddStatusAilment(stun);
						hits[x]->getCharacter()->AddStatusAilment(rad);
					}
				}
			}

			if (lightning > 0)
				SGD::AudioManager::GetInstance()->PlayAudio(GameplayState::GetInstance()->chain_lightnin);
		}

		const Character* tempEnemy = dynamic_cast<const Character*>(other);
		Character* newEnemy = const_cast<Character*>(tempEnemy);

		if (tempEnemy && owner && owner->GetOwner() && owner->GetOwner()->GetClass() == ClassType::Gunslinger && owner->GetOwner()->GetLevel() >= 24)
		{
			int theOdds = rand() % 100 + 1;
			// gunslinger passive debug code
			GameplayState::GetInstance()->nRiccochetOdds = theOdds;

			if (theOdds < 45 && tempEnemy->GetIsEnemy() != owner->GetOwner()->GetIsEnemy())
			{
				// adjust angle to aim at another enemy (see chain lightning: cyborg lv 18 - line 120)
				// instead of destroying, redirect to another (and don't destroy)
				// if no other near, destroy this
				Weapon* tempWeapon = newEnemy->GetWeapon();

				// gunslinger debug code
				for (unsigned i = 0; i < GameplayState::GetInstance()->npcs.size(); i++)
				{
					if (GameplayState::GetInstance()->npcs[i]->getCharacter() != tempEnemy)
					{
						SGD::Vector tempDiff = GameplayState::GetInstance()->npcs[i]->getCharacter()->GetPosition() - GetPosition();
						//tempDiff.Normalize();

						float tempDist = tempDiff.ComputeLength();

						if (tempDist < 300)
						{
							// calculate angle to new target
							float toTarget = atan2(tempDiff.y, tempDiff.x);

							SGD::Vector newVelocity = GetVelocity();

							// set the velocity to the newly rotated velocity
							SetVelocity(newVelocity.ComputeRotated(toTarget));
							SetRotation(toTarget);
							SGD::AudioManager::GetInstance()->PlayAudio(GameplayState::GetInstance()->trickShot);
							break;
						}
					}
				}
			}
		}
		if (owner && owner->GetOwner() && owner->GetOwner()->GetClass() == ClassType::Medic && owner->GetOwner()->GetLevel() >= 24 && temp && damage >= temp->GetCurrHealth()*1.2f)
		{
			vector<NPC*> hits = GameplayState::GetInstance()->npcs;
			if ((GameplayState::GetInstance()->player->GetCharacter()->GetPosition() - temp->GetPosition()).ComputeLength() < 300.0f)
			{
				GameplayState::GetInstance()->player->GetCharacter()->ModifyHealth((temp->GetHealth())*0.1f);
				Emitter* heals = GameplayState::GetInstance()->CreateEmitter("heals", (temp != nullptr));
				heals->lifeTime = 0.25f;
			}

			for (unsigned int x = 0; x < hits.size(); x++)
			{
				float length = (hits[x]->getCharacter()->GetPosition() - temp->GetPosition()).ComputeLength();
				if (length < 300.0f && !hits[x]->getCharacter()->GetIsEnemy())
				{
					hits[x]->getCharacter()->ModifyHealth((temp->GetHealth())*0.05f);
					Emitter* heals = GameplayState::GetInstance()->CreateEmitter("heals", (temp != nullptr));
					heals->lifeTime = 0.25f;
					//visual effect
				}
			}
			SGD::AudioManager::GetInstance()->PlayAudio(GameplayState::GetInstance()->tranfusion);
		}
		if (GetOwner()->bulType.size() == 0)
		{
			if (tempEnemy && owner && owner->GetOwner() && (tempEnemy->GetIsEnemy() != owner->GetOwner()->GetIsEnemy()))
			{
				if (critPassive)
					SGD::AudioManager::GetInstance()->PlayAudio(GameplayState::GetInstance()->crit_shot);

				// allocate a message to destroy this instance
				DestroyEntityMsg* msg = new DestroyEntityMsg(this);

				// dispatch destroy message
				SGD::MessageManager::GetInstance()->QueueMessage(msg);
			}
		}
		// allocate a message to destroy this instance if the bullet is not peircing
		for (unsigned int i = 0; i < GetOwner()->bulType.size(); i++)
		{
			//if its a peircing bullet, don't continue to where you delete the bullet
			if (GetOwner()->bulType[i] == BulletType::fmj || GetOwner()->bulType[i] == BulletType::FMJFTW)
			{
				break;
			}

			if (owner && owner->GetOwner() && owner->GetOwner()->GetClass() == ClassType::Gunslinger && owner->GetOwner()->GetLevel() >= 24 && GameplayState::GetInstance()->nTheOdds < 45); // do nothing
			else if (!deflectBullet && i == GetOwner()->bulType.size() - 1)
			{
				// allocate a message to destroy this instance if you make it through the entire vector of bullet types and don't find fmj or fmjftw
				DestroyEntityMsg* msg = new DestroyEntityMsg(this);

				// dispatch destroy message
				SGD::MessageManager::GetInstance()->QueueMessage(msg);
			}
		}
		deflectBullet = false;
	}

}

bool Bullet::CheckTileCollision()
{
	//if(!this) return;

	//== == == == == == == == == == == == == == == == == == Inilitize local values == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == =
	const SGD::Point ref_position = { m_ptPosition.x + m_szSize.width / 2.f, m_ptPosition.y + m_szSize.height / 2.f };
	const int tileSize_width = (int)collisionLayer->GetTileSize().width;
	const int tileSize_height = (int)collisionLayer->GetTileSize().height;
	const int tilesWide = collisionLayer->layerColumns - 1;
	const int tilesHigh = collisionLayer->layerRows - 1;
	//=================================================================================================================================================

	// store the tile index of our current position
	SGD::Point index = { Math::Clamp((ref_position.x / (float)tileSize_width), 0.f, (float)tilesWide), Math::Clamp(ref_position.y / (float)tileSize_height, 0.f, (float)tilesHigh) };

	// find neighboring tile indicies
	SGD::Point index_up = { index.x, (float)Math::Clamp(int(index.y - 1), 0, tilesHigh) };
	SGD::Point index_down = { index.x, (float)Math::Clamp(int(index.y + 1), 0, tilesHigh) };
	SGD::Point index_left = { (float)Math::Clamp(int(index.x - 1), 0, tilesWide), index.y };
	SGD::Point index_right = { (float)Math::Clamp(int(index.x + 1), 0, tilesWide), index.y };

	if (collisionLayer->GetTileAt((int)index.x, (int)index.y)->isPassable)
		return false;
	else if (collisionLayer->GetTileAt((int)index.x, (int)index.y)->event == "bp")
		return false;

	// not passable (collision) destroy bullet
	DestroyEntityMsg* msg = new DestroyEntityMsg(this);
	// dispatch the destroy message
	SGD::MessageManager::GetInstance()->GetInstance()->QueueMessage(msg);
	return true;
}
