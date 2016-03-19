/***************************************************************
|	File:		Character.cpp
|	Author:		Michael Mozdzierz
Ethan Mills
|	Date:		04/29/2014
|
***************************************************************/

#include "globals.h"
#include "Character.h"
#include "Player.h"
#include "../Abilities/Ambulance.h"
#include "../Abilities/Laser.h"
#include "../Abilities/LaserBullet.h"
#include "../Abilities/Nuke.h"
#include "../Abilities/Mortar.h"
#include "../Abilities/Chainsaw.h"
#include "../Abilities/Stampede.h"

#include "../Particle System/Emitter.h"
#include "../Particle System/Particle.h"
#include "../TinyXML/tinyxml.h"
#include "../TinyXML/tinystr.h"
#include "../EventProc/EventProc.h"
#include "../Messages/DestroyEntityMsg.h"
#include "../MessageID.h"

#include "../Weapons/Weapon.h"
#include "../BitmapFont.h"
#include "../Weapons/Bullet.h"
#include "../Game.h"
#include "../Tile System/Tile.h"
#include "../Tile System/World.h"
#include "../Tile System/TileLayer.h"
#include "../Hazards/RadiationPool.h"

#include "../Status Effects/StatusEffect.h"
#include "../Status Effects/Radiation.h"
#include "../Status Effects/Allure.h"
#include "../Status Effects/Bleeding.h"
#include "../Status Effects/Burning.h"
#include "../Status Effects/Slow.h"
#include "../Status Effects/Stun.h"
#include "../Status Effects/Transfusion.h"

#include "../OptionsState.h"
#include "../Animation System/AnimationSystem.h"
#include "../GameplayState.h"
#include "../EntityManager.h"
#include <ctime>
#include "../Math.h"
#include "../Weapons/MeleeWeapon.h"
#include "../Objective System/ObjectiveList.h"
#include <sstream>
#include "../Dialogue System/Dialogue.h"
#include "../Spawn System/SpawnList.h"
#include "Mutant.h"
#include "Radicorn.h"

#include "../Agents/NPC.h"

#include "../Hazards/Pitfall.h"
#include "../Hazards/BearTrap.h"
#include "../Weapons/SpecialWeapons/Flamethrower.h"
#include "../Weapons/Shotgun.h"
#include "../Weapons/MutantAttack.h"

Character::Character(void)
{
	// ensure no garbage in data members
	currExp = 0;
	SetSize({ 64, 48 });
	level = 1;
	critHitChance = 0;
	currDamage = 0;

	origin.x = m_szSize.width / 2.0f;
	origin.y = m_szSize.height / 2.0f;

	// Subscribing for melee weapon attacks
	EventProc::GetInstance()->Subscribe(this, "meleeWeaponUsed");

	// subscribe for ZE END OF ZE VERLD!
	EventProc::GetInstance()->Subscribe(this, "GameExit");
}

Character::~Character()
{
	stats.clear();
	SetWeapon(nullptr);
	DestroyEntityMsg* msg = new DestroyEntityMsg(ability);
	msg->QueueMessage();

	EventProc::GetInstance()->UnSubscribe(this, "meleeWeaponUsed");
	EventProc::GetInstance()->UnSubscribe(this, "GameExit");

	_ASSERTE(_CrtCheckMemory());
}

std::stringstream Character::GetActiveDiscription()
{
	std::stringstream line;
	return line;
}

std::stringstream Character::GetPassiveDiscription()
{
	std::stringstream line;
	return line;
}

void Character::UseWeapon(void)
{
	if (weapon)
	{
		weapon->Use();
	}
}

/*virtual*/ void Character::UseAbility(void)
{
	if (!cooldownTimerStarted && currHealth > 0)
	{
		cooldownTimerStarted = true;
		currCooldown = cooldown;
		ability->Activate();
	}
}

void Character::ClearStatusEffects()
{
	for (unsigned int i = 0; i < effects.size();)
		RemoveStatusAilment(effects[i]->GetType());

	effects.clear();
}

void Character::WalkTowards(SGD::Point target)
{
	SGD::Vector toTarget = target - m_ptPosition;
	toTarget.Normalize();

	if (GetClass() == ClassType::Mutant)
		toTarget *= (float)((Mutant *)this)->GetMoveSpeed();
	else
		toTarget *= (float)GetStat(StatType::moveSpeed);

	m_vtVelocity += toTarget;
	isMoving = true;
}

void Character::WalkTowards(Entity *target)
{
	WalkTowards(target->GetPosition());
}

void Character::WalkDirection(SGD::Vector target)
{
	target.Normalize();
	if (GetClass() == ClassType::Mutant)
		target *= (float)((Mutant *)this)->GetMoveSpeed();
	else
		target *= (float)GetStat(StatType::moveSpeed);

	m_vtVelocity += target;
	isMoving = true;
}

/*virtual*/ void Character::HandlePassive(void)
{

}

/*virtual*/ void Character::LevelUp(void)
{
	level++;
	float healBuff = 0.1f;
	healBuff *= Game::GetInstance()->playerDifficulty;
	ModifyHealth(GetStat(StatType::health)*(0.15f + healBuff));
	health = GetStat(StatType::health);

	Emitter* Lvlup = GameplayState::GetInstance()->CreateEmitter("levelUp", this);
	Lvlup->lifeTime = 0.25f;

	if (ability)
	{
		if (level >= 21)
			ability->SetSkillLvl(3);
		else if (level >= 11)
			ability->SetSkillLvl(2);
		else
			ability->SetSkillLvl(1);
	}

	if (FindControllingNPC(this) != nullptr)
		FindControllingNPC(this)->SetRepRequirement(FindControllingNPC(this)->GetRepRequirement() + 500);

}

void Character::SetExp(int exp)
{
	if (level == 25)
		exp = 0;

	currExp = exp;
}

void Character::ModifyHealth(float value)
{
	currHealth += value;

	if (GameplayState::GetInstance()->player && currHealth <= 0)
	{
		ClearStatusEffects();

		DestroyEntityMsg* msg = new DestroyEntityMsg(this);
		SGD::MessageManager::GetInstance()->QueueMessage(msg);

		if (isEnemy && GetClass() != ClassType::Mutant)
		{
			//Droping stuff
			int dropAnything = rand() % 100 + 1;
			if (dropAnything < 45 && FindControllingNPC(this) && !FindControllingNPC(this)->GetIsDead())
			{
				int healthKitOrWep = rand() % 2;

				if (healthKitOrWep == 1 && weapon != nullptr)
				{
					CreateWeaponMsg* msg = new CreateWeaponMsg(weapon);
					SGD::MessageManager::GetInstance()->QueueMessage(msg);
				}
				else
				{
					CreateHealthPickupMsg* msg = new CreateHealthPickupMsg(m_ptPosition);

					SGD::MessageManager::GetInstance()->QueueMessage(msg);
				}
			}
		}
		else if (weapon != nullptr && GetClass() != ClassType::Mutant)
		{
			CreateWeaponMsg* msg = new CreateWeaponMsg(weapon);
			SGD::MessageManager::GetInstance()->QueueMessage(msg);
		}
		else if (GetClass() == ClassType::Mutant)
		{
			int dropAnything = rand() % 100 + 1;
			if (dropAnything < 25)
			{
				CreateHealthPickupMsg* msg = new CreateHealthPickupMsg(m_ptPosition);
				SGD::MessageManager::GetInstance()->QueueMessage(msg);
			}
		}

		SpawnList::GetInstance()->SearchAndKill(FindControllingNPC(this));
		EventProc::GetInstance()->Dispatch("NPC_DEAD", this);

		//Make a blood effect on death
		Emitter *emtr = GameplayState::GetInstance()->CreateEmitter("bloodEffect");
		emtr->SetPosition(m_ptPosition);
		emtr->particleLifeTime = 45.0f;
		emtr->particleLifeTimeRand = 0.8f;

		// signal gain of experience if this is an enemy and the player isn't equal to nullptr
		if (isEnemy && GameplayState::GetInstance()->player)
			EventProc::GetInstance()->Dispatch("GainExp", &level);

		if (GameplayState::GetInstance()->player)
			GameplayState::GetInstance()->player->ModifyReputation(this);

		// If this character was a character with an objective, handle it
		if (eventString != "")
			ObjectiveList::GetInstance()->NextObjective(eventString);

		// If the enemy is a miniboss/boss, give the player extra reputation
		if (this != GameplayState::GetInstance()->player->GetCharacter() && isEnemy)
		{
			BossType thisType = FindControllingNPC(this)->GetBossType();

			if (thisType == BossType::GATLING_GUN ||
				thisType == BossType::BUZZSAW ||
				thisType == BossType::RADZILLA)
			{
				GameplayState::GetInstance()->player->SetReputation(GameplayState::GetInstance()->player->GetReputation() + 500);
			}
			else if (thisType == BossType::JANE)
			{
				GameplayState::GetInstance()->player->SetReputation(GameplayState::GetInstance()->player->GetReputation() + 2500);
			}
			else // if (thisType == BossType::GRUNT)
			{
				if (GameplayState::GetInstance()->currentTutorialObjective == Tutorial::Active)
				{
					GameplayState::GetInstance()->currentTutorialObjective++;
					GameplayState::GetInstance()->sinceLastTutorial = 0.0f;
					// Create a health kit
					CreateHealthPickupMsg* msg = new CreateHealthPickupMsg(m_ptPosition);

					SGD::MessageManager::GetInstance()->QueueMessage(msg);
				}
			}
		}

		// If this is not an enemy, remove it from the npcs vector
		if (this != GameplayState::GetInstance()->player->GetCharacter())
		{

			if (!isEnemy)
			{
				//Loop through player's party and remove ref to character
				Player *playerPtr = GameplayState::GetInstance()->player;
				vector<NPC*>::iterator i;
				i = playerPtr->party.begin();
				while (i != playerPtr->party.end())
				{
					if ((*i)->getCharacter() == this)
					{
						playerPtr->party.erase(i);
						break;
					}

					i++;
				}

				vector<NPC*>::iterator iter = GameplayState::GetInstance()->npcs.begin();
				for (; iter != GameplayState::GetInstance()->npcs.end(); iter++)
				{
					if ((*iter)->getCharacter() == this)
					{
						delete *iter;
						GameplayState::GetInstance()->npcs.erase(iter);
						break;
					}
				}
			}
		}


	}

	// if we're the players active character, shake the camera
	if (this == GameplayState::GetInstance()->player->GetCharacter() && value < 0)
		GameplayState::GetInstance()->GetWorld()->GetCamera()->CameraShake(2, .3f);

	if (GetClass() != ClassType::Mutant)
	{
		if (currHealth > health)
			currHealth = health;
	}
}

