/***************************************************************
|	File:		Player.cpp
|	Authors:		Michael Mozdzierz/Ethan Mills
|	Date:		04/11/2014
|
***************************************************************/

#include "globals.h"
#include "Player.h"
#include "../OptionsState.h"
#include "../EntityManager.h"
#include "../CharacterTypes/Brawler.h"
#include <string>
#include <sstream>
#include "../Tile System/World.h"
#include "../EventProc/EventProc.h"
#include "../Weapons/Weapon.h"
#include "../Messages/DestroyEntityMsg.h"
#include "../Tile System/TileLayer.h"
#include "../Tile System/Tile.h"
#include "../Animation System/AnimationSystem.h"

#include "../Status Effects/Slow.h"
#include "../Status Effects/Bleeding.h"
#include "../Status Effects/Burning.h"
#include "../Status Effects/Radiation.h"
#include "../Status Effects/Stun.h"
#include "../Math.h"
#include "../Status Effects/Command.h"
#include "../Status Effects/Rage.h"
#include "../Objective System/ObjectiveList.h"
#include "../Objective System/Objective.h"
#include "NPC.h"

#include "../Console.h"
#include "../BitmapFont.h"
#include "../Game.h"

using namespace std;

Player::Player(void)
{
	reputation = 250;

	healthBar = SGD::GraphicsManager::GetInstance()->LoadTexture("resources/graphics/healthTexture.png");
	experienceBar = SGD::GraphicsManager::GetInstance()->LoadTexture("resources/graphics/experienceTexture.png");

	EventProc::GetInstance()->Subscribe(this, "Pickup Weapon");
	EventProc::GetInstance()->Subscribe(this, "OnChangeColor");
	EventProc::GetInstance()->Subscribe(this, "Pickup Health Kit");
	EventProc::GetInstance()->Subscribe(this, "GainExp");
	EventProc::GetInstance()->Subscribe(this, "Save");
	EventProc::GetInstance()->Subscribe(this, "PlayerPos");
	EventProc::GetInstance()->Subscribe(this, "Kill");
	EventProc::GetInstance()->Subscribe(this, "GiveRep");
	EventProc::GetInstance()->Subscribe(this, "SetLevel");
	EventProc::GetInstance()->Subscribe(this, "SetHealth");

	// Subscribing for events for music changes
	EventProc::GetInstance()->Subscribe(this, "Leaving");
	EventProc::GetInstance()->Subscribe(this, "Reached Bobville");
	EventProc::GetInstance()->Subscribe(this, "Reached Fudge");
	EventProc::GetInstance()->Subscribe(this, "Reached That Other Place");
	EventProc::GetInstance()->Subscribe(this, "Reached Area 15");
	EventProc::GetInstance()->Subscribe(this, "Reached Jamestown");
	EventProc::GetInstance()->Subscribe(this, "Janes Compound Wall");
	EventProc::GetInstance()->Subscribe(this, "Reached The Outskirts");
	EventProc::GetInstance()->Subscribe(this, "Reached That Place");

}

Player::~Player(void)
{

	SGD::GraphicsManager::GetInstance()->UnloadTexture(healthBar);
	SGD::GraphicsManager::GetInstance()->UnloadTexture(experienceBar);

	EventProc::GetInstance()->UnSubscribe(this, "Pickup Weapon");
	EventProc::GetInstance()->UnSubscribe(this, "OnChangeColor");
	EventProc::GetInstance()->UnSubscribe(this, "Pickup Health Kit");
	EventProc::GetInstance()->UnSubscribe(this, "GainExp");
	EventProc::GetInstance()->UnSubscribe(this, "Save");
	EventProc::GetInstance()->UnSubscribe(this, "PlayerPos");
	EventProc::GetInstance()->UnSubscribe(this, "Kill");
	EventProc::GetInstance()->UnSubscribe(this, "GiveRep");
	EventProc::GetInstance()->UnSubscribe(this, "SetLevel");
	EventProc::GetInstance()->UnSubscribe(this, "SetHealth");


	// Unsubscribing from all music changes
	EventProc::GetInstance()->UnSubscribe(this, "Leaving");
	EventProc::GetInstance()->UnSubscribe(this, "Reached Bobville");
	EventProc::GetInstance()->UnSubscribe(this, "Reached Fudge");
	EventProc::GetInstance()->UnSubscribe(this, "Reached That Other Place");
	EventProc::GetInstance()->UnSubscribe(this, "Reached Area 15");
	EventProc::GetInstance()->UnSubscribe(this, "Reached Jamestown");
	EventProc::GetInstance()->UnSubscribe(this, "Janes Compound Wall");
	EventProc::GetInstance()->UnSubscribe(this, "Reached The Outskirts");
	EventProc::GetInstance()->UnSubscribe(this, "Reached That Place");

	vector<Objective*> objectives = ObjectiveList::GetInstance()->GetObjectivesList();
	for (unsigned int i = 0; i < objectives.size(); i++)
		EventProc::GetInstance()->UnSubscribe(this, (char*)objectives[i]->GetCompareString().c_str());
	character->Release();
	if (otherWeapon)
		otherWeapon->Release();
}

