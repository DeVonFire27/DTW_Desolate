#include "globals.h"

#include "Jane.h"
#include "../Game.h"
#include "../Agents/NPC.h"
#include "../Status Effects/Allure.h"
#include "../Abilities/Ability.h"
#include "../Abilities/Mortar.h"
#include "../Status Effects/Radiation.h"
#include "../Animation System/AnimationSystem.h"
#include "../Weapons/Weapon.h"
#include "../EventProc/EventProc.h"
#include "../Spawn System/Spawn.h"
#include "../Spawn System/SpawnList.h"
#include "../Messages/CreateNPCMsg.h"

#include "../Weapons/Shotgun.h"
#include "../Abilities/Stampede.h"
#include "../CharacterTypes/Brawler.h"
#include "../CharacterTypes/Cyborg.h"
#include "../CharacterTypes/Gunslinger.h"
#include "../CharacterTypes/Sniper.h"


Jane::Jane()
{
	stats[StatType::health].base = 8500.0f;
	stats[StatType::accuracy].base = 175.0f;
	stats[StatType::strength].base = 115.0f;
	stats[StatType::defense].base = 300.0f;
	stats[StatType::dexterity].base = 75.0f;
	stats[StatType::moveSpeed].base = 340.0f;
	ability = new Mortar();
	ability->SetOwner(this);
	dash = new Stampede();
	dash->SetOwner(this);

	health = 8500.0f;
	currHealth = health;

	SetWeapon(new Shotgun(Shotgun::ShotgunTypes::SawedOff));
	GetWeapon()->SetPrefix(GunPrefix::futuristic);
	GetWeapon()->BuffWeapon();
	level = 25;
	isEnemy = true;
	charAnim.SetCurrAnimation("Jane_Walk");
}

void Jane::Update(float dt)
{
	Character::Update(dt);

	if (isMoving && charAnim.GetCurrAnimation() != "Jane_Walk")
		charAnim.SetCurrAnimation("Jane_Walk");
	else if (!isMoving)
		charAnim.SetCurrAnimation("Jane_Idle");
}

Jane::~Jane()
{
	SGD::GraphicsManager::GetInstance()->UnloadTexture(m_hImage);
}

void Jane::UseAbility(void)
{
	if (!cooldownTimerStarted && abilityNum == 1 && currHealth > 0)
	{
		NPC* control = FindControllingNPC(this);
		if (control && control->GetTarget() != nullptr)
		{
			reinterpret_cast<Character*>(control->GetTarget())->AddStatusAilment(new Allure(reinterpret_cast<Character*>(control->GetTarget()), this));
			SGD::AudioManager::GetInstance()->PlayAudio(GameplayState::GetInstance()->allure_sfx);
			currCooldown = 8.0f;
			cooldownTimerStarted = true;
		}
	}
	else if (!cooldownTimerStarted && abilityNum == 2 && currHealth > 0)
	{
		NPC* control = FindControllingNPC(this);
		if (control && control->GetTarget() != nullptr)
		{
			CreateAbility* msg = new CreateAbility(ability);
			SGD::MessageManager::GetInstance()->QueueMessage(msg);
			currCooldown = 6.0f;
			cooldownTimerStarted = true;
			ability->Activate();
		}
	}
	else if (!cooldownTimerStarted && abilityNum == 3 && currHealth > 0 && !ultimate)
	{
		//for (int x = 0; x < 4; x++)
		//{
		//	Radicorn *m = new Radicorn();
		//	m->SetPosition({ GetPosition().x - 200 + (x*100), GetPosition().y + 64 });
		//	m->SetIsEnemy(true);
		//	NPC *n = new NPC(m, GameplayState::GetInstance()->player, &GameplayState::GetInstance()->npcs, this);
		//	n->SetNearestTarget();
		//	CreateNPCMsg* msg = new CreateNPCMsg(n);
		//	msg->QueueMessage();
		//	m->Release();
		//}

		//ultimate = true;
		NPC* control = FindControllingNPC(this);
		if (control && control->GetTarget() != nullptr)
		{
			reinterpret_cast<Stampede*>(dash)->FromTheLeft(true);
			CreateAbility* msg = new CreateAbility(dash);
			SGD::MessageManager::GetInstance()->QueueMessage(msg);
			dash->Activate();
			//currCooldown = 6.0f;
			//cooldownTimerStarted = true;

			//reinterpret_cast<Stampede*>(dash)->FromTheLeft(false);
			//msg = new CreateAbility(dash);
			//SGD::MessageManager::GetInstance()->QueueMessage(msg);
			//dash->Activate();
		}
	}
}

void Jane::SpawnMinion(ClassType character)
{
	//Character* m;
	ClassType type;

	if ((int)character)
		type = character;
	else
		type = ClassType(rand() % 4);

	switch (character)
	{
	case ClassType::Brawler:
	{
		Brawler* m = new Brawler();
		m->SetPosition({ GetPosition().x, GetPosition().y });
		m->SetIsEnemy(true);
		NPC *n = new NPC(m, GameplayState::GetInstance()->player, &GameplayState::GetInstance()->npcs);
		n->getCharacter()->spawnCollision = true;
		spawnCollision = true;
		n->SetAnchor(this);
		Spawn *s = new Spawn(n, (int)m->GetPosition().x, (int)m->GetPosition().y);
		//s->SetIsDead(false);
		SpawnList::GetInstance()->AddSpawn(s);
		break;	}
	case ClassType::Cyborg:
	{
		Cyborg* m = new Cyborg();
		m->SetPosition({ GetPosition().x, GetPosition().y });
		m->SetIsEnemy(true);
		NPC *n = new NPC(m, GameplayState::GetInstance()->player, &GameplayState::GetInstance()->npcs);
		n->getCharacter()->spawnCollision = true;
		spawnCollision = true;
		n->SetAnchor(this);
		Spawn *s = new Spawn(n, (int)m->GetPosition().x, (int)m->GetPosition().y);
		//s->SetIsDead(false);
		SpawnList::GetInstance()->AddSpawn(s);
		break;
	}
	case ClassType::Gunslinger:
	{
		Gunslinger* m = new Gunslinger();
		m->SetPosition(GetPosition());
		m->SetIsEnemy(true);
		NPC *n = new NPC(m, GameplayState::GetInstance()->player, &GameplayState::GetInstance()->npcs);
		n->getCharacter()->spawnCollision = true;
		spawnCollision = true;
		n->SetAnchor(this);
		Spawn *s = new Spawn(n, (int)m->GetPosition().x, (int)m->GetPosition().y);
		//s->SetIsDead(false);
		SpawnList::GetInstance()->AddSpawn(s);
		break;	
	}
	case ClassType::Sniper:
	{
		Sniper* m = new Sniper();
		m->SetPosition(GetPosition());
		m->SetIsEnemy(true);
		NPC *n = new NPC(m, GameplayState::GetInstance()->player, &GameplayState::GetInstance()->npcs);
		n->getCharacter()->spawnCollision = true;
		spawnCollision = true;
		n->SetAnchor(this);
		Spawn *s = new Spawn(n, (int)m->GetPosition().x, (int)m->GetPosition().y);
		//s->SetIsDead(false);
		SpawnList::GetInstance()->AddSpawn(s);
		break;
	}
	}
}
