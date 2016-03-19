/***************************************************************
|	File:		Game.cpp
|	Author:
|	Course:
|	Purpose:	Game class controls the SGD wrappers
|				& runs the game state machine
***************************************************************/


#include "globals.h"
#include "Game.h"
#include "Console.h"

#include "EventProc\EventProc.h"
#include "BitmapFont.h"
#include "IGameState.h"
#include "MainMenuState.h"
#include "GameplayState.h"
#include "OptionsState.h"

#include "Weapons\Weapon.h"
#include "Weapons\Pistol.h"
#include "Weapons\Shotgun.h"
#include "Weapons\SniperRifle.h"
#include "Weapons\MeleeWeapon.h"

#include <ctime>
#include <cstdlib>
#include <cassert>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>


/**************************************************************/
// Singleton
//	- instantiate the static member
/*static*/ Game* Game::s_pInstance = nullptr;

// GetInstance
//	- allocate the ONE instance & return it
/*static*/ Game* Game::GetInstance(void)
{
	if (s_pInstance == nullptr)
		s_pInstance = new Game;

	return s_pInstance;
}

// DeleteInstance
//	- deallocate the ONE instance
/*static*/ void Game::DeleteInstance(void)
{
	delete s_pInstance;
	s_pInstance = nullptr;
}

void Game::SetMusic(HAudio newAudio, bool useFade)
{
	SGD::AudioManager::GetInstance()->StopAudio(Game::GetInstance()->GetMusic());
	backgroundMusic = newAudio;
	//SGD::AudioManager::GetInstance()->SetAudioVolume(backgroundMusic, OptionsState::GetInstance()->getMusicVolume());
	SGD::AudioManager::GetInstance()->PlayAudio(backgroundMusic, true);
	if (useFade)
		musicFadeVolume = 0.f;
}

/**************************************************************/
// Initialize
//	- initialize the SGD wrappers
//	- load resources / assets
//	- allocate & initialize the game entities
bool Game::Initialize(int width, int height)
{
	// Seed First!
	srand((unsigned int)time(nullptr));
	rand();

	// Store the size parameters
	m_nScreenWidth = width;
	m_nScreenHeight = height;


	// Store the SGD singletons
	m_pAudio = SGD::AudioManager::GetInstance();
	m_pGraphics = SGD::GraphicsManager::GetInstance();
	m_pInput = SGD::InputManager::GetInstance();

	// Initialize each singleton
	if (m_pAudio->Initialize() == false
		|| m_pGraphics->Initialize(false) == false
		|| m_pInput->Initialize() == false)
	{
		return false;
	}
	//load cursor textures
	cursorTex = SGD::GraphicsManager::GetInstance()->LoadTexture("resources\\graphics\\cursorTex.png");
	reticuleTex = SGD::GraphicsManager::GetInstance()->LoadTexture("resources\\graphics\\crossHairs.png");

	//Initialize Audio
	creditsMusic = SGD::AudioManager::GetInstance()->LoadAudio("resources/audio/Background Music/Credits Music.xwm");
	fightMusic = SGD::AudioManager::GetInstance()->LoadAudio("resources/audio/Background Music/In-the-Vortex (fight music).xwm");
	mainMenuMusic = SGD::AudioManager::GetInstance()->LoadAudio("resources/audio/Background Music/Main Menu Music.xwm");
	townMusic1 = SGD::AudioManager::GetInstance()->LoadAudio("resources/audio/Background Music/Petticoat-Waltz (town music).xwm");
	townMusic2 = SGD::AudioManager::GetInstance()->LoadAudio("resources/audio/Background Music/Town Music 1.xwm");
	townMusic3 = SGD::AudioManager::GetInstance()->LoadAudio("resources/audio/Background Music/Six-Shooter.xwm");
	explorationMusic1 = SGD::AudioManager::GetInstance()->LoadAudio("resources/audio/Background Music/Vacquero (Walking Music).xwm");

	backgroundMusic = mainMenuMusic;

	SGD::AudioManager::GetInstance()->PlayAudio(backgroundMusic, true);

	m_pFont = new BitmapFont("resources/graphics/fonts/font.fnt");
	m_pFontThick = new BitmapFont("resources/graphics/fonts/fontThick.fnt");
	m_pFontTitle = new BitmapFont("resources/graphics/fonts/title.fnt");
	m_pFontButton = new BitmapFont("resources/graphics/fonts/button.fnt");

	ChangeState(MainMenuState::GetInstance());

	// Store the current time (in milliseconds)
	m_ulGameTime = GetTickCount64();

	background = SGD::GraphicsManager::GetInstance()->LoadTexture("resources/graphics/background.png");
	backParticles = GameplayState::GetInstance()->LoadEmitterFile("resources/Particles/menuDust.xml");

	return true;	// success!
}