void Player::SetOtherWeapon(Weapon* w)
{
	if (w && w != otherWeapon)
		w->AddRef();

	otherWeapon = w;
}

bool Player::AlterParty(void)
{
	return false;
}

void Player::DrawCharacterPortraits()	//place is the 0-3 value. 0 for player, 1 for his first ally, etc
{
	float activeTop, activeBottom;
	GameplayState *gameplay = GameplayState::GetInstance();

	if (OptionsState::GetInstance()->getFullScreen())
	{
		activeTop = 103.0f;
		activeBottom = 137.0f;

		SGD::GraphicsManager::GetInstance()->DrawTexture(gameplay->GetCharClassPortrait(character->GetType()), { 30, 55 }, 0.0f, {}, {}, { .5f, .5f });
		//Add the ability icons + cooldown meters
		SGD::GraphicsManager::GetInstance()->DrawTexture(gameplay->GetCharActive(character->GetType()), { 100, activeTop }, 0.0f, {}, {}, { .12f, .12f });
		SGD::GraphicsManager::GetInstance()->DrawRectangle({ 100, activeBottom - (character->GetCurrCoolDown() / character->GetCoolDown() * (activeBottom - activeTop)), 131, activeBottom }, { 100, 255, 0, 0 }, {}, 0);

		if (character->GetLastCoolDown() > 0.0f && character->GetCurrCoolDown() != character->GetLastCoolDown() && character->GetCurrCoolDown() <= 0.0f)
		{
			//No longer cooling down
			Emitter *newEmitter = gameplay->CreateEmitter("skillCooldownFinished", false);
			newEmitter->SetPosition({ 100, activeTop });
			gameplay->HUDentities.push_back(newEmitter);
		}
	}
	else
	{
		activeTop = 115.0f;
		activeBottom = 151.0f;

		SGD::GraphicsManager::GetInstance()->DrawTexture(gameplay->GetCharClassPortrait(character->GetType()), { 30, 55 }, 0.0f, {}, {}, { .6f, .6f });
		//Add the ability icons + cooldown meters
		SGD::GraphicsManager::GetInstance()->DrawTexture(gameplay->GetCharActive(character->GetType()), { 115, activeTop }, 0.0f, {}, {}, { .15f, .15f });
		SGD::GraphicsManager::GetInstance()->DrawRectangle({ 115, activeBottom - (character->GetCurrCoolDown() / character->GetCoolDown() * (activeBottom - activeTop)), 153, activeBottom }, { 100, 255, 0, 0 }, {}, 0);

		if (character->GetLastCoolDown() > 0.0f && character->GetCurrCoolDown() != character->GetLastCoolDown() && character->GetCurrCoolDown() <= 0.0f)
		{
			//No longer cooling down
			Emitter *newEmitter = gameplay->CreateEmitter("skillCooldownFinished", false);
			newEmitter->SetPosition({ 115, activeTop });
			gameplay->HUDentities.push_back(newEmitter);
		}
	}

	if (party.size() > 0)
	{
		for (unsigned int i = 0; i < party.size(); i++)
		{
			float activeTop, activeBottom;
			Character *c = party[i]->getCharacter();
			if (OptionsState::GetInstance()->getFullScreen())
			{
				activeTop = 202.0f + i * 90;
				activeBottom = 236.0f + i * 90;

				SGD::GraphicsManager::GetInstance()->DrawTexture(gameplay->GetCharClassPortrait(c->GetType()), { 30, (150.0f + i * 90) }, 0.0f, {}, {}, { .5f, .5f });
				//SGD::GraphicsManager::GetInstance()->DrawTexture(gunslingerActivePic, { 87, 236.0f + i * 120 }, 0.0f, {}, {}, { .18f, .18f });
				SGD::GraphicsManager::GetInstance()->DrawTexture(gameplay->GetCharActive(c->GetType()), { 100, activeTop }, 0.0f, {}, {}, { .12f, .12f });
				SGD::GraphicsManager::GetInstance()->DrawRectangle({ 100, activeBottom - (c->GetCurrCoolDown() / c->GetCoolDown() * (activeBottom - activeTop)), 131, activeBottom }, { 100, 255, 0, 0 }, {}, 0);
				
				if (c->GetLastCoolDown() > 0.0f && c->GetCurrCoolDown() != c->GetLastCoolDown() && c->GetCurrCoolDown() <= 0.0f)
				{
					//No longer cooling down
					Emitter *newEmitter = gameplay->CreateEmitter("skillCooldownFinished", false);
					newEmitter->SetPosition({ 100, activeTop });
					gameplay->HUDentities.push_back(newEmitter);
				}
			}
			else
			{
				activeTop = 210.0f + i * 100;
				activeBottom = 248.0f + i * 100;

				SGD::GraphicsManager::GetInstance()->DrawTexture(gameplay->GetCharClassPortrait(c->GetType()), { 30.0f, (160.0f + i * 100) }, 0.0f, {}, {}, { .6f, .6f });
				//SGD::GraphicsManager::GetInstance()->DrawTexture(gunslingerActivePic, { 87, 236.0f + i * 120 }, 0.0f, {}, {}, { .18f, .18f });
				SGD::GraphicsManager::GetInstance()->DrawTexture(gameplay->GetCharActive(c->GetType()), { 115, activeTop }, 0.0f, {}, {}, { .15f, .15f });
				SGD::GraphicsManager::GetInstance()->DrawRectangle({ 115, activeBottom - (c->GetCurrCoolDown() / c->GetCoolDown() * (activeBottom - activeTop)), 153, activeBottom }, { 100, 255, 0, 0 }, {}, 0);
				
				if (c->GetLastCoolDown() > 0.0f && c->GetCurrCoolDown() != c->GetLastCoolDown() && c->GetCurrCoolDown() <= 0.0f)
				{
					//No longer cooling down
					Emitter *newEmitter = gameplay->CreateEmitter("skillCooldownFinished", false);
					newEmitter->SetPosition({ 100, activeTop });
					gameplay->HUDentities.push_back(newEmitter);
				}
			}
		}
	}
}