void Character::LoadStats(const char* filename)
{
	// Load xml doc
	TiXmlDocument doc = TiXmlDocument(filename);
	doc.LoadFile();

	// get top node
	TiXmlNode* root = doc.FirstChild()->NextSibling();

	// get the first stat
	TiXmlElement* ClassName = (TiXmlElement*)root->FirstChild();

	std::string name;
	switch (GetClass())
	{
	case ClassType::Brawler:
		name = "Brawler";
		break;
	case ClassType::Cyborg:
		name = "Cyborg";
		break;
	case ClassType::Gunslinger:
		name = "Gunslinger";
		break;
	case ClassType::Medic:
		name = "Medic";
		break;
	case ClassType::Sniper:
		name = "Sniper";
		break;
	}

	TiXmlElement* stat = ClassName;

	if (name != "Brawler")
		stat = ClassName->NextSiblingElement(name.c_str());

	stat = (TiXmlElement*)stat->FirstChild();

	//// get the first stat node for this class
	//TiXmlElement* stat = ClassName->NextSiblingElement(name.c_str());
	//stat = (TiXmlElement*)stat->FirstChild();

	// loop over all the stats
	while (stat != nullptr)
	{
		// store what type of value this is
		int type;
		// store what the linear increace of this stat is
		double increace;
		// store the base value
		double base;

		// grab values out of xml
		stat->Attribute("Type", &type);
		stat->Attribute("Increace", &increace);
		stat->Attribute("base", &base);

		//difficulty stat adjustments
		int diff = Game::GetInstance()->playerDifficulty;
		if (!isEnemy  && type != 5)
		{
			if (diff != 2)
			{
				increace *= 1.25f;
				base *= 1.25f;
			}
			else if (diff == 2)
			{
				increace *= 1.15f;
				base *= 1.15f;
			}
		}
		else if (isEnemy && type != 5)
		{
			if (diff == 0)
			{
				increace *= 0.55f;
				base *= 0.55f;
			}
			else if (diff == 1)
			{
				increace *= 0.7f;
				base *= 0.7f;
			}
			else if (diff == 2)
			{
				increace *= 0.9f;
				base *= 0.9f;
			}

			if (level <= 5 && type != 0)
			{
				increace *= 0.7f;
				base *= 0.7f;
			}
		}

		if (GetClass() != ClassType::Mutant)
		{
			// use 'type' and 'increace' to modify stata
			stats[(StatType)type].increace = (float)increace;
			// send base to the stat
			stats[(StatType)type].base = (float)base;
		}

		// go to the next node
		stat = (TiXmlElement*)stat->NextSibling();
	}

	currHealth = health = GetStat(StatType::health);
}

NPC *FindControllingNPC(Character *character)
{
	vector<NPC*> npcs = GameplayState::GetInstance()->npcs;


	for (unsigned int i = 0; i < npcs.size(); i++)
	{
		if (character == (npcs)[i]->getCharacter())
		{
			return npcs[i];
		}
	}

	return nullptr;
}

void Character::Update(float dt)
{
	//If bad timestep or too far from player
	if (dt <= 0 ||
		GameplayState::GetInstance()->player && (m_ptPosition - GameplayState::GetInstance()->player->GetPosition()).ComputeLength() > Game::GetInstance()->GetScreenWidth() * 2)
	{
		return;
	}

	lastCooldown = currCooldown;

#pragma region Knockback and Recoil (DISABLED)
	//if (weaponRecoil.x != 0 && weaponRecoil.y != 0)
	//{
	//	//// Make sure their knockback location to move to is valid
	//	//int tileHeight = World::GetInstance()->GetTileLayers()[1]->GetTileSize().height;
	//	//int tileWidth = World::GetInstance()->GetTileLayers()[1]->GetTileSize().width;
	//	//
	//	//int futureX = (m_ptPosition.x + (weaponRecoil.x * (dt / .05))) / tileWidth;
	//	//int futureY = (m_ptPosition.y + (weaponRecoil.y * (dt / .05))) / tileHeight;
	//	//
	//	//int tileColumns = World::GetInstance()->GetTileLayers()[1]->GetTileColumns();
	//	//
	//	//// If their future location is passable
	//	//if (World::GetInstance()->GetTileLayers()[1]->tiles[futureX + futureY * tileColumns]->isPassable)
	//	//{
	//	// Update the character based upon the weaponRecoil
	//	//m_ptPosition.x += (float)(weaponRecoil.x * (dt / .05));
	//	//m_ptPosition.y += (float)(weaponRecoil.y * (dt / .05));

	//	bool positiveRecoilX = weaponRecoil.x > 0;
	//	bool positiveRecoilY = weaponRecoil.y > 0;

	//	// Remove the recoil amount that's been applied
	//	weaponRecoil.x -= (float)(weaponRecoil.x * (dt / .05));
	//	weaponRecoil.y -= (float)(weaponRecoil.y * (dt / .05));

	//	// Clamping
	//	if (positiveRecoilX)
	//	{
	//		if (weaponRecoil.x < 0.5)
	//			weaponRecoil.x = 0;
	//	}
	//	else
	//	{
	//		if (weaponRecoil.x > -.5)
	//			weaponRecoil.x = 0;
	//	}

	//	if (positiveRecoilY)
	//	{
	//		if (weaponRecoil.y < 0.5)
	//			weaponRecoil.y = 0;
	//	}
	//	else
	//	{
	//		if (weaponRecoil.y > -.5)
	//			weaponRecoil.y = 0;
	//	}
	//	//}
	//	//else
	//	//{
	//	//	weaponRecoil = SGD::Point(0, 0);
	//	//}
	//}

	//// Update the character based upon the knockbackVector
	//m_ptPosition.x += (float)(knockbackVector.x * (dt / .025));
	//m_ptPosition.y += (float)(knockbackVector.y * (dt / .025));

	//bool positiveKnockbackX = knockbackVector.x > 0;
	//bool positiveKnockbackY = knockbackVector.y > 0;

	//// Remove the knockback amount that's been applied
	//knockbackVector.x -= (float)(knockbackVector.x * (dt / .025));
	//knockbackVector.y -= (float)(knockbackVector.y * (dt / .025));

	//CheckTileCollision();

	//// Clamping
	//if (positiveKnockbackX)
	//{
	//	if (knockbackVector.x <= .5)
	//		knockbackVector.x = 0;
	//}
	//else
	//{
	//	if (knockbackVector.x >= -.5)
	//		knockbackVector.x = 0;
	//}

	//if (positiveKnockbackY)
	//{
	//	if (knockbackVector.y <= .5)
	//		knockbackVector.y = 0;
	//}
	//else
	//{
	//	if (knockbackVector.y >= -.5)
	//		knockbackVector.y = 0;
	//}
#pragma endregion

	//if (SGD::InputManager::GetInstance()->IsKeyPressed(SGD::Key::End))
	//	healthDebug = !healthDebug;

	if (!GetStunned())
	{
		// decrament radiation timer if we are in contact with radiation

		if (radiationStackTimer && GetClass() != ClassType::Mutant)
		{
			radiationStackTimer -= dt;
		}
		futureLocation.left = m_ptPosition.x + m_vtVelocity.x * dt;
		futureLocation.top = m_ptPosition.y + m_vtVelocity.y * dt;
		futureLocation.right = futureLocation.left + m_szSize.width;
		futureLocation.bottom = futureLocation.top + m_szSize.height;

		// check for collision with tiles
		CheckTileCollision();
		if (charAnim.GetCurrAnimation() != "")
			AnimationSystem::GetInstance()->Update(charAnim, dt);

		if (CheckCharacterCollision(futureLocation))
			Entity::Update(dt);
	}

	if (weapon)
		weapon->Update(dt);
	if (ambulHit > 0)
		ambulHit -= dt;
	if (hitTimer > 0)
		hitTimer -= dt;
	if (pushTimer > 0)
		pushTimer -= dt;
	if (bullPushTimer > 0)
		bullPushTimer -= dt;

	HandlePassive();

	if (cooldownTimerStarted)
		currCooldown -= dt;

	// reset all status ailments
	for (int i = 0; i < 6; i++)
		stats[(StatType)i].affliction = 0.f;

	// update stat ailment values
	for (unsigned int i = 0; i < effects.size(); i++)
	{
		StatusEffect* effect_at = effects[i];
		for (auto iter = effect_at->affectors.begin(); iter != effect_at->affectors.end(); iter++)
		{

			if (!effect_at->end)
				stats[(*iter).first].affliction = (*iter).second;
		}
	}


	// if we have enough exp, level up
	if (currExp > GetLevelUpExp() && level < 25)
	{
		currExp -= GetLevelUpExp();
		currExp /= 2;
		LevelUp();
	}

	// update the hazard timers so that you don't hear the sound effects every frame you are standing on them
	if (radiationSoundTimer >= 0)
		radiationSoundTimer -= dt;
	if (pitfallSoundTimer >= 0)
		pitfallSoundTimer -= dt;
	if (bearTrapSoundTimer >= 0)
		bearTrapSoundTimer -= dt;

	// apply each status ailment in the status effects list
	for (unsigned int i = 0; i < effects.size();)
	{
		if (this->currHealth > 0)
			effects[i]->Update(dt);

		if (i >= effects.size())
			break;

		if (effects.size() > 0 && effects[i]->end)
		{
			delete effects[i];
			effects.erase(effects.begin() + i);
			continue;
		}

		i++;
	}

	if (currCooldown <= 0 && cooldownTimerStarted == true)
	{
		cooldownTimerStarted = false;
	}

	// set the charecters weapon position
	if (weapon)
		weapon->SetPosition(m_ptPosition);

	// set the characters weapon rotation
	if (weapon)
		weapon->SetRotation(m_fRotation);

	plusBulletVector = m_vtVelocity;
	m_vtVelocity = {};
}

