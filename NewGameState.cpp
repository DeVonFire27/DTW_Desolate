#include "globals.h"
#include "NewGameState.h"
#include "GameplayState.h"
#include "MainMenuState.h"
#include "CreditsState.h"
#include "Game.h"
#include <fstream>
#include "BitmapFont.h"
//#include <string>
#include <sstream>
#include <iostream>
using namespace std;

NewGameState* NewGameState::GetInstance(void)
{
	static NewGameState s_Instance;
	return &s_Instance;
}

void NewGameState::Enter(void)
{
	GameplayState::GetInstance()->tutorialChosen = false;

	// GameplayState::GetInstance()->tutorial = true;

	newGameScreen = SGD::GraphicsManager::GetInstance()->LoadTexture(L"resources/graphics/Desolate New Game Screen.png");
	newGameScreen_selected = SGD::GraphicsManager::GetInstance()->LoadTexture(L"resources/graphics/Desolate New Game Screen_selected.png");
	backRect = { 500, 500, 670, 550 };
	beginRect = { 500, 400, 670, 450 };
	
	ifstream ifs("resources/Class Info.txt");
	if (ifs.is_open())
	{
		string charClass, charDescription, charStats, startingWeapon;
		stringstream classInfo;
		stringstream classStatInfo;
		for (int i = 0; i < 5; i++)
		{
			getline(ifs, charDescription, '\t');
			getline(ifs, charStats, '\t');
			getline(ifs, startingWeapon, '\n');
			
			classInfo << charDescription;
			classStatInfo << charStats << '\n' << startingWeapon;

			if (i == 0)
			{
				gunslingerStatInfo = classStatInfo.str();
				gunslingerInfo = classInfo.str();
			}
			else if (i == 1)
			{
				sniperStatInfo = classStatInfo.str();
				sniperInfo = classInfo.str();
			}
			else if (i == 2)
			{
				brawlerStatInfo = classStatInfo.str();
				brawlerInfo = classInfo.str();
			}
			else if (i == 3)
			{
				medicStatInfo = classStatInfo.str();
				medicInfo = classInfo.str();
			}
			else if (i == 4)
			{
				cyborgStatInfo = classStatInfo.str();
				cyborgInfo = classInfo.str();
			}
			classInfo.str("");
			classStatInfo.str("");
		}
	}
	ifs.close();
}

void NewGameState::Exit(void)
{
	// unload background
	SGD::GraphicsManager::GetInstance()->UnloadTexture(newGameScreen);
	SGD::GraphicsManager::GetInstance()->UnloadTexture(newGameScreen_selected);
}
		
bool NewGameState::Input(void)
{
	SGD::InputManager* pInput = SGD::InputManager::GetInstance();
	SGD::Point cursorPos = pInput->GetMousePosition();

	if (pInput->IsKeyPressed(SGD::Key::MouseLeft))
	{
		usingKeyboardInput = false;
		//Check for when the user chooses a character class
		if (cursorPos.x > 47.0f && cursorPos.x < 475.0f && cursorPos.y > 110.0f && cursorPos.y < 219.0f)
		{
			characterCursor = (int)((cursorPos.x - 47.0f) / 86.0f);
		}

		//check for when a player chooses a difficulty
		else if (cursorPos.x > 47.0f && cursorPos.x < 290.0f && cursorPos.y > 391.0f && cursorPos.y < 500.0f)
		{
			difficultyCursor = (int)((cursorPos.x - 47.0f) / 86.0f);
		}

		//Go back to main menu:
		else if (pInput->GetMousePosition().IsWithinRectangle(backRect))
		{
			Game::GetInstance()->ChangeState(MainMenuState::GetInstance());
			return true;
		}

		//Begin new game using chosen settings:
		else if (pInput->GetMousePosition().IsWithinRectangle(beginRect))
		{
			//TODO: set all players stats once character editor is finished
			SavePlayerInfo();
			GameplayState::GetInstance()->newGame = true;

			Game::GetInstance()->ChangeState(GameplayState::GetInstance());
			return true;
		}
	}

	//Select the thingies
	if (pInput->GetMousePosition().IsWithinRectangle(beginRect) && pInput->GetMouseMovement() != SGD::Vector::Zero) section = 2;
	else if (pInput->GetMousePosition().IsWithinRectangle(backRect) && pInput->GetMouseMovement() != SGD::Vector::Zero) section = 3;

	//If you are within the character select portion, use the left and right arrow keys to change character use up and down arrow keys to go to next category

	if (Game::GetInstance()->IsMenuRightInput())
	{
		usingKeyboardInput = true;
		if (section == 0)
		{
			++characterCursor;
			if (characterCursor > 4)
				characterCursor = 0;
		}
		else if (section == 1)
		{
			++difficultyCursor;
			if (difficultyCursor > 2)
				difficultyCursor = 0;
		}
	}

	if (Game::GetInstance()->IsMenuLeftInput())
	{
		usingKeyboardInput = true;
		if (section == 0)
		{
			--characterCursor;

			// Wrap around
			if (characterCursor < 0)
				characterCursor = 4;
		}
		else if (section == 1)
		{
			--difficultyCursor;
			if (difficultyCursor < 0)
			{
				difficultyCursor = 2;
			}
		}
	}

	if (Game::GetInstance()->IsMenuDownInput())
	{
		usingKeyboardInput = true;
		section++;
		if (section > 3)
			section = 0;
	}
	else if (Game::GetInstance()->IsMenuUpInput())
	{
		usingKeyboardInput = true;
		section--;
		if (section < 0)
			section = 3;
	}

	// Select an option?
	if (Game::GetInstance()->IsMenuConfirmInput())
	{
		usingKeyboardInput = true;
		// Which option?
		if (section == 2)	//Start Game
		{
			//Save all information chosen by the player
			SavePlayerInfo();
			GameplayState::GetInstance()->newGame = true;

			Game::GetInstance()->ChangeState(GameplayState::GetInstance());
			return true;
		}
		else if (section == 3)		//Back
		{
			Game::GetInstance()->ChangeState(MainMenuState::GetInstance());
			return true;
		}
	}

	//Go back to a previous field
	if (Game::GetInstance()->IsMenuBackInput())
	{
		Game::GetInstance()->ChangeState(MainMenuState::GetInstance());
	}
	return true;
}