void Player::SetCharacter(Character* c)
{
	if (character)
		character->Release();

	character = c;

	if (c != nullptr)
		character->AddRef();
}

void Player::RenderHUD(void)
{
	SGD::InputManager* input = SGD::InputManager::GetInstance();

	//Draw character portraits at 60 x 60
	DrawCharacterPortraits();

	//Draw hp info: rectangles at 60, actual health bars at 58
	if (OptionsState::GetInstance()->getFullScreen())
	{
		SGD::GraphicsManager::GetInstance()->DrawRectangle({ 30, 110, 95, 120 }, { 255, 150, 150, 150 });
		SGD::GraphicsManager::GetInstance()->DrawTextureSection(healthBar, { 31.0f, 111.0f }, { 0, 0, character->GetCurrHealth() / character->GetHealth() * 98.0f, 8.0f }, 0.f, {}, {}, {.65f, 1.f});
		//SGD::GraphicsManager::GetInstance()->DrawRectangle({ 30.0f, 100.0f, 30.0f + ((character->GetCurrHealth() / character->GetHealth()) * 100.0f), 110 }, { 255, 25, 255, 25 });
	}
	else
	{
		SGD::GraphicsManager::GetInstance()->DrawRectangle({ 30, 130, 108, 140 }, { 255, 150, 150, 150 });
		SGD::GraphicsManager::GetInstance()->DrawTextureSection(healthBar, { 31.0f, 130.0f }, { 0, 0, character->GetCurrHealth() / character->GetHealth() * 98.0f, 8.0f }, 0.f, {}, {}, { .78f, 1.f });

	}

	if (OptionsState::GetInstance()->getFullScreen())
	{

		SGD::GraphicsManager::GetInstance()->DrawRectangle({ 30, 122, 95, 132 }, { 255, 150, 150, 150 });
		SGD::GraphicsManager::GetInstance()->DrawTextureSection(experienceBar, { 31.0f, 123.0f }, { 0.0f, 0.0f, (float)character->GetCurrExp() / character->GetLevelUpExp() * 98, 8.0f }, 0.f, {}, {}, { .65f, 1.f });
		stringstream lvl;
		lvl.str("");
		lvl.clear();
		lvl << character->GetLevel();
		Game::GetInstance()->GetFont()->Draw(lvl.str().c_str(), 108, 80, 1.0f, { 0, 255, 255 });
	}
	else
	{
		SGD::GraphicsManager::GetInstance()->DrawRectangle({ 30, 142, 108, 150 }, { 150, 150, 150, 150 });
		SGD::GraphicsManager::GetInstance()->DrawTextureSection(experienceBar, { 30.0f, 142.0f }, { 0.0f, 0.0f, (float)character->GetCurrExp() / character->GetLevelUpExp() * 98, 8.0f }, 0.f, {}, {}, { .78f, 1.f });
		stringstream lvl;
		lvl.str("");
		lvl.clear();
		lvl << character->GetLevel();
		Game::GetInstance()->GetFont()->Draw(lvl.str().c_str(), 128, 85, 1.0f, { 0, 255, 255 });
	}
	//Draw the hp and xp bars for the players party
	for (unsigned int i = 0; i < party.size(); ++i)
	{
		Character* c = party[i]->getCharacter();
		//Draw hp info:
		if (OptionsState::GetInstance()->getFullScreen())
		{
			SGD::GraphicsManager::GetInstance()->DrawRectangle({ 30, 207 + 90.0f * i, 95, 215 + 90.0f * i }, { 255, 150, 150, 150 });
			SGD::GraphicsManager::GetInstance()->DrawTextureSection(healthBar, { 30.0f, 208 + 90.0f * i }, { 0, 0, (float)c->GetCurrHealth() / c->GetHealth() * 98.0f, 8.0f }, 0.f, {}, {}, { .65f, 1.f });

			SGD::GraphicsManager::GetInstance()->DrawRectangle({ 30, 217 + 90.0f * i, 95, 225 + 90.0f * i }, { 150, 150, 150, 150 });
			SGD::GraphicsManager::GetInstance()->DrawTextureSection(experienceBar, { 30.0f, 218 + 90.0f * i }, { 0.0f, 0.0f, (float)c->GetCurrExp() / c->GetLevelUpExp() * 98, 8.0f }, 0.f, {}, {}, { .65f, 1.f });
			stringstream lvl;
			lvl.str("");
			lvl << party[i]->getCharacter()->GetLevel();
			Game::GetInstance()->GetFont()->Draw(lvl.str().c_str(), 108, 170 + 90 * i, 1.0f, { 0, 255, 255 });
			lvl.clear();
		}
		else
		{
			SGD::GraphicsManager::GetInstance()->DrawRectangle({ 30, 228 + 100.0f * i, 108, 236 + 100.0f * i }, { 150, 150, 150, 150 });
			SGD::GraphicsManager::GetInstance()->DrawTextureSection(healthBar, { 30.0f, 228 + 100.0f * i }, { 0, 0, (float)c->GetCurrHealth() / c->GetHealth() * 98.0f, 8.0f }, 0.f, {}, {}, { .78f, 1.f });

			SGD::GraphicsManager::GetInstance()->DrawRectangle({ 30, 238 + 100.0f * i, 108, 246 + 100.0f * i }, { 255, 150, 150, 150 });
			SGD::GraphicsManager::GetInstance()->DrawTextureSection(experienceBar, { 30.0f, 238 + 100.0f * i }, { 0.0f, 0.0f, (float)c->GetCurrExp() / c->GetLevelUpExp() * 98, 8.0f }, 0.f, {}, {}, { .78f, 1.f });
			stringstream lvl;
			lvl.str("");
			lvl << c->GetLevel();
			Game::GetInstance()->GetFont()->Draw(lvl.str().c_str(), 128, 190 + 100 * i, 1.0f, { 0, 255, 255 });
			lvl.clear();
			//SGD::GraphicsManager::GetInstance()->DrawString(lvl.str().c_str(), { 128, 190 + 100.0f * i }, { 0, 255, 255 });
		}
	}

}

