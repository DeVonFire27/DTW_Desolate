#include "globals.h"
#include "CreditsState.h"
#include "MainMenuState.h"
#include "Game.h"
#include "CharacterTypes\Cyborg.h"
#include "CharacterTypes\Gunslinger.h"
#include "CharacterTypes\Sniper.h"

CreditsState* CreditsState::GetInstance(void)
{
	static CreditsState s_Instance;
	return &s_Instance;
}

// IGameState Interface:
void CreditsState::Enter(void)
{
	yposition = 600;
	creditsMusic = SGD::AudioManager::GetInstance()->LoadAudio("resources/audio/Background Music/Credits Music.xwm");
	creditsScreen = SGD::GraphicsManager::GetInstance()->LoadTexture("resources/graphics/Credits Screen.png");
	Game::GetInstance()->SetMusic(creditsMusic);
}

void CreditsState::Exit(void)
{
	SGD::GraphicsManager::GetInstance()->UnloadTexture(creditsScreen);
	SGD::AudioManager::GetInstance()->UnloadAudio(creditsMusic);
}

bool CreditsState::Input(void)
{
	SGD::InputManager* pInput = SGD::InputManager::GetInstance();

	//if you are using the arcade machine use these controls:
	
	//What numbers are associated with which buttons on the controller?
	if ((ARCADEBUILD == 0 && pInput->IsButtonPressed(0, 1)) || (ARCADEBUILD == 1 && pInput->IsButtonPressed(0, 0)))
	{
		Game::GetInstance()->ChangeState(MainMenuState::GetInstance());
		return true;
	}

	//else if there is no controller being used, use these controls:
	if (pInput->IsAnyKeyPressed())
	{
		Game::GetInstance()->ChangeState(MainMenuState::GetInstance());
		return true;
	}

	return true;
}

void CreditsState::Update(float elapsedTime)
{
	yposition -= elapsedTime * 50;
}

void CreditsState::Render(void)
{
	//SGD::GraphicsManager::GetInstance()->DrawRectangle({ 0, 0, 800, 600 }, { 0, 0, 0 });
	SGD::GraphicsManager::GetInstance()->DrawTexture(creditsScreen, { -50, yposition }, 0.0f, {}, {}, { 1.0f, .8f });
	//SGD::GraphicsManager::GetInstance()->DrawString("Press any key to skip credits", { 250, 550 }, { 255, 255, 255 });
}