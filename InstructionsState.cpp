#include "globals.h"
#include "InstructionsState.h"
#include "MainMenuState.h"
#include "GameplayState.h"
#include "BitmapFont.h"
#include "Game.h"

InstructionsState* InstructionsState::GetInstance(void)
{
	static InstructionsState s_Instance;
	return &s_Instance;
}

// IGameState Interface:
void InstructionsState::Enter(void)
{
	page2 = false;
	if (!GameplayState::GetInstance()->fromGameplayState)
		m_nCursor = 0;
	else
		m_nCursor = 1;

	backRect = { 200, 550, 370, 600 };
	nextRect = { 500, 550, 670, 600 };

	//Load textures for character portraits etc
	if (ARCADEBUILD)
		pcControlsScreen = SGD::GraphicsManager::GetInstance()->LoadTexture(L"resources/graphics/Desolate ARCADE Controls Screen.png");
	else
		pcControlsScreen = SGD::GraphicsManager::GetInstance()->LoadTexture(L"resources/graphics/Desolate PC Controls Screen.png");

	characterClassesScreen = SGD::GraphicsManager::GetInstance()->LoadTexture(L"resources/graphics/Desolate Character Classes Screen.png");

}

void InstructionsState::Exit(void)
{
	//Unload textures
	SGD::GraphicsManager::GetInstance()->UnloadTexture(pcControlsScreen);
	SGD::GraphicsManager::GetInstance()->UnloadTexture(characterClassesScreen);
}
		
bool InstructionsState::Input(void)
{
	SGD::InputManager* pInput = SGD::InputManager::GetInstance();

	//Move the cursor
	if (pInput->GetMousePosition().IsWithinRectangle(nextRect) && pInput->GetMouseMovement() != SGD::Vector::Zero)
		m_nCursor = 1;
	else if (pInput->GetMousePosition().IsWithinRectangle(backRect) && pInput->GetMouseMovement() != SGD::Vector::Zero)
		m_nCursor = 0;

	if (pInput->IsKeyPressed(Key::MouseLeft))
	{
		//Check for when the user presses the Next button
		if (pInput->GetMousePosition().IsWithinRectangle(nextRect))
		{
			if (page2)	//if on page 2 and the player presses the play button, go to gameplay state
			{
				if (GameplayState::GetInstance()->fromGameplayState)
				{
					Game::GetInstance()->InteruptState(GameplayState::GetInstance());
				}
				else 
				{
					MainMenuState::GetInstance()->setPlayMenu(true);
					Game::GetInstance()->ChangeState(MainMenuState::GetInstance());
					return true;
				}
			}
			else		//if on page 1 and the player presses the next button, go to page 2
				page2 = true;
		}
		//Check for when the user presses the Back button
		if (pInput->GetMousePosition().IsWithinRectangle(backRect) || pInput->IsButtonPressed(0, 1))
		{
			if (page2)
			{//if on page 2 and the player presses the back button, go back to page 1
				page2 = false;
				if (GameplayState::GetInstance()->fromGameplayState)
					m_nCursor = 1;
			}
			else if (!GameplayState::GetInstance()->fromGameplayState)//if on page 1 and the player presses the back button, go back to the main menu so long as they aren't from the gameplay state
			{
				Game::GetInstance()->ChangeState(MainMenuState::GetInstance());
				return true;
			}
		}
	}

	if (Game::GetInstance()->IsMenuRightInput())
	{
		if (page2 == false && GameplayState::GetInstance()->fromGameplayState)
		{

		}
		else
		{
			m_nCursor++;
			if (m_nCursor > 1)
				m_nCursor = 0;
		}
	}
	else if (Game::GetInstance()->IsMenuLeftInput())
	{
		if (page2 == false && GameplayState::GetInstance()->fromGameplayState)
		{
			
		}
		else
		{
			m_nCursor--;
			if (m_nCursor < 0)
				m_nCursor = 1;
		}
	}
	if (Game::GetInstance()->IsMenuConfirmInput())
	{
		if (m_nCursor == 0)
		{
			if (page2)
			{
				page2 = false;
				if (GameplayState::GetInstance()->fromGameplayState)
					m_nCursor = 1;
			}
			else if (!GameplayState::GetInstance()->fromGameplayState)
			{
				Game::GetInstance()->ChangeState(MainMenuState::GetInstance());
			}
		}
		else if (m_nCursor == 1)
		{
			if (page2)
			{
				if (GameplayState::GetInstance()->fromGameplayState)
				{
					Game::GetInstance()->InteruptState(GameplayState::GetInstance());
				}
				else
				{
					MainMenuState::GetInstance()->setPlayMenu(true);
					Game::GetInstance()->ChangeState(MainMenuState::GetInstance());
				}
			}
			else
				page2 = true;
		}
	}

	if (Game::GetInstance()->IsMenuBackInput())
	{
		if (page2)
		{
			page2 = false;
		}
		else
		{
			Game::GetInstance()->ChangeState(MainMenuState::GetInstance());
			return true;
		}
	}

	return true;
}

void InstructionsState::Update(float elapsedTime)
{

}

void InstructionsState::Render(void)
{
	//Render the background
	
	//Render page 1 of instructions
	if (!page2)
	{
		//Load jpg file for instructions pages
		SGD::GraphicsManager::GetInstance()->DrawTexture(pcControlsScreen, { 0, 0 });
		if (GameplayState::GetInstance()->fromGameplayState)
		{
			DrawButton({ 500, 550 }, *Game::GetInstance()->GetFontButton(), "Next");
		}
		else
		{
			DrawButton({ 200, 550 }, *Game::GetInstance()->GetFontButton(), "Back");
			DrawButton({ 500, 550 }, *Game::GetInstance()->GetFontButton(), "Next");
		}
	}
	//Render page 2 of instructions when the next button is pressed
	else
	{
		SGD::GraphicsManager::GetInstance()->DrawTexture(characterClassesScreen, { 0, 0 }, 0.0f, {}, {}, { .8f, .6f });

		DrawButton({ 200, 550 }, *Game::GetInstance()->GetFontButton(), "Back");
		if (GameplayState::GetInstance()->fromGameplayState)
			DrawButton({ 500, 550 }, *Game::GetInstance()->GetFontButton(), "Resume");
		else
			DrawButton({ 500, 550 }, *Game::GetInstance()->GetFontButton(), "Play");
	}
	//SGD::GraphicsManager::GetInstance()->DrawString("----->", { 145.0f + 300.0f * m_nCursor, 545.0f }, { 0, 255, 0 });
	Game::GetInstance()->GetFontTitle()->Draw("I", 190 + 300 * m_nCursor, 547, 0.8f, { (unsigned char)(Game::GetInstance()->opacity * 255), 255, 255, 255 });
}