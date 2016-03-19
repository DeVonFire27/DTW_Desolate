/***************************************************************
|	File:		MainMenuState.cpp
|	Author:		Ethan Mills
|	Course:		SGP
|	Purpose:	MainMenuState class handles the main menu
***************************************************************/

#include "globals.h"
#include "MainMenuState.h"
#include "NewGameState.h"
#include "LoadGameState.h"
#include "InstructionsState.h"
#include "OptionsState.h"
#include "CreditsState.h"
#include "BitmapFont.h"

#include "Game.h"
#include "BitmapFont.h"
#include "GameplayState.h"
#include <iostream>
#include <fstream>

#include "Animation System\Animation.h"
#include "Animation System\AnimationTimeStamp.h"
#include "Animation System\Frame.h"
#include "Animation System\AnimationSystem.h"


#include <string>
#include <vector>

/**************************************************************/
// GetInstance
//	- store the ONLY instance in global memory
//	- return the only instance by pointer
/*static*/ MainMenuState* MainMenuState::GetInstance(void)
{
	static MainMenuState s_Instance;

	return &s_Instance;
}


/**************************************************************/
// Enter
//	- reset the game & load resources
/*virtual*/ void MainMenuState::Enter(void)
{
	// Reset the cursor to the top
	// (commented out to keep the last cursor position)
	//m_nCursor = 0;
	//define the four buttons that will appear on screen to make mouse input easier in the render method
	playRect = { 250, 120, 420, 170 };
	instructRect = { 250, 200, 420, 250 };
	optionRect = { 250, 280, 420, 330 };
	creditsRect = { 250, 360, 420, 410 };
	exitRect = { 250, 440, 420, 490 };

	//Upon entering the MainMenuState the volume info last used should be loaded from a text file
	ifstream ifs("resources/Options.txt");
	if (ifs.is_open() && ifs.good())
	{
		int mVol, sfxVol;
		bool fullscreen;
		ifs >> mVol;
		OptionsState::GetInstance()->setMusicVolume(mVol);
		ifs.ignore(INT_MAX, ',');
		ifs >> sfxVol;
		OptionsState::GetInstance()->setSFXVolume(sfxVol);
		ifs.ignore(INT_MAX, ',');
		ifs >> fullscreen;
		OptionsState::GetInstance()->setFullScreen(fullscreen);

		if (OptionsState::GetInstance()->getFullScreen())
			SGD::GraphicsManager::GetInstance()->Resize({ 800, 600 }, false);
		else
			SGD::GraphicsManager::GetInstance()->Resize({ WINDOW_WIDTH, WINDOW_HEIGHT }, true);
	}
	ifs.close();

	// Set music
	Game::GetInstance()->SetMusic(Game::GetInstance()->mainMenuMusic);
	// Set volume properly
	SGD::AudioManager::GetInstance()->SetMasterVolume(SGD::AudioGroup::Music, OptionsState::GetInstance()->getMusicVolume());
	// Set background color
	//SGD::GraphicsManager::GetInstance()->SetClearColor( SGD::Color::Black );
}

/**************************************************************/
// Exit
//	- deallocate / unload resources
/*virtual*/ void MainMenuState::Exit(void)
{
}