void Character::HandleCollision(const IEntity* pOther)
{
	// check to see if pOther is a character
	const Character* character = dynamic_cast<const Character*>(pOther);


	if (character)
	{
		if (character->GetCurrHealth() <= 0)
			return;

		characterCollision = true;
		//m_ptPosition -= m_vtVelocity;
	}

	/*if (character && character->GetClass() == ClassType::Mutant && ((Mutant*)character)->GetMutantType() != MutantType::RADIANT && characterCollision)
	{
		if (((Mutant*)character)->GetAttackTimer() <= 0)
		{
			if (((Mutant*)character)->GetMutantType() == MutantType::MUTT)
			{
				int r = rand() % 100;
				if (r < (10 + level * 2.6))
				{
					Radiation *rad = new Radiation(this);
					rad->SetStacks(2);
					AddStatusAilment(rad);
				}
			}
			else if (((Mutant*)character)->GetMutantType() == MutantType::RADICORN && ((Radicorn*)character)->GetDashing())
			{
				((Radicorn*)character)->SetMoveSpeed(350);
				Bleeding *bleed = new Bleeding(this);
				AddStatusAilment(bleed);
				Radiation *rad = new Radiation(this);
				rad->SetStacks(5);
				AddStatusAilment(rad);
				((Radicorn*)character)->SetDashing(false);
			}
			((Mutant*)character)->SetAttackTimer(((Mutant*)character)->GetRateOfAttack());
			ModifyHealth(-((Mutant*)character)->GetStrength());
		}

	}*/

	// check to see if the other object is a bullet
	const Bullet* bullet = dynamic_cast<const Bullet*>(pOther);

	// check to see if the character is shooting itself
	if (bullet && bullet->IsFoeAtk() == isEnemy && !bullet->deflectBullet)
		return;

	// if the shooter is a brawler
	if (bullet && bullet->GetOwner()->GetOwner() && bullet->GetOwner()->GetOwner()->GetClass() == ClassType::Brawler)
	{
		// if brawler is level 24+ gain health with each kill
		if (bullet->GetOwner()->GetOwner()->GetLevel() >= 24)
		{
			// if the bullet damage > this->characters currHealth, increase shooters health (brawler)
			if (bullet->GetDamage() > this->currHealth)
				bullet->GetOwner()->GetOwner()->ModifyHealth(bullet->GetOwner()->GetOwner()->GetHealth() * .03f);
		}
	}

	bool cheapShot = false;
	if (bullet && bullet->GetOwner() && bullet->GetOwner()->GetOwner())
	{
		bool piercing = false;

		if (bullet && bullet->GetOwner() && bullet->GetOwner()->GetOwner() && bullet->GetOwner()->GetOwner()->GetClass() == ClassType::Mutant)
		{
			int radChance = rand() % 100 + 1;
			int mutRad = (int)(bullet->GetOwner()->GetOwner()->GetLevel()*1.6f);
			mutRad += 10;
			if (radChance < mutRad)
				AddStatusAilment(new Radiation(this));

			if (reinterpret_cast<MutantAttack*>(bullet->GetOwner())->GetMutantAtkType() == MutantAttack::MutantAtkType::MUTTS)
			{
				Emitter* bit = GameplayState::GetInstance()->CreateEmitter("Bite");
				bit->SetPosition(AnimationSystem::GetInstance()->GetAnchorPoint(GetAnimation(), GetPosition()));
				bit->lifeTime = 0.15f;
			}
			else if(reinterpret_cast<MutantAttack*>(bullet->GetOwner())->GetMutantAtkType() == MutantAttack::MutantAtkType::RAZARD)
			{
				Emitter* claw = GameplayState::GetInstance()->CreateEmitter("Slash");
				claw->SetPosition(AnimationSystem::GetInstance()->GetAnchorPoint(GetAnimation(), GetPosition()));
				claw->lifeTime = 0.15f;
			}

			SGD::AudioManager::GetInstance()->PlayAudio(GameplayState::GetInstance()->slash_sfx);
		}

		// Checking to see if the bullet is piercing
		for (unsigned int i = 0; i < bullet->GetOwner()->bulType.size(); i++)
		{
			if (bullet->GetOwner()->bulType[i] == fmj || bullet->GetOwner()->bulType[i] == FMJFTW)
				piercing = true;
		}

		// If it's not, adding to their knockback
		if (!piercing/* && bullPushTimer <= 0*/)
		{
			knockbackVector += (bullet->GetVelocity() / 50)  * (bullet->GetDamage()*0.01f);
			bullPushTimer = 0.2f;

			if(knockbackVector.x > 15)
				knockbackVector.x = 15;

			if(knockbackVector.x < -15)
				knockbackVector.x = -15;

			if(knockbackVector.y > 15)
				knockbackVector.y = 15;

			if(knockbackVector.y < -15)
				knockbackVector.y = -15;
		}

		if (bullet->GetOwner()->GetOwner())
		{
			if (bullet->GetOwner()->GetOwner()->GetClass() == ClassType::Medic)
			{
				int trans = rand() % 100 + 1;
				if (trans < bullet->GetOwner()->GetOwner()->GetLevel() + 50)
				{
					Transfusion *tran = new Transfusion(this);
					//need to make new push_back for status ailments
					AddStatusAilment(tran);
				}
			}
			else if (bullet->GetOwner()->GetOwner()->GetClass() == ClassType::Gunslinger)
			{
				if (bullet->GetOwner()->GetOwner()->GetLevel() >= 24 && bullet->GetOwner()->GetOwner()->GetAbility()->GetDuration() > 0)
				{
					Bleeding* bleed = new Bleeding(this);
					AddStatusAilment(bleed);
				}
			}
			else if (bullet->GetOwner()->GetOwner()->GetClass() == ClassType::Jane)
			{
				// for all status ailments
				for (auto i = effects.begin(); i != effects.end(); i++)
				{
					// if this is a bleed status ailment
					if ((*i)->GetType() == StatEffectType::Bleeding)
					{
						cheapShot = true;
						break;
					}
				}
			}
		}

		// for all status ailments
		for (auto i = effects.begin(); i != effects.end(); i++)
		{

			// if this is a transfusion status ailment
			if ((*i)->GetType() == StatEffectType::Transfusion)
			{
				const_cast<Bullet*>(bullet)->GetOwner()->GetOwner()->ModifyHealth(bullet->GetDamage()*0.35f);
				break;
			}
		}

		// if the bullet was fired by a flame thrower
		if (bullet->GetOwner()->GetGunType() == Weapon::GunType::flameThrower)
		{
			// apply burn status ailment
			AddStatusAilment(new Burning(this));
		}
		// if the bullet was fired by a buzzsaw launcher
		if (bullet->GetOwner()->GetGunType() == Weapon::GunType::chainsawLauncher)
		{
			if (GetClass() == ClassType::Mutant && ((((Mutant*)this)->GetMutantType() == MutantType::RAZARD && this->GetLevel() >= 12) || ((Mutant*)this)->GetMutantType() == MutantType::RADZILLA));
			else// apply bleeding status ailment

				AddStatusAilment(new Bleeding(this));
		}

		if (bullet->GetOwner()->GetOwner())
		{
			// if the shooter is a brawler
			if (bullet && bullet->GetOwner()->GetOwner()->GetClass() == ClassType::Brawler)
			{

				// if brawler is level 24+ gain health with each kill
				if (bullet->GetOwner()->GetOwner()->GetLevel() >= 24)
				{
					// if the bullet damage > this->characters currHealth, increase shooters health (brawler)
					if (bullet->GetDamage() > this->currHealth)
						bullet->GetOwner()->GetOwner()->ModifyHealth(bullet->GetOwner()->GetOwner()->GetHealth() * .03f);
				}
			}

			// if the shooter is a sniper
			if (bullet && bullet->GetOwner()->GetOwner()->GetClass() == ClassType::Sniper)
			{
				// @ level 6, Add bleeding damage if the critPassive check passed (checked in bullet.cpp)
				if (bullet->GetOwner()->GetOwner()->GetLevel() >= 6 && bullet->GetCritPassive())
				{
					if (GetClass() == ClassType::Mutant && ((((Mutant*)this)->GetMutantType() == MutantType::RAZARD && this->GetLevel() >= 12) || ((Mutant*)this)->GetMutantType() == MutantType::RADZILLA));
					else
					{
						// add bleeding for 2.5 seconds
						Bleeding* bleed = new Bleeding(this);
						bleed->SetDuration(2.5f);
						AddStatusAilment(bleed);
					}
				}

				// @ level 18, Add stun effect if the critPassive check passed (checked in bullet.cpp)
				if (bullet->GetOwner()->GetOwner()->GetLevel() >= 18 && bullet->GetCritPassive())
				{
					Stun* stun = new Stun(this);
					stun->SetDuration(0.5f);
					AddStatusAilment(stun);
				}

				// @ level 25, Add AoE slow effect if the critPassive check passed (checked in bullet.cpp)
				if (bullet->GetOwner()->GetOwner()->GetLevel() >= 24)
				{
					// if the bullet damage > this->characters currHealth, set off AoE slow

					if (bullet->GetDamage() > this->currHealth*1.2f)
					{
						// loop through NPC vector
						for (unsigned i = 0; i < GameplayState::GetInstance()->npcs.size(); i++)
						{
							// no need to slow this character its about to die
							if (GameplayState::GetInstance()->npcs[i]->getCharacter() == this)
								continue;

							// calculate distance from this->character to npcs[i]
							SGD::Vector dist = GameplayState::GetInstance()->npcs[i]->getCharacter()->GetPosition() - GetPosition();
							// if within AoE range (300)
							if (dist.ComputeLength() < 300)
							{
								// add slow effect to current npc
								Slow* slow = new Slow(GameplayState::GetInstance()->npcs[i]->getCharacter());
								slow->SetDuration(3.0f);
								AddStatusAilment(slow);
							}
						}
						SGD::AudioManager::GetInstance()->PlayAudio(GameplayState::GetInstance()->bloodSpolsion);
					}
				}
			}
		}




		if (bullet && !GameplayState::GetInstance()->GetPaused() && (bullet->IsFoeAtk() != isEnemy || bullet->deflectBullet))

		{
			//when character loses health, play a random grunt sound effect.
			if (this->GetClass() == ClassType::Mutant && ((Mutant *)(this))->GetMutantType() == MutantType::RADIANT)
			{
				int ran = rand() % 100;
				if (ran < 10 + level)
				{
					const_cast<Bullet*>(bullet)->deflectBullet = true;
					const_cast<Bullet*>(bullet)->SetVelocity(-bullet->GetVelocity());
					const_cast<Bullet*>(bullet)->SetDamage(bullet->GetDamage()*0.5f);
					const_cast<Bullet*>(bullet)->SetIsEnemyAtk(!bullet->IsFoeAtk());
				}
			}
			if (!bullet->deflectBullet)
			{
				unsigned int i = rand() % 3;
				if (i == 0)
					SGD::AudioManager::GetInstance()->PlayAudio(GameplayState::GetInstance()->grunt);
				else if (i == 1)
					SGD::AudioManager::GetInstance()->PlayAudio(GameplayState::GetInstance()->grunt2);
				else if (i == 2)
					SGD::AudioManager::GetInstance()->PlayAudio(GameplayState::GetInstance()->grunt3);

				//drop blood particles when someone loses health
				Emitter *e = GameplayState::GetInstance()->CreateEmitter("bloodEffect");
				e->SetPosition(this->GetPosition());
				e->SetFollowTarget(this);
				GameplayState::GetInstance()->m_pEntities->AddEntity(e, PARTICLES);


				bool ignoreDefense = false;
				//loop through the vector of bullet types in the weapon
				for (unsigned int i = 0; i < bullet->GetOwner()->bulType.size(); i++)
				{
					//if we can assume this is a futuristic desert eagle, we know we should be applying an extra 25% critical hit chance
					if (bullet->GetOwner()->bulType[i] == BulletType::critChance && bullet->GetOwner()->GetGunType() == Weapon::GunType::Pistol)
					{
						bullet->GetOwner()->GetOwner()->SetCritHitChance(GetCritHitChance() + 25);
						//only way for me to modify this damn bullet, const_cast FTW

						bullet->GetOwner()->GetOwner()->SetCritHitChance(GetCritHitChance() - 25);
					}
					else if (bullet->GetOwner()->bulType[i] == BulletType::fire)
					{
						if (GetClass() == ClassType::Mutant && ((((Mutant*)this)->GetMutantType() == MutantType::RAZARD && this->GetLevel() >= 12) || ((Mutant*)this)->GetMutantType() == MutantType::RADZILLA));
						else
						{
							Burning *b = new Burning(this);

							AddStatusAilment(b);
						}
					}

					//all mutants are immune to radiation
					else if (bullet->GetOwner()->bulType[i] == BulletType::radiation && GetClass() != ClassType::Mutant)
					{
						Radiation *rad = new Radiation(this);
						if (bullet->GetOwner()->GetName() == "Barret .50 cal")
							rad->SetStacks(3);
						else if (bullet->GetOwner()->GetName() == "Intervention")
							rad->SetStacks(5);

						AddStatusAilment(rad);



					}
					else if (bullet->GetOwner()->bulType[i] == BulletType::FMJFTW)
					{
						//ignore defense
						ignoreDefense = true;
					}
					else if (bullet->GetOwner()->bulType[i] == BulletType::lifesteal)
					{
						if (GetClass() == ClassType::Mutant && ((((Mutant*)this)->GetMutantType() == MutantType::RAZARD && this->GetLevel() >= 12) || ((Mutant*)this)->GetMutantType() == MutantType::RADZILLA));
						else
						{
							Transfusion *tran = new Transfusion(this);

							AddStatusAilment(tran);
						}
					}
					else if (bullet->GetOwner()->bulType[i] == BulletType::multiplyingShots && !bullet->alreadyMultiplied)
					{
						SetAlreadyHit(true);
						for (unsigned int i = 0; i < 3; i++)
						{
							CreateBulletMsg *cbm = new CreateBulletMsg(const_cast<Bullet*>(bullet), this);
							SGD::MessageManager::GetInstance()->QueueMessage(cbm);
						}
					}
				}
				// find what the damage of this bullet should be

				float damage = 0.0f;
				if (ignoreDefense)
					damage = Math::Clamp(bullet->GetDamage(), 1.f, float(INT_MAX));
				else if (cheapShot)
					damage = Math::Clamp((bullet->GetDamage()*1.25f) - GetStat(StatType::defense) * 0.015f, 1.f, float(INT_MAX));
				else
					damage = Math::Clamp((bullet->GetDamage()) - GetStat(StatType::defense) * 0.015f, 1.f, float(INT_MAX));

				if (bullet->GetGunActive())
					damage *= bullet->GetDistance();

				// modify health according to 'damage'
				if (!healthDebug)
					ModifyHealth(-damage);

				NPC* aggro = FindControllingNPC(this);
				if (aggro)
				{
					if (!aggro->GetTarget() && bullet->GetOwner()->GetOwner())
						aggro->SetNearestTarget(true);
				}
			}
		}
	}
	// if the other object is a radiation pool
	if (dynamic_cast<const RadiationPool*>(pOther))
	{
		AddStatusAilment(new Radiation(this));
		if (radiationSoundTimer <= 0.f)
		{
			SGD::AudioManager::GetInstance()->PlayAudio(GameplayState::GetInstance()->geigerCounter, false);
			radiationSoundTimer = 5.f;
		}
	}
	//else
	//	SGD::AudioManager::GetInstance()->StopAudio(GameplayState::GetInstance()->geigerCounter);

	if (dynamic_cast <const Pitfall*>(pOther))
	{
		if (pitfallSoundTimer <= 0.f)
		{
			SGD::AudioManager::GetInstance()->PlayAudio(GameplayState::GetInstance()->pitfallSound, false);
			ModifyHealth(-(GetHealth()*0.05f));
			pitfallSoundTimer = 5.f;
		}
		Stun *stlblflrbr = new Stun(this);
		stlblflrbr->SetDuration(1.5f);
		AddStatusAilment(stlblflrbr);
		DestroyEntityMsg *dem = new DestroyEntityMsg(((Entity*)(pOther)));
		dem->QueueMessage();
		return;
	}
	if (dynamic_cast <const BearTrap*>(pOther))
	{
		if (bearTrapSoundTimer <= 0.f)
		{
			SGD::AudioManager::GetInstance()->PlayAudio(GameplayState::GetInstance()->bearTrapSound, false);
			ModifyHealth(-(GetHealth()*0.05f));
			bearTrapSoundTimer = 1.f;
		}
		Bleeding *stlblflrbr = new Bleeding(this);
		AddStatusAilment(stlblflrbr);
	}

	// check to see if the other object is a bullet
	// if it is, modify health, according to the bullets 'damage' attribute

	//if player collides with a friendly NPC that is not yet in their party

	if (dynamic_cast<const Character*>(pOther) && GameplayState::GetInstance()->player && GameplayState::GetInstance()->player->GetCharacter() == this && ((Character*)pOther)->isEnemy == false)
	{
		Character* newAlly = (Character*)pOther;
		NPC* tempNPC = FindControllingNPC(newAlly);

		if (newAlly->IsACharacter() && newAlly->GetIsEnemy() == false)
		{
			if (timer > 50)
			{
				recruitEmitterCreated = false;
				timer = 0;
			}
			timer++;
			for (unsigned int i = 0; i < GameplayState::GetInstance()->player->party.size(); i++)
			{
				//make sure that the character isn't already in your party
				if (newAlly == GameplayState::GetInstance()->player->party[i]->getCharacter())
				{
					GameplayState::GetInstance()->collidingWithAlly = true;
					// cast entity pointer into weapon pointer
					Weapon* pWep = newAlly->weapon;
					//Ask if the player wants to switch weapons with thier ally

					//string s = "Press 'E' To Swap " + string(weapon->GetPrefixName()) + ' ' + string(weapon->GetName()) + " for allies " + string(pWep->GetPrefixName()) + ' ' + string(pWep->GetName());
					//font->Draw(s.c_str(), 400 - (int)font->MeasureString(s.c_str()) / 2, 500, 0.75f, { 255, 255, 255 });
					if (!GameplayState::GetInstance()->tutorial)
					{
						GameplayState::GetInstance()->gameplayDialogue->SetWeaponSwapDialogue(weapon, pWep);
						//GameplayState::GetInstance()->gameplayDialogue->SetStringToDisplay(" ");
						//Game::GetInstance()->GetFont()->DrawStoredString(210, SCREENHEIGHT - 140, .7f);
					}
					//if (!GameplayState::GetInstance()->collidingWithAlly)
					GameplayState::GetInstance()->SetWeaponCompare(pWep);

					if (!GameplayState::GetInstance()->pausedSave && (ARCADEBUILD == 1 && (SGD::InputManager::GetInstance()->IsButtonPressed(0, 5))) || (!GameplayState::GetInstance()->pausedSave && (SGD::InputManager::GetInstance()->IsKeyPressed(SGD::Key::E) || SGD::InputManager::GetInstance()->IsButtonPressed(0, 0))))
					{
						//weapon swapping with allies
						Weapon *temp = weapon;
						weapon = pWep;
						newAlly->weapon = temp;
						newAlly->weapon->SetOwner(newAlly);
						weapon->SetOwner(GameplayState::GetInstance()->player->GetCharacter());
						GameplayState::GetInstance()->gameplayDialogue->StopDrawing();
					}
					break;
				}
				else if (i == GameplayState::GetInstance()->player->party.size() - 1)
				{
					//else if you've been through all your party members and this isn't one of them have them ask if they can join
					if (!GameplayState::GetInstance()->collidingWithAlly)
						GameplayState::GetInstance()->comparingWeapons = false;
					//if (!GameplayState::GetInstance()->inRecruitmentDialogue)
					//{
					//	string s = "Press 'E' To Recruit Nearby NPCs ";
					//	GameplayState::GetInstance()->gameplayDialogue->SetStringToDisplay(s);
					//}
					if (!recruitEmitterCreated)
					{
						recruitEmitterCreated = true;
						Emitter *e = GameplayState::GetInstance()->CreateEmitter("recruitEffectv2");
						e->SetPosition(newAlly->GetPosition() + newAlly->GetSize() / 2);
						e->SetFollowTarget(newAlly);
						e->lifeTime = .75f;
					}
					if (!GameplayState::GetInstance()->pausedSave && (ARCADEBUILD == 1 && (SGD::InputManager::GetInstance()->IsButtonPressed(0, 5))) || (!GameplayState::GetInstance()->pausedSave && (SGD::InputManager::GetInstance()->IsKeyPressed(SGD::Key::E) || SGD::InputManager::GetInstance()->IsButtonPressed(0, 0))))
					{
						if (GameplayState::GetInstance()->currentTutorialObjective == Tutorial::Recruitment)
						{
							GameplayState::GetInstance()->currentTutorialObjective++;
							GameplayState::GetInstance()->sinceLastTutorial = 0.0f;
						}

						GameplayState::GetInstance()->gameplayDialogue->StopDrawing();
						//if (GameplayState::GetInstance()->GetInstance()->player->party.size() < (unsigned int)3 - Game::GetInstance()->playerDifficulty)
						//{
						//	tempNPC->SetAnchor(GameplayState::GetInstance()->player->GetCharacter());
						//	GameplayState::GetInstance()->player->party.push_back(tempNPC);
						//	break;
						//}
						//else
						//{
						//	//ask player if they would like to switch a character from their party for the new character
						//	GameplayState::GetInstance()->SetPaused(true);
						//	GameplayState::GetInstance()->SetCharacterSelect(true);
						//	GameplayState::GetInstance()->SetSwapParty(true);
						//	GameplayState::GetInstance()->newPartyMember = tempNPC;
						//}
						//TODO{
						GameplayState::GetInstance()->inRecruitmentDialogue = true;
						GameplayState::GetInstance()->npcBeingSpokenTo = tempNPC;
					}
				}
			}

		}
		if (GameplayState::GetInstance()->player->party.size() == 0)
		{
			//create golden particles to appear around the character that wants to join your party

			//if (!GameplayState::GetInstance()->inRecruitmentDialogue)
			//{
			//	string s = "Your Party Is Empty!\nPress 'E' To Recruit Nearby NPCs ";
			//	GameplayState::GetInstance()->gameplayDialogue->SetStringToDisplay(s);
			//}
			if (!recruitEmitterCreated)
			{
				recruitEmitterCreated = true;
				Emitter *e = GameplayState::GetInstance()->CreateEmitter("recruitEffectv2");
				e->SetPosition(newAlly->GetPosition() + newAlly->GetSize() / 2);
				e->SetFollowTarget(newAlly);
				e->lifeTime = .75f;
			}
			//SGD::GraphicsManager::GetInstance()->DrawString("Click 'E' to have me join your party.", { 300, 500 }, { 255, 255, 255 });
			//if (SGD::InputManager::GetInstance()->IsKeyPressed(SGD::Key::E))
			//{
			//	tempNPC->SetAnchor(GameplayState::GetInstance()->player->GetCharacter());
			//	GameplayState::GetInstance()->player->party.push_back(tempNPC);
			//}

			if (!GameplayState::GetInstance()->pausedSave && (ARCADEBUILD == 1 && (SGD::InputManager::GetInstance()->IsButtonPressed(0, 5))) || (!GameplayState::GetInstance()->pausedSave && (SGD::InputManager::GetInstance()->IsKeyPressed(SGD::Key::E) || SGD::InputManager::GetInstance()->IsButtonPressed(0, 0))))
			{
				if (GameplayState::GetInstance()->currentTutorialObjective == Tutorial::Recruitment)
				{
					GameplayState::GetInstance()->currentTutorialObjective++;
					GameplayState::GetInstance()->sinceLastTutorial = 0.0f;
				}

				GameplayState::GetInstance()->inRecruitmentDialogue = true;
				GameplayState::GetInstance()->npcBeingSpokenTo = tempNPC;
			}
		}
	}
	//If the player is standing on a weapon, give them the option to swap it with their current one
	else if (pOther && pOther->GetType() == Entity::ENT_WEAPON)
	{
		// cast entity pointer into weapon pointer
		Weapon* pWep = (Weapon*)pOther;
		//Ask if the player wants to pick up the weapon on the ground
		GameplayState::GetInstance()->player->SetOtherWeapon((Weapon*)pOther);

		if (!GameplayState::GetInstance()->tutorial)
		{
			GameplayState::GetInstance()->gameplayDialogue->SetWeaponSwapDialogue(weapon, pWep);
			//string s = "Press 'E' To Swap " + string(string(weapon->GetPrefixName()) + ' ' + weapon->GetName()) + " with " + string(pWep->GetPrefixName()) + ' ' + pWep->GetName();
			//GameplayState::GetInstance()->gameplayDialogue->SetStringToDisplay(" ");
			//Game::GetInstance()->GetFont()->DrawStoredString(210, SCREENHEIGHT - 140, .7f);
		}

		//if (!GameplayState::GetInstance()->collidingWithAlly)
		GameplayState::GetInstance()->SetWeaponCompare(pWep);

		if (!GameplayState::GetInstance()->pausedSave && ((ARCADEBUILD == 1 && SGD::InputManager::GetInstance()->IsButtonPressed(0, 5)) || (!GameplayState::GetInstance()->pausedSave && (SGD::InputManager::GetInstance()->IsKeyPressed(SGD::Key::E) || SGD::InputManager::GetInstance()->IsButtonPressed(0, 0)))))
			//((ARCADEBUILD == 1 && (SGD::InputManager::GetInstance()->IsButtonPressed(0, 5)) || (SGD::InputManager::GetInstance()->IsKeyPressed(SGD::Key::E)))))
		{
			EventProc::GetInstance()->Dispatch("Pickup Weapon");
			SGD::AudioManager::GetInstance()->PlayAudio(GameplayState::GetInstance()->weaponPickup, false);
		}
	}
	//If the player collides with a medkit, heal their party
	else  if (pOther && pOther->GetType() == Entity::ENT_MEDKIT && !isEnemy && (inParty || this == GameplayState::GetInstance()->player->GetCharacter()))
	{
		GameplayState::GetInstance()->comparingWeapons = false;

		if (GameplayState::GetInstance()->currentTutorialObjective == Tutorial::HealthKit)
		{
			GameplayState::GetInstance()->currentTutorialObjective++;
			GameplayState::GetInstance()->sinceLastTutorial = 0.0f;
		}

		//SGD::AudioManager::GetInstance()->SetAudioVolume(GameplayState::GetInstance()->healthPickupSFX, OptionsState::GetInstance()->getSFXVolume());
		//SGD::AudioManager::GetInstance()->PlayAudio(GameplayState::GetInstance()->healthPickupSFX);
		EventProc::GetInstance()->Dispatch("Pickup Health Kit");
	}

	else
	{
		const Entity* tempHit = reinterpret_cast<const Entity*>(pOther);
		if (tempHit && tempHit->GetType() == EntityType::ENT_ABILITY)

			switch (tempHit->GetAbilityType())
		{
			case AbilityType::MEDIC_ACT:
			{
				const Ambulance* ambul = reinterpret_cast<const Ambulance*>(tempHit);

				if (ambulHit <= 0)
				{
					const Ambulance* ambul = reinterpret_cast<const Ambulance*>(tempHit);
					if (isEnemy)
					{
						if (ambul && !ambul->isFirstHit() && ambulHit <= 0)
						{
							ModifyHealth(-ambul->GetDamage()*2.5f);
							if (currHealth <= 0)
								const_cast<Ambulance*>(ambul)->HandleSpecial(true);
							else
								const_cast<Ambulance*>(ambul)->HandleSpecial(false);

							ambulHit = 3.0f;
						}
						else if (ambulHit <= 0)
						{
							ModifyHealth(-ambul->GetDamage()*1.5f);
							ambulHit = 3.0f;
						}

						unsigned int i = rand() % 3;
						if (i == 0)
							SGD::AudioManager::GetInstance()->PlayAudio(GameplayState::GetInstance()->grunt);
						else if (i == 1)
							SGD::AudioManager::GetInstance()->PlayAudio(GameplayState::GetInstance()->grunt2);
						else if (i == 2)
							SGD::AudioManager::GetInstance()->PlayAudio(GameplayState::GetInstance()->grunt3);

						if (ambul->GetSkillLvl() >= 2)
						{

							Transfusion *tran = new Transfusion(this);
							this->effects.push_back(tran);
							Stun* stun = new Stun(this);
							stun->SetDuration(2.5f);
							AddStatusAilment(stun);
						}

						if (ambul->GetSkillLvl() >= 3)
						{
							Burning *b = new Burning(this);
							AddStatusAilment(b);
						}

						if (FindControllingNPC(this))
							FindControllingNPC(this)->setTarget(ambul->GetOwner());
					}
				}
				break;
			}

			case AbilityType::SNIPE_ACT:
			{
				const LaserBullet* beam = reinterpret_cast<const LaserBullet*>(tempHit);

				if (beam->IsFoeAtk() != isEnemy && hitTimer <= 0)
				{
					ModifyHealth(-beam->GetDamage());
					hitTimer = 0.075f;
					//apply slow and any other effect for higher level skill
					this->AddStatusAilment(new Slow(this));

					if (beam->GetSkillLvl() >= 2)
						AddStatusAilment(new Radiation(this));

					if (FindControllingNPC(this))
						FindControllingNPC(this)->setTarget(beam->GetOwner()->GetOwner());
				}
			}
				break;
			case AbilityType::RADICORN_ACT:
			{
				const Stampede* dash = reinterpret_cast<const Stampede*>(tempHit);

				if (ambulHit <= 0)
				{
					const Stampede* dash = reinterpret_cast<const Stampede*>(tempHit);
					if (!isEnemy)
					{
						if (!dash->isFirstHit() && ambulHit <= 0)
						{
							ModifyHealth(-dash->GetDamage()*1.75f);
							ambulHit = 3.0f;
							const_cast<Stampede*>(dash)->HandleSpecial();
						}
						else if (ambulHit <= 0)
						{
							ModifyHealth(-dash->GetDamage()*1.15f);
							ambulHit = 3.0f;
						}

						Radiation* rad = new Radiation(this);
						rad->SetStacks(8);
						AddStatusAilment(rad);
						AddStatusAilment(new Bleeding(this));
					}
				}
				break;
			}

			case AbilityType::CYBORG_ACT:
			{
				const Nuke* boom = reinterpret_cast<const Nuke*>(tempHit);
				if (boom->GetIsFoe() != isEnemy)
				{
					if (boom->HasExplode())
					{
						//apply stun
						this->AddStatusAilment(new Stun(this));
						unsigned int i = rand() % 3;
						if (i == 0)
							SGD::AudioManager::GetInstance()->PlayAudio(GameplayState::GetInstance()->grunt);
						else if (i == 1)
							SGD::AudioManager::GetInstance()->PlayAudio(GameplayState::GetInstance()->grunt2);
						else if (i == 2)
							SGD::AudioManager::GetInstance()->PlayAudio(GameplayState::GetInstance()->grunt3);

					}

					ModifyHealth(-boom->GetDamage());


					AddStatusAilment(new Radiation(this));

					if (FindControllingNPC(this))
						FindControllingNPC(this)->setTarget(boom->GetOwner());
				}

				if (boom->GetSkillLvl() >= 2 && !boom->HasExplode() && boom->GetIsFoe() == isEnemy)
					ModifyHealth(GetStat(StatType::health)*0.04f);
			}
				break;
			case AbilityType::MORTAR_ACT:
			{
				const Mortar* boom = reinterpret_cast<const Mortar*>(tempHit);
				if (!isEnemy && !boom->HasHit() && boom->GetDropTimer() < 0.5f && boom->GetDropTimer() > 0.0f)
				{
					ModifyHealth(-boom->GetDamage());
					Radiation* rad = new Radiation(this);
					rad->SetStacks(10);
					AddStatusAilment(rad);
					const_cast<Mortar*>(boom)->hasHit = true;
				}
				else if (!isEnemy && boom->GetDropTimer() <= 0)
					AddStatusAilment(new Radiation(this));
			}
				break;
			case AbilityType::BRAWL_ACT:
			{
				const LaserBullet* rev = reinterpret_cast<const LaserBullet*>(tempHit);

				if (rev->IsFoeAtk() != isEnemy && hitTimer <= 0)
				{
					hitTimer = 0.25f;
					ModifyHealth(-rev->GetDamage());
					AddStatusAilment(new Bleeding(this));

					if (rev->GetSkillLvl() >= 2)
					{
						Stun* bleed = new Stun(this);
						bleed->SetDuration(0.15f);
						AddStatusAilment(bleed);
					}
				}

				if (FindControllingNPC(this))
					FindControllingNPC(this)->setTarget(rev->GetOwner()->GetOwner());
			}
				break;
		}

		// if the other object is a pitfall
		if (dynamic_cast<const Pitfall*>(pOther))
		{
			// cast the other object as a Pitfall
			Pitfall* pitfall = (Pitfall*)pOther;

			// if the other pitfall is not currently active
			if (!pitfall->GetIsActive())
			{
				effects.push_back(new Stun(this));
				pitfall->SetDuration(10.f);
			}
		}
	}
}

