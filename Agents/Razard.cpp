#include "globals.h"
#include "Razard.h"
#include "../Weapons/MutantAttack.h"

Razard::Razard(int lvl)
{
	level = lvl;
	SetStats();
	rateOfAttack = (100 - dexterity) * 0.01f;
	attackTimer = rateOfAttack;
	m_szSize = { 64, 64 };

	charAnim.SetCurrAnimation("Radzard_Idle");
	weapon = new MutantAttack(MutantAttack::MutantAtkType::RAZARD);
	weapon->SetOwner(this);
	//m_hImage = SGD::GraphicsManager::GetInstance()->LoadTexture("resources/graphics/razard.png");
}

void Razard::SetStats()
{
	health = 260 + 85.0f * level;
	currHealth = health;
	strength = 14 + 8.0f * level;
	accuracy = 0;
	dexterity = 35 + .5f * level;
	defense = 37 + 12.5f * level;
	moveSpeed = 350;
}

void Razard::Update(float dt)
{
	Character::Update(dt);

	if (isMoving &&  charAnim.GetCurrAnimation() != "Radzard_Walk")
		charAnim.SetCurrAnimation("Radzard_Walk");
	else if (!isMoving)
		charAnim.SetCurrAnimation("Radzard_Idle");
}

void Razard::HandlePassive()
{
	//immune to radiation,  regenerate 1.5% of its max health per second
	if (mutantPassiveCooldown >= 1 && currHealth < health)
	{
		mutantPassiveCooldown = 0;
		ModifyHealth((GetHealth()*0.005f));
	}
}

void Razard::AddStatusAilment(StatusEffect* effect)
{
	effect->SetDuration(effect->GetDuration()*0.25f);
	Character::AddStatusAilment(effect);
}


void Razard::Render()
{
	Character::Render();

	if (isEnemy && currHealth < health)
	{
		SGD::GraphicsManager::GetInstance()->DrawRectangle({ GetPosition().x, GetPosition().y - 15.0f, GetPosition().x + 50.0f, GetPosition().y - 5.0f }, { 25, 25, 25 });
		SGD::GraphicsManager::GetInstance()->DrawRectangle({ GetPosition().x, GetPosition().y - 15.0f, GetPosition().x + ((currHealth / health * 50)), GetPosition().y - 5.0f }, { 25, 255, 25 });
	}
}