void NewGameState::SavePlayerInfo()
{
	if (characterCursor == 0)
	{
		Game::GetInstance()->playerClass = "Gunslinger";
	}
	else if (characterCursor == 1)
	{
		Game::GetInstance()->playerClass = "Sniper";
	}
	else if (characterCursor == 2)
	{
		Game::GetInstance()->playerClass = "Brawler";
	}
	else if (characterCursor == 3)
	{
		Game::GetInstance()->playerClass = "Medic";
	}
	else if (characterCursor == 4)
	{
		Game::GetInstance()->playerClass = "Cyborg";
	}
	if (difficultyCursor == 0)
	{
		Game::GetInstance()->playerDifficulty = difficulty::standard;
	}
	else if (difficultyCursor == 1)
	{
		Game::GetInstance()->playerDifficulty = difficulty::difficult;
	}
	else if (difficultyCursor == 2)
	{
		Game::GetInstance()->playerDifficulty = difficulty::veryhard;
	}
}

void NewGameState::Update(float elapsedTime)
{
}

void NewGameState::Render(void)
{
	unsigned char opacity = (unsigned char)(Game::GetInstance()->opacity * 255);
	unsigned char charCursor = 255;
	unsigned char diffCursor = 255;
	if (section == 0) charCursor = opacity;
	else if (section == 1) diffCursor = opacity;

	//Draw the new game screen texture:
	SGD::GraphicsManager::GetInstance()->DrawTexture(newGameScreen, { 50.0f, 50.0f });

	//Draw the selection rectangle around a character portrait
	SGD::GraphicsManager::GetInstance()->DrawRectangle({ 47.0f + characterCursor * 92.0f, 110.0f, 133.0f + characterCursor * 92.0f, 219.0f }, { 0, 0, 0, 0 }, { charCursor, 255, 255, 255 }, 3);
	//Draw the selection rectangle around a difficulty level
	SGD::GraphicsManager::GetInstance()->DrawRectangle({ 47.0f + difficultyCursor * 92.0f, 391.0f, 133.0f + difficultyCursor * 92.0f, 500.0f }, { 0, 0, 0, 0 }, { diffCursor, 255, 255, 255 }, 3);

	//cursor on back and begin
	if (section == 2)
	{
		Game::GetInstance()->GetFontTitle()->Draw("I", (int)beginRect.left - 10, (int)beginRect.top - 2, 0.8f, { (unsigned char)(Game::GetInstance()->opacity * 255), 255, 255, 255 });
	}
	else if (section == 3)
	{
		Game::GetInstance()->GetFontTitle()->Draw("I", (int)backRect.left - 10, (int)backRect.top - 2, 0.8f, { (unsigned char)(Game::GetInstance()->opacity * 255), 255, 255, 255 });
	}

	//Draw the Back and Begin buttons
	DrawButton({ backRect.left, backRect.top }, *Game::GetInstance()->GetFontButton(), "Back");
	DrawButton({ beginRect.left, beginRect.top }, *Game::GetInstance()->GetFontButton(), "Begin");

	//Draw the character info to the screen depending on which portrait is currently selected
	if (characterCursor == 0)
	{
		Game::GetInstance()->GetFontThin()->Draw(gunslingerInfo.c_str(), 50, 230, 1.0f, { 255, 255, 255 });
		Game::GetInstance()->GetFontThin()->Draw(gunslingerStatInfo.c_str(), 520, 130, 1.0f, { 255, 255, 255 });
	}
	else if (characterCursor == 1)
	{
		Game::GetInstance()->GetFontThin()->Draw(sniperInfo.c_str(), 50, 230, 1.0f, { 255, 255, 255 });
		Game::GetInstance()->GetFontThin()->Draw(sniperStatInfo.c_str(), 520, 130, 1.0f, { 255, 255, 255 });
	}
	else if (characterCursor == 2)
	{
		Game::GetInstance()->GetFontThin()->Draw(brawlerInfo.c_str(), 50, 230, 1.0f, { 255, 255, 255 });
		Game::GetInstance()->GetFontThin()->Draw(brawlerStatInfo.c_str(), 520, 130, 1.0f, { 255, 255, 255 });
	}
	else if (characterCursor == 3)
	{
		Game::GetInstance()->GetFontThin()->Draw(medicInfo.c_str(), 50, 230, 1.0f, { 255, 255, 255 });
		Game::GetInstance()->GetFontThin()->Draw(medicStatInfo.c_str(), 520, 130, 1.0f, { 255, 255, 255 });
	}
	else if (characterCursor == 4)
	{
		Game::GetInstance()->GetFontThin()->Draw(cyborgInfo.c_str(), 50, 230, 1.0f, { 255, 255, 255 });
		Game::GetInstance()->GetFontThin()->Draw(cyborgStatInfo.c_str(), 520, 130, 1.0f, { 255, 255, 255 });
	}
}