void Character::SetCurrCooldown(float cool)
{
	currCooldown = cool;
	if (currCooldown < 0)
		currCooldown = 0;
}

int Character::GetStartingPrefix(int chance)
{
	switch (Game::GetInstance()->playerDifficulty)
	{
	case 0:
	{
		if (chance > 0 && chance < 30)
			return 0;
		else if (chance > 30 && chance < 65)
			return 1;
		else if (chance > 65 && chance < 85)
			return 2;
		else if (chance > 85 && chance < 95)
			return 3;
		else if (chance > 95)
			return 4;
	}
		break;
	case 1:
	{
		if (chance > 0 && chance < 20)
			return 0;
		else if (chance > 20 && chance < 40)
			return 1;
		else if (chance > 40 && chance < 60)
			return 2;
		else if (chance > 60 && chance < 85)
			return 3;
		else if (chance > 85)
			return 4;
	}
		break;
	case 2:
	{
		if (chance > 0 && chance < 5)
			return 0;
		else if (chance > 5 && chance < 20)
			return 1;
		else if (chance > 20 && chance < 45)
			return 2;
		else if (chance > 45 && chance < 70)
			return 3;
		else if (chance > 70)
			return 4;
	}
		break;
	}

	return 0;
}

void Character::Render()
{
	//If too far then dont render
	if ((m_ptPosition - GameplayState::GetInstance()->player->GetPosition()).ComputeLength() > Game::GetInstance()->GetScreenWidth()) return;

	if (charAnim.GetCurrAnimation() != "")
	{
		if (!isEnemy || GetClass() == ClassType::Jane || (GetClass() == ClassType::Mutant && reinterpret_cast<Mutant*>(this)->GetMutantType() == MutantType::RADZILLA))
			AnimationSystem::GetInstance()->Render(charAnim, SGD::Point(m_ptPosition.x + m_szSize.width*0.5f, m_ptPosition.y + m_szSize.height*0.5f), m_fRotation, SGD::Color::White, { 1.f, 1.f });
		else
			AnimationSystem::GetInstance()->Render(charAnim, SGD::Point(m_ptPosition.x + m_szSize.width*0.5f, m_ptPosition.y + m_szSize.height*0.5f), m_fRotation, SGD::Color(125, 0, 0));
	}
	else
		Entity::Render();

	if (weapon != nullptr && GetClass() != ClassType::Mutant)
		weapon->Render();

	SGD::Point weapon = AnimationSystem::GetInstance()->GetWeaponPoint(charAnim, m_ptPosition, m_fRotation);

	if (isEnemy && currHealth < health)
	{
		SGD::GraphicsManager::GetInstance()->DrawRectangle({ GetPosition().x, GetPosition().y - 15.0f, GetPosition().x + 50.0f, GetPosition().y - 5.0f }, { 25, 25, 25 });
		SGD::GraphicsManager::GetInstance()->DrawRectangle({ GetPosition().x, GetPosition().y - 15.0f, GetPosition().x + ((currHealth / health * 50)), GetPosition().y - 5.0f }, { 25, 255, 25 });
	}
}