void Player::Update(float dt)
{
	sinceLocationReached += dt;
	sinceLeft += dt;
	switchTargetCooldown -= dt;

	// tempEmitter to reduce code
	for (unsigned int i = 0; i < GameplayState::GetInstance()->saveEmitters.size(); i++)
	{
		Emitter* tempEmitter = GameplayState::GetInstance()->saveEmitters[i];

		// is the player intersecting with the emitter
		if (this->GetCharacter()->GetRect().IsIntersecting(SGD::Rectangle({ tempEmitter->GetPosition(), tempEmitter->GetSize() / 2 })))
		{
			// change particle colors
			GameplayState::GetInstance()->saveEmitters[i]->particleStartColor = SGD::Color::Red;
			GameplayState::GetInstance()->saveEmitters[i]->particleEndColor = SGD::Color::Black;
		}
		// particle colors = default
		else
		{
			GameplayState::GetInstance()->saveEmitters[i]->particleStartColor = {};
			GameplayState::GetInstance()->saveEmitters[i]->particleEndColor = {};
		}
	}
	character->SetPosition(character->GetPosition());
	//camera.SetPosition({ 330.0f - character->GetPosition().x, 280.0f - character->GetPosition().y });
	//SGD::GraphicsManager::GetInstance()->SetTransform(camera.GetMatrix());
}

