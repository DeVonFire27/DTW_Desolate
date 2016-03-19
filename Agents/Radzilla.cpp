#include "globals.h"
#include "Radzilla.h"
#include "../EntityManager.h"
#include "Player.h"
#include "../Math.h"
#include "NPC.h"
#include "../Status Effects/Radiation.h"
#include "../Status Effects/Burning.h"
#include "../Weapons/MutantAttack.h"

Radzilla::Radzilla()
{
	m_szSize = { 118, 128 };
	//m_hImage = SGD::GraphicsManager::GetInstance()->LoadTexture("resources/graphics/radzilla.png");

	health = 850;
	currHealth = health;
	strength = 135;
	accuracy = 100;
	dexterity = 45;
	defense = 175;
	moveSpeed = 345;
	mutantActiveCooldown = 0;
	cooldown = 10;
	level = 16;
	rateOfAttack = (100 - dexterity) * 0.01f;
	attackTimer = rateOfAttack;

	weapon = new MutantAttack(MutantAttack::MutantAtkType::RAZARD);
	weapon->SetOwner(this);

	charAnim.SetCurrAnimation("Radzilla_Idle");

}

Radzilla::~Radzilla()
{
	SGD::GraphicsManager::GetInstance()->UnloadTexture(m_hImage);
}

void Radzilla::Render()
{
	/*SGD::GraphicsManager::GetInstance()->DrawTextureSection(m_hImage, m_ptPosition,
	{ 10, 0, 128, 128 }, m_fRotation, m_szSize / 2);*/

	Character::Render();

	if (isEnemy && currHealth < health)
	{
		SGD::GraphicsManager::GetInstance()->DrawRectangle({ GetPosition().x, GetPosition().y - 15.0f, GetPosition().x + 50.0f, GetPosition().y - 5.0f }, { 25, 25, 25 });
		SGD::GraphicsManager::GetInstance()->DrawRectangle({ GetPosition().x, GetPosition().y - 15.0f, GetPosition().x + ((currHealth / health * 50)), GetPosition().y - 5.0f }, { 25, 255, 25 });
	}
}
void Radzilla::UseAbility()
{
	if (mutantActiveCooldown >= cooldown)
	{
		isUsingActive = true;
		mutantActiveCooldown = 0;
		//apply radiation and fire damage to nearby enemies
		Emitter *e = GameplayState::GetInstance()->CreateEmitter("radzilla_AOE");
		e->SetPosition(GetPosition() + GetSize() / 2);
		e->SetFollowTarget(this);
		e->lifeTime = 12;
		GameplayState::GetInstance()->m_pEntities->AddEntity(e, PARTICLES);
		mutantActiveCooldown = 15.0f;		
	}
}

void Radzilla::Update(float dt)
{
	Mutant::Update(dt);

	if (isMoving &&  charAnim.GetCurrAnimation() != "Radzilla_Walk")
		charAnim.SetCurrAnimation("Radzilla_Walk");
	else if (!isMoving)
		charAnim.SetCurrAnimation("Radzilla_Idle");

	if (isUsingActive)
	{
		activeLifetime -= dt;
		if (activeLifetime <= 0)
		{
			isUsingActive = false;
			activeLifetime = 6;
		}
		if (activeLifetime <= 4.5f)
		{
			float distanceToRadzilla = FindControllingNPC(this)->Distance(GameplayState::GetInstance()->player->GetCharacter()->GetPosition() + GetSize() / 2, this->GetPosition() + GetSize() / 2);
			if (distanceToRadzilla <= 235.5f)
			{
				Radiation *rad = new Radiation(GameplayState::GetInstance()->player->GetCharacter());
				rad->SetStacks(1);
				GameplayState::GetInstance()->player->GetCharacter()->AddStatusAilment(rad);
				GameplayState::GetInstance()->player->GetCharacter()->AddStatusAilment(new Burning(GameplayState::GetInstance()->player->GetCharacter()));
			}
			for (unsigned int i = 0; i < GameplayState::GetInstance()->player->party.size(); i++)
			{
				distanceToRadzilla = FindControllingNPC(this)->Distance(GameplayState::GetInstance()->player->party[i]->getCharacter()->GetPosition() + GetSize() / 2, this->GetPosition() + GetSize() / 2);
				if (distanceToRadzilla <= 235.5f)
				{
					Radiation *rad = new Radiation(GameplayState::GetInstance()->player->party[i]->getCharacter());
					rad->SetStacks(1);
					GameplayState::GetInstance()->player->party[i]->getCharacter()->AddStatusAilment(rad);
					GameplayState::GetInstance()->player->party[i]->getCharacter()->AddStatusAilment(new Burning(GameplayState::GetInstance()->player->GetCharacter()));
				}
			}
		}
	}
}

void Radzilla::HandlePassive()
{
	Razard::HandlePassive();
}