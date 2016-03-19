/***************************************************************
|	File:		Flamethrower.cpp
|	Author:		Michael Mozdzierz
|	Date:		05/13/2014
|
***************************************************************/

#include "globals.h"
#include "Flamethrower.h"
#include "../../EventProc/EventProc.h"
#include "../../../SGD Wrappers/SGD_GraphicsManager.h"
#include "../../Particle System/Emitter.h"
#include "../../GameplayState.h"
#include "../../Math.h"

Flamethrower::Flamethrower(void)
{
	rateOfFire = 20.f;
	damage = 10.f;
	sway = 45.f;
	m_hImage = SGD::GraphicsManager::GetInstance()->LoadTexture("resources\\graphics\\weaponPickups\\flamethrower.png");
	m_szSize = { 25.f, 25.f };
	emitter = GameplayState::GetInstance()->CreateEmitter("flameThrowerEffect");
	emitter->AddRef();
	emitter->SetRunning(false);

	heldImg = SGD::GraphicsManager::GetInstance()->LoadTexture("resources\\graphics\\weaponsHeld\\flamethrower_top.png");
	m_szSize = SGD::Size{ 16, 60 };
}

Flamethrower::~Flamethrower(void)
{
	emitter->Release();
}

/*virtual*/ bool Flamethrower::Use(void) /*override*/
{
	if (fireCoolDown <= 0.f)
	{
		Attack();
		//SGD::AudioManager::GetInstance()->PlayAudio(GameplayState::GetInstance()->flameThrowerFiring);

		return true;
	}
	
	emitTimeout = 0.1f;
	emitter->SetRunning(true);

	return false;
}

/*virtual*/ void Flamethrower::Update(float dt) /*override*/
{
	if (emitTimeout > 0.f && owner && owner->GetCurrHealth() > 0)
		emitTimeout -= dt;
	else
		emitter->SetRunning(false);
	emitter->SetPosition(m_ptPosition);
	emitter->SetRotation(m_fRotation - Math::to_radians(180.f));

	Weapon::Update(dt);
}