void Player::Input(void)
{
	if (GameplayState::GetInstance()->tutorialChosen)
	{
		//when game is not paused, handle player input
		if (!GameplayState::GetInstance()->IsGamePaused())
		{
			SGD::InputManager* pInput = SGD::InputManager::GetInstance();

#pragma region movement
			character->isMoving = false;

			if (!character->GetStunned())
			{
				if (pInput->GetLeftJoystick(0).y <= -.8f || pInput->IsKeyDown(SGD::Key::W) || pInput->IsKeyDown(SGD::Key::Up) || pInput->IsDPadDown(0, SGD::DPad::Up))
				{
					character->SetVelocity({ character->GetVelocity().x, -character->GetStat(StatType::moveSpeed) });
					character->isMoving = true;
				}
				if (pInput->GetLeftJoystick(0).x <= -.8f || pInput->IsKeyDown(SGD::Key::A) || pInput->IsKeyDown(SGD::Key::Left) || pInput->IsDPadDown(0, SGD::DPad::Left))
				{
					character->SetVelocity({ -character->GetStat(StatType::moveSpeed), character->GetVelocity().y });
					character->isMoving = true;
				}
				if (pInput->GetLeftJoystick(0).y >= .8f || pInput->IsKeyDown(SGD::Key::S) || pInput->IsKeyDown(SGD::Key::Down) || pInput->IsDPadDown(0, SGD::DPad::Down))
				{
					character->SetVelocity({ character->GetVelocity().x, character->GetStat(StatType::moveSpeed) });
					character->isMoving = true;
				}
				if (pInput->GetLeftJoystick(0).x >= .8f || pInput->IsKeyDown(SGD::Key::D) || pInput->IsKeyDown(SGD::Key::Right) || pInput->IsDPadDown(0, SGD::DPad::Right))
				{
					character->SetVelocity({ character->GetStat(StatType::moveSpeed), character->GetVelocity().y });
					character->isMoving = true;
				}
			}
#pragma endregion

#pragma region Rotation
			D3DXVECTOR2 scr_pos = D3DXVECTOR2(GetPosition().x + character->GetSize().width / 2.f, GetPosition().y + character->GetSize().height / 2.f);
			if (ARCADEBUILD)
			{
				//Check if there are any enemies to aggro
				if (target && ((character->GetPosition() - target->getCharacter()->GetPosition()).ComputeLength() > Game::GetInstance()->GetScreenWidth() / 2.0f || target->GetIsDead()))
					SetTarget(nullptr);
 
				if (switchTargetCooldown <= 0.0f)
				{
					vector<NPC *> &npcs = GameplayState::GetInstance()->npcs;

					unsigned int i = 0;
					for (; i < npcs.size(); i++)
					{
						//Check if there is a character
						if (!npcs[i]->getCharacter() || !npcs[i]->GetIsEnemy() || npcs[i]->GetIsDead()) continue;

						//If in range (of anchor)
						float distanceToPotentialEnemy = (character->GetPosition() - npcs[i]->getCharacter()->GetPosition()).ComputeLength();

						if (distanceToPotentialEnemy < Game::GetInstance()->GetScreenWidth() / 2.0f)
						{
							//Continue if current target is closer
							if (target && (character->GetPosition() - target->getCharacter()->GetPosition()).ComputeLength() < distanceToPotentialEnemy) continue;

							//Else then mi casa es tu casa
							SetTarget(npcs[i]);

							switchTargetCooldown = 0.2f;
						}
					}
				}

				if (target && target->getCharacter())
				{
					SGD::Point point = World::GetInstance()->GetCamera()->GetPosition();
					point.x += target->getCharacter()->GetPosition().x + target->getCharacter()->GetSize().width / 2;
					point.y += target->getCharacter()->GetPosition().y + target->getCharacter()->GetSize().height / 2;
					pInput->SetMousePosition(point);
				}
				else
				{
					//Point the mouse straight ahead or something
					SGD::Point point = World::GetInstance()->GetCamera()->GetPosition();

					SGD::Vector tempVel = lastVelocity;
					tempVel.Normalize();
					tempVel *= 200;

					point.x += character->GetPosition().x + character->GetSize().width / 2 + tempVel.x;
					point.y += character->GetPosition().y + character->GetSize().height / 2 + tempVel.y;
					pInput->SetMousePosition(point);
				}
			}
			else
			{
			SGD::Vector cursorPosition = pInput->GetRightJoystick(0) * 15.f;
			pInput->SetMousePosition({ pInput->GetMousePosition() + cursorPosition });

			//Create a vector from the player to the cursor position
			}

			D3DXVECTOR4 transform_scrpos;

			D3DXMATRIX transform = World::GetInstance()->GetCamera()->GetMatrix();

			D3DXVec2Transform(&transform_scrpos, &scr_pos, &transform);
			SGD::Vector toCursor = SGD::Vector(pInput->GetMousePosition().x - transform_scrpos.x, pInput->GetMousePosition().y - transform_scrpos.y);

			//Create a vector for the current orientation
			SGD::Vector orientation = { 0, -1 };

			//Rotate the vector to the players current rotation
			orientation.Rotate(character->GetRotation() + .11f);

			////Calculate the angle between the vectors
			float angle = orientation.ComputeAngle(toCursor);

			//rotate the character towards the cursor
			if (orientation.ComputeSteering(toCursor) < 0.0f)
				character->SetRotation(character->GetRotation() - (angle));
			else
				character->SetRotation(character->GetRotation() + (angle));
#pragma endregion

#pragma region Abilities
			//Allow the player to use his ability if his cooldown is 0 and he presses the right mouse button
			if (!GameplayState::GetInstance()->pausedSave && ((pInput->IsKeyPressed(SGD::Key::RButton) || (ARCADEBUILD == 0 && pInput->IsButtonPressed(0, 4))) && character->GetCurrCoolDown() <= 0))
			{
				character->UseAbility();
			}

			//Allow the player to tell his allies when to use their abilities
			if (!GameplayState::GetInstance()->IsGamePaused() && ((ARCADEBUILD == 1 && pInput->IsButtonPressed(0, 1)) || (ARCADEBUILD == 0 && (pInput->IsKeyPressed(SGD::Key::N1) || pInput->IsButtonPressed(0, 0)))))
			{
				if (party.size() > 0)
				{
					party[0]->getCharacter()->UseAbility();

					if (GameplayState::GetInstance()->currentTutorialObjective == Tutorial::PartySkills)
						GameplayState::GetInstance()->tutorial = false;
				}
			}
			if (!GameplayState::GetInstance()->IsGamePaused() && ((ARCADEBUILD == 1 && pInput->IsButtonPressed(0, 2)) || (ARCADEBUILD == 0 && (pInput->IsKeyPressed(SGD::Key::N2) || pInput->IsButtonPressed(0, 3)))))
			{

				if (party.size() > 1)
				{
					party[1]->getCharacter()->UseAbility();

					if (GameplayState::GetInstance()->currentTutorialObjective == Tutorial::PartySkills)
						GameplayState::GetInstance()->tutorial = false;
				}
			}
			if (!GameplayState::GetInstance()->IsGamePaused() && ((ARCADEBUILD == 1 && pInput->IsButtonPressed(0, 3)) || (ARCADEBUILD == 0 && (pInput->IsKeyPressed(SGD::Key::N3) || pInput->IsButtonPressed(0, 2)))))
			{

				if (party.size() > 2)
				{
					party[2]->getCharacter()->UseAbility();

					if (GameplayState::GetInstance()->currentTutorialObjective == Tutorial::PartySkills)
						GameplayState::GetInstance()->tutorial = false;
				}
			}
#pragma endregion

#pragma region Shoot Weps
			// character can not attak if stunned
			if (!character->GetStunned())
			{
				// if the charecter has a weapon
				if (character && character->GetWeapon() != nullptr)
				{
					// if the player presses the fire button, fire a bullet
					if (((pInput->IsKeyDown(SGD::Key::MouseLeft) || pInput->GetTrigger(0) > 0.8f)))
					{
						if (GameplayState::GetInstance()->currentTutorialObjective == Tutorial::Shoot)
						{
							GameplayState::GetInstance()->currentTutorialObjective++;
							GameplayState::GetInstance()->sinceLastTutorial = 0.0f;
						}

						character->UseWeapon();
					}

					if ((ARCADEBUILD == 1 && pInput->IsButtonPressed(0, 5)) || (ARCADEBUILD == 0 && (pInput->IsKeyPressed(SGD::Key::MouseLeft) || pInput->IsButtonPressed(0, 7))))
					{
						if (GetCharacter()->GetWeapon()->GetGunType() == Weapon::GunType::flameThrower)
							SGD::AudioManager::GetInstance()->PlayAudio(GameplayState::GetInstance()->flameThrowerFiring);
					}
					if ((pInput->IsButtonReleased(0, 5)) || (ARCADEBUILD == 0 && (pInput->IsKeyReleased(SGD::Key::MouseLeft) || pInput->IsButtonReleased(0, 7))))
					{
						if (GetCharacter()->GetWeapon()->GetGunType() == Weapon::GunType::flameThrower)
							SGD::AudioManager::GetInstance()->StopAudio(GameplayState::GetInstance()->flameThrowerFiring);
					}


				}
			}
#pragma endregion

#pragma region push back

			if (((ARCADEBUILD == 1 && pInput->IsButtonPressed(0, 3)) || (ARCADEBUILD == 0 && ((pInput->GetInstance()->IsKeyDown(SGD::Key::F) || pInput->IsButtonPressed(0, 5))))) && character->pushTimer <= 0)
			{
				// get a value to send with the event
				PushBack knock;
				knock.pos = character->GetPosition();
				knock.owner = character->GetClass();

				SGD::AudioManager::GetInstance()->PlayAudio(GameplayState::GetInstance()->knockSFX);

				// if Brawler @ level 18+
				if (knock.owner == ClassType::Brawler && character->GetLevel() >= 18)
				{
					knock.bonusKB = true;
					Emitter* saws = GameplayState::GetInstance()->CreateEmitter("brawl_knockback");
					saws->SetPosition(AnimationSystem::GetInstance()->GetAnchorPoint(character->GetAnimation(), character->GetPosition()));
					saws->lifeTime = 0.15f;
				}
				// if Cyborg @ level 6+
				else if (knock.owner == ClassType::Cyborg && character->GetLevel() >= 6)
				{
					knock.bonusKB = true;
					Emitter* pulse = GameplayState::GetInstance()->CreateEmitter("cyborg_knockback");
					pulse->SetPosition(AnimationSystem::GetInstance()->GetAnchorPoint(character->GetAnimation(), character->GetPosition()));
					pulse->lifeTime = 0.15f;
				}
				// if Medic @ level 18+
				else if (knock.owner == ClassType::Medic && character->GetLevel() >= 18)
				{
					knock.bonusKB = true;
					character->ClearStatusEffects();
					Emitter* esuna = GameplayState::GetInstance()->CreateEmitter("medic_knockback");
					esuna->SetPosition(AnimationSystem::GetInstance()->GetAnchorPoint(character->GetAnimation(), character->GetPosition()));
					esuna->lifeTime = 0.15f;
				}
				else
				{
					Emitter* knock = GameplayState::GetInstance()->CreateEmitter("knockback");
					knock->SetPosition(AnimationSystem::GetInstance()->GetAnchorPoint(character->GetAnimation(), character->GetPosition()));
					knock->lifeTime = 0.15f;
				}

				if (GameplayState::GetInstance()->currentTutorialObjective == Tutorial::Knockback)
				{
					GameplayState::GetInstance()->currentTutorialObjective++;
					GameplayState::GetInstance()->sinceLastTutorial = 0.0f;
				}

				// send the push back event with the 'pos' as a parameter
				EventProc::GetInstance()->Dispatch("PushBack", &knock);
				character->pushTimer = 5.0f;
			}

			//evolve testing code MUST REMOVE LATER!!!
			//if (pInput->GetInstance()->IsKeyDown(SGD::Key::Home))
			//{
			//	character->SetExp(character->GetCurrExp() + 20);
			//	reputation += 100;
			//}

#pragma endregion
		}
	}

	if (character->GetVelocity() != SGD::Vector::Zero)
		lastVelocity = character->GetVelocity();
}

