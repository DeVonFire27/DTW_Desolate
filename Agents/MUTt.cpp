#include "globals.h"
#include "MUTt.h"
#include "NPC.h"
#include "../Weapons/MutantAttack.h"
#include "../Spawn System/SpawnList.h"
#include "../Spawn System/Spawn.h"

MUTt::MUTt(int lvl)
{
	level = lvl;
	SetStats();
	//m_szSize = { 103, 35 };
	rateOfAttack = (100 - dexterity) * 0.01f;
	attackTimer = rateOfAttack;
	//m_hImage = SGD::GraphicsManager::GetInstance()->LoadTexture("resources/graphics/mutt.png");
	charAnim.SetCurrAnimation("Mutt_Idle");
	cooldown = 45.0f;
	mutantActiveCooldown = 0;

	weapon = new MutantAttack(MutantAttack::MutantAtkType::MUTTS);
	weapon->SetOwner(this);

}

void MUTt::SetStats()
{
	health = 130 + 35.0f * level;
	currHealth = health;
	strength = 12 + 8.5f * level;
	accuracy = 0.0f; 
	dexterity = 50.0f + level;
	defense = 6 + 7.0f * level;
	moveSpeed = 385 + level;
}

void MUTt::Update(float dt)
{
	Mutant::Update(dt);

	if (isMoving &&  charAnim.GetCurrAnimation() != "Mutt_Walk")
		charAnim.SetCurrAnimation("Mutt_Walk");
	else if (!isMoving)
		charAnim.SetCurrAnimation("Mutt_Idle");
	
}

void MUTt::UseAbility()
{
	//every ? seconds (if the MUTt is level 5 or higher) allow it the chance to spawn an ally
	if (GetLevel() >= 5 && mutantActiveCooldown >= cooldown)
	{
		mutantActiveCooldown = 0;
		int r = rand() % 100;
		if (r < (5 + level))
		{
			//add to entity manager and to npcs vector
			//MUTt dies if you spawn it at level 1?????
			MUTt *m = new MUTt(this->GetLevel());
			m->SetPosition({ GetPosition().x - 100, GetPosition().y - 25 });
			m->SetIsEnemy(true);
			NPC *n = new NPC(m, GameplayState::GetInstance()->player, &GameplayState::GetInstance()->npcs);
			Spawn *s = new Spawn(n, (int)m->GetPosition().x, (int)m->GetPosition().y);
			//s->SetIsDead(false);
			SpawnList::GetInstance()->AddSpawn(s);
			m->Release();
		}
	}
}

void MUTt::HandlePassive()
{

}