/**************************************************************/
// Main
//	- update the SGD wrappers
//	- update the game entities
//	- render the game entities
int Game::Main(void)
{
	joystick_prev = SGD::InputManager::GetInstance()->GetLeftJoystick(0);
	joystick2_prev = SGD::InputManager::GetInstance()->GetRightJoystick(0);

	if (m_pAudio->Update() == false
		|| m_pGraphics->Update() == false
		|| m_pInput->Update() == false)
	{
		return -10;		// abort!
	}
	// Current time
	unsigned long long now = GetTickCount64();
	float elapsedTime = (now - m_ulGameTime) / 1000.0f;
	m_ulGameTime = now;

	if (musicFadeVolume < 1.f)
		musicFadeVolume += 0.01f;
	SGD::AudioManager::GetInstance()->SetMasterVolume(SGD::AudioGroup::Music, int(OptionsState::GetInstance()->getMusicVolume() * musicFadeVolume));

	// Cap the elapsed time to 1/8th of a second
	//if (elapsedTime < 0.00005f)
	//	elapsedTime = 0.00005f;
	if (elapsedTime > 0.075f)
		elapsedTime = 0.075f;

	//If in a menu then render the menu stuff
	if (m_pCurrState != GameplayState::GetInstance() || GameplayState::GetInstance()->IsGamePaused())
	{
		SGD::GraphicsManager::GetInstance()->DrawTexture(background, { m_nScreenWidth - MENU_BACKIMAGE_WIDTH, m_nScreenHeight - MENU_BACKIMAGE_HEIGHT });
		backParticles->SetPosition({ -200.0f, m_nScreenHeight - 160.0f });
		backParticles->Update(elapsedTime);
		backParticles->Render();
	}

	//Update the pulse thingy
	if (fading)
	{
		opacity -= 2.0f * elapsedTime;
		if (opacity < 0.0f)
		{
			opacity = 0.0f;
			fading = false;
		}
	}
	else
	{
		opacity += 2.0f * elapsedTime;
		if (opacity > 1.0f)
		{
			opacity = 1.0f;
			fading = true;
		}
	}

	if (SGD::InputManager::GetInstance()->IsKeyDown(SGD::Key::Alt))
	{
		if (SGD::InputManager::GetInstance()->IsKeyPressed(SGD::Key::Enter))
		{
			bool isFullscreen = OptionsState::GetInstance()->getFullScreen();
			OptionsState::GetInstance()->setFullScreen(!isFullscreen);

			if (OptionsState::GetInstance()->getFullScreen())
				SGD::GraphicsManager::GetInstance()->Resize({ 800, 600 }, false);
			else
				SGD::GraphicsManager::GetInstance()->Resize({ WINDOW_WIDTH, WINDOW_HEIGHT }, true);
		}
	}
	// Let the current state handle input if the console is not visible
	else if (!Console::GetInstance()->visible && m_pCurrState->Input() == false)
		return 1;	// exit success!

#if DEBUG_CONSOLE
	// if the console is visible, update only the console, otherwise, update the current state
	if (Console::GetInstance()->visible)
		Console::GetInstance()->Update();
	else
		m_pCurrState->Update(elapsedTime);

	// toggle the console on/off
	if (SGD::InputManager::GetInstance()->IsKeyPressed(SGD::Key::Tilde))
		Console::GetInstance()->visible = !Console::GetInstance()->visible;

	// render the current state
	m_pCurrState->Render();

	// render the console, if it is visible
	if (Console::GetInstance()->visible)
		Console::GetInstance()->Draw();
#else

	// update/render current state
	m_pCurrState->Update(elapsedTime);
	m_pCurrState->Render();
#endif
	// store the location of the mouse at this frame
	SGD::Point mousePos = m_pInput->GetMousePosition();

	// if we're not in gameplay (or if we are, if the game is paused) draw the menu cursor
	if (m_pCurrState != GameplayState::GetInstance() || GameplayState::GetInstance()->IsGamePaused())
		SGD::GraphicsManager::GetInstance()->DrawTexture(cursorTex, mousePos);
	else // otherwise draw the reticule
		SGD::GraphicsManager::GetInstance()->DrawTexture(reticuleTex, SGD::Point(mousePos.x - 25, mousePos.y - 25));

	//Show an FPS counter
	if (showFPS)
	{
		sinceFPS += elapsedTime;
		frames++;
		if (sinceFPS > 1.0f)
		{
			fps = frames;

			sinceFPS = 0.0f;
			frames = 0;
		}
		char buffer[100];
		sprintf_s(buffer, 100, "%d", fps);
		SGD::GraphicsManager::GetInstance()->DrawString(buffer, { 0.0f, 0.0f });
	}
	if (SGD::InputManager::GetInstance()->IsKeyPressed(SGD::Key::F1)) showFPS = !showFPS;


	return 0;		// keep playing!
}