void Character::CheckTileCollision()
{
	//==================================== Inilitize local values =====================================================================================
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

	SGD::Point index_up_left = { index_left.x, index_up.y };
	SGD::Point index_up_right = { index_right.x, index_up.y };

	SGD::Point index_down_left = { index_left.x, index_down.y };
	SGD::Point index_down_right = { index_right.x, index_down.y };

	//Tile* currTile = collisionLayer->GetTileAt((int)index.x, (int)index.y);
	//if (currTile->isPassable)
	//{
	// retrieve neighboring tiles
	Tile* tile_up = collisionLayer->GetTileAt((int)index_up.x, (int)index_up.y);
	Tile* tile_down = collisionLayer->GetTileAt((int)index_down.x, (int)index_down.y);
	Tile* tile_left = collisionLayer->GetTileAt((int)index_left.x, (int)index_left.y);
	Tile* tile_right = collisionLayer->GetTileAt((int)index_right.x, (int)index_right.y);
	Tile* tile_up_left = collisionLayer->GetTileAt((int)index_up_left.x, (int)index_up_left.y);
	Tile* tile_up_right = collisionLayer->GetTileAt((int)index_up_right.x, (int)index_up_right.y);
	Tile* tile_down_left = collisionLayer->GetTileAt((int)index_down_left.x, (int)index_down_left.y);
	Tile* tile_down_right = collisionLayer->GetTileAt((int)index_down_right.x, (int)index_down_right.y);
	Tile* tile_at = collisionLayer->GetTileAt((int)index.x, (int)index.y);

	// the ammount to offset by (if we collided with a tile)
	SGD::Vector offset{ 0.f, 0.f };
	// recycleable local variable for storing the ammount of intersect with a particular tile
	SGD::Rectangle intersect = { 0, 0, 0, 0 };

	// dispatch tile events, if they have any. ***this is assuming that the player is the only one who will ever dispatch a tile event
	if (GameplayState::GetInstance()->player && this == GameplayState::GetInstance()->player->GetCharacter() && GameplayState::GetInstance()->player->GetCharacter()->GetCurrHealth() > 0)
	{
		if (tile_right && tile_right->event != "")
			EventProc::GetInstance()->Dispatch(tile_right->event.c_str(), this);
		if (tile_up && tile_up->event != "")
			EventProc::GetInstance()->Dispatch(tile_up->event.c_str(), this);
		if (tile_left && tile_left->event != "")
			EventProc::GetInstance()->Dispatch(tile_left->event.c_str(), this);
		if (tile_down && tile_down->event != "")
			EventProc::GetInstance()->Dispatch(tile_down->event.c_str(), this);
		if (tile_at && tile_at->event != "")
			EventProc::GetInstance()->Dispatch(tile_at->event.c_str(), this);
	}
	// resolve collision 
	if (tile_up && !tile_up->isPassable)
	{
		intersect = { index_up.x * tileSize_width, index_up.y * tileSize_height, index_up.x * tileSize_width + tileSize_width, index_up.y * tileSize_height + tileSize_height };
		if (GetRect().IsIntersecting(intersect))
		{
			offset.y += (int)abs(GetRect().ComputeIntersection(intersect).ComputeHeight());
			m_vtVelocity.y = Math::Clamp(m_vtVelocity.y, 0.f, float(INT_MAX));
			if (dynamic_cast<Radicorn*>(this) && ((Radicorn*)this)->GetDashing())
			{
				//if a radicorn dashes into a wall stop it from dashing and subtract 5% of its max health from its currHealth and stun it 
				((Radicorn*)this)->SetDashing(false);
				ModifyHealth(-GetHealth() / 20);
				Stun* s = new Stun(this);
				s->SetDuration(1.5f);
				AddStatusAilment(s);
			}
		}
	}
	if (tile_left && !tile_left->isPassable)
	{
		intersect = { index_left.x * tileSize_width, index_left.y * tileSize_height, index_left.x * tileSize_width + tileSize_width, index_left.y * tileSize_height + tileSize_height };
		if (GetRect().IsIntersecting(intersect))
		{
			offset.x += (int)abs(GetRect().ComputeIntersection(intersect).ComputeWidth());
			m_vtVelocity.x = Math::Clamp(m_vtVelocity.x, 0.f, float(INT_MAX));
			if (dynamic_cast<Radicorn*>(this) && ((Radicorn*)this)->GetDashing())
			{
				//if a radicorn dashes into a wall stop it from dashing and subtract 5% of its max health from its currHealth and stun it 
				((Radicorn*)this)->SetDashing(false);
				ModifyHealth(-GetHealth() / 20);
				Stun* s = new Stun(this);
				s->SetDuration(1.5f);
				AddStatusAilment(s);
			}
		}
	}
	if (tile_down && !tile_down->isPassable)
	{
		intersect = { index_down.x * tileSize_width, index_down.y * tileSize_height, index_down.x * tileSize_width + tileSize_width, index_down.y * tileSize_height + tileSize_height };
		if (GetRect().IsIntersecting(intersect))
		{
			offset.y -= (int)abs(GetRect().ComputeIntersection(intersect).ComputeHeight());
			m_vtVelocity.y = Math::Clamp(m_vtVelocity.y, float(INT_MIN), 0.f);
			if (dynamic_cast<Radicorn*>(this) && ((Radicorn*)this)->GetDashing())
			{
				//if a radicorn dashes into a wall stop it from dashing and subtract 5% of its max health from its currHealth and stun it 
				((Radicorn*)this)->SetDashing(false);
				ModifyHealth(-GetHealth() / 20);
				Stun* s = new Stun(this);
				s->SetDuration(1.5f);
				AddStatusAilment(s);
			}
		}
	}
	if (tile_right && !tile_right->isPassable)
	{
		intersect = { index_right.x * tileSize_width, index_right.y * tileSize_height, index_right.x * tileSize_width + tileSize_width, index_right.y * tileSize_height + tileSize_height };
		if (GetRect().IsIntersecting(intersect))
		{
			offset.x -= (int)abs(GetRect().ComputeIntersection(intersect).ComputeWidth());
			m_vtVelocity.x = Math::Clamp(m_vtVelocity.x, float(INT_MIN), 0.f);
			if (dynamic_cast<Radicorn*>(this) && ((Radicorn*)this)->GetDashing())
			{
				//if a radicorn dashes into a wall stop it from dashing and subtract 5% of its max health from its currHealth and stun it 
				((Radicorn*)this)->SetDashing(false);
				ModifyHealth(-GetHealth() / 20);
				Stun* s = new Stun(this);
				s->SetDuration(1.5f);
				AddStatusAilment(s);
			}
		}	
	}

	m_ptPosition += offset;
	//}
}