/**************************************************************/
// Input
//	- handle user input
/*virtual*/ bool MainMenuState::Input(void)
{
	SGD::InputManager* pInput = SGD::InputManager::GetInstance();
	static SGD::Vector joystick_prev = pInput->GetLeftJoystick(0);


	//if there is a controller connected use these controls:
	//if (pInput->IsControllerConnected(0))	How do you get this to return true????
	//What numbers are associated with which buttons on the controller? apparently everything is actually 1 less than is labeled on the controller

	// Press Escape to quit
	//else
	//{
	if (((ARCADEBUILD == 1) && pInput->IsKeyPressed(SGD::Key::RButton)) || pInput->IsKeyPressed(SGD::Key::Escape) == true || pInput->IsButtonPressed(0, 1))
	{
		if (playMenu)
		{
			playMenu = false;
		}
	}


	//Check for mouse movement
	SGD::Point mousepos = pInput->GetMousePosition();
	if (mousepos.x > 50.0f && mousepos.x < 300.0f && mousepos.y > 50.0f && mousepos.y < 300 && pInput->GetMouseMovement() != SGD::Vector::Zero)
	{
		//Mouse is over buttons
		int newCursorPos = (int)(mousepos.y / 50.0f) - 1;

		if (playMenu)
		{
			if (newCursorPos != 2 && newCursorPos != 3) m_nCursor = newCursorPos;
		}
		else m_nCursor = newCursorPos;
	}

	//Check for click
	if ((((ARCADEBUILD == 1) && pInput->IsButtonPressed(0, 0)) || pInput->IsKeyPressed(SGD::Key::LButton) && mousepos.x > 50.0f && mousepos.x < 300.0f && mousepos.y > 50.0f && mousepos.y < 300))
	{
		if (!playMenu)
		{
			if (m_nCursor == 0)
			{
				playMenu = true;
			}
			else if (m_nCursor == 1)
			{
				Game::GetInstance()->ChangeState(InstructionsState::GetInstance());
				return true;
			}
			else if (m_nCursor == 2)
			{
				Game::GetInstance()->ChangeState(OptionsState::GetInstance());
				return true;
			}
			else if (m_nCursor == 3)
			{
				Game::GetInstance()->ChangeState(CreditsState::GetInstance());
				return true;
			}
			else if (m_nCursor == 4)
			{
				return false;
			}
		}
		else
		{
			if (m_nCursor == 0)
			{
				playMenu = false;
				Game::GetInstance()->ChangeState(NewGameState::GetInstance());
				return true;
			}
			else if (m_nCursor == 1)
			{
				Game::GetInstance()->ChangeState(LoadGameState::GetInstance());
				return true;
			}
			else if (m_nCursor == 4)
			{
				playMenu = false;
				return true;
			}
		}
	}

	// Move cursor
	if (Game::GetInstance()->IsMenuDownInput())
	{
		++m_nCursor;

		// Wrap around
		if (!playMenu && m_nCursor > 4)		// should enumerate the options
			m_nCursor = 0;
		else if (playMenu && m_nCursor > 3)
			m_nCursor = 0;
		if (playMenu && m_nCursor == 2)
			m_nCursor = 4;
	}
	else if (Game::GetInstance()->IsMenuUpInput())
	{
		--m_nCursor;

		// Wrap around
		if (m_nCursor < 0)
			m_nCursor = 4;
		else if (playMenu && (m_nCursor == 2 || m_nCursor == 3))
			m_nCursor = 1;
	}


	// Select an option?
	if (Game::GetInstance()->IsMenuConfirmInput())
	{
		// Which option?
		if (m_nCursor == 0)		// PLAY
		{
			if (playMenu)
			{
				m_nCursor = 0;
				Game::GetInstance()->ChangeState(NewGameState::GetInstance());
				return true;
			}
			else
			{
				playMenu = true;
			}
			return true;
		}
		else if (m_nCursor == 1)
		{
			if (playMenu)
			{
				//go to load game state where you will see all previous saved games
				//Game::GetInstance()->ChangeState(LoadGameState::GetInstance());
				m_nCursor = 0;
				Game::GetInstance()->ChangeState(LoadGameState::GetInstance());
				return true;
			}
			else
			{
				m_nCursor = 0;
				Game::GetInstance()->ChangeState(InstructionsState::GetInstance());
				return true;
			}
		}
		else if (!playMenu && m_nCursor == 2)
		{
			m_nCursor = 0;
			Game::GetInstance()->ChangeState(OptionsState::GetInstance());
			return true;
		}
		//KNOWN BUG FIX #1: allowed the use of the Enter button on the main menu to let you get to the credits state
		else if (!playMenu && m_nCursor == 3)
		{
			m_nCursor = 0;
			Game::GetInstance()->ChangeState(CreditsState::GetInstance());
			return true;
		}
		else if (m_nCursor == 4)
		{
			if (playMenu)
			{
				playMenu = false;
				m_nCursor = 0;
				return true;
			}
			else
			{
				return false;
			}
		}

	}
	joystick_prev = pInput->GetLeftJoystick(0);
	//}
	return true;	// keep playing
}


/**************************************************************/
// Update
//	- update entities / animations
/*virtual*/ void MainMenuState::Update(float elapsedTime)
{
}


/**************************************************************/
// Render
//	- render entities / menu options
/*virtual*/ void MainMenuState::Render(void)
{

	// Use the game's font
	//BitmapFont* pFont = Game::GetInstance()->GetFont();

	Game::GetInstance()->GetFontTitle()->Draw("Desolate", Game::GetInstance()->GetScreenWidth() - 200, 50, 1.f, SGD::Color::White);

	if (playMenu)
	{
		DrawButton({ 50, 50 }, *Game::GetInstance()->GetFontButton(), "New Game");
		DrawButton({ 50, 100 }, *Game::GetInstance()->GetFontButton(), "Load Game");
		DrawButton({ 50, 250 }, *Game::GetInstance()->GetFontButton(), "Back");
	}
	else
	{

		//// Display the game title centered at 4x scale
		DrawButton({ 50, 50 }, *Game::GetInstance()->GetFontButton(), "Play");
		DrawButton({ 50, 100 }, *Game::GetInstance()->GetFontButton(), "Instructions");
		DrawButton({ 50, 150 }, *Game::GetInstance()->GetFontButton(), "Options");
		DrawButton({ 50, 200 }, *Game::GetInstance()->GetFontButton(), "Credits");
		DrawButton({ 50, 250 }, *Game::GetInstance()->GetFontButton(), "Exit");

		//font->Draw("Use the up/down + ENTER buttons or the mouse to select an option", 100, 520, 0.5f, SGD::Color::White);
		//pFont->Draw( "GAME", (width - (4 * 32 * 4.0f))/2, 50, 4.0f, {} );
		//
		//// Display the menu options centered at 1x scale
		//pFont->Draw( "Play", (width - (4 * 32))/2, 300, 1.0f, {255, 0, 0} );
		//pFont->Draw( "Exit", (width - (4 * 32))/2, 350, 1.0f, {255, 0, 0} );
		//
		// Display the cursor next to the option

	}
	//SGD::GraphicsManager::GetInstance()->DrawString("I", { 45.0f, 130.0f + 80.0f * m_nCursor }, { 0, 255, 0 });
	Game::GetInstance()->GetFontTitle()->Draw("I", 40, 47 + 50 * m_nCursor, 0.8f, { (unsigned char)(Game::GetInstance()->opacity * 255), 255, 255, 255 });
}