/**************************************************************/
// Terminate
//	- deallocate game entities
//	- unload resources / assets
//	- terminate the SGD wrappers
void Game::Terminate(void)
{
	// Exit the current state
	ChangeState(nullptr);

	// Terminate & deallocate the font
	m_pFont->Terminate();
	delete m_pFont;
	delete m_pFontThick;
	delete m_pFontTitle;
	delete m_pFontButton;
	m_pFontTitle = nullptr;
	m_pFontButton = nullptr;
	m_pFont = nullptr;
	m_pFontThick = nullptr;

	// Unload all audio
	SGD::AudioManager::GetInstance()->UnloadAudio(creditsMusic);
	SGD::AudioManager::GetInstance()->UnloadAudio(fightMusic);
	SGD::AudioManager::GetInstance()->UnloadAudio(mainMenuMusic);
	SGD::AudioManager::GetInstance()->UnloadAudio(townMusic1);
	SGD::AudioManager::GetInstance()->UnloadAudio(townMusic2);
	SGD::AudioManager::GetInstance()->UnloadAudio(townMusic3);
	SGD::AudioManager::GetInstance()->UnloadAudio(explorationMusic1);

	// unload cursor textures
	m_pGraphics->UnloadTexture(reticuleTex);
	m_pGraphics->UnloadTexture(cursorTex);

	//Unload menu background stuff
	SGD::GraphicsManager::GetInstance()->UnloadTexture(background);
	delete backParticles;
	backParticles = nullptr;

	// Terminate & deallocate the SGD wrappers
	m_pAudio->Terminate();
	m_pAudio = nullptr;
	SGD::AudioManager::DeleteInstance();

	m_pGraphics->Terminate();
	m_pGraphics = nullptr;
	SGD::GraphicsManager::DeleteInstance();

	m_pInput->Terminate();
	m_pInput = nullptr;
	SGD::InputManager::DeleteInstance();

}

/**************************************************************/
// ChangeState
//	- exit the current state to release resources
//	- enter the new state to allocate resources
//	- DANGER! Exiting the current state can CRASH the program!
//	  The state can ONLY be exited from the
//	  Input, Update, and Render methods!!!
void Game::ChangeState(IGameState* pNewState)
{
	// Exit the old state
	if (m_pCurrState != nullptr)
		m_pCurrState->Exit();

	// Store the new state
	m_pCurrState = pNewState;

	// Enter the new state
	if (m_pCurrState != nullptr)
		m_pCurrState->Enter();
}

void Game::InteruptState(IGameState* newState)
{
	// if there is not a currently stored state
	if (!interuptedState)
	{
		// store which state has been interupted
		interuptedState = m_pCurrState;
		// switch to the temporary state
		m_pCurrState = newState;
		// enter the current state
		m_pCurrState->Enter();
	}
	else if (newState == interuptedState) // InteruptState is only allowed to be called if there is no interupted state
	{
		// exit the temporary state
		m_pCurrState->Exit();
		// switch back to the old state
		m_pCurrState = newState;
		// signal that there is no longer an interupted state
		interuptedState = nullptr;
	}
}