SGD::Point Player::GetPosition()
{
	return character->GetPosition();
}

void Player::SetTarget(NPC *set)
{
	if (target)
	{
		target = nullptr;
	}

	target = set;

	if (target)
	{
	}
}

SGD::Rectangle Player::GetRect()
{
	SGD::Rectangle playerPos;
	playerPos.left = GetPosition().x;
	playerPos.top = GetPosition().y;
	playerPos.right = GetPosition().x + character->GetSize().width;
	playerPos.bottom = GetPosition().y + character->GetSize().height;
	return playerPos;
}

/*virtual*/ void Player::HandleEvent(string name, void* args) /*override*/
{
	if (name == "GainExp")
	{
		int* bonusXP = reinterpret_cast<int*>(args);
		this->character->SetExp(character->GetCurrExp() + (int)(1.6f*(*bonusXP)*0.75f));

	}
	//if (name == "OnChangeColor")
	//	this->character->SetPosition(*reinterpret_cast<SGD::Point*>(args));
	//When a player picks up a health kit have them regain up to 15% of their health
	if (name == "Pickup Health Kit")
	{
		float healAmount = 0.2f;
		SGD::AudioManager::GetInstance()->PlayAudio(GameplayState::GetInstance()->heals);
		if (character->GetClass() == ClassType::Medic && character->GetLevel() >= 15)
			healAmount += 0.1f;
		for (unsigned int i = 0; i < party.size(); ++i)
		{
			Character* partyMate = party[i]->getCharacter();
			if (partyMate->GetClass() == ClassType::Medic && partyMate->GetLevel() >= 15)
				healAmount += 0.1f;
		}

		character->ModifyHealth(character->GetHealth()*healAmount);
		Emitter* heals = GameplayState::GetInstance()->CreateEmitter("heals", character);
		heals->lifeTime = 0.25f;

		for (unsigned int i = 0; i < party.size(); ++i)
		{
			party[i]->getCharacter()->ModifyHealth(party[i]->getCharacter()->GetHealth()*healAmount);
			Emitter* heals = GameplayState::GetInstance()->CreateEmitter("heals", party[i]->getCharacter());
			heals->lifeTime = 0.25f;
		}
	}

	if (name == "Pickup Weapon")
		GameplayState::GetInstance()->SwapWeapons(otherWeapon);
	//character->SwapWeapons(otherWeapon);

	if (name == "Save" && !GameplayState::GetInstance()->inRecruitmentDialogue)
	{
		if (args == this->GetCharacter())
		{
			if ((ARCADEBUILD == 1 && InputManager::GetInstance()->IsKeyPressed(SGD::Key::LButton)) || (ARCADEBUILD == 0 && (InputManager::GetInstance()->IsKeyPressed(Key::E) || InputManager::GetInstance()->IsButtonPressed(0, 0))))
			{
				GameplayState::GetInstance()->pausedSave = true;
				GameplayState::GetInstance()->EnterSaveScreen();
			}
		}
	}

	if (args == this->GetCharacter())
	{

		if (name == "Reached Bobville")
		{
			if (Game::GetInstance()->GetMusic() != Game::GetInstance()->townMusic1)
				Game::GetInstance()->SetMusic(Game::GetInstance()->townMusic1, true);

			if (currentLocation != "Bobville" && sinceLeft > 1.0f)
				sinceLocationReached = 0.0f;

			currentLocation = "Bobville";
		}

		else if (name == "Reached Fudge")
		{
			if (Game::GetInstance()->GetMusic() != Game::GetInstance()->townMusic2)
				Game::GetInstance()->SetMusic(Game::GetInstance()->townMusic2, true);

			if (currentLocation != "Fudge" && sinceLeft > 1.0f)
				sinceLocationReached = 0.0f;

			currentLocation = "Fudge";
		}

		else if (name == "Reached That Other Place")
		{
			if (Game::GetInstance()->GetMusic() != Game::GetInstance()->townMusic1)
				Game::GetInstance()->SetMusic(Game::GetInstance()->townMusic1, true);

			if (currentLocation != "That Other Place" && sinceLeft > 1.0f)
				sinceLocationReached = 0.0f;

			currentLocation = "That Other Place";
		}

		else if (name == "Reached Area 15")
		{
			if (Game::GetInstance()->GetMusic() != Game::GetInstance()->townMusic2)
				Game::GetInstance()->SetMusic(Game::GetInstance()->townMusic2, true);

			if (currentLocation != "Area 15" && sinceLeft > 1.0f)
				sinceLocationReached = 0.0f;

			currentLocation = "Area 15";
		}

		else if (name == "Reached Jamestown")
		{
			if (Game::GetInstance()->GetMusic() != Game::GetInstance()->townMusic3)
				Game::GetInstance()->SetMusic(Game::GetInstance()->townMusic3, true);

			if (currentLocation != "Jamestown" && sinceLeft > 1.0f)
				sinceLocationReached = 0.0f;

			currentLocation = "Jamestown";
		}

		else if (name == "Janes Compound Wall")
		{
			if (Game::GetInstance()->GetMusic() != Game::GetInstance()->fightMusic)
				Game::GetInstance()->SetMusic(Game::GetInstance()->fightMusic, true);

			if (currentLocation != "Jane's Compound" && sinceLeft > 1.0f)
				sinceLocationReached = 0.0f;

			currentLocation = "Jane's Compound";
		}

		else if (name == "Reached The Outskirts")
		{
			if (Game::GetInstance()->GetMusic() != Game::GetInstance()->townMusic2)
				Game::GetInstance()->SetMusic(Game::GetInstance()->townMusic2, true);

			if (currentLocation != "The Outskirts" && sinceLeft > 1.0f)
				sinceLocationReached = 0.0f;

			currentLocation = "The Outskirts";
		}

		else if (name == "Reached That Place")
		{
			if (Game::GetInstance()->GetMusic() != Game::GetInstance()->townMusic1)
				Game::GetInstance()->SetMusic(Game::GetInstance()->townMusic1, true);

			if (currentLocation != "That Place" && sinceLeft > 1.0f)
				sinceLocationReached = 0.0f;

			currentLocation = "That Place";
		}

		else if (name == "Leaving")
		{
			if (currentLocation == "Bobville" && sinceLeft > 5.0f)
			{
				GameplayState::GetInstance()->tutorial = false;
			}

			if (Game::GetInstance()->GetMusic() != Game::GetInstance()->explorationMusic1)
				Game::GetInstance()->SetMusic(Game::GetInstance()->explorationMusic1, true);

			if (currentLocation != "")
				sinceLeft = 0.0f;

			currentLocation = "";
		}
		else
			sinceLeft = 0.0f;
	}

	if (name == "PlayerPos")
	{
		vector<string> parameters = *((vector<string>*)args);
		float x = float(atoi(parameters[0].c_str()));
		float y = float(atoi(parameters[1].c_str()));

		character->SetPosition({ x, y });

		Console::GetInstance()->WriteLine("Changed player position", SGD::Color::Blue);
	}
	else if (name == "Kill")
	{
		character->SetCurrHealth(-1.f);
	}
	else if (name == "GiveRep")
	{
		vector<string> prams = *((vector<string>*)args);
		float rep = (float)atoi(prams[0].c_str());

		reputation += (int)rep;
	}
	else if (name == "SetLevel")
	{
		vector <string> prams = *((vector<string>*)args);
		int level = atoi(prams[0].c_str());
		character->SetLevel(level);
	}
	else if (name == "SetHealth")
	{
		vector<string> prams = *((vector<string>*)args);
		float newHealth = (float)atoi(prams[0].c_str());
		character->SetHealth(newHealth);
	}
	GameplayState::GetInstance()->objectives->NextObjective(name);
}

Character* Player::GetPartyMember(int partyIndex)
{
	switch (partyIndex)
	{
	case 0:
		return character;
		break;
	case 1:
	{
		if (party.size() >= 1)
			return party[0]->getCharacter();
	}
		break;
	case 2:
	{
		if (party.size() >= 2)
			return party[1]->getCharacter();
	}
		break;
	case 3:
	{
		if (party.size() == 3)
			return party[2]->getCharacter();
	}
		break;
	}

	return nullptr;
}

void Player::ModifyReputation(Character* who)
{
	if (who->GetIsEnemy())
		reputation += (50 * who->GetLevel());

	if (!who->GetIsEnemy())
		reputation -= (100 * who->GetLevel());

	if (who == GameplayState::GetInstance()->player->GetCharacter())
		reputation -= (250 * who->GetLevel());
}