// check to see if current character will collide with any other characters
bool Character::CheckCharacterCollision(SGD::Rectangle futureLocation)
{
	//if (GameplayState::GetInstance()->player->GetCharacter() == this)
	//	return true;

	for (unsigned int i = 0; i < GameplayState::GetInstance()->npcs.size(); i++)
	{
		if (!GameplayState::GetInstance()->npcs[i]->GetIsDead())
		{
			if (GameplayState::GetInstance()->player)
			{
				// If the current character is the player and the npc we're checking against is an ally, don't collide
				if (this == GameplayState::GetInstance()->player->GetCharacter() && !GameplayState::GetInstance()->npcs[i]->GetIsEnemy())
					continue;

				bool isPartyMember = false;

				// If this is a party member and the npc we're checking against is an ally, don't collide
				for (unsigned int j = 0; j < GameplayState::GetInstance()->player->party.size(); j++)
				{
					if (GameplayState::GetInstance()->player->party[j]->getCharacter() == this && !GameplayState::GetInstance()->npcs[i]->GetIsEnemy())
						isPartyMember = true;
				}

				if (isPartyMember)
					continue;

				// NPCs comparing to the player
				if (GameplayState::GetInstance()->player->GetCharacter() != this && futureLocation.IsIntersecting(GameplayState::GetInstance()->player->GetCharacter()->GetRect()) && !spawnCollision)
					// will collide with player, dont move there
					return false;
				if (GameplayState::GetInstance()->player->GetCharacter() != this && this->GetClass() != ClassType::Jane && !futureLocation.IsIntersecting(GameplayState::GetInstance()->player->GetCharacter()->GetRect()) && spawnCollision)
					spawnCollision = false;

				// EVERYONE checks against all NPCs (except themselves)
				if (GameplayState::GetInstance()->npcs[i]->getCharacter() && GameplayState::GetInstance()->npcs[i]->getCharacter() != this)
				{
					// check to see if rects are intersecting and isn't fellow partner
					if (futureLocation.IsIntersecting(GameplayState::GetInstance()->npcs[i]->getCharacter()->GetRect()) && !spawnCollision
						&& GameplayState::GetInstance()->npcs[i]->getCharacter()->GetIsEnemy() != isEnemy)
					{
						// will collide, dont move there
						return false;
					}
				}
			}
		}
	}
	// no collision, can move there
	return true;
}