//Create a weapon to place somewhere in the world
Weapon* Game::CreateWeapon(SGD::Point p, Weapon::GunType gt, int gunLevel, bool world, int Gun)
{
	if (world)
	{
		//TODO: with a random prefix based on gun level:
		int randPrefix = rand() % 3;
		int randWeapon = rand() % 3;

		GunPrefix gp;
		if (gunLevel == 0)
		{
			//level 1 gun has a 1 in 3 chance to be rusty, antique, or standard
			if (randPrefix == 0)
				gp = GunPrefix::rusty;
			else if (randPrefix == 1)
				gp = GunPrefix::antique;
			else
				gp = GunPrefix::standard;
		}
		else if (gunLevel == 1)
		{
			//level 2 gun has a 1 in 3 chance to be antique, standard, or high quality
			if (randPrefix == 0)
				gp = GunPrefix::antique;
			else if (randPrefix == 1)
				gp = GunPrefix::standard;
			else
				gp = GunPrefix::highQuality;
		}
		else
		{
			//level 3 gun has a 1 in 3 chance to be standard, high quality, or futuristic
			if (randPrefix == 0)
				gp = GunPrefix::standard;
			else if (randPrefix == 1)
				gp = GunPrefix::highQuality;
			else
				gp = GunPrefix::futuristic;
		}

		if (gp == GunPrefix::rusty)
			col = SGD::Color(139, 69, 19); // brown tint for rusty weapons
		else if (gp == GunPrefix::antique)
			col = SGD::Color::Green; // green tint for antique weapons
		else if (gp == GunPrefix::standard)
			col = SGD::Color::Blue; // blue tint for standard weapons
		else if (gp == GunPrefix::highQuality)
			col = SGD::Color::Purple; // purple tint for high quality weapons
		else if (gp == GunPrefix::futuristic)
			col = SGD::Color::Red; // red tint for futuristic weapons

		if (gt == Weapon::GunType::Pistol)
		{
			Pistol* pis = new Pistol(Pistol::PistolType(randWeapon));
			pis->SetColor(col);
			pis->SetPosition(p);
			pis->SetPrefix(gp);
			pis->BuffWeapon();
			pis->SetSpawnLevel(gunLevel);


			return pis;
		}
		else if (gt == Weapon::GunType::Shotgun)
		{
			Shotgun* sg = new Shotgun(Shotgun::ShotgunTypes(randWeapon));
			sg->SetColor(col);
			sg->SetPosition(p);
			sg->SetPrefix(gp);
			sg->BuffWeapon();
			sg->SetSpawnLevel(gunLevel);

			return sg;
		}
		else if (gt == Weapon::GunType::SniperRifle)
		{
			SniperRifle* sr = new SniperRifle(SniperRifle::SniperRifleTypes(randWeapon));
			sr->SetColor(col);
			sr->SetPosition(p);
			sr->SetPrefix(gp);
			sr->BuffWeapon();
			sr->SetSpawnLevel(gunLevel);

			return sr;
		}
		else if (gt == Weapon::GunType::meleeWeapon)
		{
			MeleeWeapon* mw = new MeleeWeapon(MeleeWeapon::meleeWeaponTypes(randWeapon));
			mw->SetColor(col);
			mw->SetPosition(p);
			mw->SetPrefix(gp);
			mw->BuffWeapon();
			mw->SetSpawnLevel(gunLevel);

			return mw;
		}
	}
	else if (!world)
	{
		if (gt == Weapon::GunType::Pistol)
		{
			Pistol* pis = new Pistol(Pistol::PistolType(Gun));

			pis->SetPosition(p);
			pis->SetPrefix(GunPrefix(gunLevel));
			pis->BuffWeapon();
			pis->SetSpawnLevel(gunLevel);

			return pis;
		}
		else if (gt == Weapon::GunType::Shotgun)
		{
			Shotgun* sg = new Shotgun(Shotgun::ShotgunTypes(Gun));
			sg->SetPosition(p);
			sg->SetPrefix(GunPrefix(gunLevel));
			sg->BuffWeapon();
			sg->SetSpawnLevel(gunLevel);

			return sg;
		}
		else if (gt == Weapon::GunType::SniperRifle)
		{
			SniperRifle* sr = new SniperRifle(SniperRifle::SniperRifleTypes(Gun));
			sr->SetPosition(p);
			sr->SetPrefix(GunPrefix(gunLevel));
			sr->BuffWeapon();
			sr->SetSpawnLevel(gunLevel);

			return sr;
		}
		else if (gt == Weapon::GunType::meleeWeapon)
		{
			MeleeWeapon* mw = new MeleeWeapon(MeleeWeapon::meleeWeaponTypes(Gun)) ;

			mw->SetPosition(p);
			mw->SetPrefix(GunPrefix(gunLevel));
			mw->BuffWeapon();
			mw->SetSpawnLevel(gunLevel);

			return mw;
		}
	}

	return nullptr;
}

