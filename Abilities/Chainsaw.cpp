#include "globals.h"
#include "Chainsaw.h"
#include "../Agents/Character.h"
#include "../GameplayState.h"
#include "../Weapons/Weapon.h"
#include "../Weapons/SpecialWeapons/BuzzsawLauncher.h"
#include "../Animation System/AnimationSystem.h"
#include "../../SGD Wrappers/SGD_MessageManager.h"
#include "../Messages/CreateBulletMsg.h"
#include "../Messages/CreateLaserBullet.h"


Chainsaw::Chainsaw(void)
{
	rev = SGD::AudioManager::GetInstance()->LoadAudio("resources/audio/Sound Effects/revving.wav");
}

Chainsaw::~Chainsaw(void)
{
	if (buzz != nullptr)
		buzz->Release();
	SGD::AudioManager::GetInstance()->UnloadAudio(rev);
}

AnimationTimeStamp Chainsaw::GetAnimTS()
{
	return owner->GetAnimation();
}

void Chainsaw::Activate()
{
	damage = (45 + owner->GetStat(StatType::strength)*1.25f)*0.25f;
	reset = fired = false;
	swingVictim.clear();
	duration = 1.0f;

	buzz = new BuzzsawLauncher();
	buzz->SetOwner(owner);
	buzz->SetPosition(owner->GetPosition());
	buzz->SetRotation(owner->GetRotation());
	reinterpret_cast<BuzzsawLauncher*>(buzz)->SetisBrawlActive(true);

	if (owner->GetLevel() >= 21)
		SkillLvl = 3;
	else if (owner->GetLevel() >= 11)
		SkillLvl = 2;


	for (int x = 0; x < 3; x++)
	{
		CreateLaserBullet* msg = new CreateLaserBullet(this);
		msg->QueueMessage();
	}

	SGD::AudioManager::GetInstance()->PlayAudio(rev);
}

void Chainsaw::Update(float dt)
{
	duration -= dt;

	m_fRotation = owner->GetRotation();
	m_ptPosition = owner->GetPosition();

	if (duration < 0 || owner == nullptr)
	{
		//delete ambulance message
		DestroyEntityMsg* die = new DestroyEntityMsg(this);
		die->QueueMessage();
	}
	else if (duration < 0.9f && !fired)
	{
		fired = true;
		for (int x = 0; x < 3; x++)
		{
			SGD::AudioManager::GetInstance()->PlayAudio(GameplayState::GetInstance()->chainSawLauncher);
			CreateBulletMsg* msg = new CreateBulletMsg(buzz);

			// dispatch message
			SGD::MessageManager::GetInstance()->QueueMessage(msg);
		}
	}
	else if (duration < 0.65f && !reset)
	{
		swingVictim.clear();
		reset = true;
	}

	//AnimationSystem::GetInstance()->Update(action, dt);
	//collide = AnimationSystem::GetInstance()->ActiveRect(owner->GetAnimation(), m_ptPosition, m_fRotation);
	//
	//SGD::Point anchor = AnimationSystem::GetInstance()->GetAnchorPoint(owner->GetAnimation(), owner->GetPosition(), owner->GetRotation());
	//SGD::Vector topLeft = (SGD::Point(collide.left, collide.top) - anchor);
	//SGD::Vector botRight = (SGD::Point(collide.right, collide.bottom) - anchor);
	//
	//topLeft.Rotate(owner->GetRotation());
	//botRight.Rotate(owner->GetRotation());
	//
	//topLeft.x = (anchor.x + topLeft.x);
	//topLeft.y = (anchor.y + topLeft.y);
	//botRight.x = (anchor.x + botRight.x);
	//botRight.y = (anchor.y + botRight.y);
	//
	//collide = { topLeft.x, topLeft.y, botRight.x, botRight.y };

	for (int x = 0; x < 3; x++)
	{
		CreateLaserBullet* msg = new CreateLaserBullet(this);
		msg->QueueMessage();
	}
}

void Chainsaw::Render(void)
{
	//AnimationSystem::GetInstance()->Render(action, m_ptPosition);
	//SGD::GraphicsManager::GetInstance()->DrawRectangle(collide, SGD::Color(255, 0, 0));
}

void Chainsaw::HandleCollision(const IEntity* other)
{
	const Character* hit = reinterpret_cast<const Character*>(other);
}

bool Chainsaw::WasHit(const Character* hit)
{
	bool pain = false;
	for (unsigned int x = 0; x < swingVictim.size() && !pain; x++)
	{
		if (hit == swingVictim[x])
			pain = true;
	}

	if (!pain)
		swingVictim.push_back(hit);

	return pain;
}

SGD::Rectangle Chainsaw::GetRect(void) const
{
	return SGD::Rectangle{};
}