void Character::RemoveStatusAilment(StatEffectType type)
{
	for (unsigned int i = 0; i < effects.size();)
	{
		if (effects[i]->GetType() == type)
		{
			delete effects[i];
			effects.erase(effects.begin() + i);
			return;
		}
		i++;
	}
}

string Character::GetClassString()
{
	switch (GetClass())
	{
	case ClassType::Brawler:
		return "Brawler";
		break;
	case ClassType::Cyborg:
		return "Cyborg";
		break;
	case ClassType::Medic:
		return "Medic";
		break;
	case ClassType::Sniper:
		return "Sniper";
		break;
	default:
		return "Gunslinger";
		break;
	}
}


void Character::HandleEvent(string name, void* args)
{
	if (name == "Avoided Trap")
	{
		if (GameplayState::GetInstance()->player && args == GameplayState::GetInstance()->player->GetCharacter())
		{
			if (GameplayState::GetInstance()->currentTutorialObjective == Tutorial::Trap)
			{
				GameplayState::GetInstance()->currentTutorialObjective++;
				GameplayState::GetInstance()->sinceLastTutorial = 0.0f;
			}
		}
	}
	else if (name == "meleeWeaponUsed")
	{
		// cast args as a melee weapon
		MeleeWeapon* wep = (MeleeWeapon*)args;
		// if the user of the weapon is not hostile, or there is no way we could have been hit
		// by the melee weapon: ignore this event
		if (wep->GetOwner())
		{
			if (wep->GetOwner()->GetIsEnemy() == isEnemy || (m_ptPosition - wep->GetPosition()).ComputeLength() > 75.f)
				return;
		}
		// find the rotation of the melee weapon
		float rotation = wep->GetRotation();

		// dispatch a hitmarker message
		CreateHitMarkerMsg* hit_msg = new CreateHitMarkerMsg((int)wep->GetDamage(), m_ptPosition, Math::to_radians(-90.f), SGD::Color::Yellow);
		SGD::MessageManager::GetInstance()->QueueMessage(hit_msg);

		// find where the melee weapon 'hit'
		SGD::Vector offset = SGD::Vector(0, -75.f);
		offset.Rotate(rotation);

		SGD::Point hit_location = wep->GetOwner()->GetPosition() + wep->GetOwner()->GetSize() / 2 + offset;
		float damage = 0.0f;
		// store current rect for math 'contains' computation
		SGD::Rectangle rect = GetRect();
		// see if we've been hit by the melee weapon
		if (Math::RectContains(&rect, &hit_location))
		{

			// find what the damage is
			//ignore 50% of defense if its a futuristic knife
			if (wep->GetPrefix() == GunPrefix::futuristic)
			{
				if (wep->GetName() == "Knife" && GetClass() != ClassType::Mutant)
				{
					//apply 2 stacks of radiation
					Radiation *rad = new Radiation(this);
					rad->SetStacks(2);
					AddStatusAilment(rad);


					damage = Math::Clamp(wep->GetDamage() - GetStat(StatType::defense) / 2 * 0.015f, 1.f, float(INT_MAX));
				}
				else if (wep->GetName() == "Sharp Pointy Stick")
				{
					if (GetClass() == ClassType::Mutant && ((((Mutant*)this)->GetMutantType() == MutantType::RAZARD && this->GetLevel() >= 12) || ((Mutant*)this)->GetMutantType() == MutantType::RADZILLA));
					else
					{
						Burning *b = new Burning(this);

						AddStatusAilment(b);
						if (GetClass() != ClassType::Mutant)
						{
							Radiation *rad = new Radiation(this);
							rad->SetStacks(5);

							AddStatusAilment(rad);
						}

						Transfusion *trans = new Transfusion(this);

						AddStatusAilment(trans);

						Bleeding *bleed = new Bleeding(this);

						AddStatusAilment(bleed);

						Slow *s = new Slow(this);

						AddStatusAilment(s);
					}
				}
				else if (wep->GetName() == "Sledgehammer")
				{
					//15% chance to stun an enemy hit with a futuristic sledghammer

					int ran = rand() % 100 + 1;
					if (ran <= 15)
					{
						Stun *s = new Stun(this);
						s->SetDuration(150);

						AddStatusAilment(s);
					}
				}
			}


			// brawler passive debug code
#if _DEBUG
			GameplayState::GetInstance()->fBaseDamageDealt = damage;
#endif

			if (this->GetClass() == ClassType::Brawler)
			{
				if (GetLevel() >= 15)
				{
					damage *= 1.15f;
					GameplayState::GetInstance()->fPassiveDamageDealt = damage;
				}

				// this is NOT the Player level 24+
				if (GetLevel() >= 24)
				{
					// if NPC will die, increase player health
					if (damage > GetCurrHealth())
					{
						// increase player health
						GameplayState::GetInstance()->player->GetCharacter()->ModifyHealth(GameplayState::GetInstance()->player->GetCharacter()->GetHealth() * .03f);
						// set test variable
						GameplayState::GetInstance()->fPassiveKillLifeGain = GameplayState::GetInstance()->player->GetCharacter()->GetHealth() * .03f;
					}
				}
			}

			// modify health according to 'damage'
			ModifyHealth(-damage);

		}
		return;
	}
	if (name == "GameExit")
	{
		SetWeapon(nullptr);
		if (ability)
		{
			ability->SetOwner(nullptr);
			ability->Release();
			ability = nullptr;
		}
	}

	// If none of the above situations were true, check the string vs your current objective


}

