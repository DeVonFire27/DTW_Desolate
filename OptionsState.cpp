#include "globals.h"
#include "OptionsState.h"
#include "MainMenuState.h"
#include "GameplayState.h"
#include "BitmapFont.h"
#include "Game.h"
#include <fstream>

OptionsState* OptionsState::GetInstance(void)
{
	static OptionsState s_Instance;
	return &s_Instance;
}

// IGameState Interface:
void OptionsState::Enter(void)
{
	fullscreenRect = { 380, 405, 420, 445 };
	backRect = { 300, 500, 470, 550 };
	sfxRect = { 300, 302, 600, 342 };
	musicRect = { 300, 202, 600, 242 };

	//Load in any textures
	emptyCheckBox = SGD::GraphicsManager::GetInstance()->LoadTexture("resources/graphics/emptyCheckBox.png");
	checkedBox = SGD::GraphicsManager::GetInstance()->LoadTexture("resources/graphics/checkedBox.png");

	//Load in the test audio files that play when using the volume sliders
	//backgroundMusic = SGD::AudioManager::GetInstance()->LoadAudio("resources/audio/Background Music/Six-Shooter.wav");
	gunshotSFX = SGD::AudioManager::GetInstance()->LoadAudio("resources/audio/Sound Effects/Gunfire.wav");


}

void OptionsState::Exit(void)
{
	//Unload any textures
	SGD::GraphicsManager::GetInstance()->UnloadTexture(emptyCheckBox);
	SGD::GraphicsManager::GetInstance()->UnloadTexture(checkedBox);

	//Upon Exiting the optionsState the volume info should be saved to a text file
	ofstream ofs("resources/Options.txt", ios_base::out | ios_base::trunc);	//open up the file
	if (ofs.is_open() && ofs.good())
	{
		ofs << musicVolume << ',' << SFXVolume << ',' << fullScreen;
	}
	ofs.close();
	ofs.clear();

	SGD::AudioManager::GetInstance()->UnloadAudio(backgroundMusic);
	SGD::AudioManager::GetInstance()->UnloadAudio(gunshotSFX);

}

bool OptionsState::Input(void)
{
	//Check for keyboard/mouse input
	SGD::InputManager* pInput = SGD::InputManager::GetInstance();
	static SGD::Vector joystick_prev = pInput->GetLeftJoystick(0);

	if (Game::GetInstance()->IsMenuConfirmInput() || pInput->IsKeyPressed(Key::MouseLeft))
	{
		if (mouseLocation.IsWithinRectangle(musicRect))
		{
			musicVolume = (int)((mouseLocation.x - musicRect.left) / 3);
			SGD::AudioManager::GetInstance()->SetMasterVolume(SGD::AudioGroup::Music, musicVolume);

			if (musicVolume >= 99)
				musicVolume = 100;
		}
		else if (mouseLocation.IsWithinRectangle(sfxRect))
		{
			SFXVolume = (int)((mouseLocation.x - sfxRect.left) / 3);
			SGD::AudioManager::GetInstance()->SetMasterVolume(SGD::AudioGroup::SoundEffects, SFXVolume);

			if (SFXVolume >= 99)
				SFXVolume = 100;
		}
		else if (mouseLocation.IsWithinRectangle(backRect) || pInput->IsButtonPressed(0, 1))
		{
			if (GameplayState::GetInstance()->fromGameplayState)
				Game::GetInstance()->InteruptState(GameplayState::GetInstance());
			else
				Game::GetInstance()->ChangeState(MainMenuState::GetInstance());
		}
		else if (mouseLocation.IsWithinRectangle(fullscreenRect))
		{
			fullScreen = !fullScreen;
			//make the game full screen:
			if (fullScreen)
				SGD::GraphicsManager::GetInstance()->Resize({ 800, 600 }, false);
			else
				SGD::GraphicsManager::GetInstance()->Resize({ WINDOW_WIDTH, WINDOW_HEIGHT }, true);
		}
	}

	// If the player's mouse is inside of sound effects and they release the left button
	if (mouseLocation.IsWithinRectangle(sfxRect))
	{
		if (pInput->IsKeyReleased(SGD::Key::LButton))
		{
			SGD::AudioManager::GetInstance()->StopAudio(gunshotSFX);
			SGD::AudioManager::GetInstance()->PlayAudio(gunshotSFX);
		}
	}

	if (Game::GetInstance()->IsMenuDownInput())
	{
		if (m_nCursor == 1)
			SGD::AudioManager::GetInstance()->StopAudio(gunshotSFX);
		++m_nCursor;

		// Wrap around
		if (m_nCursor > 3)		// should enumerate the options
			m_nCursor = 0;
	}
	if (Game::GetInstance()->IsMenuUpInput())
	{
		if (m_nCursor == 1)
			SGD::AudioManager::GetInstance()->StopAudio(gunshotSFX);

		--m_nCursor;

		// Wrap around
		if (m_nCursor < 0)		// should enumerate the options
			m_nCursor = 3;
	}

	if (Game::GetInstance()->IsMenuRightInput())
	{
		if (m_nCursor == 0 && musicVolume < 100)
		{
			musicVolume += 5;
			SGD::AudioManager::GetInstance()->SetMasterVolume(SGD::AudioGroup::Music, musicVolume);

			if (musicVolume >= 100)
				musicVolume = 100;
		}
		if (m_nCursor == 1 && SFXVolume < 100)
		{
			SGD::AudioManager::GetInstance()->StopAudio(gunshotSFX);
			SFXVolume += 5;
			SGD::AudioManager::GetInstance()->SetMasterVolume(SGD::AudioGroup::SoundEffects, SFXVolume);
			SGD::AudioManager::GetInstance()->PlayAudio(gunshotSFX);

			if (SFXVolume >= 100)
				SFXVolume = 100;
		}
	}

	if (Game::GetInstance()->IsMenuLeftInput())
	{
		if (m_nCursor == 0 && musicVolume > 0)
		{
			musicVolume -= 5;
			SGD::AudioManager::GetInstance()->SetMasterVolume(SGD::AudioGroup::Music, musicVolume);

		}
		if (m_nCursor == 1 && SFXVolume > 0)
		{
			SGD::AudioManager::GetInstance()->StopAudio(gunshotSFX);
			SFXVolume -= 5;
			SGD::AudioManager::GetInstance()->SetMasterVolume(SGD::AudioGroup::SoundEffects, SFXVolume);

			SGD::AudioManager::GetInstance()->PlayAudio(gunshotSFX);
		}
	}

	if (Game::GetInstance()->IsMenuConfirmInput())
	{
		if (m_nCursor == 2)
		{
			fullScreen = !fullScreen;
			//make the game full screen:
			if (fullScreen)
				SGD::GraphicsManager::GetInstance()->Resize({ 800, 600 }, false);
			else
				SGD::GraphicsManager::GetInstance()->Resize({ WINDOW_WIDTH, WINDOW_HEIGHT }, true);
		}
		else if (m_nCursor == 3)
		{
			if (GameplayState::GetInstance()->fromGameplayState)
				Game::GetInstance()->InteruptState(GameplayState::GetInstance());
			else
				Game::GetInstance()->ChangeState(MainMenuState::GetInstance());

			return true;
		}
	}

	if (Game::GetInstance()->IsMenuBackInput())
	{
		if (GameplayState::GetInstance()->fromGameplayState)
			Game::GetInstance()->InteruptState(GameplayState::GetInstance());
		else
			Game::GetInstance()->ChangeState(MainMenuState::GetInstance());

		return true;
	}

	joystick_prev = pInput->GetLeftJoystick(0);
	return true;
}