bool Game::IsMenuUpInput()
{
	SGD::InputManager *pInput = SGD::InputManager::GetInstance();

	if ((pInput->GetLeftJoystick(0).y < -0.8f && joystick_prev.y >= -0.8f) || (pInput->GetRightJoystick(0).y < -0.8f && joystick2_prev.y >= -0.8f)
		|| pInput->IsKeyPressed(SGD::Key::Up) || pInput->IsDPadPressed(0, SGD::DPad::Up) || pInput->IsKeyPressed(SGD::Key::W))
		return true;
	return false;
}

bool Game::IsMenuDownInput()
{
	SGD::InputManager *pInput = SGD::InputManager::GetInstance();

	if ((pInput->GetLeftJoystick(0).y > 0.8f && joystick_prev.y <= 0.8f) || (pInput->GetRightJoystick(0).y > 0.8f && joystick2_prev.y <= 0.8f)
		|| pInput->IsKeyPressed(SGD::Key::Down) || pInput->IsDPadPressed(0, SGD::DPad::Down) || pInput->IsKeyPressed(SGD::Key::S))
		return true;
	return false;
}

bool Game::IsMenuLeftInput()
{
	SGD::InputManager *pInput = SGD::InputManager::GetInstance();

	if ((pInput->GetLeftJoystick(0).x < -0.8f && joystick_prev.x >= -0.8f) || (pInput->GetRightJoystick(0).x < -0.8f && joystick2_prev.x >= -0.8f)
		|| pInput->IsKeyPressed(SGD::Key::Left) || pInput->IsDPadPressed(0, SGD::DPad::Left) || pInput->IsKeyPressed(SGD::Key::A))
		return true;
	return false;
}

bool Game::IsMenuRightInput()
{
	SGD::InputManager *pInput = SGD::InputManager::GetInstance();

	if ((pInput->GetLeftJoystick(0).x > 0.8f && joystick_prev.x <= 0.8f) || (pInput->GetRightJoystick(0).x > 0.8f && joystick2_prev.x <= 0.8f)
		|| pInput->IsKeyPressed(SGD::Key::Right) || pInput->IsDPadPressed(0, SGD::DPad::Right) || pInput->IsKeyPressed(SGD::Key::D))
		return true;
	return false;
}

bool Game::IsMenuConfirmInput()
{
	SGD::InputManager *pInput = SGD::InputManager::GetInstance();

	if (ARCADEBUILD == 1 && (pInput->IsKeyPressed(SGD::Key::LButton) || pInput->IsButtonPressed(0, 5)))
		return true;
	else if (pInput->IsKeyPressed(SGD::Key::Enter) || pInput->IsButtonPressed(0, 0))
		return true;

	return false;
}

bool Game::IsMenuBackInput()
{
	SGD::InputManager *pInput = SGD::InputManager::GetInstance();

	if (ARCADEBUILD == 1 && (pInput->IsKeyPressed(SGD::Key::RButton) || pInput->IsButtonPressed(0, 4)))
		return true;
	else if (pInput->IsKeyPressed(SGD::Key::Escape) || pInput->IsButtonPressed(0, 1))
		return true;

	return false;
}