void Character::SetWeapon(Weapon* w)
{
	if (weapon)
	{
		weapon->SetOwner(nullptr);
		weapon->Release();
		weapon = nullptr;
	}
	if (w)
	{
		weapon = w;
		weapon->AddRef();
		weapon->SetOwner(this);
	}
}

void Character::AddStatusAilment(StatusEffect* effect)
{

	if (effect->GetType() == StatEffectType::Radiation && GetClass() != ClassType::Mutant)
	{
		bool found = false;
		if (radiationStackTimer <= 0.f)
		{
			// for all status ailments
			for (auto i = effects.begin(); i != effects.end() && !found; i++)
			{
				// if this is a radiation status ailment
				if ((*i)->GetType() == StatEffectType::Radiation)
				{
					// store a pointer to the status ailment
					Radiation* r = (Radiation*)(*i);
					//// incrament number of stacks
					//
					//
					//r->SetStacks(r->GetStacks() + reinterpret_cast<Radiation*>(effect)->GetStacks());
					//r->SetDuration(6.0f);
					reinterpret_cast<Radiation*>(effect)->SetStacks(r->GetStacks() + reinterpret_cast<Radiation*>(effect)->GetStacks());
					found = true;

					RemoveStatusAilment(StatEffectType::Radiation);
				}
			}

			//if (!found)
				effects.push_back(effect);
				float dmg = (level*1.5f);
				dmg *= reinterpret_cast<Radiation*>(effect)->GetStacks();
				ModifyHealth(-dmg);

			// return radiationStackTmier back to 1 seconds
			radiationStackTimer = 1.0f;
		}
	}
	else if (effect->GetType() != StatEffectType::Radiation)
	{
		bool push = true;
		// for all status ailments
		for (unsigned int x = 0; x < effects.size(); x++)
		{
			if (effects[x]->GetType() == effect->GetType())
			{
				//if ((*i)->GetDuration() < effect->GetDuration())
				//	(*i)->SetDuration(effect->GetDuration());
				//
				//push = false;
				//return;
				RemoveStatusAilment(effect->GetType());
			}
		}

		//if (push)
			effects.push_back(effect);
	}

}

void Character::AOEPull(SGD::Point pos, float range, float pullSpeed, float rotationSpeed)
{

	if (GameplayState::GetInstance()->player)
	{
		Character* pullVictim = GameplayState::GetInstance()->player->GetCharacter();
		float distance = (pullVictim->GetPosition() - pos).ComputeLength();
		float angle = 0.0f;


		vector<NPC*> hits = GameplayState::GetInstance()->npcs;
		for (unsigned int x = 0; x < hits.size(); x++)
		{
			pullVictim = hits[x]->getCharacter();
			distance = (pullVictim->GetPosition() - pos).ComputeLength();
			if (pullVictim->GetIsEnemy() != isEnemy && distance < range)
			{
				//Get a vector to new pos
				SGD::Vector toVictim = pullVictim->GetPosition() - pos;
				toVictim *= 0.9f;
				toVictim.Rotate(rotationSpeed);

				//Set up the new target pos
				SGD::Point newPoint = pos + toVictim;

				//Get velocity to the new pos
				SGD::Vector toNewPos = newPoint - pullVictim->GetPosition();
				toNewPos.Normalize();
				toNewPos *= pullSpeed * 200;

				pullVictim->SetVelocity(toNewPos);

				//angle = atan2(pullVictim->GetPosition().y - pos.y, pullVictim->GetPosition().x - pos.x);
				//pullVictim->SetVelocity(SGD::Vector{ cos(angle), sin(angle) } *-200 * pullSpeed);
			}
		}
	}
}

bool Character::GetStunned(void) const
{
	for (unsigned int i = 0; i < effects.size(); i++)

		if (effects[i]->GetType() == StatEffectType::Allure || effects[i]->GetType() == StatEffectType::Stun)
			return true;


	return false;
}

/*virtual*/ void Character::RenderMiniMap(void) /*override*/
{
	bool isAlly = !isEnemy;
	if (isAlly)
	{
		if (GameplayState::GetInstance()->player)
		{
			// reset isAlly
			isAlly = false;
			for (unsigned int i = 0; i < GameplayState::GetInstance()->player->party.size(); i++)
			{
				if (GameplayState::GetInstance()->player->party[i]->getCharacter() == this)
				{
					isAlly = true;
					break;
				}
			}
		}
	}
	SGD::Color color = isEnemy ? SGD::Color::Red : SGD::Color::Yellow;
	if (isAlly)
		color = SGD::Color::Green;
	SGD::GraphicsManager::GetInstance()->DrawRectangle(GetRect(), color);
}

SGD::Rectangle Character::GetRect(void)	const
{
	return Entity::GetRect();
}

float Character::DistanceToPlayer()
{
	return (GameplayState::GetInstance()->player->GetCharacter()->GetPosition() - GetPosition()).ComputeLength();
}