void OptionsState::Update(float elapsedTime)
{
	mouseLocation = SGD::InputManager::GetInstance()->GetMousePosition();
}

void OptionsState::Render(void)
{
	//Render background, music/sfx sliders, full screen option, and back button
	Game::GetInstance()->GetFontTitle()->Draw("Options", 370, 70, 1.f, { 255, 255, 255 });

	Game::GetInstance()->GetFontButton()->Draw("Music", 200, 200, 1.0, { 255, 255, 255 });
	SGD::GraphicsManager::GetInstance()->DrawRectangle(musicRect, { 0, 0, 0, 0 }, { 255, 255, 255, 255 }, 2);
	SGD::GraphicsManager::GetInstance()->DrawRectangle({ 300.0f, 202.0f, 300.0f + musicVolume * 3.0f, 242.0f }, { 255, 255, 255, 255 }, { 255, 255, 255, 255 }, 1);

	Game::GetInstance()->GetFontButton()->Draw("SFX", 200, 300, 1.0, { 255, 255, 255 });
	SGD::GraphicsManager::GetInstance()->DrawRectangle(sfxRect, { 0, 0, 0, 0 }, { 255, 255, 255, 255 }, 2);
	SGD::GraphicsManager::GetInstance()->DrawRectangle({ 300.0f, 302.0f, 300.0f + SFXVolume * 3.0f, 342.0f }, { 255, 255, 255, 255 }, { 255, 255, 255, 255 }, 2);

	Game::GetInstance()->GetFontButton()->Draw("FullScreen", 200, 400, 1.0, { 255, 255, 255 });

	if (fullScreen)
		//SGD::GraphicsManager::GetInstance()->DrawTexture(checkedBox, { 400, 390 }, 0.0f, {}, {}, { .2f, .2f });
		SGD::GraphicsManager::GetInstance()->DrawRectangle(SGD::Rectangle({ 380, 405 }, SGD::Size(40, 40)), SGD::Color::White);
	else
		//SGD::GraphicsManager::GetInstance()->DrawTexture(emptyCheckBox, { 400, 390 }, 0.0f, {}, {}, { .2f, .2f });
		SGD::GraphicsManager::GetInstance()->DrawRectangle(SGD::Rectangle({ 380, 405 }, SGD::Size(40, 40)), SGD::Color(0, 0, 0, 0), SGD::Color(255, 255, 255, 255), 2);

	if (m_nCursor < 3)
		Game::GetInstance()->GetFontTitle()->Draw("I", 185, 192 + 100 * m_nCursor, 1.0f, SGD::Color(int(Game::GetInstance()->opacity * 255), 255, 255, 255 ));
	else
		Game::GetInstance()->GetFontTitle()->Draw("I", 265, 500, 1.f, SGD::Color(int(Game::GetInstance()->opacity * 255), 255, 255, 255));

	DrawButton({ 280, 508 }, *Game::GetInstance()->GetFontButton(), "Back");
}